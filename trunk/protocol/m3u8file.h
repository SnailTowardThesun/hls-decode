//
// Created by Hank 2016/3/15
//

#ifndef HLS_DECODE_M3U8FILE_H
#define HLS_DECODE_M3U8FILE_H

#include "../kernel/hls_decode_kernel.h"

typedef enum
{
	TS_FILE_URL = 0,
	COMMAND_LINE,
}CMDType;

class m3u8file
{
public:
	m3u8file();
	virtual ~m3u8file();
protected:
	 CMDType judge_cmd_type(string cmd);
public:
	bool parse_m3u8(string str, vector<string>& ts_file_list);
};

#endif
