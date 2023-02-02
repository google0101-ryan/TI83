#pragma once

#include <src/bus.h>

#include <cstdint>

class CPU
{
private:
	Bus* bus;

	uint16_t pc;
	uint16_t sp;
	uint16_t ix, iy;
	
	union
	{
		uint16_t af;
		struct
		{
			uint8_t f;
			uint8_t a;
		};
	};
	
	union
	{
		uint16_t hl;
		struct
		{
			uint8_t l;
			uint8_t h;
		};
	};
	
	union
	{
		uint16_t bc;
		struct
		{
			uint8_t c;
			uint8_t b;
		};
	};
	
	union
	{
		uint16_t de;
		struct
		{
			uint8_t e;
			uint8_t d;
		};
	};

	enum Flags
	{
		C_FLAG = (1 << 0),
		N_FLAG = (1 << 1),
		P_FLAG = (1 << 2),
		H_FLAG = (1 << 4),
		Z_FLAG = (1 << 6),
		S_FLAG = (1 << 7),
	};

	void SetFlag(Flags flag, bool set)
	{
		if (set)
			f |= flag;
		else
			f &= ~(flag);
	}

	bool GetFlag(Flags flag)
	{
		return f & flag;
	}

	bool iff1, iff2;
	int im = 0;

	uint8_t ReadImm8(uint16_t& addr);
	uint16_t ReadImm16(uint16_t& addr);

	void ld_bc_nn(); // 0x01
	void dec_b(); // 0x05
	void ld_b_n(); // 0x06
	void ld_c_n(); // 0x0e
	void djnz(); // 0x10
	void ld_hl_nn(); // 0x21
	void jr_z_d(); // 0x28
	void add_hl_hl(); // 0x29
	void ld_hl_mm(); // 0x2a
	void inc_l(); // 0x2c
	void jr_nc_d(); // 0x30
	void ld_sp_nn(); // 0x31
	void ld_a_n(); // 0x3e
	void ld_b_a(); // 0x47
	void ld_a_b(); // 0x78
	void ld_a_h(); // 0x7c
	void ld_a_l(); // 0x7d
	void xor_a(); // 0xaf
	void or_a(); // 0xb7
	void ret_nz(); // 0xc0
	void pop_bc(); // 0xc1
	void jp_nn(); // 0xc3
	void push_bc(); // 0xc5
	void ret(); // 0xc9
	void jp_z_nn(); // 0xca
	void call_nn(); // 0xcd
	void out_n_a(); // 0xd3
	void sub_n(); // 0xd6
	void ret_c(); // 0xd8
	void in_a_n(); // 0xdb
	void push_hl(); // 0xe5
	void and_n(); // 0xe6
	void xor_n(); // 0xee
	void pop_af(); // 0xf1
	void di(); // 0xf3
	void push_af(); // 0xf5
	void ld_sp_hl(); // 0xf9
	void jp_m_nn(); // 0xfa

	// 0xdd
	void ld_ix_nn(); // 0x21
	void add_ix_sp(); // 0x39
	void ld_sp_ix(); // 0xf9

	// 0xed
	void sbc_hl_bc(); // 0x42
	void im_1(); // 0x56
	void in_c(); // 0x70
public:
	CPU(Bus* bus);
	void Dump();

	void clock();
};