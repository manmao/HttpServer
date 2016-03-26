#ifndef __HTTPRESPONSE_H_
#define __HTTPRESPONSE_H_

#include <string>
#include <sys/socket.h>

using std::string;

#include "http/http_util.h"

class HttpResponse{
public:
    HttpResponse(int sockfd,string content_type="text/html");
    virtual int send_reply(string content);
    virtual int add_header(string head_name,string head_value);
    virtual void set_content_type(string type);
    virtual void send_redirect(string redirect_url);
    virtual ~HttpResponse();
private:
    int m_sockfd;
    string add_head;
    string content_type;
};


#endif
