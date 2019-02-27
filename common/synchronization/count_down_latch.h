#ifndef __COUNTDOWN_LATCH_H__
#define __COUNTDOWN_LATCH_H__

class CountdownLatch
{
public:
	enum class NotifyReason
	{
		None,
		CountdownedToZero,
		Reset,
	};

public:
	CountdownLatch();
	~CountdownLatch();

	CountdownLatch(const CountdownLatch&) = delete;
	CountdownLatch& operator=(const CountdownLatch&) = delete;

	void			Reset(unsigned int count);

	void			Wait();

	bool			Countdown();

	unsigned int	CurrentCounter() const;

	bool			CurrentCounterIsZero() const;

	NotifyReason	GetNotifyReason() const;

private:
	class CountdownLatchImpl;
	CountdownLatchImpl* _impl;
};

#endif