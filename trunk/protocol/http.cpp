//
//Created by Hank 2016/3/10
//

#include "http.h"
using namespace AppSocket;
http::http():http_file_p_(nullptr),
    message_read_from_http_(nullptr),
    socket_(-1),
    data_left_(0)
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
    // decode the reponse we got
    if(tcp_socket_.recvMsg(message_buffer_,AppSocket::DEFAULT_SOCKET_TIME_OUT) < 1) {
        return false;
    }
    size_t size = message_buffer_.find_first_of("\r\n\r\n");
    if(size == string::npos) return false;
    data_left_ -= message_buffer_.size() - size - 4;
    string http_response;
    http_response.assign(message_buffer_,0,size + 4);
    while(size > 0)
    {
        size_t size1 = 0;
        string line_str;
    }
#endif
    return true;
}

char* http::get_msg_by_content_size()
{
    if(content_size_ == 0)
    {
        return nullptr;
    }
#ifndef USING_SOCKET
    if(fread(message_read_from_http_,1,content_size_,http_file_p_)!=0)
    {
        cout<<message_read_from_http_<<endl;
    }
#else

#endif
    return message_read_from_http_;
}

bool http::get_msg_by_size(int size,char* buffer)
{
#ifndef USING_SOCKET
    if(feof(http_file_p_)) return false;
    if((int)fread(buffer,1,size,http_file_p_) < size) return false;
#else
#endif
    return true;
}

