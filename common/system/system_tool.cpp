#include "base\network\network_util.h"
#include "system_tool.h"
#include "base\util\string_util.h"
#include "base\util\string_number_conversions.h"
#include <sys\timeb.h>
#include <sstream>
#include "base\win32\path_util.h"
#include "base\win32\platform_string_util.h"
#include <iostream>
#include <fstream>
#include <windows.h>

const int CHN_NUM_CHAR_COUNT = 10;
const char *chnNumChar[CHN_NUM_CHAR_COUNT] = { "��", "һ", "��", "��", "��", "��", "��", "��", "��", "��" };
const char *chnUnitSection[] = { "", "��", "��", "����" };
const char *chnUnitChar[] = { "", "ʮ", "��", "ǧ" };

namespace systembase
{
	std::string GetMac()
	{
		std::string mac_str = "";
		nbase::GetMacAddress(mac_str);
		return mac_str;
	}

	int GetOSVersion()
	{
		static int os_version = 0;
		if (os_version > 0) {
			return os_version;
		}

		typedef void(__stdcall*NTPROC)(DWORD*, DWORD*, DWORD*);
		HINSTANCE hinst = LoadLibrary(L"ntdll.dll");
		DWORD dwMajor, dwMinor, dwBuildNumber;
		NTPROC proc = (NTPROC)GetProcAddress(hinst, "RtlGetNtVersionNumbers");
		proc(&dwMajor, &dwMinor, &dwBuildNumber);
		os_version = dwMajor * 100 + dwMinor;

		return os_version;
	}

	std::string GetOSVersionString()
	{
		int ver = GetOSVersion();
		switch (ver)
		{
		case 500:
			return "Windows_2000";
		case 501:
			return "Windows_XP";
		case 502:
			return "Windows_Server_2003";
		case 600:
			return "Windows_Vista";
		case 601:
			return "Windows_7";
		case 602:
			return "Windows_8";
		case 603:
			return "Windows_8.1";
		case 1000:
			return "Windows_10";
		default:
			return nbase::StringPrintf("[%d]", ver);
		}
	}

	void GetCallWayAndChannel(std::vector<int>& type_vec, std::string channel)
	{
		if (channel.size() < 3)
		{
			return;
		}
		std::string service_type = channel.substr(0, 1);
		std::string data_channel = channel.substr(1, 1);
		std::string chat_channel = channel.substr(2, 1);

		int i_service_type = 0;
		int i_data_channel = 0;
		int i_chat_channel = 0;
		nbase::StringToInt(service_type, &i_service_type);
		nbase::StringToInt(data_channel, &i_data_channel);
		nbase::StringToInt(chat_channel, &i_chat_channel);
		type_vec.push_back(i_service_type);
		type_vec.push_back(i_data_channel);
		type_vec.push_back(i_chat_channel);
	}

	std::string FormatDouble2Digit(double price_)
	{
		char _s[100] = { 0 };
		sprintf(_s, "%.2f", price_);

		return _s;
	}

	std::string FormatDouble1Digit(double price_)
	{
		char _s[100] = { 0 };
		sprintf(_s, "%.1f", price_);

		return _s;
	}

	__int64 get_time_ms()
	{
		timeb time_now;
		ftime(&time_now); // ����
		__int64 time_cur = time_now.time * 1000 + time_now.millitm; // ����
		return time_cur;
	}

	COLORREF GetPenColor(std::string pen_color)
	{
		int r, g, b;
		if (pen_color.at(0) == '#') {
			pen_color = pen_color.erase(0, 1);
		}

		std::istringstream(pen_color.substr(0, 2)) >> std::hex >> r;
		std::istringstream(pen_color.substr(2, 2)) >> std::hex >> g;
		std::istringstream(pen_color.substr(4, 2)) >> std::hex >> b;

		return RGB(r, g, b);
	}

	bool IntConverHexString(int num, char *str, int str_size)
	{
		__int64 sum = 1;
		for (int i = 0; i < str_size; i++)
		{
			int a = num % (sum * 256);
			str[i] = a / sum;
			num -= a;
			sum *= 256;
		}

		return true;
	}

	int GetDirectorySpace(std::wstring filePath)
	{
		WIN32_FIND_DATA finder;
		HANDLE hFileFind;
		std::wstring search = filePath;
		search.append(L"*.*");
		int size = 0;
		hFileFind = FindFirstFile(search.c_str(), &finder);

		if (hFileFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				std::wstring path = filePath;
				path.append(finder.cFileName);

				if ((finder.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
				{
					size += GetFileSpace(path);
				}
				else if (wcscmp(finder.cFileName, L".") != 0 && wcscmp(finder.cFileName, L"..") != 0)
				{
					path.append(L"\\");
					size += GetDirectorySpace(path);
				}
			} while (FindNextFile(hFileFind, &finder) != 0);

			FindClose(hFileFind);
		}

		return size;
	}

	int GetFileSpace(std::wstring fileName)
	{
		std::string file;
		nbase::win32::UnicodeToMBCS(fileName, file, CP_ACP);
		std::ifstream fin(file.c_str());
		int size = 0;
		if (fin.is_open())
		{
			fin.seekg(0, std::ios::end);
			std::streampos pos = fin.tellg();
			size = pos;
			fin.close();
		}

		return size;
	}

	void DeleteEmptyDirectories(std::wstring dir)
	{
		WIN32_FIND_DATA finder;
		HANDLE hFileFind;
		std::wstring search = dir;
		search.append(L"*.*");

		hFileFind = FindFirstFile(search.c_str(), &finder);

		if (hFileFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				std::wstring path = dir;
				path.append(finder.cFileName);

				if ((finder.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
				{
					DeleteFile(path.c_str());
				}
				else if (wcscmp(finder.cFileName, L".") != 0 && wcscmp(finder.cFileName, L"..") != 0)
				{
					path.append(L"\\");
					DeleteEmptyDirectories(path);
				}
			} while (FindNextFile(hFileFind, &finder) != 0);

			FindClose(hFileFind);
			RemoveDirectory(dir.c_str());
		}
	}

	void SplitString(const std::string &str, const std::string &seperator, std::vector<std::string> &array)
	{
		array.clear();
		if (str.empty() || seperator.empty())
			return;

		size_t position;
		std::string tmp_str = str;

		position = tmp_str.find(seperator.c_str());
		while (position != tmp_str.npos)
		{
			if (position)
				array.push_back(tmp_str.substr(0, position));
			tmp_str.erase(0, position + seperator.length());
			position = tmp_str.find(seperator);
		}

		if (!tmp_str.empty())
			array.push_back(tmp_str);
	}

	void SplitString(const std::wstring &str, const std::wstring &seperator, std::vector<std::wstring> &array)
	{
		array.clear();
		if (str.empty() || seperator.empty())
			return;

		size_t position;
		std::wstring tmp_str = str;

		position = tmp_str.find(seperator.c_str());
		while (position != tmp_str.npos)
		{
			if (position)
				array.push_back(tmp_str.substr(0, position));
			tmp_str.erase(0, position + seperator.length());
			position = tmp_str.find(seperator);
		}

		if (!tmp_str.empty())
			array.push_back(tmp_str);
	}


	std::wstring GetRegValue(const std::wstring& strUrl, const std::wstring& strKey)
	{
		std::wstring strValue = L"";
		HKEY hKey = HKEY_LOCAL_MACHINE;
		HKEY hKeyResult = NULL;
		DWORD dwSize = 0;
		DWORD dwDataType = 0;

		//��ע���
		if (ERROR_SUCCESS == ::RegOpenKeyEx(hKey, strUrl.c_str(), 0, KEY_QUERY_VALUE, &hKeyResult))
		{
			// ��ȡ����ĳ���dwSize������dwDataType
			::RegQueryValueEx(hKeyResult, strKey.c_str(), 0, &dwDataType, NULL, &dwSize);
			switch (dwDataType)
			{
			case REG_MULTI_SZ:
			{
				//�����ڴ��С
				BYTE* lpValue = new BYTE[dwSize];
				//��ȡע�����ָ���ļ�����Ӧ��ֵ
				LONG lRet = ::RegQueryValueEx(hKeyResult, strKey.c_str(), 0, &dwDataType, lpValue, &dwSize);
				delete[] lpValue;
				break;
			}
			case REG_SZ:
			{
				//�����ڴ��С
				wchar_t* lpValue = new wchar_t[dwSize];
				memset(lpValue, 0, dwSize * sizeof(wchar_t));
				//��ȡע�����ָ���ļ�����Ӧ��ֵ
				if (ERROR_SUCCESS == ::RegQueryValueEx(hKeyResult, strKey.c_str(), 0, &dwDataType, (LPBYTE)lpValue, &dwSize))
				{
					strValue = std::wstring(lpValue);
				}
				delete[] lpValue;
				break;
			}
			default:
				break;
			}
		}

		//�ر�ע���
		::RegCloseKey(hKeyResult);


		return strValue;
	}

	unsigned int BKDRHash(std::string str)
	{
		unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
		unsigned int hash = 0;

		for (int i = 0; i < str.size(); i++)
		{
			hash = hash * seed + (str[i]);
		}

		return (hash & 0x7FFFFFFF);
	}

	void NumberToChinese(unsigned int num, std::string& chnStr)
	{
		chnStr.clear();
		int unitPos = 0;
		std::string strIns;
		bool needZero = false;

		if (num == 0)
		{
			chnStr = chnNumChar[0];
			return;
		}

		while (num > 0)
		{
			unsigned int section = num % 10000;
			if (needZero)
			{
				chnStr.insert(0, chnNumChar[0]);
			}
			SectionToChinese(section, strIns);
			/*�Ƿ���Ҫ��Ȩλ��*/
			strIns += (section != 0) ? chnUnitSection[unitPos] : chnUnitSection[0];
			chnStr.insert(0, strIns);
			/*ǧλ��0����Ҫ����һ��section����*/
			needZero = (section < 1000) && (section > 0);
			num = num / 10000;
			unitPos++;
		}
	}

	void SectionToChinese(unsigned int section, std::string& chnStr)
	{
		chnStr.clear();
		std::string strIns;
		int unitPos = 0;
		bool zero = true;
		while (section > 0)
		{
			int v = section % 10;
			if (v == 0)
			{
				if ((section == 0) || !zero)
				{
					zero = true; /*��Ҫ��0��zero��������ȷ���������Ķ��0��ֻ��һ��������*/
					chnStr.insert(0, chnNumChar[v]);
				}
			}
			else
			{
				zero = false; //������һ�����ֲ���0
				strIns = chnNumChar[v]; //��λ��Ӧ����������
				strIns += chnUnitChar[unitPos]; //��λ��Ӧ������Ȩλ
				chnStr.insert(0, strIns);
			}
			unitPos++; //��λ
			section = section / 10;
		}
	}
}


