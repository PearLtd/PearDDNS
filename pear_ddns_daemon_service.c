#include "pear_ddns_daemon_service.h"
#include "pear_ddns_conf.h"
#include "pear_ddns_query.h"
#include "pear_ddns_base32.h" /* for encoding to domain allowable characters:
http://stackoverflow.com/questions/7111881/what-are-the-allowed-characters-in-a-sub-domain */

#define TIME_SLOT (10*60) //s  or how about in config???

extern char *old_ip, *new_ip;

int pear_ddns_main(int argc, const char *argv[])
{
    old_ip = NULL, new_ip = NULL;
#ifdef _WIN32
    Windows_Start();
#else
    Linux_Start();
#endif
    if(old_ip)
        free(old_ip);
    if(new_ip)
        free(new_ip);
    return 0;
}

int pear_ddns_process(char *old_ip, char *new_ip)
{
    if(old_ip == NULL)
    {
        old_ip = get_external_ip();
        check_and_update_records();
    }
    else
    {
        new_ip = get_external_ip();
        if(strcmp(old_ip, new_ip) != 0)
        {
            check_and_update_records();
        }
        free(old_ip);
        old_ip = new_ip;
        new_ip = NULL;
    }
}
