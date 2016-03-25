// 
// Created by Hank 2016/3/16
//
#ifndef HLS_DECODE_HD_APP_SOCKET_H
#define HLS_DECODE_HD_APP_SOCKET_H

#include "../kernel/hls_decode_kernel.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <csignal>

namespace AppSocket
{
    static const long MAX_LENGTH_READ_FROM_SOCKET = 1024;
    static const long DEFAULT_SOCKET_TIME_OUT = 1000;
    class TcpSocket
    {
    public:
        TcpSocket();
        virtual ~TcpSocket();
    private:
        int socket_;
        std::string host_ip_;
        std::string host_port_;
        char* message_;
        int socket_time_out_;
        bool is_connected_;
    public:
        bool initialize(std::string ip, std::string port);
        int sendMsg(std::string msg);
        int recvMsg(std::string& msg, int size);
    };

    class UdpSocket
    {
    };
};

#endif
