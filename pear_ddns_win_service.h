#ifdef _WIN32
#ifndef _PEAR_WIN_SERVICE_H_
#define _PEAR_WIN_SERVICE_H_

#pragma once

#include <windows.h>
#include <stdio.h>
#include "pear_ddns_daemon_service.h"

#define SERVICE_NAME "PearDaemonService"

void ServiceMain(int argc, char** argv);
void ServiceCtrlHandler(int CtrlCode);
int win_write();
void Windows_Start();

#endif /* _PEAR_WIN_SERVICE_H_ */
#endif /* _WIN32 */
