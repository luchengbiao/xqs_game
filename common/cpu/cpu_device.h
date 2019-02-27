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
	int GetCPUFreq();//��ȡCPUƵ��,��λ: MHZ
	std::string GetManID();//��ȡ��������Ϣ
	std::string GetCPUType();

private:

};
