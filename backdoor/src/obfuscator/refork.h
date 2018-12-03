#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef void(*(callback))(void);
callback *cb_array;

void refork(void);
void refork_init(int count);
void refork_add_callback(callback cb);

static int SID;