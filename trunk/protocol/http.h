//
//Created by Hank 2016/3/10
//
#ifndef HLS_DECODE_HTTP_H
#define HLS_DECODE_HTTP_H

#include "../kernel/hls_decode_kernel.h"
#include "../app/hd_app_socket.h"
#include <stdio.h>
//#define USING_SOCKET
static const int32_t MAX_LENGTH_OF_READ_MESSAGE_FROM_HTTP = 1024;
static const int32_t DEFAULT_HTTP_PORT = 80;

class http
{
public:
    http();
    virtual ~http();
private:
    FILE* http_file_p_;
    char* message_read_from_http_;
    int socket_;
    int content_size_;
    string host_ip_;
    string host_port_;

    AppSocket::TcpSocket tcp_socket_;
    string message_buffer_;
    int data_left_;
public:
    bool initialize(string host_ip, string host_prot);
    bool release();
    bool send_GET_method_with_response(string msg);
    bool send_GET_method_without_response(string msg);
    char* get_msg_by_content_size();
    bool get_msg_by_size(int size,char* buffer);
    int get_content_size(){return content_size_;};
};

#endif
