#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "inet_sockets.h"
#include "http/http_util.h"

#include <iostream>

int main(int argc,char *argv[]){
    int fd;
    char buff[4096];
    fd=inetListen("8080",SOCK_STREAM, NULL);
    printf("server waiting: \n");
    int afd=inetAccept(fd,"8080",NULL);
    CHttpParser *parser=NULL;
    CHttpResponseMaker maker;

    while(1){

      if(afd>0){

          int size=read(afd,buff,4096);

          if(size >0){

             parser=new CHttpParser(buff,4096);
             std::cout<< parser->get_param_string()<<std::endl;
             std::cout<< parser->get_uri()<<std::endl;

             string res;
             string content="i am server";
             string type="text/html";
             string head="";
             maker.make_string(content,res,type,head);

             write(afd,res.c_str(),4069);

          }
          delete parser;
      }
    }
    return 0;
}