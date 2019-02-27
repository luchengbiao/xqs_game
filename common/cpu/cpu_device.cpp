#include "cpu_device.h"
#include "base\util\string_number_conversions.h"
#include "iostream"
DWORD deax;
DWORD debx;
DWORD decx;
DWORD dedx;
CpuDevice::CpuDevice()
{

}

CpuDevice::~CpuDevice()
{

}

std::string CpuDevice::GetDeviceInfo()
{
	std::string cpu_info;
	cpu_info = cpu_info +"cpu_frequence: "+ nbase::IntToString(GetCPUFreq()) + " MHZ";
	cpu_info = cpu_info + " cpu_producer_id:" + GetManID();
	cpu_info = cpu_info + " cpu_type: " +  GetCPUType();
	return cpu_info;
}

void CpuDevice::ExeCPUID(DWORD veax)
{
	__asm
	{
		mov eax, veax
			cpuid
			mov deax, eax
			mov debx, ebx
			mov decx, ecx
			mov dedx, edx
	}
}

int CpuDevice::GetCPUFreq()
{
	{
		int start1, start2;
		_asm rdtsc
		_asm mov start1, eax
		Sleep(50);
		_asm rdtsc
		_asm mov start2, eax
		return ((start2 - start1) / 50) / (1024);
	}
}

std::string CpuDevice::GetManID()
{
	char ID[25];//存储制造商信息
	memset(ID, 0, sizeof(ID));//先清空数组 ID
	ExeCPUID(0);//初始化
	memcpy(ID + 0, &debx, 4);//制造商信息前四个字符复制到数组
	memcpy(ID + 4, &dedx, 4);//中间四个
	memcpy(ID + 8, &decx, 4);//最后四个
	//如果返回 char * ,会出现乱码;故返回 string 值
	return std::string(ID);
}

std::string CpuDevice::GetCPUType()
{
	const DWORD id = 0x80000002; //从0x80000002开始,到0x80000004结束
	char CPUType[49];//用来存储CPU型号信息
	memset(CPUType, 0, sizeof(CPUType));//初始化数组

	for (DWORD t = 0; t < 3; t++)
	{
		ExeCPUID(id + t);
		//每次循环结束,保存信息到数组
		memcpy(CPUType + 16 * t + 0, &deax, 4);
		memcpy(CPUType + 16 * t + 4, &debx, 4);
		memcpy(CPUType + 16 * t + 8, &decx, 4);
		memcpy(CPUType + 16 * t + 12, &dedx, 4);
	}

	return std::string(CPUType);
}