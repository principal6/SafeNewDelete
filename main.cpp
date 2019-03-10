#include "SafeNewDelete.h"

int main()
{
	int* a{};
	new_s(a);
	delete_s(a);

	float* b{};
	new_init_s(b, 1.f); // Convert this line into comment to make assertion fail.
	//delete_s(b);
	//delete_s(b);

	long long* c{};
	new_init_s(c, 1234);
	//delete_s(c);

	return 0;
}