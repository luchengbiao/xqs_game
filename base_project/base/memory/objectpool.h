#ifndef BASE_MEMORY_OBJECT_POOL_H_
#define BASE_MEMORY_OBJECT_POOL_H_
#include <algorithm>
#include <mutex>
#include <stddef.h> // for std::size_t
#include <unordered_map>

namespace nbase
{
	template<typename Object>
	class ObjectPool
	{
		typedef std::pair<Object*, bool> PairType; // false: idle, true: taken out
		typedef std::unordered_map<Object*, bool> PoolType;
		typedef std::recursive_mutex MutexType;

	public:
		// constructor
		explicit ObjectPool(std::size_t reserved = 1, std::size_t increased_per_time = 1, bool thread_safe = false)
			: pool_(new PoolType)
			, increased_per_time_(increased_per_time)
			, mutex_(thread_safe ? new MutexType : nullptr)
		{
		}

		// destructor
		~ObjectPool()
		{
			for (const auto& pair : *pool_)
			{
				delete pair.first;
			}

			delete pool_;

			delete mutex_;
		}

		// delete copy constructor
		ObjectPool(const ObjectPool&) = delete;

		// delete assign operator
		ObjectPool& operator=(const ObjectPool&) = delete;

		// take out an object from pool
		Object* takeOut()
		{
			Object* obj = nullptr;

			{
				std::lock_guard<ObjectPool> lg(*this);

				auto it = std::find_if(pool_->begin(), pool_->end(), [](const PairType& pair) { return !pair.second; });
				if (it != pool_->end())
				{
					it->second = true;

					obj = it->first;
				}
				else
				{
					obj = increaseWithoutLock(increased_per_time_);

					if (obj)
					{
						(*pool_)[obj] = true;
					}
				}
			}

			return obj;
		}

		// give back an object to pool
		void giveBack(Object* obj)
		{
			if (obj)
			{
				std::lock_guard<ObjectPool> lg(*this);

				auto it = pool_->find(obj);
				if (it != pool_->end())
				{
					it->second = false;
				}
			}
		}

	private:
		void lock()
		{
			if (mutex_) { mutex_->lock(); }
		}

		void unlock()
		{
			if (mutex_) { mutex_->unlock(); }
		}

		Object* increaseWithoutLock(std::size_t count)
		{
			Object* ret = nullptr;

			while (count > 0)
			{
				count -= 1;

				auto obj = new (std::nothrow) Object;

				if (obj)
				{
					ret = obj;
					pool_->emplace(obj, false);
				}
			}

			return ret;
		}

	private:
		PoolType*			pool_;
		std::size_t			increased_per_time_;
		mutable MutexType*	mutex_;

		friend class std::lock_guard<ObjectPool>;
	};
}

#endif