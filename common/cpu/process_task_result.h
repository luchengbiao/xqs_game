#pragma once
#include <string>
#include <vector>

struct CpuProcess
{
	int id_;
	int memory_;//��λΪMB;
	float CPU_;	//CPUռ����ƽ��ֵ
	std::string name_;
	
	CpuProcess() : id_(0), memory_(0), CPU_(0), name_("")
	{

	}

	bool operator<(const CpuProcess& other) const
	{
		return memory_ > other.memory_;
	}
};

struct CpuResult
{
	CpuProcess current_process_;	//��������ѧ�Ե��ڴ��CPUռ����
	double total_cpu;	//�ܵ�CPU
	CpuProcess worst_pro;	//ռ���ڴ����Ľ���
	std::vector<CpuProcess> pro_;//���н���
	int total_memory_;	//ϵͳ�ڴ�
	int memory_percent_; //�ڴ�ʹ����
	CpuResult() : total_cpu(0), total_memory_(0), memory_percent_(0)
	{

	}
};
typedef std::function<void(int, std::wstring, CpuResult)> API_GetCpuResCb;