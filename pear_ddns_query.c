#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <curl/curl.h>
#include <json-c/json.h>  /* may be by <json/json.h> in other systems */
#include "pear_ddns_query.h"
#include "pear_ddns_conf.h"
#include "pear_ddns_base32.h"

static size_t
WriteMemoryCallback(void *contents,
                    size_t size,
                    size_t nmemb,
                    void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL)
    {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

int pear_ddns_query(pear_ddns_req_t *req,
                    pear_ddns_rsp_t *rsp)
{
    CURL *curl_handle;
    CURLcode res;
    curl_handle = curl_easy_init();
    rsp->memory = malloc(1);  /* will be grown as needed by the realloc above */
    rsp->size = 0;            /* no data at this point */

    curl_global_init(CURL_GLOBAL_ALL);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)rsp);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, req->conf->user_agent);
    /*curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L); CURLOPT_SSL_VERIFYHOST: on!*/
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 2L);

    curl_easy_setopt(curl_handle, CURLOPT_URL, req->api_url);
    if (strlen(req->query_str) > 0)
    {
        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, req->query_str);
    }

    res = curl_easy_perform(curl_handle);
    /*
    if(res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }
    else
    {
        printf("%lu bytes retrieved\n", (unsigned long)rsp->size);
        printf("%s\n", rsp->memory);
    }
    */

    curl_easy_cleanup(curl_handle);
    /* free(chunk.memory); Do not free here! Should release in application*/
    curl_global_cleanup();
    return res;
}

char *get_external_ip()
{
    pear_ddns_req_t req;
    pear_ddns_rsp_t rsp;
    pear_ddns_conf_t conf;
    req.conf = &conf;
    pear_ddns_init_conf(req.conf);
    req.query_str[0] = '\0';

    char *ret_ip = NULL;
    /* fault tolerance */
    strncpy(req.api_url, "https://api.ipify.org?format=json", sizeof(req.api_url));
    pear_ddns_query(&req, &rsp);

    if (rsp.size == 0)
    {
        strncpy(req.api_url, "http://ipinfo.io/json", sizeof(req.api_url));
        pear_ddns_query(&req, &rsp);
    }
    if (rsp.size == 0)
    {
        strncpy(req.api_url, "http://myexternalip.com/json", sizeof(req.api_url));
        pear_ddns_query(&req, &rsp);
    }
    if (rsp.size != 0)
    {
        json_object *jobj, *j_ip;
        jobj = json_tokener_parse(rsp.memory);
        json_object_object_get_ex(jobj, "ip", &j_ip);
        size_t len = strlen(json_object_get_string(j_ip));
        ret_ip = (char *)malloc(len + 1);
        strcpy(ret_ip, json_object_get_string(j_ip));
    }
    free(rsp.memory);
    return ret_ip;
}

char *get_domain_id()
{
    pear_ddns_req_t req;
    pear_ddns_rsp_t rsp;
    pear_ddns_conf_t conf;
    req.conf = &conf;
    pear_ddns_init_conf(req.conf);

    char *ret_domain_id = NULL;
    strncpy(req.api_url, "https://dnsapi.cn/Domain.Info", sizeof(req.api_url));
    snprintf(req.query_str, sizeof(req.query_str),
             "login_token=%s&format=%s&domain=%s",
             req.conf->login_token,
             req.conf->format,
             req.conf->domain);
    //printf("query_str: %s\n", req.query_str);

    pear_ddns_query(&req, &rsp);

    json_object *jobj, *j_status, *j_code;
    jobj = json_tokener_parse(rsp.memory);
    if (rsp.size)
    {
        json_object_object_get_ex(jobj, "status", &j_status);
        json_object_object_get_ex(j_status, "code", &j_code);
        char *ret_code;
        ret_code = json_object_get_string(j_code);
        if (strcmp(ret_code, "1") == 0)
        {
            json_object *j_domain, *j_id;
            json_object_object_get_ex(jobj, "domain", &j_domain);
            json_object_object_get_ex(j_domain, "id", &j_id);
            size_t len = strlen(json_object_get_string(j_id));
            ret_domain_id = (char *)malloc(len + 1);
            strcpy(ret_domain_id, json_object_get_string(j_id));
        }
    }
    free(rsp.memory);
    return ret_domain_id;
}

/* 1: succ; other: fail*/
int add_record(const char *domain_id,
               const char *sub_domain,
               const char *record_type,
               const char *record_line,
               const char *value,
               const char *mx,
               const char *ttl)
{
    pear_ddns_req_t req;
    pear_ddns_rsp_t rsp;
    pear_ddns_conf_t conf;
    req.conf = &conf;
    pear_ddns_init_conf(req.conf);

    strncpy(req.api_url, "https://dnsapi.cn/Record.Create", sizeof(req.api_url));
    snprintf(req.query_str, sizeof(req.query_str),
             "login_token=%s&format=%s&domain_id=%s&sub_domain=%s&record_type=%s&record_line=%s&value=%s",
             req.conf->login_token,
             req.conf->format,
             domain_id,
             sub_domain,
             record_type,
             record_line,
             value);
    if (mx && strcmp(record_type, "MX") == 0)
        sprintf(req.query_str, "%s&mx=%s", req.query_str, mx);
    if (ttl)
        sprintf(req.query_str, "%s&ttl=%s", req.query_str, ttl);

    //printf("Add record: query_str: %s\n", req.query_str);

    pear_ddns_query(&req, &rsp);

    int ret_code;
    if (rsp.size)
    {
        json_object *jobj, *j_status, *j_code;
        jobj = json_tokener_parse(rsp.memory);
        json_object_object_get_ex(jobj, "status", &j_status);
        json_object_object_get_ex(j_status, "code", &j_code);
        if(strcmp(json_object_get_string(j_code), "1") == 0)
            ret_code = 0;
    }
    else
    {
        ret_code = PEAR_DDNS_ERR_MESSAGE;
    }
    free(rsp.memory);
    return ret_code;
}

/* @return: 1: succ; other: fail */
int update_record(const char *domain_id,
                  const char *record_id,
                  const char *sub_domain,
                  const char *record_type,
                  const char *record_line,
                  const char *value,
                  const char *mx,
                  const char *ttl)
{
    pear_ddns_req_t req;
    pear_ddns_rsp_t rsp;
    pear_ddns_conf_t conf;
    req.conf = &conf;
    pear_ddns_init_conf(req.conf);

    strncpy(req.api_url, "https://dnsapi.cn/Record.Create", sizeof(req.api_url));
    snprintf(req.query_str, sizeof(req.query_str),
             "login_token=%s&format=%s&domain_id=%s&record_id=%s&sub_domain=%s&record_type=%s&record_line=%s&value=%s",
             req.conf->login_token,
             req.conf->format,
             domain_id,
             record_id,
             sub_domain,
             record_type,
             record_line,
             value);
    if (mx && strcmp(record_type, "MX") == 0)
        sprintf(req.query_str, "%s&mx=%s", req.query_str, mx);
    if (ttl)
        sprintf(req.query_str, "%s&ttl=%s", req.query_str, ttl);
    //printf("query_str: %s\n", req.query_str);

    pear_ddns_query(&req, &rsp);

    int ret_code;
    if (rsp.size)
    {
        json_object *jobj, *j_status, *j_code;
        jobj = json_tokener_parse(rsp.memory);
        json_object_object_get_ex(jobj, "status", &j_status);
        json_object_object_get_ex(j_status, "code", &j_code);
        if(strcmp(json_object_get_string(j_code), "1") == 0)
            ret_code = 0;
    }
    else
    {
        ret_code = PEAR_DDNS_ERR_MESSAGE;
    }

    free(rsp.memory);
    return ret_code;
}

static char
*get_record_id(json_object *j_records,
               const char *sub_domain,
               const char *record_type,
               const char *mx)
{

    char *ret_record_id = NULL;
    int array_len = json_object_array_length(j_records);
    //printf("get_record_id: sub_domain=%s, record_type=%s, mx=%s\n", sub_domain, record_type, mx);
    int i;
    for (i = 0; i < array_len; i++)
    {
        json_object *j_record, *j_id, *j_sub_domain, *j_record_type, *j_mx;
        char *i_id, *i_sub_domain, *i_record_type, *i_mx = NULL;
        j_record = json_object_array_get_idx(j_records, i);

        json_object_object_get_ex(j_record, "id", &j_id);
        i_id = json_object_get_string(j_id);

        json_object_object_get_ex(j_record, "name", &j_sub_domain);
        i_sub_domain = json_object_get_string(j_sub_domain);

        json_object_object_get_ex(j_record, "type", &j_record_type);
        i_record_type = json_object_get_string(j_record_type);

        json_object_object_get_ex(j_record, "mx", &j_mx);
        i_mx = json_object_get_string(j_mx);

        if (strcmp(record_type, "MX") == 0)
        {
            if (strcmp(sub_domain, i_sub_domain) == 0 &&
                    strcmp(record_type, i_record_type) == 0 &&
                    strcmp(mx, i_mx) == 0)
            {
                size_t len = strlen(i_id);
                ret_record_id = (char *)malloc(len + 1);
                strcpy(ret_record_id, i_id);
                ret_record_id = i_id;
                break;
            }
        }
        else
        {
            if (strcmp(sub_domain, i_sub_domain) == 0 &&
                    strcmp(record_type, i_record_type) == 0)
            {
                size_t len = strlen(i_id);
                ret_record_id = (char *)malloc(len + 1);
                strcpy(ret_record_id, i_id);
                ret_record_id = i_id;
                break;
            }
        }
    }
    return ret_record_id;
}

static int
go_through_records(json_object *jobj,
                   const char *domain,
                   const char *domain_id,
                   const char *my_ip,
                   const char *sub_domain)
{
    json_object *j_records;
    json_object_object_get_ex(jobj, "records", &j_records);

    char *record_id = NULL;

    if (j_records)
    {
        record_id = get_record_id(j_records, sub_domain, "A", "0");
        if (record_id == NULL)
        {
            add_record(domain_id, sub_domain, "A", "默认", my_ip, "0", "600");
        }
        else
            free(record_id);
    }
    else
    {
        add_record(domain_id, sub_domain, "A", "默认", my_ip, "0", "600");
    }

    char mx_value[64];
    snprintf(mx_value, sizeof(mx_value), "%s.%s.", sub_domain, domain);
    if (j_records)
    {
        record_id = get_record_id(j_records, sub_domain, "MX", "5");
        if (record_id == NULL)
        {
            add_record(domain_id, sub_domain, "MX", "默认", mx_value, "5", "600");
        }
        free(record_id);
    }
    else
    {
        add_record(domain_id, sub_domain, "MX", "默认", mx_value, "5", "600");
    }

    snprintf(mx_value, sizeof(mx_value), "mail.%s.", domain);
    if (j_records)
    {
        record_id = get_record_id(j_records, sub_domain, "MX", "10");
        if (record_id == NULL)
        {
            add_record(domain_id, sub_domain, "MX", "默认", mx_value, "10", "600");
        }
        free(record_id);
    }
    else
    {
        add_record(domain_id, sub_domain, "MX", "默认", mx_value, "10", "600");
    }

    char spf[64];
    snprintf(spf, sizeof(spf), "v=spf1 ip4:%s/24 -all", my_ip);
    if (j_records)
    {
        record_id = get_record_id(j_records, sub_domain, "TXT", "0");
        if (record_id == NULL)
        {
            add_record(domain_id, sub_domain, "TXT", "默认", spf, "0", "600");
        }
        free(record_id);
    }
    else
    {
        add_record(domain_id, sub_domain, "TXT", "默认", spf, "0", "600");
    }
    return 0;
}

int check_and_update_records()
{
    pear_ddns_req_t req;
    pear_ddns_rsp_t rsp;
    pear_ddns_conf_t conf;
    req.conf = &conf;
    pear_ddns_init_conf(req.conf);

    int ret;
    char *domain_id = NULL, *my_ip = NULL;
    char sub_domain[16];
    char *domain = conf.domain;
    domain_id = get_domain_id();
    my_ip = get_external_ip();

    if (domain_id != NULL && my_ip != NULL)
    {
        ipv4_str_to_pear_str(sub_domain, my_ip);

        strncpy(req.api_url, "https://dnsapi.cn/Record.List", sizeof(req.api_url));
        snprintf(req.query_str, sizeof(req.query_str),
                 "login_token=%s&format=%s&domain_id=%s&sub_domain=%s",
                 req.conf->login_token,
                 req.conf->format,
                 domain_id,
                 sub_domain);
        printf("Check and update records: query_str: %s\n", req.query_str);

        pear_ddns_query(&req, &rsp);

        if(rsp.size)
        {
            json_object *jobj, *j_status, *j_code;
            jobj = json_tokener_parse(rsp.memory);
            printf("Check and update records: rsp: %s\n", rsp.memory);
            json_object_object_get_ex(jobj, "status", &j_status);
            json_object_object_get_ex(j_status, "code", &j_code);
            char *ret_code = json_object_get_string(j_code);

            go_through_records(jobj, domain, domain_id, my_ip, sub_domain);

            /* using json_object_object_get_ex(), we do not need to manually free these:
            json_object_put(jobj);
            json_object_put(j_status);
            json_object_put(j_code); */
            ret = 0;
        }
        else
        {
            ret = PEAR_DDNS_ERR_MESSAGE;
        }
    }
    else
    {
        ret = PEAR_DDNS_ERR_GET_CLIENT_INFO;
    }
    free(domain_id);
    free(my_ip);
    return ret;
}

