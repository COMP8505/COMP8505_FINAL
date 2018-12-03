#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
//#include <sys/inotify.h>
#include <inotifytools/inotify.h>
#include <inotifytools/inotifytools.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main(void) {
    if (!inotifytools_initialize() ||
        !inotifytools_watch_recursively("/home/dimitry/Documents", IN_ALL_EVENTS)) {
        fprintf(stderr, "%s\n", strerror(inotifytools_error()));
        return -1;
    }

    inotifytools_set_printf_timefmt( "%T" );

    // Output all events as "<timestamp> <path> <events>"
        struct inotify_event * event = inotifytools_next_event( -1 );
        while ( event ) {
                inotifytools_printf( event, "%T %w%f %e\n" );
                event = inotifytools_next_event( -1 );
        }
}