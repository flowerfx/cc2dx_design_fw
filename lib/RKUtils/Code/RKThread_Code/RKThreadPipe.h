#ifndef __RKTHREADPIPE_H_
#define __RKTHREADPIPE_H_
namespace RKUtils
{
	extern RKThread_ReadBarrier();
	extern RKThread_WriteBarrier();

	/***
	 * Non-blocking pipe meant for inter-thread communication. The caveat for usage
	 * is that there is a single producer and a single consumer.
	 */
	template <class T>
	class RKThreadPipe
	{
		T* m_queue;
		volatile uint32 m_head;
		volatile uint32 m_tail;
		uint32 m_capacity;
	public:
		RKThreadPipe() :
			m_queue(0)
			, m_head(0)
			, m_tail(0)
			, m_size(0)
		{
		}

		~RKThreadPipe()
		{
		}

		// This function is not thread-safe
		void SetCapacity(size_t capacity)
		{
			if (m_queue)
			{
				RKDELETEARRAY(m_queue);
			}
			m_capacity = capacity;
			m_queue = RKNEWARRAY(T, size);
			m_head = 0;
			m_tail = 0;
		}

		inline uint32 GetSize()
		{
			return m_tail - m_head;
		}

		bool Push(const T& item)
		{
			if (GetSize() < capacity)
			{
				uint32 index = m_tail % capacity;
				m_queue[index] = item;
				RKThread_WriteBarrier();
				++m_tail;
				return true;
			}
			return false;
		}

		bool Pop(T& item)
		{
			if (GetSize() > 0)
			{
				uint32 index = m_head % capacity;
				item = m_queue[index];
				RKThread_ReadBarrier();
				++m_head;
				return true;
			}
		}
	};
}
#endif
