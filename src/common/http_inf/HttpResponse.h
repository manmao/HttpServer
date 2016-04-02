#ifndef __HTTPRESPONSE_H_
#define __HTTPRESPONSE_H_

#include <string>
#include <sys/socket.h>

using std::string;

#include "http/http_util.h"

#ifdef _USE_HTTP_SSL
#include "config_ssl.h"
#endif

class HttpResponse{
public:
    HttpResponse(int sockfd);
#ifdef _USE_HTTP_SSL
    HttpResponse(int sockfd,SSL* ssl);
#endif
    virtual int  send_reply(string content);
    virtual int  set_header(string head_name,string head_value);
    virtual void set_content_type(string type);
    virtual void send_redirect(string redirect_url);
    virtual void set_char_encode(string endcode);
    virtual ~HttpResponse();
private:
    int m_sockfd;
    string header_str;
    string content_type;
    map<string,string> headers;
    string charset;

#ifdef _USE_HTTP_SSL
    SSL* m_ssl;
#endif

};


#endif
