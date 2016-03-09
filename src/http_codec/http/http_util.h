/***************************************************************************
 *
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file http_util.h
 * @hemingzhe02@baidu.com
 * @date 2013/03/19
 * @brief
 *
 **/

#ifndef  __HTTP_UTIL_H_
#define  __HTTP_UTIL_H_

#include <assert.h>
#include <string>
#include <map>
#include <list>
#include <iterator>
using namespace std;

enum HttpMethodType{
	HTTP_UTIL_METHOD_NONE,
	HTTP_UTIL_METHOD_GET,
	HTTP_UTIL_METHOD_POST,
	HTTP_UTIL_METHOD_RESP
};


enum HttpParamType{
	HTTP_UTIL_PARAM_ALL,			//�������͵Ĳ���
	HTTP_UTIL_PARAM_HEADPARAM,		//ֻ��ȡHEADPARAM
	HTTP_UTIL_PARAM_CONTENT			//ֻ��ȡCONTENT
};

/////////////////////////////////////////////////////////////////////////////
//����http��Ӧ��
class CHttpResponseMaker
{
public:
	CHttpResponseMaker(){}
	virtual ~CHttpResponseMaker(){}
public:
	static int		make(const char* szContent, int nContentLen,int totalContentLen ,char* szBuffer, int nBufferSize, const char* content_type);
    static int		make_headers(int totalContentLen ,char* szBuffer,const char* content_type);
    static void		make_string(const string& strContent, string& strResp, string& content_type, string& add_head);
	static void		make_404_error(string& strResp);
    static void		make_400_error(string& strResp);
	static void		make_302_error(const string& strLocation, const string& strMoveTo, string& strResp);
protected:
private:
};

/////////////////////////////////////////////////////////////////////////////
//��������ַ���
class CHttpParamStringMaker
{
public:
	CHttpParamStringMaker(){}
	virtual ~CHttpParamStringMaker(){}

public:
	void	add_param(const string& strKey, const string& strValue);
	void	add_param(const string& strKey, int nValue);
	void	set_paramlines(const string& strLines);
	string  get_params();
protected:
private:
	typedef struct
	{
		string strKey;
		string strValue;
	}HttpGetMakerParam;
	list<HttpGetMakerParam>	m_params;
	string m_strParamLines;
};

/////////////////////////////////////////////////////////////////////////////
//���������
class CHttpMaker	: public CHttpParamStringMaker
{
public:
	CHttpMaker(){}
	virtual ~CHttpMaker(){}

public:
	int		make(const string& strHost, unsigned short nPort, const string& strUri, char* szBuffer, int nBufferSize);
	void	make_string(const string& strHost, unsigned short nPort, const string& strUri, string& strRequest);

public:
	int		GET_make(const string& strHost, unsigned short nPort, const string& strUri, char* szBuffer, int nBufferSize);
	void	GET_make_string(const string& strHost, unsigned short nPort, const string& strUri, string& strRequest);

	int		POST_make(const string& strHost, unsigned short nPort, const string& strUri, char* szBuffer, int nBufferSize);
	void	POST_make_string(const string& strHost, unsigned short nPort, const string& strUri, string& strRequest);
};

/////////////////////////////////////////////////////////////////////////////
//����GET�����
class CHttpGetMaker	: public CHttpMaker
{
public:
	CHttpGetMaker(){}
	virtual ~CHttpGetMaker(){}

public:
	int make(const string& strHost, unsigned short nPort, const string& strUri, char* szBuffer, int nBufferSize)
	{
		return GET_make(strHost, nPort, strUri, szBuffer, nBufferSize);
	}
	void make_string(const string& strHost, unsigned short nPort, const string& strUri, string& strRequest)
	{
		GET_make_string(strHost, nPort, strUri, strRequest);
	}
};

/////////////////////////////////////////////////////////////////////////////
//����POST�����
class CHttpPostMaker	: public CHttpMaker
{
public:
	CHttpPostMaker(){}
	virtual ~CHttpPostMaker(){}

public:
	int make(const string& strHost, unsigned short nPort, const string& strUri, char* szBuffer, int nBufferSize)
	{
		return POST_make(strHost, nPort, strUri, szBuffer, nBufferSize);
	}
	void make_string(const string& strHost, unsigned short nPort, const string& strUri, string& strRequest)
	{
		POST_make_string(strHost, nPort, strUri, strRequest);
	}
};

/////////////////////////////////////////////////////////////////////////////
//����Http�����ݰ����ȣ�֧��GET\POST\RESP
class CHttpLengthAnaly
{
public:
	CHttpLengthAnaly(){}
	virtual ~CHttpLengthAnaly(){}
public:
	//��ȡ���ȣ��������ݷ���-1�����ݲ���������0��������ȫ����>0
	static int		get_length(const char* szData, int nDataLen);

	//��ȡ���ȣ��������ݷ���-1�����ݲ���������0��������ȫ����>0
	//nContentPos�������������λ��
	static int		get_length_ex(const char* szData, int nDataLen, int& nContentPos, int& nContentLen);
};

/////////////////////////////////////////////////////////////////////////////
//��������
//˵�������ڽ�����aaa=123&bbb=321&ccc=888֮����ַ���
class CHttpParamParser
{
public:
	CHttpParamParser(const char* szHttpParam = NULL, int nLen = 0);
	virtual ~CHttpParamParser(){}

public:
	bool parse(const char* szHttpParam, int nLen);
	string get_param(const char* szKey);
	int get_param_int(const char* szKey);

private:
	map<string, string> m_mapValues;
};

/////////////////////////////////////////////////////////////////////////////
//�����ַ�������
class CInsertTempZero
{
public:
	CInsertTempZero(char* pSrc)
	{
		m_szOld = *pSrc;
		m_pSrc = pSrc;
		*m_pSrc = 0;
	}
	virtual ~CInsertTempZero()
	{
		*m_pSrc = m_szOld;
	}
protected:
private:
	char* m_pSrc;
	char  m_szOld;
};

/////////////////////////////////////////////////////////////////////////////
//HTTPЭ�������
class CHttpParser
{
public:
	CHttpParser(const char* szHttpReq = NULL, int nDataLen = 0, int nExtraParamType = HTTP_UTIL_PARAM_ALL);

	virtual ~CHttpParser(){}

public:

    static string UrlDecode(const std::string& szToDecode);

	int parse_head(const char* szHttpReq, int nDataLen, int nExtraParamType = HTTP_UTIL_PARAM_ALL);
    void parse_form_body();

    int getContentLen();

	//��ȡ����ͷ�����ֵ
	string get_head_field(const string& strFieldName);

	//��ȡcookie
	string get_cookie();

	//��ȡ���������ֵ
	string get_param(const char* szKey);
	int get_param_int(const char* szKey);

	//��ȡ�����ַ������� "aaa=123&bbb=321&ccc=888"
	string		get_param_string();

	//��ȡuri���ݣ��� "/update/mytest"
	string		get_uri();

	//��ȡobject������http://127/aa/bb/cc?fff=999 �е�cc
	string		get_object();

	//��ȡ����
	int			get_http_method();

protected:
	//����ͷ��
	bool parseField(const char* szHttpReq, int nTotalLen);
	//������һ�У���ȡmethod��headparam
	bool parseFirstLine();
	//����method
	bool parseMethod(const char* szFirstLine, const char* szMethod, int nMethodType);

private:
	map<string, string>	m_mapFields;
	char				m_szFirstLine[4096];
	const char*			m_pszContent;
	int					m_nContentLen;
	int					m_nHttpMethod;

	const char*			m_pszUri;
	const char*			m_pszActParam;

	CHttpParamParser	m_paramParser;		//����������

	int					m_nExtraParamType;
};

#endif

