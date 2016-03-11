//
//Created by Hank 2016/3/16
//
#ifndef TS_HLS_DISTRIBUTE_H
#define TS_HLS_DISTRIBUTE_H

#include "../kernel/hls_decode_kernel.h"

class http;

class TsHlsDistribute
{
public:
	TsHlsDistribute();
	virtual ~TsHlsDistribute();
private:
	http* http_;
protected:
	bool parse_url(string url);
public:
	bool distribute_hls_stream(string url);
};

#endif
