#ifndef _HTTP_SVR_BASE_CODEC_H_
#define _HTTP_SVR_BASE_CODEC_H_

#include <stdio.h>
#include <string>
#include "http_arg_define.h"
#include "http/http_req.h"

using namespace std;
using namespace httpsvr;


/**
* @author feimat@baidu.com
*
*
* <pre>
* Http ����Э�������
*
* </pre>
**/

/*
	HTTPЭ��ļ��麯��
	0       ����Ҫ��������
	-1      ʧ��
*/
int http_complete_func(const char* p_data, unsigned int data_len);


class ServerHttpCodec
{
public:

    // �Ƚ��Http Header
    int decode_head(const string &buffer);

    // ����http://127.0.0.1/download?name=xxxx�е� download ���������ǳ�Ϊ�����
    string get_cmd_string();
    // ����http://127.0.0.1/download?name=xxxx�е� name ����
    string get_argu(const string &argu);

    //**************Э�����ӣ������ļ�********************
    //http://127.0.0.1/download?fname=xxxx
    int decode_download_file_req(string &fname);
    int encode_download_file_rsp(const int &retcode,const string &fdata,string &buffer);

	//**************Э�����ӣ��ϴ��ļ�********************
	//http://127.0.0.1/upload?fname=xxxx
	int decode_upload_file_req(string &fname,string &fdata);
	int encode_upload_file_rsp(const int &retcode,string &buffer);

    //**************Э�����ӣ�ʹ��sendfile��ǿ���ܵ������ļ�********************
    //http://127.0.0.1/download?fname=xxxx
    int decode_sendfile_download_file_req(string &fname,long &offset,unsigned &len);
    // ֻ����ͷ�����ļ�������sendfile����
    int encode_sendfile_download_file_rsp(const int &retcode,unsigned filelen,string &buffer);

    //**************Э�����ӣ��첽ʵ���������񻯵�����********************
    /*
        http://127.0.0.1/echo?text=xxxx

        ��̨��������Э�鶼�������ĸ�ʽ

        FlowNo:111111\r\n\r\n
        Argument in Http Body

    */
    int decode_async_proxy_req(string &strWen);     // client req
    int encode_async_proxy_rsp(const string &strRspWen,string &buffer); // client rsp

    //**************Э�����ӣ��첽ʵ���������񻯵�����********************
    /*
        http://127.0.0.1/upload?fname=xxxx
        BODY

        ע�⣬����ʹ��string��Ϊbody����body���ܰ�������������
        ���Զ���body�ĸ�ֵӦ���������ģ� body.assign(szData,szLen)
        ��body��������Ӧ����memcpy(szData,body.c_str(),body.length())
    */
    int decode_web_upload_req(string &fname,string &body);
    int encode_web_upload_rsp(const string &data , string &buffer);

    int encode_back_proto(int ret,unsigned flow,const string &strRsp,string &buffer);

private:
    CHttpReq http_req;
};

#endif
