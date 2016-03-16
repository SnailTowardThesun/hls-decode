#include "kernel/hls_decode_kernel.h"
#include "app/ts_hls_distribute.h"
#include "app/ts_file_distribute.h"
int main(int argc, char* argv[])
{
	if(argc == 2)
	{
		cout<<argv[0]<<endl;
	}
	HlsLog::getInstance()->log("trace","main.cpp","ready to decode");
	TsHlsDistribute dis;
	dis.distribute_hls_stream("http://192.168.1.237:8080/live/livestream.m3u8");
	//TsFileDistribute dis;
	//dis.distribute_one_file("test.ts");
	return 0;
}
