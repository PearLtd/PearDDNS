#ifndef _PEAR_LNX_DAEMON_H_
#define _PEAR_LNX_DAEMON_H_

#pragma once

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "pear_ddns_daemon_service.h"

void Linux_Start();
int init_daemon();
int lnx_write();

#endif /* _PEAR_LNX_DAEMON_H_ */
