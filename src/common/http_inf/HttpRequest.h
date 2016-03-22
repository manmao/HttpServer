#ifndef __HTTPREQUEST_H_
#define __HTTPREQUEST_H_

#include "http/http_util.h"

class HttpRequest : public CHttpParser
{
public:
    HttpRequest(const char* szHttpReq = NULL,
        int nDataLen = 0,
        int nExtraParamType = HTTP_UTIL_PARAM_ALL):CHttpParser(szHttpReq,nDataLen,nExtraParamType){}
    ~HttpRequest(){}
private:

};

#endif

