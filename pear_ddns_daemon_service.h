#include <stdio.h>
#include <stdlib.h>

#pragma once

#ifdef _WIN32
#include "pear_ddns_win_service.h"
#else
#include "pear_ddns_lnx_daemon.h"
#endif

/* pear callback function */
typedef int (*pear_task_cb)(void *args);

typedef struct
{
    pear_task_cb cb;
    void *args;
} pear_task_t;

char *old_ip, *new_ip;

int pear_ddns_process();


