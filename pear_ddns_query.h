#ifndef _PEAR_DDNS_QUERY_H_
#define _PEAR_DDNS_QUERY_H_

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pear_ddns_conf.h"

enum
{
    PEAR_DDNS_ERR_MEM_OVERFLOW = -9,
    PEAR_DDNS_ERR_PARAMETER,
    PEAR_DDNS_ERR_TIMEOUT,
    PEAR_DDNS_ERR_GET_CLIENT_INFO,
    PEAR_DDNS_ERR_MESSAGE,
    PEAR_DDNS_ERR_NO_RECORD,
    PEAR_DDNS_ERR_UNKNOWN
};

typedef struct
{
    pear_ddns_conf_t *conf;
    char api_url   [64];
    char query_str [512];
    char domain    [56];
} pear_ddns_req_t;

typedef struct MemoryStruct
{
    char *memory;
    size_t size;
} pear_ddns_rsp_t;

typedef struct
{
    pear_ddns_req_t *req;
    pear_ddns_rsp_t *rsp;
} pear_ddns_req_cxt_t;

int pear_ddns_query(pear_ddns_req_t *req,
                    pear_ddns_rsp_t *rsp);

char *get_external_ip();

char *get_domain_id();

int add_record(const char *domain_id,
               const char *sub_domain,
               const char *record_type,
               const char *record_line,
               const char *value,
               const char *mx,
               const char *ttl);

int update_record(const char *domain_id,
                  const char *record_id,
                  const char *sub_domain,
                  const char *record_type,
                  const char *record_line,
                  const char *value,
                  const char *mx,
                  const char *ttl);

int check_and_update_records();

#endif
