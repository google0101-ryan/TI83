#pragma once

#include <cstdint>
#include <string>
#include <fstream>

class Bus
{
private:
	uint8_t** flash;
	uint8_t** ram;

	uint8_t keys_pressed = 0xff;
	uint8_t interrupt_mask = 0;
	
	uint8_t* read_flash; // The current flash bank for 0000h - 3FFFh
	uint8_t* read_membank_a; // The current flash/ram bank for 4000h - 7FFFh
	uint8_t* read_membank_b; // The current flash/ram bank for 8000h - BFFFh
	uint8_t* ram_bank_0; // RAM bank 0 is permanently mapped at C000h - FFFFh
public:
	Bus(std::string file_name);

	void remap_flash();

	uint8_t read8(uint16_t addr);
	uint16_t read16(uint16_t addr);

	void write8(uint16_t addr, uint8_t data);
	void write16(uint16_t addr, uint16_t data);

	void write8_io(uint8_t addr, uint8_t data);
};