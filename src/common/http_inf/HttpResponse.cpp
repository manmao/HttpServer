#include "HttpResponse.h"

  HttpResponse::HttpResponse(int sockfd,string content_type){
       this->add_head="";
       this->m_sockfd=sockfd;
       this->content_type=content_type;
  }

  HttpResponse::~HttpResponse()
  {

  }

  void  HttpResponse::send_redirect(string redirect_url)
  {
      string res;
      CHttpResponseMaker::make_302_error(redirect_url,"",res);
      send(this->m_sockfd,res.c_str(),res.length(),0);
  }

   //·¢ËÍÊý¾Ý
   int  HttpResponse::send_reply(string content){
       string res;
       CHttpResponseMaker::make_string(content,res,this->content_type,this->add_head);
       send(this->m_sockfd,res.c_str(),res.length(),0);
       return 0;
   }

   int  HttpResponse::add_header(string head_name,string head_value)
   {
       this->add_head += head_name+": "+head_value+"\r\n";
       return 0;
   }

   void  HttpResponse::set_content_type(string type){
       this->content_type=type;
   }

