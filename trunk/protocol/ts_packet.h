//
// Created by Hank 2016/3/14
//
#ifndef HLS_DECODE_TS_PACKET_H
#define HLS_DECODE_TS_PACKET_H

#include "../kernel/hls_decode_kernel.h"
namespace TsPacket
{	
	static const  int32_t DEFAULT_TS_PACKET_LENGTH = 188;
	// 4-byte Transport Stream Header
	typedef struct _tagTSPacketHeader
	{
		unsigned char sync_byte_;
		unsigned TEI_ : 1;
		unsigned payload_uint_start_indicator_ : 1;
		unsigned transport_priority_ : 1;
		unsigned PID_ : 13;
		unsigned scrambling_control_ : 2;
		unsigned adaptation_field_flag_ : 1;
		unsigned payload_flag_ : 1;
		unsigned continuity_counter_ : 4;
	}PacketHeader,*pPacketHeader;
	
	// 2-byte adaptional field
	typedef struct _tagAdaptationFieldFormat
	{
		unsigned char adaptation_field_length_;
		unsigned discontinuity_indicator_ : 1;
		unsigned random_access_indicator_ : 1;
		unsigned elementary_stream_priority_indicator_ : 1;
		unsigned PCR_flag_ : 1;
		unsigned OPCR_flag_ : 1;
		unsigned splicing_point_flag_ : 1;
		unsigned transport_private_data_flag_ : 1;
		unsigned adaptation_field_extension_flag_ : 1;
	}AdaptationFieldFormat,*pAdaptationFieldFormat;

	//adaptation extension field format
	typedef struct _tagAdaptationExtensionFieldFormat
	{
		unsigned char adapation_extension_length_;
		unsigned legal_time_windows_flas_ : 1;
		unsigned piecewise_rate_flag_ : 1;
		unsigned seamless_splice_flah_ : 1;
		unsigned reserved_ : 5;
	}AdaptationExtensionFieldFormat,*pAdaptationExtensionFieldFormat;

	// adaptation extension field optional fields
	typedef struct _tagAdaptationExtensionOptionalFieldFormat
	{
		// LTW flag set
		unsigned ltw_flag_set_[2];
		// Piecewise flag set
		unsigned char piecewise_flag_set_[3];
		//seamless splice flag set
		unsigned seamless_splice_flag_[5];
	}AdaptationExtensionOptionalFieldFormat,*pAdaptationExtensionOptionalFieldFormat;

	//adaptional optional field
	typedef struct _tagAdaptationFieldOptionalField
	{
		unsigned char PCR_[6];
		unsigned char OPCR_[6];
		unsigned char splice_countdown;
		unsigned char transport_private_data_length_;
		unsigned char* transport_private_data_;
		pAdaptationExtensionFieldFormat adaptation_extension_;
		unsigned char* stuffing_bytes;
	}AdaptationOptionalField,*pAdaptationOptionalField;
	
	class Packet
	{
	public:
		Packet();
		virtual ~Packet();
	private:
		//header
		pPacketHeader packet_header_;
		// adaptation field 
		pAdaptationFieldFormat adaptation_field_;
		// adaptation optional field
		pAdaptationOptionalField adaptation_optional_field_;
		// adaptation extension field
		pAdaptationExtensionFieldFormat adaptation_extension_field_;
		// adaptation extension optional field
		pAdaptationExtensionOptionalFieldFormat adaptation_extension_optional_field_;
		// payload data
		unsigned char* payload_data_;
	public:
		bool distribute_one_packet(const char* msg, int msg_size);
	};

};
#endif
