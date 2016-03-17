//
//Created by Hank 2016/3/11
//
#include "ts_hls_distribute.h"
#include "../protocol/http.h"
#include "../protocol/m3u8file.h"
#include "../protocol/ts_packet.h"
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
	if(!http_->initialize("192.168.1.237","8080"))
	{
		HlsLog::getInstance()->log("error","ts_hls_distribute.cpp-distribute_hls_stream","fail to init the socket");
		return false;
	}
	// send the GET method 
	if(!http_->send_GET_method_with_response("/live/livestream.m3u8"))
	{
		HlsLog::getInstance()->log("error","ts_hls_distribute.cpp-distribute_hls_stream","fail to send GET request");
		return false;
	}
	// get m3u8 file
	string  m3u8_buffer;
	http_->get_msg_by_content_size(m3u8_buffer);
	m3u8file m3u8;
	if(m3u8_buffer.empty()) return false;
	if(!m3u8.parse_m3u8(m3u8_buffer,ts_stream_list_)) return false;
	TsPacket::Packet pack;
	string ts;
	for(auto i :ts_stream_list_)
	{
		string str = "/live/"+i;
		http_->send_GET_method_with_response(str);
		cout<<endl<<"decode one ts file"<<endl;
		int temp = 0;
		while(http_->get_msg_by_size(188,ts))
		{
			cout<<"this is NO."<<temp++<<endl;
			pack.distribute_one_packet(ts.c_str(),188);
			cout<<"decode one header"<<endl;
		}
		break;
	}
	return true;
}
