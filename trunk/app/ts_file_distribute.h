// 
// Created by Hank 2016/3/11
//

#ifndef TS_FILE_DISTRIBUTE_H
#define TS_FILE_DISTRIBUTE_H

#include "../kernel/hls_decode_kernel.h"
#include <fstream>

static const int32_t MAX_LENGTH_OF_READ_MESSAGE_FROM_FILE = 1024;

class TsFileDistribute
{
public:
	TsFileDistribute();
	virtual ~TsFileDistribute();
private:
	char* message_read_from_file_;
	ifstream file_stream_;
public:
	bool distribute_one_file(string file_name);
};


#endif
