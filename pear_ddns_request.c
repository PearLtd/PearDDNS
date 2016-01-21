#include "pear_ddns_request.h"
#include <string.h>

static unsigned int
WriteMemoryCallback(void *contents, unsigned int size, unsigned int nmemb, void *userp);

static void setcommonopt(CURL *curl, const char *address, 
                struct curl_slist *headerlist,
                struct curl_httppost *formpost, 
                void * chunk,
                const char *agent
                );
static int fillinfo(domain_info_t *info, const result_t *result);
static void free_result(result_t *result);


int update_record(domain_info_t *info)
{
    //组装基本参数
    CURL *curl;
    CURLcode res;
    result_t result = {0};
    struct MemoryStruct chunk = {0};
    struct curl_httppost *formpost=NULL;
    struct curl_httppost *lastptr=NULL;
    struct curl_slist *headerlist=NULL;
    static const char buf[] = "Expect:";
    char buffer[10];
    const char * address = "https://dnsapi.cn/Record.Ddns";
    
    curl_global_init(CURL_GLOBAL_ALL);

    if ( setcommonparam(&formpost, &lastptr, info) )
    {
        return -1;
    }

    //addition
    sprintf(buffer, "%d", info->domain_id);
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "domain_id",
                 CURLFORM_COPYCONTENTS, buffer,
                 CURLFORM_END);
                 
    sprintf(buffer, "%d", info->record_id);
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "record_id",
                 CURLFORM_COPYCONTENTS, buffer,
                 CURLFORM_END);
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "record_type",
                 CURLFORM_COPYCONTENTS, info->record_type,
                 CURLFORM_END);
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "record_line",
                 CURLFORM_COPYCONTENTS, info->line_type,
                 CURLFORM_END);             
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "sub_domain",
                 CURLFORM_COPYCONTENTS, info->sub_domain_name,
                 CURLFORM_END);
                 
    curl = curl_easy_init();
    /* initalize custom header list (stating that Expect: 100-continue is not
     wanted */
    headerlist = curl_slist_append(headerlist, buf);
    if(curl) {
    
        setcommonopt(curl, address, headerlist, formpost, (void *)&chunk, "ningxuan1@gmail.com/raspddns v0.1a");
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);

        /* then cleanup the formpost chain */
        curl_formfree(formpost);
        /* free slist */
        curl_slist_free_all (headerlist);
        
        if ( parseresult(chunk.memory, chunk.size, &result) )
        {
            printf("parse memory error!\n");
            return -1;
        }
        
        if( !chunk.memory )
        {
            free(chunk.memory);
            chunk.memory = NULL;
            chunk.size = 0;
        }
        
        
        free_result(&result);
        return res;
    }
    
    return -1;
}
int getdomainid(domain_info_t *info)
{
    //组装基本参数
    CURL *curl;
    CURLcode res;
    result_t result = {0};
    struct MemoryStruct chunk = {0};
    struct curl_httppost *formpost=NULL;
    struct curl_httppost *lastptr=NULL;
    struct curl_slist *headerlist=NULL;
    static const char buf[] = "Expect:";

    const char * address = "https://dnsapi.cn/Domain.List";
    curl_global_init(CURL_GLOBAL_ALL);

    if ( setcommonparam(&formpost, &lastptr, info) )
    {
        return -1;
    }

    curl = curl_easy_init();
    /* initalize custom header list (stating that Expect: 100-continue is not
     wanted */
    headerlist = curl_slist_append(headerlist, buf);
    if(curl) {
    
        setcommonopt(curl, address, headerlist, formpost, (void *)&chunk, "ningxuan1@gmail.com/raspddns v0.1a");
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);

        /* then cleanup the formpost chain */
        curl_formfree(formpost);
        /* free slist */
        curl_slist_free_all (headerlist);
        
        //printf("%s\n", chunk.memory);
        
        if ( parseresult(chunk.memory, chunk.size, &result) )
        {
            printf("parse memory error!\n");
            return -1;
        }
        
        if( !chunk.memory )
        {
            free(chunk.memory);
            chunk.memory = NULL;
            chunk.size = 0;
        }
        
        if( fillinfo(info, &result) )
        {
            free_result(&result);
            return -1;
        }
        
        free_result(&result);
        return res;
    }
    
    return -1;
}

int getrecordid(domain_info_t *info)
{
    CURL *curl;
    CURLcode res;
    result_t result = {0};
    struct MemoryStruct chunk = {0};
    struct curl_httppost *formpost=NULL;
    struct curl_httppost *lastptr=NULL;
    struct curl_slist *headerlist=NULL;
    static const char buf[] = "Expect:";
    char buffer[10] = {'\0'};
    const char * address = "https://dnsapi.cn/Record.List";
    
    curl_global_init(CURL_GLOBAL_ALL);

    if ( !info || info->domain_id == 0 )
    {
        return -1;
    }
    
    if ( setcommonparam(&formpost, &lastptr, info) )
    {
        return -1;
    }
    //addition param
    //itoa(info->domain_id, buffer, 10);
    sprintf(buffer, "%d", info->domain_id);
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "domain_id",
                 CURLFORM_COPYCONTENTS, buffer,
                 CURLFORM_END);
                 
    curl = curl_easy_init();
    /* initalize custom header list (stating that Expect: 100-continue is not
     wanted */
    headerlist = curl_slist_append(headerlist, buf);
    if(curl) {
    
        setcommonopt(curl, address, headerlist, formpost, (void *)&chunk, "ningxuan1@gmail.com/raspddns v0.1a");
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);

        /* then cleanup the formpost chain */
        curl_formfree(formpost);
        /* free slist */
        curl_slist_free_all (headerlist);
        
        //printf("%s\n", chunk.memory);
        
        if ( parseresult(chunk.memory, chunk.size, &result) )
        {
            printf("parse memory error!\n");
            return -1;
        }
        
        
        
        if( !chunk.memory )
        {
            free(chunk.memory);
            chunk.memory = NULL;
            chunk.size = 0;
        }
        
        if( fillinfo(info, &result) )
        {
            free_result(&result);
            return -1;
        }
        
        
        free_result(&result);
        return res;
    }
    
    return -1;
}


int setcommonparam(struct curl_httppost **formpost, struct curl_httppost **lastptr, const domain_info_t *info)
{
    if ( !formpost || !lastptr || !info )
    {
        return -1;
    }
    
    curl_formadd(formpost,
                 lastptr,
                 CURLFORM_COPYNAME, "login_email",
                 CURLFORM_COPYCONTENTS, info->username,
                 CURLFORM_END);
                
    curl_formadd(formpost,
                 lastptr,
                 CURLFORM_COPYNAME, "login_password",
                 CURLFORM_COPYCONTENTS, info->password,
                 CURLFORM_END);
    
    curl_formadd(formpost,
                 lastptr,
                 CURLFORM_COPYNAME, "format",
                 CURLFORM_COPYCONTENTS, "xml",
                 CURLFORM_END);
                 
    return 0;
}

static void 
setcommonopt(CURL *curl, const char *address, 
                struct curl_slist *headerlist,
                struct curl_httppost *formpost, 
                void * chunk,
                const char *agent
                )
{
    if ( !curl || !address || !chunk || !agent || !headerlist)
    {
        return;
    }
    /* what URL that receives this POST */
    curl_easy_setopt(curl, CURLOPT_URL, address);
    /* disable 100-continue header if explicitly requested */
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)chunk);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, agent);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
}


int parseresult(const char *src, int length, result_t *result)
{
    xmlDocPtr doc; /* the resulting document tree */
    xmlNodePtr pRoot;
    xmlNodePtr pCode;
    char buffer[255];
    
    if ( src == NULL || result == NULL )
    {
        return -1;
    }
    /*
     * The document being in memory, it have no base per RFC 2396,
     * and the "noname.xml" argument will serve as its base.
     */
    doc = xmlParseMemory(src, length);
    if (doc == NULL) 
    {
        printf("parse doc error\n");
        return -1;
    }
    
    pRoot = xmlDocGetRootElement(doc);
    
    if (pRoot == NULL)
    {
        printf("get root node error\n");
        return -1;
    }
    
    if ( (pCode = _xmlGetContentCptr(pRoot, "status")) == NULL || _xmlGetContentText(pCode, "code", buffer, 255)  )
    {
        printf("_xmlGetContentText error");
        xmlFreeDoc(doc);
        return -1;
    }
    
    result->code = atoi(buffer);
    
    if(result->code != 1)
    {
        return -1;
    }
    //
    if ( _fillreslult(pRoot, result) )
    {
        printf("_fillreslult() error\n");
        xmlFreeDoc(doc);
        return -1;
    }
    
    xmlFreeDoc(doc);

    return 0;
}

static unsigned int
WriteMemoryCallback(void *contents, unsigned int size, unsigned int nmemb, void *userp)
{
  unsigned int realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  mem->memory = realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

static int fillinfo(domain_info_t *info, const result_t *result)
{
    unsigned int i = 0;
    if( !info || !result )
    {
        return -1;
    }
    
    for( i = 0; i < result->domain_num; ++i )
    {
        if( strcmp(result->domain[i].name, info->domain_name ) == 0 && result->domain[i].status == 1)
        {
            info->domain_id = result->domain[i].domain_id;
            break;
        }
    }
    
    if( i == result->domain_num && i != 0 )
    {
        printf("error: 域名记录%s没找到或状态错误，请查证\n", info->domain_name);
        return -1;
    }
    
    for( i = 0; i < result->sub_domain_num; ++i )
    {
        if( strcmp(result->sub_domain[i].name, info->sub_domain_name ) == 0 && result->sub_domain[i].status == 1)
        {
            info->record_id = result->sub_domain[i].record_id;
            strncpy(info->record, result->sub_domain[i].value, sizeof(info->record));
            break;
        }
    }
    
    if( i == result->sub_domain_num && i != 0 )
    {
        printf("error: 子域名记录%s没找到或状态错误，请查证\n", info->sub_domain_name);
        return -1;
    }
    
    return 0;
}
static void free_result(result_t *result)
{
    if( !result )
    {
        return;
    }
    
    result->domain_num = 0;
    result->sub_domain_num = 0;
    
    if( result->domain )
    {
        free(result->domain);
        result->domain = NULL;
    }
    
    if( result->sub_domain )
    {
        free(result->sub_domain);
        result->sub_domain = NULL;
    }
}
