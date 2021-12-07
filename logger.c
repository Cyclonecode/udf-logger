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

static CURL* pch = 0;

my_bool Log_init(UDF_INIT *initid,
                               UDF_ARGS *args,
                               char *message) {

    pch = curl_easy_init();
    curl_easy_setopt(pch, CURLOPT_URL, SERVER_ADDRESS);

    return 0;
}

void Log_deinit(UDF_INIT *initid) {
  if (pch) {
    curl_easy_cleanup(pch);
    pch = 0;
  }
}

longlong Log(UDF_INIT *initid,
                           UDF_ARGS *args,
                           char *is_null,
                           char *error) {
    // Add severity level
    // Allocate dynamic buffer
    char buffer[1024];
    sprintf(buffer, "%s", args->args[0]);
    curl_easy_setopt(pch, CURLOPT_POSTFIELDS, buffer);
    curl_easy_perform(pch);
    return 0;
} 
