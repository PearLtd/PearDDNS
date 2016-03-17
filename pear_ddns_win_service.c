#ifdef _WIN32
#include "pear_ddns_win_service.h"

SERVICE_STATUS Service_Status;  // service status
SERVICE_STATUS_HANDLE h_Status; // handler for service status

HANDLE g_ServiceStopEvent = INVALID_HANDLE_VALUE;

int Windows_Start()
{
    SERVICE_TABLE_ENTRY Service_Table[2];
    Service_Table[0].lpServiceName = SERVICE_NAME;
    Service_Table[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
    Service_Table[1].lpServiceName = NULL;
    Service_Table[1].lpServiceProc = NULL;
    // start service control dispatcher thread
    StartServiceCtrlDispatcher(Service_Table);
    return 0;
}

void ServiceMain(int argc, char** argv)
{
    h_Status = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);

    Service_Status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    Service_Status.dwControlsAccepted = 0;
    Service_Status.dwCurrentState = SERVICE_START_PENDING;
    Service_Status.dwWin32ExitCode = 0;
    Service_Status.dwServiceSpecificExitCode = 0;
    Service_Status.dwCheckPoint = 0;

    SetServiceStatus(h_Status, &Service_Status);

    g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (g_ServiceStopEvent == NULL)
    {
        Service_Status.dwControlsAccepted = 0;
        Service_Status.dwCurrentState = SERVICE_STOPPED;
        Service_Status.dwWin32ExitCode = GetLastError();
        Service_Status.dwCheckPoint = 1;
        SetServiceStatus(h_Status, &Service_Status);
        return -1;
    }

    /// prepare work thread
    Service_Status.dwControlsAccepted = SERVICE_ACCEPT_STOP; // allow manually stop
    Service_Status.dwCurrentState = SERVICE_RUNNING;
    Service_Status.dwWin32ExitCode = 0;
    Service_Status.dwCheckPoint = 0;
    SetServiceStatus(h_Status, &Service_Status);

    /// create thread
    HANDLE h_Thread = CreateThread(NULL, 0, win_write, NULL, 0, NULL);
    /// wait till end of thread
    WaitForSingleObject(h_Thread, INFINITE);

    /// close service
    CloseHandle(g_ServiceStopEvent);
    Service_Status.dwControlsAccepted = 0;
    Service_Status.dwCurrentState = SERVICE_STOPPED;
    Service_Status.dwWin32ExitCode = 0;
    Service_Status.dwCheckPoint = 3;
    SetServiceStatus(h_Status, &Service_Status);
    return 0;
}

void ServiceCtrlHandler(int CtrlCode)
{
    switch (CtrlCode)
    {
    case SERVICE_CONTROL_STOP:
    {
        if (Service_Status.dwCurrentState != SERVICE_RUNNING)
            break;

        Service_Status.dwControlsAccepted = 0;
        Service_Status.dwCurrentState = SERVICE_STOP_PENDING;
        Service_Status.dwWin32ExitCode = 0;
        Service_Status.dwCheckPoint = 4;
        SetServiceStatus(h_Status, &Service_Status);
        SetEvent(g_ServiceStopEvent);
        break;
    }
    default:
    {
        break;
    }
    }
}

int win_write()
{
    printf("Start Service\n");
    int i = 0;
    while(WaitForSingleObject(g_ServiceStopEvent, 0) != WAIT_OBJECT_0)
    {
        if(i == 559)
        {
            printf("HeartBeat\n");
            pear_ddns_process();
            i = 0;
        }
        else
        {
            i++;
        }
        Sleep(1000);
    }
    printf("Stop Service\n");
    return 0;
}

#endif
