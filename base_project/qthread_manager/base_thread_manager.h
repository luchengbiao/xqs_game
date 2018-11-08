#pragma once
#include "base_task_thread.h"
#include <memory>
#include "base\memory\singleton.h"

class BaseThreadManager
{
public:
	SINGLETON_DEFINE(BaseThreadManager);
	BaseThreadManager(){}
	virtual ~BaseThreadManager() {}

	void Init();
	/**
	* ���߳��˳�ʱ����һЩ������
	* @return void	�޷���ֵ
	*/
	void Cleanup();

private:
	/**
	* ���߳̿�ʼѭ��ǰ��misc�̺߳�db�߳��ȿ�ʼѭ��
	* @return void	�޷���ֵ
	*/
	void PreMessageLoop();

	/**
	* ���߳̽���ѭ��ǰ��misc�̺߳�db�߳��Ƚ���ѭ��
	* @return void	�޷���ֵ
	*/
	void PostMessageLoop();
private:
	std::unique_ptr<BaseTaskThread>	  api_task_thread_;
	std::unique_ptr<BaseTaskThread>	  heartbeat_task_thread_;
	std::unique_ptr<BaseTaskThread>	  http_task_thread_;
	std::unique_ptr<BaseTaskThread>	  uihelper_task_thread_;
	std::unique_ptr<BaseTaskThread>	  more_task_helper_thread_;
	std::unique_ptr<BaseTaskThread>	  ws_task_thread_;
};