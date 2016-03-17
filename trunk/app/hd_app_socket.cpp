// 
// Created by Hank 2016/3/16
//
#include "hd_app_socket.h"
#include <boost/array.hpp>
using namespace AppSocket;

TcpSocket::TcpSocket() : socket_(-1),
	message_(nullptr),
	socket_time_out_(DEFAULT_SOCKET_TIME_OUT),
	is_connected_(false)
{
	message_ = new char[MAX_LENGTH_READ_FROM_SOCKET];
}

TcpSocket::~TcpSocket()
{
	if(message_ != nullptr) delete[] message_;
}

bool TcpSocket::initialize(std::string ip, std::string port)
{
	if(ip.empty() || port.empty())
	{
		HlsLog::getInstance()->log("error","hd_app_socket.cpp","the ip or port is empty");
		return false;
	}
	host_ip_ = ip;
	host_port_ = port;
	// initialize the socket
	struct sockaddr_in socket_addr;
	socket_ = socket(AF_INET, SOCK_STREAM,0);
	if(socket_ == -1)
	{
		HlsLog::getInstance()->log("error","hd_app_socket.cpp","error to create tcp socket");
		return false;
	}
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = htons(atoi(host_port_.c_str()));
	socket_addr.sin_addr.s_addr = inet_addr(host_ip_.c_str());
	setsockopt(socket_,SOL_SOCKET,SO_RCVTIMEO,(char*)&socket_time_out_,sizeof(socket_time_out_));
	// connect to server
	if(connect(socket_,(const sockaddr*)&socket_addr,sizeof(socket_addr)) != 0)
	{
		HlsLog::getInstance()->log("error","hd_app_socket.cpp","error to connect to the server");
		return false;
	}
	is_connected_ = true;
	return true;
}

int TcpSocket::sendMsg(std::string msg)
{
	// is socket created and able to connect to server
	if(socket_ == -1 || !is_connected_) return -1;
	if(msg.empty()) return 0;
	return send(socket_,msg.c_str(),msg.length(),0);
}

int TcpSocket::recvMsg(std::string& msg, int size)
{
	boost::array<int,4> arr = {{1,2,3,4}};
	if(socket_ == -1 || !is_connected_) return -1;
	if(!msg.empty()) msg.clear();
	int data_left = size,data_to_read = 0;
	while(data_left > 0)
	{
		// initialize the memory
		memset(message_,0,MAX_LENGTH_READ_FROM_SOCKET);
		// calculate the size of data to read
		data_to_read = data_left < MAX_LENGTH_READ_FROM_SOCKET ? data_left : MAX_LENGTH_READ_FROM_SOCKET;
		// get data
		if(recv(socket_,message_,MAX_LENGTH_READ_FROM_SOCKET,0) == 0) break;
		msg += message_;
		// calculate the size of data left
		data_left -= data_to_read;
	}
	return msg.length();
}
