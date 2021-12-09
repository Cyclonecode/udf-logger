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
#define MAX_RESPONSE_SIZE 1024*4

struct memory {
  char* string;
  size_t size;
};

static void* _realloc(void* ptr, size_t size) {
  if (ptr) {
    return realloc(ptr, size);
  } else {
    return malloc(size);
  }
}

static size_t
writeFunc(void *ptr, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct memory *res = (struct memory*)userp;

  res->string = (char *)_realloc(res->string, res->size + realsize + 1);
  if (res->string) {
    memcpy(&(res->string[res->size]), ptr, realsize);
    res->size += realsize;
    res->string[res->size] = 0;
  }
  return realsize;
}

my_bool udfLog_init(UDF_INIT *initid,
                    UDF_ARGS *args,
                    char *message) {
    struct memory* chunk;
    if (args->arg_count != 1) {
      strncpy(message, "One argument must be supplied", MYSQL_ERRMSG_SIZE);
      return 1;
    }
    args->arg_type[0] = STRING_RESULT;
    initid->max_length = MAX_RESPONSE_SIZE;

    chunk = (struct memory*)malloc(sizeof(struct memory));
    initid->ptr = (char*)chunk;

    return 0;
}

void udfLog_deinit(UDF_INIT *initid) {
   struct memory* chunk = (struct memory*)initid->ptr;
   if (chunk) {
     if (chunk->string) {
       free(chunk->string);
     }
     free(chunk);
   }
}

char *udfLog(UDF_INIT *initid, UDF_ARGS *args,
          char *result, unsigned long *length,
          char *is_null, char *error) {
    CURL* pch = curl_easy_init();
    struct memory* chunk = (struct memory*)initid->ptr;
    curl_easy_setopt(pch, CURLOPT_URL, SERVER_ADDRESS);
    curl_easy_setopt(pch, CURLOPT_WRITEDATA, (void*)chunk);
    curl_easy_setopt(pch, CURLOPT_WRITEFUNCTION, writeFunc);


    chunk->size = 0;
    chunk->string = NULL;

    // TODO: Add severity level

    curl_easy_setopt(pch, CURLOPT_POSTFIELDS, (char*)args->args[0]);
    CURLcode ret = curl_easy_perform(pch);

    if (ret) {
      fprintf(stderr, "error\n");
      if (chunk->string) {
        strcpy(chunk->string, ""); 
      }
      chunk->size = 0;
    }
    curl_easy_cleanup(pch);
    *length = chunk->size;

    return (char*)chunk->string;
} 
