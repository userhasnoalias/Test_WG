#include "is_even.h"
#include <iostream>

int main()
{
	int a;
	std::cin >> a;
	
	std::cout << isEven(a);
	std::cout << isEven2(a);

	return 0;
}