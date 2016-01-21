#ifndef _PEAR_DDNS_UTIL_H_
#define _PEAR_DDNS_UTIL_H_

#include "pear_ddns_util.h"

#ifdef _WIN32
#include <Winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

uint32_t ipv4_str_to_int32(const char *ip_str)
{
    return (uint32_t) inet_addr(ip_str);
}

char *ipv4_int32_to_str(uint32_t ip)
{
    struct in_addr addr;
    addr.s_addr = ip;
    return inet_ntoa(addr);
}

char *ipv4_compact_to_dotted(char *ip_dotted, const unsigned char *ip_compact)
{
    int i;
    sprintf(ip_dotted, "%hhu.%hhu.%hhu.%hhu", *ip_compact,
            *(ip_compact + 1),
            *(ip_compact + 2),
            *(ip_compact + 3));
    return ip_dotted;
}

unsigned char *ipv4_dotted_to_compact(unsigned char *ip_compact, const char *ip_dotted)
{
    sscanf(ip_dotted, "%hhu.%hhu.%hhu.%hhu", &ip_compact[0],
           &ip_compact[1],
           ip_compact + 2,
           ip_compact + 3);
    *(ip_compact + 4) ='\0';
    return ip_compact;
}

char *ipv4_str_to_pear_str(char *pear_str, const char *ip_str)
{
    unsigned char *ip_compact[5];
    ipv4_dotted_to_compact(ip_compact, ip_str);
    size_t src_len = 4, dest_len;
    dest_len = pear_ddns_base32_encsize(src_len);
    pear_ddns_base32_enc(pear_str, ip_compact, src_len);
    *(pear_str + dest_len) = '\0';
    return pear_str;
}

char *pear_str_to_ipv4_str(char *ip_str, const char *pear_str)
{
    size_t src_len, dest_len;
    src_len = strlen(pear_str);
    dest_len = pear_ddns_base32_decsize(src_len);
    unsigned char *ip_compact[5];
    pear_ddns_base32_dec(ip_compact, dest_len, pear_str);
    *(ip_compact + dest_len) = '\0';
    return ipv4_compact_to_dotted(ip_str, ip_compact);
}

#endif // _PEAR_DDNS_UTIL_H_
