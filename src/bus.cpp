#include "bus.h"

int roundUp(int numToRound, int multiple)
{
    if (multiple == 0)
        return numToRound;

    int remainder = numToRound % multiple;
    if (remainder == 0)
        return numToRound;

    return numToRound + multiple - remainder;
}

Bus::Bus(std::string file_name)
{
	std::ifstream file(file_name, std::ios::binary | std::ios::ate);

	size_t size = file.tellg();
	size_t num_pages = roundUp(size, 0x4000) / 0x4000;

	printf("[emu/Bus]: Flash file %s is made of %ld pages, %ld bytes\n", file_name.c_str(), num_pages, size);

	file.seekg(0, std::ios::beg);

	flash = new uint8_t*[num_pages];

	for (size_t i = 0; i < num_pages; i++)
	{
		flash[i] = new uint8_t[0x4000];
		file.read((char*)flash[i], 0x4000);
	}
	
	read_flash = flash[num_pages - 1];
	read_membank_a = flash[0];
	read_membank_b = flash[0];

	ram = new uint8_t*[2];

	ram[0] = new uint8_t[0x4000];
	ram[1] = new uint8_t[0x4000];

	ram_bank_0 = ram[0];
}

void Bus::Dump()
{
	std::ofstream file("ram.dump");

	for (int i = 0; i < 0x4000; i++)
	{
		file << (char)ram[0][i];
	}
	for (int i = 0; i < 0x4000; i++)
	{
		file << (char)ram[1][i];
	}

	file.close();
}

void Bus::remap_flash()
{
	read_flash = flash[0];
}

uint8_t Bus::read8(uint16_t addr)
{
	if (addr <= 0x3FFF)
		return read_flash[addr];
	else if (addr >= 0x4000 && addr <= 0x7FFF)
		return read_membank_a[addr - 0x4000];
	else if (addr >= 0x8000 && addr <= 0xBFFF)
		return read_membank_b[addr - 0x8000];
	else if (addr >= 0xC000 && addr <= 0xFFFF)
		return ram_bank_0[addr - 0xC000];

	printf("How?\n");
	exit(1);
}

uint16_t Bus::read16(uint16_t addr)
{
	if (addr <= 0x3FFF)
		return *(uint16_t*)&read_flash[addr];
	else if (addr >= 0x4000 && addr <= 0x7FFF)
		return *(uint16_t*)&read_membank_a[addr - 0x4000];
	else if (addr >= 0x8000 && addr <= 0xBFFF)
		return *(uint16_t*)&read_membank_b[addr - 0x8000];
	else if (addr >= 0xC000 && addr <= 0xFFFF)
		return *(uint16_t*)&ram_bank_0[addr - 0xC000];

	printf("How?\n");
	exit(1);
}

void Bus::write8(uint16_t addr, uint8_t data)
{
	if (addr <= 0x3FFF)
	{
		read_flash[addr] = data;
		return;
	}
	else if (addr >= 0x4000 && addr <= 0x7FFF)
	{
		read_membank_a[addr - 0x4000] = data;
		return;
	}
	else if (addr >= 0x8000 && addr <= 0xBFFF)
	{
		read_membank_b[addr - 0x8000] = data;
		return;
	}
	else if (addr >= 0xC000 && addr <= 0xFFFF)
	{
		ram_bank_0[addr - 0xC000] = data;
		return;
	}

	printf("How?\n");
	exit(1);
}

void Bus::write16(uint16_t addr, uint16_t data)
{
	if (addr <= 0x3FFF)
	{
		*(uint16_t*)&read_flash[addr] = data;
		return;
	}
	else if (addr >= 0x4000 && addr <= 0x7FFF)
	{
		*(uint16_t*)&read_membank_a[addr - 0x4000] = data;
		return;
	}
	else if (addr >= 0x8000 && addr <= 0xBFFF)
	{
		*(uint16_t*)&read_membank_b[addr - 0x8000] = data;
		return;
	}
	else if (addr >= 0xC000 && addr <= 0xFFFF)
	{
		*(uint16_t*)&ram_bank_0[addr - 0xC000] = data;
		return;
	}

	printf("How?\n");
	exit(1);
}

void Bus::write8_io(uint8_t addr, uint8_t data)
{
	switch (addr)
	{
	case 0x00:
		break;
	case 0x01:
		if (data == 0xff)
			keys_pressed = 0xff;
		break;
	case 0x03:
		interrupt_mask = data;
		break;
	case 0x04:
		if (data & 1)
		{
			memory_map_mode = 1;
			write8_io(0x06, membank_a_val);
			write8_io(0x07, membank_b_val);
		}
		else
		{
			memory_map_mode = 0;
			ram_bank_0 = ram[0];
			write8_io(0x06, membank_a_val);
			write8_io(0x07, membank_b_val);
		}
		break;
	case 0x05:
		port_02 |= (data & 7) << 3;
		break;
	case 0x06:
	{
		membank_a_val = data;

		if (memory_map_mode == 0)
		{
			bool is_ram = (data >> 6) & 1;

			uint8_t flash_bank = data & 0x1F;
			bool ram_bank = data & 1;

			if (is_ram)
				read_membank_a = ram[ram_bank];
			else
				read_membank_a = flash[flash_bank];
		}
		else
		{
			bool is_ram = (data >> 6) & 1;

			uint8_t flash_bank = data & 0x1F;
			bool ram_bank = data & 1;

			if (is_ram)
			{
				read_membank_a = ram[ram_bank & 0x3FE];
				read_membank_b = ram[ram_bank];
			}
			else
			{
				read_membank_a = flash[flash_bank & 0x3FE];
				read_membank_b = flash[flash_bank];
			}
		}
		break;
	}
	case 0x07:
	{
		membank_b_val = data;

		bool is_ram = (data >> 6) & 1;

		uint8_t flash_bank = data & 0x1F;
		bool ram_bank = data & 1;

		if (memory_map_mode == 0)
		{
			if (is_ram)
				read_membank_b = ram[ram_bank];
			else
				read_membank_b = flash[flash_bank];
		}
		else
		{
			if (is_ram)
				ram_bank_0 = ram[ram_bank];
			else
				ram_bank_0 = flash[flash_bank];
		}
		
		break;
	}
	case 0x10:
		switch (data)
		{
		case 0x01:
			disp_stat |= (1 << 6);
			printf("[emu/LCDC]: Enter 8-bit mode\n");
			break;
		case 0x05:
			disp_stat |= 1;
			printf("[emu/LCDC]: Setting X-autoincrement mode\n");
			break;
		case 0x18:
			printf("[emu/LCDC]: Exit test mode\n");
			break;
		case 0xC0 ... 0xFF:
			printf("[emu/LCDC]: Set contrast to 0x%02x\n", data);
			disp_contrast = data;
			break;
		default:
			printf("[emu/LCDC]: Unknown command 0x%02x\n", data);
			exit(1);
		}
		break;
	case 0x14:
		flash_unlocked = data & 1;
		if (flash_unlocked)
		{
			port_02 |= (1 << 2);
			printf("[emu/Bus]: Flash unlocked\n");
		}
		else
		{
			port_02 &= ~(1 << 2);
			printf("[emu/Bus]: Flash locked\n");
		}
		break;
	case 0x16:
		break;
	default:
		printf("[emu/Bus]: Write8_io 0x%02x to unknown address 0x%02x\n", data, addr);
		exit(1);
	}
}

uint8_t Bus::read8_io(uint8_t addr)
{
	switch (addr)
	{
	case 0x02:
		return port_02;
	case 0x10:
		return disp_stat;
	default:
		printf("[emu/Bus]: Read8_io to unknown address 0x%02x\n", addr);
		exit(1);
	}
}
