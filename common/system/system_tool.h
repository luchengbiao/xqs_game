#pragma once
#include <string>
#include <vector>
#include <wtypes.h>

namespace systembase
{
	std::string GetMac();
	int GetOSVersion();
	std::string GetOSVersionString();
	void GetCallWayAndChannel(std::vector<int>& type_vec, std::string channel);
	std::string FormatDouble2Digit(double price_);
	std::string FormatDouble1Digit(double price_);

	__int64 get_time_ms();
	COLORREF GetPenColor(std::string pen_color);
	bool IntConverHexString(int num, char *str, int str_size);
	int GetFileSpace(std::wstring fileName);  // ��ȡ�ļ��Ĵ�С
	int GetDirectorySpace(std::wstring filePath); // ��ȡ�����ļ��еĿռ��С

	void DeleteEmptyDirectories(std::wstring dir); // ɾ�������ļ���

	void SplitString(const std::string &str, const std::string &seperator, std::vector<std::string> &array);
	void SplitString(const std::wstring &str, const std::wstring &seperator, std::vector<std::wstring> &array);
	
	std::wstring GetRegValue(const std::wstring& strUrl, const std::wstring& strKey);
	unsigned int BKDRHash(std::string str);

	void NumberToChinese(unsigned int num, std::string& chnStr);
	void SectionToChinese(unsigned int section, std::string& chnStr);
}