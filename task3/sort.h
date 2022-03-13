#ifndef SORT_H
#define SORT_H

#include <algorithm>
#include <cstdint>
#include <type_traits>

constexpr int radix = 256;

using uint32 = std::uint_least32_t;

template<typename T>
void create_counters(T* first, T* last, uint32* counters)
{
	unsigned char* beg = (unsigned char*)first;
	unsigned char* end = (unsigned char*)last;

	// проходим исходный массив байт за байтом
	while (beg != end)
	{
		// счетчики дл€ i-ого байта начинаютс€ с radix * i
		// в counters[0..255] находитс€ младший байт числа (т.к. little-endian)
		for (int i = 0; i < sizeof(T); ++i, ++beg)
		{
			++counters[radix * i + *beg];
		}
	}
}

/*
* source      -> input array we want to sort
* output      -> output array sorted by i-th byte
* size        -> size of input array
* counters    -> counters for the specific byte of a number
* byte_offset -> offset to byte by which we sort at this step
*/
template<typename T>
void radix_step(T* source, T* dest, std::size_t size, uint32* counters, int byte_offset)
{
	std::size_t sum = 0;
	//  аждый i-ый эл-т counters будет содержать
	// кол-во чисел в source, которые меньше i
	for (int i = 0; i < radix; ++i)
	{
		sum += counters[i];
		counters[i] = sum - counters[i];
	}

	unsigned char* current_byte = (unsigned char*)source + byte_offset;
	for (int i = 0; i < size; ++i, current_byte += sizeof(T))
	{
		dest[counters[*current_byte]] = source[i];
		++counters[*current_byte];
	}
}

template<typename T>
void radix_sort_negative_step(T* source, T* dest, std::size_t size, uint32* counters, int byte_offset)
{
	std::size_t sum = 0;
	// ѕосчитаем сколько всего отрицательных чисел в массиве
	for (int i = radix / 2; i < radix; ++i)
	{
		sum += counters[i];
	}
	// ѕервые 128 чисел положительные
	// ѕеред ними находитс€ sum отрицательных
	for (int i = 0; i < radix / 2; ++i)
	{
		sum += counters[i];
		counters[i] = sum - counters[i];
	}

	sum = 0;
	for (int i = radix / 2; i < radix; ++i)
	{
		sum += counters[i];
		counters[i] = sum - counters[i];
	}

	unsigned char* current_byte = (unsigned char*)source + byte_offset;
	for (int i = 0; i < size; ++i, current_byte += sizeof(T))
	{
		dest[counters[*current_byte]] = source[i];
		++counters[*current_byte];
	}
}

template<typename T>
void radix_sort(T*& first, T* last)
{
	std::size_t size = last - first;
	T* output = new T[size];
	uint32* counters = new uint32[sizeof(T) * radix]{};
	
	create_counters(first, last, counters);

	int i = 0;
	for (; i < sizeof(T) - 1; ++i)
	{
		if (counters[i * radix] == size) { continue; }

		radix_step(first, output, size, counters + i * radix, i);
		std::swap(first, output);
	}

	radix_sort_negative_step(first, output, size, counters + i * radix, i);
	std::swap(first, output);

	delete[] output;
	delete[] counters;
}

template<typename T>
void insertion_sort(T* first, T* last)
{
	for (int i = 1; i < last - first; ++i)
	{
		for (int j = i; j >= 1 && first[j] < first[j - 1]; --j)
		{
			std::swap(first[j], first[j - 1]);
		}
	}
}

template<typename T>
bool is_sorted(T* first, T* last)
{
	for (int i = 0; i < last - first - 1; ++i)
	{
		if (first[i] > first[i + 1]) { return false; }
	}

	return true;
}

template<typename T>
void sort(T*& first, T* last)
{
	static_assert(std::is_integral<T>::value && std::is_signed<T>::value,
		"sort: T must be a signed integral type");

	if (last - first <= 1 || is_sorted(first, last)) { return; }

	if (last - first <= 64)
	{
		insertion_sort(first, last);
	}
	else
	{
		radix_sort(first, last);
	}
}

#endif // !SORT_H