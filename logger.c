#ifdef STANDARD
/* STANDARD is defined. Don't use any MySQL functions */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __WIN__
typedef unsigned __int64 ulonglong;     /* Microsoft's 64 bit types */
typedef __int64 longlong;
#else
typedef unsigned long long ulonglong;
typedef long long longlong;
#endif /*__WIN__*/
#else
#include <string.h>
#include <my_global.h>
#include <my_sys.h>
#endif
#include <mysql.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <curl/curl.h>
#include <curl/easy.h>

#define SERVER_ADDRESS "http://127.0.0.1:3000"

// static pthread_mutex_t LOCK_hostname;


struct memory {
  char* string;
  size_t size;
};

static CURL* pch = 0;
static struct memory chunk = {0};

static size_t
writeFunc(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct memory *mem = (struct memory *)userp;
 
  char *ptr = realloc(mem->string, mem->size + realsize + 1);
  if (!ptr) {
    // Out of memory
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->string = ptr;
  memcpy(&(mem->string[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->string[mem->size] = 0;
 
  return realsize;
}

my_bool udfLog_init(UDF_INIT *initid,
                    UDF_ARGS *args,
                    char *message) {
    chunk.string = malloc(1);
    chunk.size = 0;

    pch = curl_easy_init();
    curl_easy_setopt(pch, CURLOPT_URL, SERVER_ADDRESS);
    curl_easy_setopt(pch, CURLOPT_WRITEDATA, (void*)&chunk);
    curl_easy_setopt(pch, CURLOPT_WRITEFUNCTION, writeFunc);

    return 0;
}

void udfLog_deinit(UDF_INIT *initid) {
  if (pch) {
    curl_easy_cleanup(pch);
    pch = 0;
  }
  if (chunk.string) {
    free(chunk.string);
    chunk.string = 0;
  }
}

char *udfLog(UDF_INIT *initid, UDF_ARGS *args,
          char *result, unsigned long *length,
          char *is_null, char *error) {
    // Add severity level
    // Use dynamic buffer
    const char* str = args->args[0];

    // Response: msg=<MSG>\0
    char* buffer = malloc(4 + strlen(str) + 1);
    sprintf(buffer, "msg=");
    strcpy(buffer + 4, str);

    curl_easy_setopt(pch, CURLOPT_POSTFIELDS, buffer);
    curl_easy_perform(pch);
    strncpy(result, chunk.string, chunk.size);
    *length = chunk.size;

    return result;
} 
