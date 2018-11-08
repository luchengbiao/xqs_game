#ifndef COMMON_TOOL_H
#define COMMON_TOOL_H
#include <iostream>
#include "base\win32\platform_string_util.h"

namespace commontool
{
	/*	��ȡ���ش洢Ŀ¼	*/
	std::string GetAppDataLocalDir();
	/*	��ȡ��������Ŀ¼	*/
	std::wstring GetAppPath();
	/*	��ȡGUID		*/
	std::string GetGuid();

	std::string GetUserAppdataDir(const std::string& app_account);
	/*��ȡ������MAC��ַ*/
	bool GetMacAddress(std::string &address);

	std::string GetMac();

	bool UnicodeToMBCS(const std::wstring& input, std::string &output, int code_page = CP_ACP);
	std::wstring GetGeneralGrade(std::wstring grade);

	bool IsJsonString(std::string json_str);
}
#endif	//	COMMON_TOOL_H