//
//
//Created by Hank 2016/3/10
//

#include "http.h"

http::http():http_file_p_(nullptr),
	message_read_from_http_(nullptr),
	socket_(-1)
{
	message_read_from_http_ = new char[MAX_LENGTH_OF_READ_MESSAGE_FROM_HTTP];
}

http::~http()
{
	release();
	if(message_read_from_http_ != nullptr) delete[] message_read_from_http_;
}

bool http::release()
{
	if(http_file_p_ != nullptr) fclose(http_file_p_);
	socket_ = -1;
	return true;
}

bool http::initialize(string host_ip,string host_port)
{
	if(host_ip.empty())
	{
		HlsLog::getInstance()->log("error","protocol/http.cpp","the ip is empty");
		return false;
	}
	//initialize the socket
	if(http_file_p_ != nullptr)
	{
		HlsLog::getInstance()->log("error","protocol/http.cpp","the connection has already been initialized");
		return false;
	}
	socket_ = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in sockaddr_addr;
	sockaddr_addr.sin_family = AF_INET;
	sockaddr_addr.sin_port = htons(atoi(host_port.c_str()));
	sockaddr_addr.sin_addr.s_addr = inet_addr(host_ip.c_str());
	//set the host ip and host port
	host_ip_ = host_ip;
	host_port_ = host_port.empty() ? to_string(DEFAULT_HTTP_PORT) : host_port;
	// connect to server
	if(connect(socket_,(const sockaddr*)&sockaddr_addr,sizeof(sockaddr_addr)))
	{	
		HlsLog::getInstance()->log("error","protocol/http.cpp","fail to connect to server");
		return false;
	}
	HlsLog::getInstance()->log("trace","protocol/http.cpp","success to connect to server");
	//use the standerd io stream to send and get message
	http_file_p_ = fdopen(socket_,"rw+");
	if(http_file_p_ == nullptr)
	{
		HlsLog::getInstance()->log("trace","protocol/http.cpp","fail to fdopen the socket");
		return false;
	}
	return true;
}

bool http::send_GET_method(string msg)
{
	if(msg.empty())
	{
		HlsLog::getInstance()->log("error","protocol/http.cpp","the msg is empty");
		return false;
	}
	if(http_file_p_ == nullptr)
	{
		HlsLog::getInstance()->log("error","protocol/http.cpp","the connection hasn't been built");
		return false;
	}
	// make up the get request
	string request = "GET ";
	request += msg;
	request += " HTTP/1.1\r\n";
	request += "Host: "+host_ip_+":"+host_port_+"\r\n\r\n";
	// send the request and deal with the response
	fwrite(request.c_str(),1,request.size(),http_file_p_);
	// deal with the response
	while(message_read_from_http_[0] != '\r' && message_read_from_http_[1] != '\n')
	{
		if(fgets(message_read_from_http_,MAX_LENGTH_OF_READ_MESSAGE_FROM_HTTP,http_file_p_)!=nullptr)
		{
			cout<<message_read_from_http_<<endl;
		}
	}
	//get the content size
	content_size_ = 358;
	return true;
}

char* http::get_msg_by_content_size()
{
	if(content_size_ == 0)
	{
		return nullptr;
	}
	if(fread(message_read_from_http_,1,content_size_,http_file_p_)!=0)
	{
		cout<<message_read_from_http_<<endl;
	}
	return message_read_from_http_;
}

bool http::get_msg_by_size(int size,char* buffer)
{
	/*
	int data_size_left = size,data_size_to_read = size;
	int current_pos = 0;
	while(data_size_left > 0)
	{
		data_size_to_read = data_size_left < MAX_LENGTH_OF_READ_MESSAGE_FROM_HTTP ? data_size_left:data_size_left - MAX_LENGTH_OF_READ_MESSAGE_FROM_HTTP;
		if((int)fread(buffer+current_pos,1,data_size_to_read,http_file_p_) != data_size_to_read) break;
		data_size_left -= data_size_to_read;
		current_pos += data_size_to_read;
	}
	if(feof(http_file_p_)) return false;
	*/
	if(feof(http_file_p_)) return false;
	if((int)fread(buffer,1,size,http_file_p_) < size) return false;
	return true;
	return true;
}

