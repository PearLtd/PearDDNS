#ifndef _PEAR_DDNS_CONF_H_
#define _PEAR_DDNS_CONF_H_

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct
{
    char login_token[64];
    char format     [8];
    char domain     [56];
    char user_agent [64];
} pear_ddns_conf_t;

#endif
