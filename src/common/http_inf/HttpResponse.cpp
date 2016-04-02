#include <stdio.h>
#include "HttpResponse.h"

 HttpResponse::HttpResponse(int sockfd){
       this->m_sockfd=sockfd;
       this->header_str="";
       this->content_type="text/html";
       this->charset="utf-8";
  }

#ifdef _USE_HTTP_SSL
   HttpResponse::HttpResponse(int sockfd,SSL* ssl){
       this->m_sockfd=sockfd;
       this->m_ssl=ssl;
       this->content_type="text/html";
       this->header_str="";
       this->charset="utf-8";

   }
#endif

   HttpResponse::~HttpResponse()
   {

   }

   //·¢ËÍÊý¾Ý
   int  HttpResponse::send_reply(string content){

       this->header_str=CHttpResponseMaker::map_to_headerstr(this->headers);

       string res;
       CHttpResponseMaker::make_string(content,res,this->content_type,this->charset,this->header_str);
    #ifdef _USE_HTTP_SSL
       int err=SSL_write (this->m_ssl, res.c_str(),res.length());
       CHK_SSL(err);
    #else
       send(this->m_sockfd,res.c_str(),res.length(),0);
    #endif
       return 0;
   }

   void  HttpResponse::send_redirect(string redirect_url)
   {
       string res;
       CHttpResponseMaker::make_302_error(redirect_url,"",res);
    #ifdef _USE_HTTP_SSL
        int err=SSL_write (this->m_ssl, res.c_str(),res.length());
        CHK_SSL(err);
    #else
       send(this->m_sockfd,res.c_str(),res.length(),0);
    #endif

   }


   int HttpResponse::set_header(string head_name,string head_value)
   {
        headers[head_name]=head_value;
        return 0;
   }

   void  HttpResponse::set_content_type(string type){
       this->content_type=type;
   }

   void HttpResponse::set_char_encode(string endcode){
        this->charset=endcode;
   }



