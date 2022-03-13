#ifndef IS_EVEN_H
#define IS_EVEN_H

bool isEven(int value)
{
	return value % 2 == 0;
}

bool isEven2(int value)
{
	return !(value & 1);
}

#endif // !IS_EVEN_H

