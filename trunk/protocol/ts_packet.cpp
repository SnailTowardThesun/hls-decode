//
// Created by Hank 2016/3/14
//
#include "ts_packet.h"
using namespace TsPacket;
Packet::Packet():packet_header_(nullptr),
	adaptation_field_(nullptr),
	adaptation_optional_field_(nullptr),
	adaptation_extension_field_(nullptr),
	adaptation_extension_optional_field_(nullptr),
	payload_data_(nullptr)
{
	packet_header_ = new PacketHeader();
	adaptation_field_ = new AdaptationFieldFormat();
	adaptation_optional_field_ = new AdaptationOptionalField();
	adaptation_extension_field_ = new AdaptationExtensionFieldFormat();
	adaptation_extension_optional_field_ = new AdaptationExtensionOptionalFieldFormat();
	payload_data_ = new unsigned char[DEFAULT_TS_PACKET_LENGTH];
}

Packet::~Packet()
{
	if(packet_header_ != nullptr) delete packet_header_;
	if(adaptation_field_ != nullptr) delete adaptation_field_;
	if(adaptation_optional_field_ != nullptr) delete adaptation_optional_field_;
	if(adaptation_extension_field_ != nullptr) delete adaptation_extension_field_;
	if(adaptation_extension_optional_field_ != nullptr) delete adaptation_extension_optional_field_;
	if(payload_data_ != nullptr) delete[] payload_data_;
}

bool Packet::decode_packet(const char* message)
{
	if(message == nullptr) return false;
	int current_pos = 4;
	int num = 0;
	string str;
	int offset_pos = 0;
	bool isAdapterFieldOptionalHasExtension = false;
	// adaptation field length
	adaptation_field_->adaptation_field_length_ = message[current_pos];
	num = (int)adaptation_field_->adaptation_field_length_;
	str = "the adaptation field's length is " + to_string(num);
	HlsLog::getInstance()->log("trace","ts_packet.cpp",str);
	// discontinuity indicator
	adaptation_field_->discontinuity_indicator_ = message[current_pos + 1] >> 7;
	if(adaptation_field_->discontinuity_indicator_ == 1)
	{
		HlsLog::getInstance()->log("trace","ts_packet.cpp","TS packet is in a discontunuity state with respect to either the continuity counter or the program clock referernce");
	}
	// random access indicator
	adaptation_field_->random_access_indicator_ = message[current_pos + 1] >> 6 & 0x01;
	if(adaptation_field_->random_access_indicator_ == 1)
	{
		HlsLog::getInstance()->log("trace","ts_packet.cpp","the stream may be decoded without errors from this point");
	}
	// elementary stream priority indicator
	adaptation_field_->elementary_stream_priority_indicator_ = message[current_pos + 1] >> 5 & 0x01;
	if(adaptation_field_->elementary_stream_priority_indicator_ == 1)
	{
		HlsLog::getInstance()->log("trace","ts_packet.cpp","this stream should be considered 'high priority'");
	}
	// PCR flag
	adaptation_field_->PCR_flag_ = message[current_pos + 1] >> 4 & 0x01;
	if(adaptation_field_->PCR_flag_ == 1)
	{
		HlsLog::getInstance()->log("trace","ts_packet.cpp","PCR field is present");
		// get PCR 
		memcpy(adaptation_optional_field_->PCR_,&message[current_pos + 2],6);
		HlsLog::getInstance()->log("trace","ts_packet.cpp","get pcr field");
		offset_pos += 6;
	}
	// OPCR
	adaptation_field_->OPCR_flag_ = message[current_pos + 1] >> 3 & 0x01;
	if(adaptation_field_->OPCR_flag_ == 1)
	{
		HlsLog::getInstance()->log("trace","ts_packet.cpp","the OPCR field is present");
		memcpy(adaptation_optional_field_->OPCR_,&message[current_pos + 2 + 6],6);
		HlsLog::getInstance()->log("trace","ts_packet.cpp","get OPCR field");
		offset_pos += 6;
	}
	// splicing point flag
	adaptation_field_->splicing_point_flag_ = message[current_pos + 1] >> 2 & 0x01;
	if(adaptation_field_->splicing_point_flag_ == 1)
	{
		HlsLog::getInstance()->log("trace","ts_packet.cpp","splice countdown field is present");
		adaptation_optional_field_->splice_countdown = message[current_pos + 2 + 6 + 6 + 1];
		HlsLog::getInstance()->log("trace","ts_packet.cpp","get splicing point");
		offset_pos += 1;
	}
	// transport private data flag
	adaptation_field_->transport_private_data_flag_ = message[current_pos + 1] >> 1 & 0x01;
	if(adaptation_field_->transport_private_data_flag_ == 1)
	{
		HlsLog::getInstance()->log("trace","ts_packet.cpp","private data field is present");
		adaptation_optional_field_->transport_private_data_length_ = message[current_pos + 2 + 6 + 6 + 1 + 1];
		HlsLog::getInstance()->log("trace","ts_packet.cpp","get transport private data length");
		offset_pos += 1;
	}
	// adaptation field extension flag
	adaptation_field_->adaptation_field_extension_flag_ = message[current_pos + 1] & 0x01;
	if(adaptation_field_->adaptation_field_extension_flag_ == 1)
	{
		HlsLog::getInstance()->log("trace","ts_packet.cpp","extension field is present");
		isAdapterFieldOptionalHasExtension = true;
	}
	//judge if the header is end
	if(offset_pos + 1 >= adaptation_field_->adaptation_field_length_)
	{
		HlsLog::getInstance()->log("trace","ts_packet.cpp","the packet header has been decoded successfully");
		goto exit;
	}
	current_pos += 2;
	// adaptation optional field
	current_pos += offset_pos;
	// get transport private data
	// copy the data
	current_pos += adaptation_optional_field_->transport_private_data_length_;
	// adaptation optional adaptation field
	if(isAdapterFieldOptionalHasExtension)
	{
		// adaptaptation extension length
		adaptation_extension_field_->adapation_extension_length_ = message[current_pos];
		str = "the adaptation extension field's length is " + to_string((int)adaptation_extension_field_->adapation_extension_length_);
		HlsLog::getInstance()->log("trace","ts_packet.cpp",str);
		// legal time window(LTW)
		adaptation_extension_field_->legal_time_windows_flas_ = message[current_pos + 1] >> 7;
		if(adaptation_extension_field_->legal_time_windows_flas_ == 1)
		{
			HlsLog::getInstance()->log("trace","ts_packet.cpp","the legal time window has been setted");
		}
		// pecewise rate flag
		adaptation_extension_field_->piecewise_rate_flag_ = message[current_pos + 1] >> 6 & 0x01;
		if(adaptation_extension_field_->piecewise_rate_flag_ == 1)
		{
			HlsLog::getInstance()->log("trace","ts_packet.cpp","the piecewise rate flag has been setted");
		}
		// seamless splice flag
		adaptation_extension_field_->seamless_splice_flah_ = message[current_pos + 1] & 0x1f;
		//judge the adaptation extension field's header length and get the messag 
		switch(adaptation_extension_field_->adapation_extension_length_)
		{
		case 0x02:
			break;
		case 0x04:
			break;
		case 0x07:
			break;
		case 0x0C:
			break;
		default :
			break;
		}
		current_pos += adaptation_extension_field_->adapation_extension_length_;
	}
	//should be stopped at 0xff stuffing byte
	adaptation_optional_field_->stuffing_bytes= (unsigned char*)&message[current_pos];
	if(adaptation_optional_field_->stuffing_bytes[0] == 0xff)
	{
		HlsLog::getInstance()->log("trace","ts_packet.cpp","the header has been decoded successfully");
	}
exit:
	return true;
}

bool Packet::distribute_one_packet(const char* message ,int message_size)
{
	if(message_size < DEFAULT_TS_PACKET_LENGTH || message == nullptr) return false;
	// init the memory of variable
	memset(packet_header_,0,sizeof(PacketHeader));
	memset(adaptation_field_,0,sizeof(AdaptationFieldFormat));
	memset(adaptation_optional_field_,0,sizeof(AdaptationOptionalField));
	memset(adaptation_extension_field_,0,sizeof(AdaptationExtensionFieldFormat));
	memset(adaptation_extension_optional_field_ ,0,sizeof(AdaptationExtensionOptionalFieldFormat));
	int current_pos = 0;
	int num;
	bool isHeaderHasAdapterField = false;
	// get the 4 bytes of header
	// sync byte
	packet_header_->sync_byte_ = message[0];
	if(packet_header_->sync_byte_ != 0x47)
	{
		HlsLog::getInstance()->log("error","ts_packet.cpp","the packet header's sync byte is error");
		return false;
	}
	// transport error indicator
	packet_header_->transport_priority_ = message[1] >> 7;
	if(packet_header_->transport_priority_ == 1)
	{
		HlsLog::getInstance()->log("warning","ts_packet.cpp","the demodulator can't correct errors from FEC data");
	}
	// payload unit start indicator
	packet_header_->payload_uint_start_indicator_ = message[1] >> 6 & 0x01;
	if(packet_header_->payload_uint_start_indicator_ == 1)
	{
		HlsLog::getInstance()->log("trace","ts_packet.cpp","the packet is PES,PSI or DVB-MIP");
	}
	// transport priority
	packet_header_->transport_priority_ = message[1] >> 5 &0x01;
	if(packet_header_->transport_priority_ == 1)
	{
		HlsLog::getInstance()->log("trace","ts_packet.cpp","the current packet has a highter priority than other packets with the same PID");
	}
	// get the PID
	packet_header_->PID_ = (message[1] & 0x1F) << 8 | message[2];
	uint16_t pid = (uint16_t)packet_header_->PID_;
	string str = "this packet's pid is " + to_string(pid);
	HlsLog::getInstance()->log("trace","ts_packet.cpp",str);
	// scrambling
	packet_header_->scrambling_control_ = message[3] >> 6;
	switch(packet_header_->scrambling_control_)
	{
	case 0x00:
		str = "not scrambled";
		break;
	case 0x01:
		str = "for dvb-csa only reserved for future use";
		break;
	case 0x02:
		str = "for dvb-csa only scrambled with even key";
		break;
	case 0x03:
		str = "for dvb-csa only scrambled with odd key";
		break;
	default:
		str = "the scrambling control is an error";
		break;
	}
	HlsLog::getInstance()->log("trace","ts_packet.cpp",str);
	// adaptation field flag
	packet_header_->adaptation_field_flag_ = message[3] >> 5 & 0x01;
	if(packet_header_->adaptation_field_flag_ == 1)
	{
		HlsLog::getInstance()->log("trace","ts_packet.cpp","the header has adaptation field");
		isHeaderHasAdapterField = true;
	}
	// payload flag
	packet_header_->payload_flag_ = message[3] >> 4 &0x01;
	if(packet_header_->payload_flag_ == 1)
	{
		HlsLog::getInstance()->log("trace","ts_packet.cpp","the header has payload flah");
	}
	//continuity counter
	packet_header_->continuity_counter_ = message[3] & 0x0F;
	if(packet_header_->payload_flag_ == 1)
	{
		HlsLog::getInstance()->log("trace","ts_packet.cpp","the payload flag is 1");
	}
	int continuity_counter = packet_header_->continuity_counter_;
	str = " the continuity counter of header is " + to_string(continuity_counter);
	HlsLog::getInstance()->log("trace","ts_packet.cpp",str);
	current_pos += 4;
	if(isHeaderHasAdapterField)
	{
		decode_packet(message);
	}
	else
	{
		HlsLog::getInstance()->log("trace","ts_packet.cpp","the header has been decoded successfully");
	}
	return true;
}
