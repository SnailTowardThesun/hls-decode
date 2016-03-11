//
//Created by Hank 2016/3/10
//
#ifndef HLS_DECODE_HTTP_H
#define HLS_DECODE_HTTP_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <csignal>

class http
{
public:
	http();
	virtual ~http();
};

#endif
