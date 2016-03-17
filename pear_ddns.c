#include "pear_ddns_daemon_service.h"

/*
int test_base36()
{
    unsigned char *my_ip, pear_ip_str[16], back[16];
    my_ip = get_external_ip();
    ipv4_str_to_pear_str(pear_ip_str, my_ip);
    printf("pear ip: %s\n", pear_ip_str);
    pear_str_to_ipv4_str(back, pear_ip_str);
    printf("ip str: %s\n", back);
    int i;
    for (i = 0; i < 6; i++)
        printf("%u, ", pear_ip_str[i]);
    printf("\n", pear_ip_str[i]);
    free(my_ip);
    //printf("indexof: %u; decoding table: %u\n", indexof('c'), base32_decoding['c']);
}

int test_dns_records()
{
    char *my_ip, *domain_id, *record_id;
    my_ip = get_external_ip();
    printf("My external IP: %s\n", my_ip);
    domain_id = get_domain_id();
    printf("My domain_id: %s\n", domain_id);
    //record_id = get_record_id(domain_id, "mail");
    //printf("My record_id: %s\n", record_id);
    //add_record("", "", );

    free(my_ip);
    free(domain_id);
    //free(record_id);
}
*/

int main()
{


    /*
        char *my_ip, *domain_id, *record_id;
        my_ip = get_external_ip();
        printf("My external IP: %s\n", my_ip);
        domain_id = get_domain_id();
        printf("My domain_id: %s\n", domain_id);
        record_id = get_record_id(domain_id,"A");
        printf("My record_id: %s\n", record_id);

        free(my_ip);
        free(domain_id);
        free(record_id);
    */
    //test_base36();
    //test_dns_records();
    pear_ddns_main();
    return 0;
}
