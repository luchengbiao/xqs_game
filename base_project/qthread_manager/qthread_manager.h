#pragma once

#include <map>
#include <memory>
#include "base/memory/singleton.h"
#include "framework_qthread.h"

namespace nbase
{
	class MessageLoop;
	class MessageLoopProxy;

	class QThreadMap
	{
	public:
		SINGLETON_DEFINE(QThreadMap);
		static bool AquireAccess();

		bool RegisterThread(int self_identifier);
		bool UnregisterThread();
		int QueryThreadId(const FrameworkQThread *thread);
		std::shared_ptr<MessageLoopProxy> GetMessageLoop(int identifier) const;
		FrameworkQThread* QueryThreadInternal(int identifier) const;

	private:
		QThreadMap() { }
		NLock lock_;
		std::map<int, FrameworkQThread*> threads_;
	};

	// ʹ��ThreadManager���Լ���ط����̼߳�ͨ��
	// ע�⣺ֻ����ThreadManager�йܵ��̣߳�ͨ��Register�йܣ���������ó�Register��Post����ĳ�Ա����
	class BASE_EXPORT QThreadManager
	{
	public:

		// �йܵ�ǰFrameworkQThread�߳�
		// identifier >= 0
		// ������self���̹߳����ڱ�����
		static bool RegisterThread(int self_identifier);
		// ȡ����ǰ�߳��й�
		// �߳����н���֮ǰ�������UnregisterThreadȡ���й�
		static bool UnregisterThread();

		// ����
		static FrameworkQThread* CurrentThread();
		template<typename T> static T* CurrentThreadT();
		static int QueryThreadId(const FrameworkQThread *thread);

		static bool PostTask(int identifier, const StdClosure &task);

		static bool PostDelayedTask(int identifier, const StdClosure &task, TimeDelta delay);

		static const int TIMES_FOREVER = -1;
		static void PostRepeatedTask(int thread_id, const WeakCallback<StdClosure>& task, const TimeDelta& delay, int times = TIMES_FOREVER);

		template<typename T1, typename T2>
		static bool Await(int identifier, const std::function<T1> &task, const std::function<T2> &reply)
		{
			std::shared_ptr<MessageLoopProxy> message_loop =
				ThreadMap::GetInstance()->GetMessageLoop(identifier);
			if (message_loop == NULL)
				return false;
			message_loop->PostTaskAndReply(task, reply);
			return true;
		}

	private:
		static void RunRepeatedly2(int thread_id, const WeakCallback<StdClosure>& task, const TimeDelta& delay, int times);

		DISALLOW_COPY_AND_ASSIGN(QThreadManager);
	};

	template<typename T>
	T* QThreadManager::CurrentThreadT()
	{
		return static_cast<T *>(CurrentThread());
	}

}

