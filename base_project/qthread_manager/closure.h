#pragma once
#include "base\callback\callback.h"
#include "base\time\time.h"

enum ThreadId
{
	kThreadApiTask = 0,	//api�����߳�
	kThreadHeartBeat, // ���������߳�
	kThreadHttpTask, // http �ص������߳�
	kThreadUIHelper, // ֪ͨ���̵߳ĸ����߳�
	kThreadMoreTaskHelper, // ���ඨʱ����֪ͨ���̵߳ĸ����߳�
	kThreadWebSocket, // ����websocket ����ʱ����߳�
};

namespace qtbase
{
	/** @fn Post2Task(int identifier, const QtTaskClosure &closure)
	* �Ѻ����ŵ�ĳ���߳�ִ��
	* @param[in] identifier �̵߳�id
	* @param[in] closure ��ִ�еĺ���
	* @return void �޷���ֵ
	*/
	void Post2Task(int identifier, const StdClosure &closure);

	/** @fn void Post2DelayedTask(int identifier, const QtTaskClosure &closure, int delay)
	* �Ѻ����ŵ�ĳ���߳���ʱִ��
	* @param[in] identifier �̵߳�id
	* @param[in] closure ��ִ�еĺ���
	* @param[in] delay ��ʱִ�е�ʱ��
	* @return void �޷���ֵ
	*/
	void Post2DelayedTask(int identifier, const StdClosure &closure, nbase::TimeDelta delay);

	/** @fn void Post2RepeatedTask(int identifier, const QtTaskClosure &closure, int delay)
	* �Ѻ����ŵ�ĳ���߳�ѭ��ִ��
	* @param[in] identifier �̵߳�id
	* @param[in] closure ��ִ�еĺ���
	* @param[in] delay ÿ��ѭ��ִ�еļ��
	* @return void �޷���ֵ
	*/
	void Post2RepeatedTask(int identifier, const nbase::WeakCallback<StdClosure> &closure, nbase::TimeDelta delay);
}