///----------------------------------------------------------------------
/// RKEngine
///
/// File		:	RKQueue.h
///
/// Description :	Templated queue class
///----------------------------------------------------------------------

// Originally by:
// queue.h	-- Thatcher Ulrich <tu@tulrich.com> 2007
// This source code has been donated to the Public Domain.  Do
// whatever you want with it.
// Very simple std::queue work-alike.

#ifndef _RK_QUEUE_H_
#define _RK_QUEUE_H_

#include "RK.h"
#include "RKMacros.h"
namespace RKUtils
{
	template<class T>
	class RKQueue
	{
	public:

		RKQueue()
		{
			m_size = 0;
			m_back = NULL;
			m_front = NULL;
		}

		~RKQueue()
		{
			Clear();
		}

		void Clear()
		{
			while (Size() > 0)
			{
				Pop();
			}
		}

		const T& Back() const
		{
			assert(m_back);
			return m_back->m_value;
		}

		T& Back()
		{
			assert(m_back);
			return m_back->m_value;
		}

		const T& Front() const
		{
			assert(m_front);
			return m_front->m_value;
		}

		T& Front()
		{
			assert(m_front);
			return m_front->m_value;
		}

		void Pop()
		{
			assert(m_front);
			Elem* to_delete = m_front;
			m_front = m_front->m_next;
			m_size--;
			RKDELETE(to_delete);
			if (m_back == to_delete)
			{
				m_back = NULL;
				assert(m_front == NULL);
				assert(m_size == 0);
			}
		}

		void Push(const T& val)
		{
			if (!m_back)
			{
				assert(!m_front);
				assert(m_size == 0);
				m_back = RKNEW(Elem(val));
				m_front = m_back;
			}
			else
			{
				assert(m_back->m_next == NULL);
				m_back->m_next = RKNEW(Elem(val));
				m_back = m_back->m_next;
			}
			m_size++;
		}

		size_t Size() const
		{
			return m_size;
		}

	private:

		struct Elem
		{
			T m_value;
			Elem* m_next;

			Elem(const T& v) : m_value(v), m_next(NULL) { }
		};

		Elem* m_back;
		Elem* m_front;
		int m_size;
	};
}
#endif // _RK_QUEUE_H_
 
