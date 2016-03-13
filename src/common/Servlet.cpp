#include "Servlet.h"

Servlet::Servlet()
{
    this->req=NULL;
    this->rsp=NULL;
}

Servlet::~Servlet()
{

}
void Servlet::create(HttpRequest*parser,HttpResponse *rsp)
{
    this->rsp=rsp;
    this->req=parser;

    if(parser->get_http_method( )== HTTP_UTIL_METHOD_GET)
    {
        this->get(req,rsp);
    }
    else if(parser->get_http_method( )== HTTP_UTIL_METHOD_POST)
    {
        parser->parse_form_body();
        this->post(req,rsp);
    }

}



