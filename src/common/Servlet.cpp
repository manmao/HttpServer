#include "Servlet.h"

Servlet::Servlet()
{
}

Servlet::~Servlet()
{

}

void Servlet::create(CHttpParser *parser,int sockfd)
{
    this->init();
    this->m_sockfd=sockfd;

    HttpResponse *rsp=new HttpResponse(sockfd);
    HttpRequest *req=(HttpRequest *)parser;

    if(parser->get_http_method( )== HTTP_UTIL_METHOD_GET)
    {
        this->get(req,rsp);
    }
    else if(parser->get_http_method( )== HTTP_UTIL_METHOD_POST)
    {
        parser->parse_form_body();
        this->post(req,rsp);
    }

    this->destroy();

}



