#ifndef RING_BUF_VAR2_H
#define RING_BUF_VAR2_H

#include <cassert>
#include <initializer_list>
#include <memory>
#include <mutex>
#include <type_traits>

template<typename T>
class ring_buffer
{
	using index_t = std::size_t;

public:

	ring_buffer(std::size_t size) : m_buf{ std::make_unique<T[]>(size) }, m_max_size{ size }
	{
		assert(size > 0 && "ring_buffer size should be more than 0");
	}

	ring_buffer(std::initializer_list<T> list) : ring_buffer(list.size())
	{
		for (const auto& value : list)
		{
			m_buf[m_count++] = value;
		}
	}

	void put(const T& value)
	{
		if (capacity() == 0) { return; }

		std::lock_guard<std::mutex> lock(m_mutex);

		m_buf[m_head] = value;
		m_head = modulo_inc(m_head, m_max_size);

		if (full())
		{
			m_tail = modulo_inc(m_tail, m_max_size);
		}
		else
		{
			++m_count;
		}
	}

	bool get(T& out_value)
	{
		if (empty()) { return false; }

		std::lock_guard<std::mutex> lock(m_mutex);

		out_value = m_buf[m_tail];
		m_tail = modulo_inc(m_tail, m_max_size);
		--m_count;

		return true;
	}

private:
	
	template<typename T>
	static constexpr T modulo_inc(T value, T mod)
	{
		static_assert(std::is_integral<T>::value, "modulo_inc: T must be of integral type");

		return (value + 1) % mod;
	}

public:

	void clear()
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		m_head = m_tail;
		m_count = 0;
	}

	bool full() const
	{
		return m_count == m_max_size;
	}

	bool empty() const
	{
		return m_count == 0;
	}

	index_t size() const
	{
		return m_count;
	}

	std::size_t capacity() const
	{
		return m_max_size;
	}

private:

	std::mutex m_mutex;

	std::unique_ptr<T[]> m_buf;

	index_t m_head = 0;

	index_t m_tail = 0;
	// Total number of elements in buffer
	std::size_t m_count = 0;

	std::size_t m_max_size;
};

#endif // !RING_BUF_VAR2_H