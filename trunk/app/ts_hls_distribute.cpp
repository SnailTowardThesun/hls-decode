//
//Created by Hank 2016/3/11
//
#include "ts_hls_distribute.h"
#include "../protocol/http.h"
TsHlsDistribute::TsHlsDistribute():http_(nullptr)
{
	http_ = new http();
}

TsHlsDistribute::~TsHlsDistribute()
{
	if(http_ != nullptr) delete http_;
}

bool TsHlsDistribute::parse_url(string url)
{
	if(url.empty()) return false;

	return true;
}

bool TsHlsDistribute::distribute_hls_stream(string url)
{
	if(url.empty())
	{
		HlsLog::getInstance()->log("error","ts_hls_distribute.cpp-distribute_hls_stream","the url is empty");
		return false;
	}
	// parse the url an initialize the socket
	if(!parse_url(url))
	{
		HlsLog::getInstance()->log("error","ts_hls_distribute.cpp-distribute_hls_stream",
				"fail to parse the url");
		return false;
	}
	// initialize the http
	if(!http_->initialize("192.168.9.237","8080"))
	{
		HlsLog::getInstance()->log("error","ts_hls_distribute.cpp-distribute_hls_stream","fail to init the socket");
		return false;
	}
	// send the GET method 
	if(!http_->send_GET_method("/live/livestream.m3u8"))
	{
		HlsLog::getInstance()->log("error","ts_hls_distribute.cpp-distribute_hls_stream","fail to send GET request");
		return false;
	}
	// get m3u8 file
	http_->get_msg_by_content_size();
	return true;
}
