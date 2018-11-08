#include "cpu_memory_detect.h"
#include <windows.h>  
#include <TlHelp32.h> 
#include <Psapi.h>
#include "cpu_usage.h"
#include <algorithm>
#include "base\thread\thread_manager.h"

#pragma comment(lib,"Psapi.lib")
DetectSystemManager::DetectSystemManager()
{
	process_id_ = -1;
	b_checking_ = false;
}

DetectSystemManager::~DetectSystemManager()
{

}

void DetectSystemManager::Entry()
{
	bool success = AdjustPurview();
	if (!b_checking_ || !TraverseProcesses())
	{
		return;
	}

}

CpuResult DetectSystemManager::GetTaskResult(int process_id)
{
	CpuResult res;
	if (b_checking_)
	{
		return res;
	}
	b_checking_ = true;
	process_id_ = process_id;
	Entry();
	res.current_process_ = current_process_;
	CpuProcess pro = GetMaxMemory();
	res.worst_pro = pro;
	double total_cpu = 0;//��CPU
	int success_count = 0;
	for (int i = 0; i < 5 && b_checking_; i++)
	{
		Sleep(1000);
		double val = TotalCPU();
		if (val > 0.001)
		{
			total_cpu += val;
			success_count++;
		}
	}
	if (b_checking_)
	{
		total_cpu /= success_count;
		res.total_cpu = total_cpu;
		res.pro_ = pro_;
	}
	b_checking_ = false;

	MEMORYSTATUSEX stMem = { 0 };
	stMem.dwLength = sizeof(stMem);
	GlobalMemoryStatusEx(&stMem);
	res.memory_percent_ = stMem.dwMemoryLoad;
	unsigned long long phys = stMem.ullTotalPhys / (1024 * 1024);
	res.total_memory_ = (int)phys;
	return res;
}

bool DetectSystemManager::AdjustPurview()
{
	//����Ȩ��
	HANDLE hToken = NULL;
	//�򿪵�ǰ���̵ķ�������
	int hRet = OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);
	if (hRet)
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		//ȡ������Ȩ�޵�LUID
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		//�����������Ƶ�Ȩ��
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		CloseHandle(hToken);
	}
	return true;
}

bool DetectSystemManager::TraverseProcesses()
{
	//�������еĽ��̵��ڴ棬�Լ��������CPUռ��
	//CLEAR
	pro_.clear();

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//��ȡ���̿���  
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		return false;
	}
	BOOL bResult = Process32First(hProcessSnap, &pe32);

	int num(0);
	
	while (bResult && b_checking_)
	{
		//string name = string(pe32.szExeFile);
		CpuProcess pro_item;
		char temp[300];
		WideCharToMultiByte(CP_ACP, 0, pe32.szExeFile, -1, temp, sizeof(temp), NULL, NULL);
		pro_item.name_ = std::string(temp);
		pro_item.id_ = pe32.th32ProcessID;
		HANDLE hprocess = NULL;

		hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pro_item.id_);
		PROCESS_MEMORY_COUNTERS pmc;
		GetProcessMemoryInfo(hprocess, &pmc, sizeof(pmc));

		PERFORMANCE_INFORMATION performanceInfo;
		memset(&performanceInfo, 0, sizeof(performanceInfo));

		if (!::GetPerformanceInfo(&performanceInfo, sizeof(performanceInfo)))
		{
			return false;
		}

		long long m_stPageSize = performanceInfo.PageSize;
		pro_item.memory_= CalcPrivateWorkSet(hprocess, m_stPageSize) / 1024 / 1024;
		if (pro_item.id_ == process_id_)
		{
			CPUusage usage;
			usage.Setpid(pro_item.id_);
			int success_count = 0;
			for (int i = 0; i < 5 && b_checking_; i++)
			{
				Sleep(1000);
				//��һ�μ��������ϵģ�ֵΪ-1�Ǽ���ʧ��
				float val = usage.GetCpuUsage();
				if (val > 0.001) {
					pro_item.CPU_ += val;
					success_count++;
				}
			}
			if (success_count > 0) {
				pro_item.CPU_ /= success_count;
			}
			//����5��ˢ��
			if (pro_item.CPU_ <= 0)
			{
				pro_item.CPU_ = 0;
			}
			current_process_ = pro_item;
		}
		bResult = Process32Next(hProcessSnap, &pe32);
		pro_.push_back(pro_item);
	}
	CloseHandle(hProcessSnap);

	return true;
}

__int64 DetectSystemManager::CalcPrivateWorkSet(HANDLE hProcess, __int64 page)
{
	BOOL bRet = TRUE;
	PSAPI_WORKING_SET_INFORMATION workSetInfo;
	PBYTE pByte = NULL;
	PSAPI_WORKING_SET_BLOCK *pWorkSetBlock = workSetInfo.WorkingSetInfo;
	memset(&workSetInfo, 0, sizeof(workSetInfo));
	bRet = ::QueryWorkingSet(hProcess, &workSetInfo, sizeof(workSetInfo));
	if (!bRet) // ����ʧ��
	{
		if (GetLastError() == ERROR_BAD_LENGTH) // ��Ҫ���·��仺����
		{
			DWORD realSize = sizeof(workSetInfo.NumberOfEntries)
				+ workSetInfo.NumberOfEntries*sizeof(PSAPI_WORKING_SET_BLOCK);
			try
			{
				pByte = new BYTE[realSize];
				memset(pByte, 0, realSize);
				pWorkSetBlock = (PSAPI_WORKING_SET_BLOCK *)(pByte + sizeof(workSetInfo.NumberOfEntries));
				// ���»�ȡ
				if (!::QueryWorkingSet(hProcess, pByte, realSize))
				{
					delete[] pByte; // �����ڴ�
					pByte = NULL;
					return 0;
				}
			}
			catch (char *e) // �����ڴ�ʧ��
			{
				return 0;
			}

		}
		else // ����������Ϊ��ȡʧ��
		{
			return 0;
		}
	}

	int64_t workSetPrivate = 0;
	for (int64_t i = 0; i < workSetInfo.NumberOfEntries; ++i)
	{
		if (!pWorkSetBlock[i].Shared) // ������ǹ���ҳ
		{
			workSetPrivate += page;
		}
	}
	if (pByte)
	{
		delete[] pByte;
		pByte = NULL;
	}
	return workSetPrivate;

}

CpuProcess DetectSystemManager::GetMaxMemory()
{
	CpuProcess pro;
	int memory_ = 0;
	std::vector<CpuProcess>::iterator it;
	for (it = pro_.begin(); it != pro_.end() && b_checking_; it++)
	{
		CpuProcess temp;
		temp = *it;
		if (temp.memory_ >= memory_)
		{
			pro = temp;
			memory_ = temp.memory_;
		}
	}
	
	return pro;
}

double DetectSystemManager::TotalCPU()
{
	FILETIME idleTime;
	FILETIME kernelTime;
	FILETIME userTime;
	double res = 0;
	GetSystemTimes(&idleTime, &kernelTime, &userTime);

	int idle = CompareFileTime2(m_preidleTime_, idleTime);
	int kernel = CompareFileTime2(m_prekernelTime_, kernelTime);
	int user = CompareFileTime2(m_preuserTime_, userTime);
	if (kernel + user == 0)
		return res;

	res = abs((kernel + user - idle) * 100 / (kernel + user));
	m_preidleTime_ = idleTime;
	m_prekernelTime_ = kernelTime;
	m_preuserTime_ = userTime;

	return res;
}

__int64 DetectSystemManager::CompareFileTime2(FILETIME time1, FILETIME time2)
{
	__int64 a = time1.dwHighDateTime << 32 | time1.dwLowDateTime;
	__int64 b = time2.dwHighDateTime << 32 | time2.dwLowDateTime;
	return   b - a;
}

void DetectSystemManager::StopTask()
{
	b_checking_ = false;
}
