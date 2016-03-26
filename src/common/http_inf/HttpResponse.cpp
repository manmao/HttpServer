#include "HttpResponse.h"

  HttpResponse::HttpResponse(int sockfd,string content_type){
       this->add_head="";
       this->m_sockfd=sockfd;
       this->content_type=content_type;
  }

#ifdef _USE_HTTP_SSL
   HttpResponse::HttpResponse(int sockfd,SSL* ssl,string content_type){
       this->add_head="";
       this->m_sockfd=sockfd;
       this->content_type=content_type;
       this->m_ssl=ssl;
   }
#endif

   HttpResponse::~HttpResponse()
   {

   }
   //·¢ËÍÊý¾Ý
   int  HttpResponse::send_reply(string content){
       string res;
       CHttpResponseMaker::make_string(content,res,this->content_type,this->add_head);
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


   int  HttpResponse::add_header(string head_name,string head_value)
   {
       this->add_head += head_name+": "+head_value+"\r\n";
       return 0;
   }

   void  HttpResponse::set_content_type(string type){
       this->content_type=type;
   }

