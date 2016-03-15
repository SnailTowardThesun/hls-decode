//
// Created by Hank 2016/3/15
//
#include "m3u8file.h"

m3u8file::m3u8file()
{

}

m3u8file::~m3u8file()
{

}

CMDType m3u8file::judge_cmd_type(string cmd)
{
	if(cmd[0] != '#') return TS_FILE_URL;
	return COMMAND_LINE;
}

bool m3u8file::parse_m3u8(string str, vector<string>& ts_file_list)
{
	if(str.empty()) return false;
	string str_m3u8 = str;
	size_t current_pos = 0;
	while(!str_m3u8.empty())
	{
		size_t size = 0;
		string line_str;
		if((size = str_m3u8.find_first_of("\n",current_pos)) == string::npos) break;
		line_str.assign(str_m3u8,current_pos,size - current_pos);
		// parse the line_str
		if(judge_cmd_type(line_str) == TS_FILE_URL)
		{
			ts_file_list.push_back(line_str);
		}
		current_pos = size + 1;
	}

	return true;
}
