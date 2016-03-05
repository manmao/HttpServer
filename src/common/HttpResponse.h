#ifndef __HTTPRESPONSE_H_
#define __HTTPRESPONSE_H_

#include <string>
using std::string;

#include "http/http_util.h"

class HttpResponse{
public:
    HttpResponse(int sockfd,string content_type="text/html"){
        this->add_head="";
        this->m_sockfd=sockfd;
        this->content_type=content_type;
    }

    int send_rsp(string content){
        string res;
        CHttpResponseMaker::make_string(content,res,this->content_type,this->add_head);
        write(this->m_sockfd,res.c_str(),res.length());
        return 0;
    }

    int add_header(string head_name,string head_value)
    {
        this->add_head += head_name+":"+head_value+"\r\n";
        return 0;
    }

    void set_contenttype(string type){
        this->content_type=type;
    }

private:
    int m_sockfd;
    string add_head;
    string content_type;

};


#endif
