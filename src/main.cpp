#include <src/bus.h>
#include <src/cpu/cpu.hpp>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <csignal>

Bus* bus;

void Dump()
{
	bus->Dump();
}

void sig(int)
{
	Dump();
	exit(1);
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("Usage: %s <os rom>\n", argv[0]);
		return -1;
	}

	bus = new Bus(argv[1]);

	std::atexit(Dump);
	std::signal(SIGINT, sig);

	CPU* cpu = new CPU(bus);

	while (1)
		cpu->clock();
}