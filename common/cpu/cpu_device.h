#pragma once
#include "windows.h"
#include "string"
class CpuDevice
{
public:
	CpuDevice();
	~CpuDevice();
	std::string GetDeviceInfo();

private:
	void ExeCPUID(DWORD veax);
	int GetCPUFreq();//获取CPU频率,单位: MHZ
	std::string GetManID();//获取制造商信息
	std::string GetCPUType();

private:

};
