#ifndef _PEAR_DDNS_UTIL_H_
#define _PEAR_DDNS_UTIL_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>


#include "pear_ddns_base32.h"

uint32_t ipv4_str_to_int32(const char *ip_str);

char *ipv4_int32_to_str(uint32_t ip);

char *ipv4_str_to_pear_str(char *pear_str, const char *ip_str);

char *pear_str_to_ipv4_str(char *ip_str, const char *pear_str);

#endif // _PEAR_DDNS_UTIL_H_
