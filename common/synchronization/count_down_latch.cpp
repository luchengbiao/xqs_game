#include "count_down_latch.h"
#include <mutex>
#include <condition_variable>

template<typename V>
class ValueGuard
{
public:
	ValueGuard(V& v) : _v(v) { _v += 1; }
	~ValueGuard() { _v -= 1; }

private:
	V& _v;
};

class CountdownLatch::CountdownLatchImpl
{
public:
	CountdownLatchImpl()
		: _count(0)
		, _waiters(0)
		, _notifyReason(NotifyReason::None)
	{}

	inline void	Reset(unsigned int count)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_count = count;

		if (_waiters > 0)
		{
			_notifyReason = NotifyReason::Reset;

			_condition.notify_all();
		}
	}

	inline void	Wait()
	{
		std::unique_lock<std::mutex> lock(_mutex);

		ValueGuard<unsigned int> vg(_waiters);

		while (_count > 0)
		{
			_condition.wait(lock);
		}
	}

	inline bool	Countdown()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_count -= 1;

		if (_count == 0)
		{
			if (_waiters > 0)
			{
				_notifyReason = NotifyReason::CountdownedToZero;

				_condition.notify_all();
			}

			return true;
		}

		return false;
	}

	inline unsigned int CurrentCounter() const
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _count;
	}

	inline NotifyReason	GetNotifyReason() const
	{
		return _notifyReason;
	}

private:
	mutable std::mutex		_mutex;
	std::condition_variable	_condition;
	unsigned int			_count;
	unsigned int			_waiters;
	NotifyReason			_notifyReason;
};

CountdownLatch::CountdownLatch()
	: _impl(new CountdownLatchImpl)
{}

CountdownLatch::~CountdownLatch()
{
	delete _impl;
}

void CountdownLatch::Reset(unsigned int count)
{
	_impl->Reset(count);
}

void  CountdownLatch::Wait()
{
	_impl->Wait();
}

bool  CountdownLatch::Countdown()
{
	return _impl->Countdown();;
}

unsigned int CountdownLatch::CurrentCounter() const
{
	return _impl->CurrentCounter();
}

bool CountdownLatch::CurrentCounterIsZero() const
{
	return _impl->CurrentCounter() == 0;
}

CountdownLatch::NotifyReason CountdownLatch::GetNotifyReason() const
{
	return _impl->GetNotifyReason();
}
