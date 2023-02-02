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
	printf("[emu/CPU]: ix\t->\t0x%04x\n", ix);
	printf("[emu/CPU]: iy\t->\t0x%04x\n", iy);
	printf("[emu/CPU]: pc\t->\t0x%04x\n", pc);
	printf("[emu/CPU]: sp\t->\t0x%04x\n", sp);
}

void CPU::clock()
{
	uint8_t opcode = ReadImm8(pc);

	switch (opcode)
	{
	case 0x00:
		printf("nop\n");
		break;
	case 0x01:
		ld_bc_nn();
		break;
	case 0x05:
		dec_b();
		break;
	case 0x06:
		ld_b_n();
		break;
	case 0x0e:
		ld_c_n();
		break;
	case 0x10:
		djnz();
		break;
	case 0x21:
		ld_hl_nn();
		break;
	case 0x28:
		jr_z_d();
		break;
	case 0x29:
		add_hl_hl();
		break;
	case 0x2a:
		ld_hl_mm();
		break;
	case 0x2c:
		inc_l();
		break;
	case 0x30:
		jr_nc_d();
		break;
	case 0x31:
		ld_sp_nn();
		break;
	case 0x3e:
		ld_a_n();
		break;
	case 0x47:
		ld_b_a();
		break;
	case 0x78:
		ld_a_b();
		break;
	case 0x7c:
		ld_a_h();
		break;
	case 0x7d:
		ld_a_l();
		break;
	case 0xaf:
		xor_a();
		break;
	case 0xb7:
		or_a();
		break;
	case 0xc0:
		ret_nz();
		break;
	case 0xc1:
		pop_bc();
		break;
	case 0xc3:
		jp_nn();
		break;
	case 0xc5:
		push_bc();
		break;
	case 0xc9:
		ret();
		break;
	case 0xca:
		jp_z_nn();
		break;
	case 0xcd:
		call_nn();
		break;
	case 0xd3:
		out_n_a();
		break;
	case 0xd6:
		sub_n();
		break;
	case 0xd8:
		ret_c();
		break;
	case 0xdb:
		in_a_n();
		break;
	case 0xdd:
	{
		opcode = ReadImm8(pc);
		switch (opcode)
		{
		case 0x21:
			ld_ix_nn();
			break;
		case 0x39:
			add_ix_sp();
			break;
		case 0xf9:
			ld_sp_ix();
			break;
		default:
			printf("[emu/CPU]: Unknown IX opcode 0xdd 0x%02x\n", opcode);
			Dump();
			exit(1);
		}
		break;
	}
	case 0xe5:
		push_hl();
		break;
	case 0xe6:
		and_n();
		break;
	case 0xed:
	{
		opcode = ReadImm8(pc);
		switch (opcode)
		{
		case 0x42:
			sbc_hl_bc();
			break;
		case 0x56:
			im_1();
			break;
		case 0x70:
			in_c();
			break;
		default:
			printf("[emu/CPU]: Unknown misc. opcode 0xed 0x%02x\n", opcode);
			Dump();
			exit(1);
		}
		break;
	}
	case 0xee:
		xor_n();
		break;
	case 0xf1:
		pop_af();
		break;
	case 0xf3:
		di();
		break;
	case 0xf5:
		push_af();
		break;
	case 0xf9:
		ld_sp_hl();
		break;
	case 0xfa:
		jp_m_nn();
		break;
	default:
		printf("[emu/CPU]: Unknown opcode 0x%02x\n", opcode);
		Dump();
		exit(1);
	}
}

void CPU::ld_bc_nn()
{
	bc = ReadImm16(pc);
	printf("ld bc, 0x%04x\n", bc);
}

void CPU::dec_b()
{
	uint16_t result = b - 1;

	int carryx = b ^ 1 ^ (result & 0xff);

	SetFlag(N_FLAG, true);
	SetFlag(P_FLAG, ((carryx << 1) ^ carryx) & 0x100);
	SetFlag(H_FLAG, carryx & 0x10);
	SetFlag(Z_FLAG, (result & 0xff) == 0);
	SetFlag(S_FLAG, (result >> 8) & 1);

	b = result;

	printf("dec b\n");
}

void CPU::ld_b_n()
{
	b = ReadImm8(pc);
	printf("ld b, 0x%02x\n", b);
}

void CPU::ld_c_n()
{
	c = ReadImm8(pc);
	printf("ld c, 0x%02x\n", c);
}

void CPU::djnz()
{
	int8_t disp = ReadImm8(pc);
	
	printf("djnz 0x%04x\n", pc + disp);

	b--;
	if (b != 0)
	{
		pc += disp;
	}
}

void CPU::ld_hl_nn()
{
	hl = ReadImm16(pc);
	printf("ld hl, 0x%04x\n", hl);
}

void CPU::jr_z_d()
{
	int8_t d = ReadImm8(pc);

	printf("jr z, 0x%04x\n", pc + d);

	if (GetFlag(Z_FLAG))
	{
		pc += d;
	}
}

void CPU::add_hl_hl()
{
	uint32_t result = hl + hl;

	int carryx = hl ^ hl ^ (result & 0xffff);

	SetFlag(C_FLAG, result >> 16);
	SetFlag(N_FLAG, 0);
	SetFlag(P_FLAG, ((carryx << 1) ^ carryx) & 0x100);
	SetFlag(H_FLAG, carryx & 0x10);
	SetFlag(Z_FLAG, (result & 0xffff) == 0);
	SetFlag(S_FLAG, (result >> 16) & 1);

	hl = result;

	printf("add hl, hl\n");
}

void CPU::ld_hl_mm()
{
	uint16_t addr = ReadImm16(pc);
	hl = bus->read16(addr);

	printf("ld hl, (%04x)\n", addr);
}

void CPU::inc_l()
{
	uint16_t result = l + 1;

	int carryx = l ^ 1 ^ (result & 0xff);

	SetFlag(N_FLAG, 0);
	SetFlag(P_FLAG, ((carryx << 1) ^ carryx) & 0x100);
	SetFlag(H_FLAG, carryx & 0x10);
	SetFlag(Z_FLAG, (result & 0xff) == 0);
	SetFlag(S_FLAG, (result >> 8) & 1);

	l = result;

	printf("inc l\n");
}

void CPU::jr_nc_d()
{
	int8_t d = ReadImm8(pc);

	printf("jr nc, 0x%04x\n", pc + d);

	if (!GetFlag(C_FLAG))
	{
		pc += d;
	}
}

void CPU::ld_sp_nn()
{
	sp = ReadImm16(pc);
	printf("ld sp, 0x%04x\n", sp);
}

void CPU::ld_a_n()
{
	a = ReadImm8(pc);
	printf("ld a, 0x%02x\n", a);
}

void CPU::ld_b_a()
{
	b = a;
	printf("ld b, a\n");
}

void CPU::ld_a_b()
{
	a = b;
	printf("ld a, b\n");
}

void CPU::ld_a_h()
{
	a = h;
	printf("ld a, h\n");
}

void CPU::ld_a_l()
{
	a = l;
	printf("ld a, l\n");
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

void CPU::or_a()
{
	SetFlag(C_FLAG, false);
	SetFlag(N_FLAG, false);
	SetFlag(P_FLAG, false);
	SetFlag(H_FLAG, false);
	SetFlag(Z_FLAG, a == 0);
	SetFlag(S_FLAG, a & (1 << 7));

	printf("or a\n");
}

void CPU::ret_nz()
{
	if (!GetFlag(Z_FLAG))
	{
		pc = bus->read16(sp);
		sp += 2;	
	}

	printf("ret nz\n");
}

void CPU::pop_bc()
{
	bc = bus->read16(sp);
	sp += 2;

	printf("pop bc\n");
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

void CPU::ret()
{
	pc = bus->read16(sp);
	sp += 2;

	printf("ret\n");
}

void CPU::jp_z_nn()
{
	uint16_t new_pc = ReadImm16(pc);

	if (GetFlag(Z_FLAG))
		pc = new_pc;
	
	printf("jp z, 0x%04x\n", new_pc);
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

void CPU::sub_n()
{
	uint8_t n = ReadImm8(pc);

	uint16_t result = a - n;

	int carryx = a ^ n ^ (result & 0xff);

	SetFlag(C_FLAG, result >> 8);
	SetFlag(N_FLAG, 1);
	SetFlag(P_FLAG, ((carryx << 1) ^ carryx) & 0x100);
	SetFlag(H_FLAG, carryx & 0x10);
	SetFlag(Z_FLAG, (result & 0xff) == 0);
	SetFlag(S_FLAG, (result >> 8) & 1);

	a = result;

	printf("sub 0x%02x\n", n);
}

void CPU::ret_c()
{
	if (GetFlag(C_FLAG))
	{
		pc = bus->read16(sp);
		sp += 2;
	}

	printf("ret c\n");
}

void CPU::in_a_n()
{
	uint8_t port = ReadImm8(pc);
	a = bus->read8_io(port);

	printf("in a, (%02x)\n", port);
}

void CPU::push_hl()
{
	sp--;
	bus->write8(sp, l);
	sp--;
	bus->write8(sp, h);

	printf("push hl\n");
}

void CPU::and_n()
{
	uint8_t n = ReadImm8(pc);

	a = a & n;

	SetFlag(C_FLAG, false);
	SetFlag(N_FLAG, false);
	SetFlag(P_FLAG, false);
	SetFlag(H_FLAG, false);
	SetFlag(Z_FLAG, a == 0);
	SetFlag(S_FLAG, a & (1 << 7));

	printf("and 0x%02x\n", n);
}

void CPU::xor_n()
{
	uint8_t n = ReadImm8(pc);

	a = a ^ n;
	
	SetFlag(C_FLAG, false);
	SetFlag(N_FLAG, false);
	SetFlag(P_FLAG, false);
	SetFlag(H_FLAG, false);
	SetFlag(Z_FLAG, a == 0);
	SetFlag(S_FLAG, a & (1 << 7));

	printf("xor 0x%02x\n", n);
}

void CPU::pop_af()
{
	af = bus->read16(sp);
	sp += 2;

	printf("pop af\n");
}

void CPU::di()
{
	iff1 = iff2 = false;
	printf("di\n");
}

void CPU::push_af()
{
	sp--;
	bus->write8(sp, a);
	sp--;
	bus->write8(sp, f);

	printf("push af\n");
}

void CPU::ld_sp_hl()
{
	sp = hl;
	printf("ld sp, hl\n");
}

void CPU::jp_m_nn()
{
	uint16_t nn = ReadImm16(pc);

	if (GetFlag(S_FLAG))
		pc = nn;
	
	printf("jp m, 0x%04x\n", nn);
}

void CPU::ld_ix_nn()
{
	ix = ReadImm16(pc);
	printf("ld ix, 0x%04x\n", ix);
}

void CPU::add_ix_sp()
{
	uint32_t result = ix + sp;

	SetFlag(C_FLAG, result >> 16);
	SetFlag(N_FLAG, 0);
	SetFlag(H_FLAG, (result & 0xff) != (ix & 0xff));

	ix = result;
	printf("add ix, sp\n");
}

void CPU::ld_sp_ix()
{
	sp = ix;
	printf("ld sp, ix\n");
}

void CPU::sbc_hl_bc()
{
	uint32_t result = hl - bc;

	int carryx = hl ^ bc ^ (result & 0xffff);

	SetFlag(C_FLAG, result >> 16);
	SetFlag(N_FLAG, 1);
	SetFlag(P_FLAG, ((carryx << 1) ^ carryx) & 0x100);
	SetFlag(H_FLAG, carryx & 0x10);
	SetFlag(Z_FLAG, (result & 0xffff) == 0);
	SetFlag(S_FLAG, (result >> 16) & 1);

	hl = result;
	
	printf("sbc hl, bc\n");
}

void CPU::im_1()
{
	im = 1;
	printf("im 1\n");
}

void CPU::in_c()
{
	uint8_t data = bus->read8_io(c);

	SetFlag(N_FLAG, false);
	SetFlag(P_FLAG, false);
	SetFlag(H_FLAG, false);
	SetFlag(Z_FLAG, data == 0);
	SetFlag(S_FLAG, data & (1 << 7));

	printf("in (c)\n");
}