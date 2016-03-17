//
//Created by Hank 2016/3/10
//

#include "http.h"
using namespace AppSocket;
http::http():http_file_p_(nullptr),
	message_read_from_http_(nullptr),
	socket_(-1),
	data_left_(0),
	data_current_pos_(0)
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
	if(host_ip.empty() || host_port.empty())
	{
		HlsLog::getInstance()->log("error","protocol/http.cpp","the ip is empty");
		return false;
	}
#ifndef USING_SOCKET
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
#else
	if(!tcp_socket_.initialize(host_ip,host_port)) return false;
#endif
	//set the host ip and host port
	host_ip_ = host_ip;
	host_port_ = host_port.empty() ? to_string(DEFAULT_HTTP_PORT) : host_port;
	return true;
}

bool http::send_GET_method_without_response(string msg)
{
	if(msg.empty())
	{
		HlsLog::getInstance()->log("error","protocol/http.cpp","the msg is empty");
		return false;
	}
	// make up the get request
	string request = "GET ";
	request += msg;
	request += " HTTP/1.1\r\n";
	request += "Host: "+host_ip_+":"+host_port_+"\r\n\r\n";
#ifndef USING_SOCKET
	if(http_file_p_ == nullptr)
	{
		HlsLog::getInstance()->log("error","protocol/http.cpp","the connection hasn't been built");
		return false;
	}
	// send the request and deal with the response
	fwrite(request.c_str(),1,request.size(),http_file_p_);
#else
	if(tcp_socket_.sendMsg(request) < 1) return false;
#endif
	return true;
}

bool http::parse_the_http_respnse(std::string http_response)
{
	if(http_response.empty()) return false;
	const string end_line = "\r\n";
	int current_pos = 0;
	const string content_key_word = "Content-Length: ";// inorder to get content length
	size_t size = 0;
	string str_length;
	while(current_pos < (int)http_response.size())
	{
		size_t size1 = 0;
		string line_str;
		if((size1 = http_response.find_first_of(end_line,current_pos)) == string::npos) break;
		line_str.assign(http_response,current_pos,size1 - current_pos);
		if((size = line_str.find(content_key_word)) != string::npos)
		{
			str_length.assign(line_str,content_key_word.length(),line_str.length() - content_key_word.length());
		}
		std::cout<<line_str<<std::endl;
		current_pos = size1 + 2;
	}
	//get the content size
	content_size_ = atoi(str_length.c_str());
	return true;
}

bool http::send_GET_method_with_response(string msg)
{
	send_GET_method_without_response(msg);
#ifndef USING_SOCKET
	// deal with the response
	string content_key_word = "Content-Length: ";
	string message_get;
	size_t size = 0;
	string str_length;
	memset(message_read_from_http_,0,MAX_LENGTH_OF_READ_MESSAGE_FROM_HTTP);
	while(message_read_from_http_[0] != '\r' && message_read_from_http_[1] != '\n')
	{
		if(fgets(message_read_from_http_,MAX_LENGTH_OF_READ_MESSAGE_FROM_HTTP,http_file_p_)!=nullptr)
		{
			cout<<message_read_from_http_<<endl;
			message_get = message_read_from_http_;
			if((size = message_get.find(content_key_word)) != string::npos)
			{
				str_length.assign(message_get,content_key_word.length(),message_get.length() - content_key_word.length() - 2);
			}
		}
		else
			break;
	}
	//get the content size
	content_size_ = atoi(str_length.c_str());
#else
	// recv the response and parse it
	if((data_left_ = tcp_socket_.recvMsg(message_buffer_,AppSocket::DEFAULT_SOCKET_TIME_OUT)) < 1) return false;
	data_current_pos_ = 0;
	// get http header
	const string end_of_http_response  = "\r\n\r\n";
	size_t size = 0;
	if((size = message_buffer_.find(end_of_http_response)) == string::npos) return false;
	string http_response;
	http_response.assign(message_buffer_,0,size + 4);
	data_left_ -= size + 4;//we got the size + 4 Bytes of the string
	data_current_pos_ += size + 4;
	return parse_the_http_respnse(http_response);
#endif
	return true;
}

bool http::get_msg_by_content_size(string& buffer)
{
	if(content_size_ == 0)
	{
		return false;
	}
#ifndef USING_SOCKET
	if(fread(message_read_from_http_,1,content_size_,http_file_p_)!=0)
	{
		cout<<message_read_from_http_<<endl;
	}
#else
	return get_msg_by_size(content_size_,buffer);
#endif
}

bool http::get_msg_by_size(int size,string& buffer)
{
#ifndef USING_SOCKET
	if(feof(http_file_p_)) return false;
	if((int)fread(buffer,1,size,http_file_p_) < size) return false;
#else
	if(size < data_left_)
	{
		buffer.assign(message_buffer_,data_current_pos_,size);
		data_left_ -= size;
		data_current_pos_ += size;
		return true;
	}
	// first get the left data
	int rest_data = size - data_left_;
	buffer.assign(message_buffer_,data_current_pos_,data_left_);
	data_left_ = 0;
	data_current_pos_ = message_buffer_.size();
	while (rest_data > 0)
	{
		// get message from http server
		if((data_left_ = tcp_socket_.recvMsg(message_buffer_,AppSocket::DEFAULT_SOCKET_TIME_OUT)) < 1) return false;
		data_current_pos_ = 0;
		// copy the rest to buffer
		int data_to_copy = data_left_ < rest_data ? data_left_ : rest_data;
		buffer.append(message_buffer_,data_current_pos_,data_to_copy);
		rest_data -= data_to_copy;
		data_left_ -= data_to_copy;
		data_current_pos_ += data_to_copy;
	}
	std::cout<<buffer.size()<<std::endl;
	std::cout<<buffer<<std::endl;
	buffer = message_buffer_;
#endif
	return true;
}

