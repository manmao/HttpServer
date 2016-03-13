#ifndef __C_LOG_H
#define __C_LOG_H

/*
����:
       1.  Log��

Created by Song, 2003-01
Change list:

*/


#include <stdio.h>

class CLog
{
public:
	CLog(const char *log_base_path,
		int max_log_size,
		int max_log_num = 10,
		int * log_level_ptr = NULL);

	virtual ~CLog();

public:
	int open();
	int close();
	const char * get_error() const { return _error_text; }

public:
	enum CLogLevel
	{
		NO_LOG = 1,  // ���е�log����д
		ERROR_LOG = 2,  // ֻд����log
		NORMAL_LOG = 3,  // д��ͨlog�ʹ���log
		DEBUG_LOG = 4  // д��ͨlog������log�͵���log
	};

	// ȡ��log�ļ���
	inline int get_log_level() const
	{return _log_level_ptr != NULL ? (*_log_level_ptr):DEBUG_LOG;}

	inline void set_log_level_ptr(int * log_level_ptr)
	{_log_level_ptr = log_level_ptr;}

	static char * get_log_level_str(int log_level, char * str);
	static bool log_level_is_valid(int log_level);

public:
	// �����ʱ��ʾ���̱��
	void set_process_no(int proc_no) { _proc_no = proc_no; }
	// �����ʱ��ʾ���̱��
	int get_process_no() { return _proc_no; }

	// ����log��ǰ��� ERROR:
	int error_log(const char *fmt, ...);

	// д��ͨlog
	int write_log(const char *fmt, ...);

	// ����log��ǰ���DEBUG:
	int debug_log(const char *fmt, ...);

	// ��16������ʽ��ӡbuf
	int print_hex(const void *buf, size_t len);

	// ������ʹ��
	int puts(const char * str);

	// ÿ��һ����log
	int day_log(const char * day_log_path, const char *fmt, ...);

	// ��day_log���ƣ���ǰ�治дʱ��
	int day_log_raw(const char * day_log_path, const char *fmt, ...);

private:

	int shilft_files();

protected:
	static const int LOG_SHIFT_COUNT  = 32;

	FILE * _fp;

	char _log_base_path[256];
	int _max_log_size;
	int _max_log_num;

	// log�ļ���ָ�룬ͨ��ָ�����ڴ�
	// �����ָ��ΪNULLʱ��ȡĬ��NORMAL_LOG
	volatile int * _log_level_ptr;

	char _log_file_name[256];

	int _write_count;
	int _proc_no;

	char _log_buf[16384];
	char _error_text[256];
};

extern class CLog * g_pLog;

#ifdef DEBUG_LOG_ARGS
    #define trpc_write_log printf
    #define trpc_error_log printf
    #define trpc_debug_log printf
#else
    #define trpc_write_log g_pLog->write_log
    #define trpc_error_log g_pLog->error_log
    #define trpc_debug_log g_pLog->debug_log
#endif

#define trpc_day_log g_pLog->day_log

#define trpc_set_log_level_ptr g_pLog->set_log_level_ptr

#endif
