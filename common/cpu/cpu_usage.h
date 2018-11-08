#pragma once

#include <Windows.h> 
#include <stdint.h>
//ԭ������GetProcessTimes()�������ϴε��õõ��Ľ����������õ�ĳ��ʱ����CPU��ʹ��ʱ��  
//C++ ��ȡ�ض����̹涨CPUʹ����  ԭ�ģ�http://blog.csdn.net/liuqx97bb/article/details/52058657  
class CPUusage 
{
public:
	CPUusage(DWORD process_id);
	CPUusage();
	~CPUusage();

	//����ֵΪ���̾�������ж�OpenProcess�Ƿ�ɹ�  
	HANDLE Setpid(DWORD process_id);

	//-1 ��Ϊʧ�ܻ�������˳��� ����ɹ����״ε��û᷵��-2����;��setpid������PID���״ε���Ҳ�᷵��-2��  
	float GetCpuUsage();

private:
	// ʱ��ת��    
	uint64_t FileTime2Utc(const FILETIME* ftime);

	// ���CPU�ĺ���    
	int GetProcessorNumber();

	//��ʼ��  
	void Init();

	//�رս��̾��  
	void Clear();

private:
	HANDLE hProcess_;
	int processor_;    //cpu����    
	int64_t last_time_;         //��һ�ε�ʱ��    
	int64_t last_system_time_;
};