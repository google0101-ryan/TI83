#include <src/bus.h>
#include <src/cpu/cpu.hpp>

#include <cstdint>
#include <cstdio>
#include <cstdlib>

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("Usage: %s <os rom>\n", argv[0]);
		return -1;
	}

	Bus* bus = new Bus(argv[1]);

	CPU* cpu = new CPU(bus);

	while (1)
		cpu->clock();
}