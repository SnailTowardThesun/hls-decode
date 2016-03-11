//
//Created by Hank 2016/3/11
//
#ifndef HLS_DECODE_LOG_H
#define HLS_DECODE_LOG_H
#include "../kernel/hls_decode_kernel.h"

class HlsLog
{
public:
	HlsLog();
	virtual ~HlsLog();
private:
	static HlsLog* instance_;
public:
	static HlsLog* getInstance();
public:
	int log(string level_name,string tag,string log_message);
};

#endif
