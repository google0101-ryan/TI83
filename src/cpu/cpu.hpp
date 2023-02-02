#pragma once

#include <src/bus.h>

#include <cstdint>

class CPU
{
private:
	Bus* bus;

	uint16_t pc;
	uint16_t sp;
	
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

	bool iff1, iff2;

	uint8_t ReadImm8(uint16_t& addr);
	uint16_t ReadImm16(uint16_t& addr);

	void ld_c_n(); // 0x0e
	void ld_hl_nn(); // 0x21
	void ld_a_n(); // 0x3e
	void xor_a(); // 0xaf
	void jp_nn(); // 0xc3
	void push_bc(); // 0xc5
	void call_nn(); // 0xcd
	void out_n_a(); // 0xd3
	void push_hl(); // 0xe5
	void di(); // 0xf3
	void ld_sp_hl(); // 0xf9

	// 0xed
	void in_c(); // 0x70
public:
	CPU(Bus* bus);
	void Dump();

	void clock();
};