#include "MMA7455.h"

void MMA7455::startOperation(uint8_t readWrite)
{
	//set SDIO as output
	ddr |= sdio;

	//pull CS low
	port &= ~cs;
	port &= ~clk;

	//say if this is read (0) or write (1) operation
	port = (readWrite == _READ) ? port & ~sdio : port | sdio;

	//tick the clock
	port |= clk;
}
	
uint8_t MMA7455::readReg(uint8_t addr)
{
	
	startOperation(_READ);  //say this is a read operation
	writeAddr(addr);   //send the address to be read
	
	port &= ~clk;  //tick the clock down
	ddr &= ~sdio;   //set SDIO as input
	port &= ~sdio;  //set SDIO to 0
	port |= clk;    //tick the clock high

	//read the register
	byte result = 0;
	for(byte a = 0x80; a; a >>= 1)
	{
		port &= ~clk;
		port |= clk;
		if (pin & sdio)
			result |= a;
	}

	//reset pins to good state
	port &= ~(clk | sdio);
	ddr |= sdio;
	port |= cs;
	
	return result;
}

void MMA7455::writeReg(uint8_t addr, byte val)
{
	//say this is a write operation
	startOperation(_WRITE);
	//send the address of the register
	writeAddr(addr);

	//do nothin bit
	port &= ~clk;
	port |= clk;

	//send the value to the register
	for(byte a = 0x80; a; a >>= 1)
	{
		port &= ~clk;
		if (val & a)
			port |= sdio;
		else
			port &= ~sdio;
		port |= clk;
	}
	port &= ~(clk | sdio);
	port |= cs;
}

//send an address to the MMA7455 (not designed for standalone use)
void MMA7455::writeAddr(uint8_t addr)
{
	for(byte a = 0x20; a; a >>= 1)
	{
		port &= ~clk;
		if (addr & a)
			port |= sdio;
		else
			port &= ~sdio;
		port |= clk;
	}
}

//configure the MODE register of the MMA7455
void MMA7455::setMode(uint8_t mode)
{
	writeReg(MCTL, mode);
}

//return the MODE register of the MMA7455
uint8_t MMA7455::getMode()
{
	return readReg(MCTL);
}

//get the short (8-bit) acceleration in a given direction
int8_t MMA7455::getShortAccel(uint8_t d)
{
	return (int8_t)readReg(d/2 + XOUT8);
}

//set the 10-bit acceleration offset in a given direction
void MMA7455::setOffset(uint8_t d, uint16_t offset)
{
	writeReg(d + XOFFL, offset & 0xFF);
	writeReg(d + XOFFH, offset >> 8);
}
