#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>
#include "pear_ddns_conf.h"

int pear_ddns_init_conf(pear_ddns_conf_t *conf)
{
    FILE *fp;
    fp = fopen("./pear_ddns.conf", "rb");
    if (!fp)
        fp = fopen("/etc/pear/pear_ddns.conf", "rb");
    if (!fp)
        fp = fopen("/etc/pear_ddns/pear_ddns.conf", "rb");
    if (!fp)
        fp = fopen("/etc/pear_ddns.conf", "rb");
    if (!fp)
    {
        strncpy(conf->login_token, "12079,84f7755f2464126c6757dd10eac202e5", sizeof(conf->login_token));
        strncpy(conf->format, "json", sizeof(conf->format));
        strncpy(conf->domain, "supear.cn", sizeof(conf->domain));
        strncpy(conf->user_agent, "Pear DDNS Client/1.0.0 (service@pear.hk)", sizeof(conf->user_agent));
    }
    else
    {
        fseek(fp, 0, SEEK_END);
        long fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char *conf_buf = malloc(fsize + 1);
        fread(conf_buf, fsize, 1, fp);
        conf_buf[fsize] = 0;
        fclose(fp);

        json_object *j_conf, *j_value;
        j_conf = json_tokener_parse(conf_buf);

        json_object_object_get_ex(j_conf, "login_token", &j_value);
        strncpy(conf->login_token, json_object_get_string(j_value), sizeof(conf->login_token));
        json_object_object_get_ex(j_conf, "format", &j_value);
        strncpy(conf->format, json_object_get_string(j_value), sizeof(conf->login_token));
        json_object_object_get_ex(j_conf, "domain", &j_value);
        strncpy(conf->domain, json_object_get_string(j_value), sizeof(conf->login_token));
        json_object_object_get_ex(j_conf, "user_agent", &j_value);
        strncpy(conf->user_agent, json_object_get_string(j_value), sizeof(conf->login_token));

        free(conf_buf);
    }
}
