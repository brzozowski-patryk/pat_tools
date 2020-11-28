
#include "../memory_manager/dynamic_memory.h"

int main()
{
	dynamic_memory mem;
	mem.initialize<float>(10);
	return 0;
}