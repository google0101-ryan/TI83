#include <src/cpu/cpu.hpp>
#include "cpu.hpp"

uint8_t CPU::ReadImm8(uint16_t& addr)
{
	return bus->read8(addr++);
}

uint16_t CPU::ReadImm16(uint16_t &addr)
{
	uint16_t data = bus->read16(addr);
	addr += 2;
	return data;
}

CPU::CPU(Bus *bus)
	: bus(bus)
{
	pc = 0;
	sp = 0;
	af = bc = de = hl = 0;
}

void CPU::Dump()
{
	printf("[emu/CPU]: a\t->\t0x%02x\n", a);
	printf("[emu/CPU]: f\t->\t0x%02x\n", f);
	printf("[emu/CPU]: b\t->\t0x%02x\n", b);
	printf("[emu/CPU]: c\t->\t0x%02x\n", c);
	printf("[emu/CPU]: d\t->\t0x%02x\n", d);
	printf("[emu/CPU]: e\t->\t0x%02x\n", e);
	printf("[emu/CPU]: h\t->\t0x%02x\n", h);
	printf("[emu/CPU]: l\t->\t0x%02x\n", l);
	printf("[emu/CPU]: pc\t->\t0x%04x\n", pc);
	printf("[emu/CPU]: sp\t->\t0x%04x\n", sp);
}

void CPU::clock()
{
	uint8_t opcode = ReadImm8(pc);

	switch (opcode)
	{
	case 0x0e:
		ld_c_n();
		break;
	case 0x21:
		ld_hl_nn();
		break;
	case 0x3e:
		ld_a_n();
		break;
	case 0xaf:
		xor_a();
		break;
	case 0xc3:
		jp_nn();
		break;
	case 0xc5:
		push_bc();
		break;
	case 0xcd:
		call_nn();
		break;
	case 0xd3:
		out_n_a();
		break;
	case 0xf3:
		di();
		break;
	case 0xe5:
		push_hl();
		break;
	case 0xf9:
		ld_sp_hl();
		break;
	default:
		printf("[emu/CPU]: Unknown opcode 0x%02x\n", opcode);
		Dump();
		exit(1);
	}
}

void CPU::ld_c_n()
{
	c = ReadImm8(pc);
	printf("ld c, 0x%02x\n", c);
}

void CPU::ld_hl_nn()
{
	hl = ReadImm16(pc);
	printf("ld hl, 0x%04x\n", hl);
}

void CPU::ld_a_n()
{
	a = ReadImm8(pc);
	printf("ld a, 0x%02x\n", a);
}

void CPU::xor_a()
{
	a = 0;

	SetFlag(C_FLAG, false);
	SetFlag(N_FLAG, false);
	SetFlag(P_FLAG, false);
	SetFlag(H_FLAG, false);
	SetFlag(Z_FLAG, true);
	SetFlag(S_FLAG, false);

	printf("xor a\n");
}

void CPU::jp_nn()
{
	uint16_t new_pc = ReadImm16(pc);
	pc = new_pc;

	if ((pc & 0xFF00) >= 0x3F00)
		bus->remap_flash();

	printf("jp 0x%04x\n", new_pc);
}

void CPU::push_bc()
{
	sp--;
	bus->write8(sp, c);
	sp--;
	bus->write8(sp, b);

	printf("push bc\n");
}

void CPU::call_nn()
{
	uint16_t new_pc = ReadImm16(pc);

	sp -= 2;
	bus->write16(sp, pc);

	pc = new_pc;

	printf("call 0x%04x\n", new_pc);
}

void CPU::out_n_a()
{
	uint8_t port = ReadImm8(pc);

	bus->write8_io(port, a);

	printf("out (%02x), a\n", port);
}

void CPU::push_hl()
{
	sp--;
	bus->write8(sp, l);
	sp--;
	bus->write8(sp, h);

	printf("push hl\n");
}

void CPU::di()
{
	iff1 = iff2 = false;
	printf("di\n");
}

void CPU::ld_sp_hl()
{
	sp = hl;
	printf("ld sp, hl\n");
}
