#ifndef RING_BUF_VAR1_H
#define RING_BUF_VAR1_H

#include <array>
#include <cassert>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>

template<typename T, std::uint_least32_t Size>
class ring_buffer
{
	// For internal use only
	template<typename T>
	static constexpr T nearest_power_of_2(T x)
	{
		static_assert(std::is_unsigned<T>::value && sizeof(T) == 4,
			"nearest_power_of_2 works only with 32-bit unsigned types");

		--x;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;

		return ++x;
	}

	static_assert(Size == nearest_power_of_2(Size),
		"ringbuffer Size is required to be the power of 2.");

	static_assert(Size - 1 <= std::numeric_limits<std::int_least32_t>::max(),
		"Max size for ringbuffer is 2^31");

	using index_t = std::size_t;
public:

	ring_buffer() = default;
	
	void push_back(const T& value)
	{
		if (full()) { return; }

		m_buf[m_head++ & m_mask] = value;
	}

	void push_back()
	{
		if (full()) { return; }

		m_buf[m_head++ & m_mask] = T();
	}

	void push_front(const T& value)
	{
		if (full()) { return; }

		m_buf[--m_tail & m_mask] = value;
	}

	void push_front()
	{
		if (full()) { return; }

		m_buf[--m_tail & m_mask] = T();
	}

	void pop_back()
	{
		if (empty()) { return; }

		--m_head;
	}

	void pop_front()
	{
		if (empty()) { return; }

		++m_tail;
	}

	T& front()
	{
		assert(!empty() && "ringbuffer is empty!");

		return m_buf[m_tail & m_mask];
	}

	T& front() const
	{
		assert(!empty() && "ringbuffer is empty!");

		return m_buf[m_tail & m_mask];
	}

	T& back()
	{
		assert(!empty() && "ringbuffer is empty!");

		return m_buf[m_head - 1 & m_mask];
	}

	T& back() const
	{
		assert(!empty() && "ringbuffer is empty!");

		return m_buf[m_head - 1 & m_mask];
	}

	T& operator[](index_t index)
	{
		return const_cast<T&>(static_cast<const ring_buffer&>(*this)[index]);
	}

	const T& operator[](index_t index) const
	{
		assert(index < size() && "ringbuffer out of bounds");

		return m_buf[m_tail + index & m_mask];
	}

	T& at(index_t index)
	{
		check_position(index);
		return (*this)[index];
	}

	const T& at(index_t index) const
	{
		check_position(index);
		return (*this)[index];
	}

private:
	// For internal use only
	void check_position(index_t index)
	{
		if (index >= size())
		{
			throw std::out_of_range("Ring buffer");
		}
	}

public:

	bool full() const
	{
		return capacity() == size();
	}

	bool empty() const
	{
		return m_head == m_tail;
	}

	index_t size() const
	{
		return m_head - m_tail;
	}

	std::uint_least32_t capacity() const
	{
		return Size;
	}

	void flush(bool clear_buf = true)
	{
		m_head = m_tail;
		if (clear_buf)
		{
			m_buf.fill(T());
		}
	}

private:

	std::array<T, Size> m_buf{};

	index_t m_head = 0;

	index_t m_tail = 0;

	index_t m_mask = Size - 1;
};

#endif // !RING_BUF_VAR1_H