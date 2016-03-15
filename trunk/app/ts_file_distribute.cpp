// 
//Created by Hank 2016/3/11
//

#include "ts_file_distribute.h"
#include "../protocol/ts_packet.h"

TsFileDistribute::TsFileDistribute():
	message_read_from_file_(nullptr)
{
	message_read_from_file_ = new char[MAX_LENGTH_OF_READ_MESSAGE_FROM_FILE];
}

TsFileDistribute::~TsFileDistribute()
{
	if(message_read_from_file_ != nullptr) delete[] message_read_from_file_;
	if(file_stream_.is_open()) file_stream_.close();
}

bool TsFileDistribute::distribute_one_file(string file_name)
{
	if(file_name.empty())
	{
		HlsLog::getInstance()->log(\
				"trace","ts_file_distribute.cpp-distribute_one_file","the file name is empty");
		return false;
	}
	// check whether the file stream has already been opened
	if(file_stream_.is_open())
	{
		file_stream_.close();
	}
	//open the file 
	file_stream_.open(file_name,istream::in);
	// read the file an parser the message we will get
	TsPacket::Packet pack;
	while(file_stream_.read(message_read_from_file_,MAX_LENGTH_OF_READ_MESSAGE_FROM_FILE))
	{
		HlsLog::getInstance()->log("","","");
		// parser the message we read from file
		pack.distribute_one_packet(message_read_from_file_,MAX_LENGTH_OF_READ_MESSAGE_FROM_FILE);
		// seek the next frame
	}
	return true;
}
