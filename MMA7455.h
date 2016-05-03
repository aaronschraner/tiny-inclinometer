#ifndef MMA7455_H
#define MMA7455_H

#include "AT84_common.h"

typedef uint8_t byte;

enum RW: uint8_t
{
	_READ = 0,
	_WRITE = 1
};

enum Register: uint8_t
{
	XOUTL   = 0x00,
	XOUTH   = 0x01,
	YOUTL   = 0x02,
	YOUTH   = 0x03,
	ZOUTL   = 0x04,
	ZOUTH   = 0x05,
	XOUT8   = 0x06,
	YOUT8   = 0x07,
	ZOUT8   = 0x08,
	STATUS  = 0x09,
	DETSRC  = 0x0A,
	TOUT    = 0x0B,
	I2CAD   = 0x0D,
	USRINF  = 0x0E,
	WHOAMI  = 0x0F,
	XOFFL   = 0x10,
	XOFFH   = 0x11,
	YOFFL   = 0x12,
	YOFFH   = 0x13,
	ZOFFL   = 0x14,
	ZOFFH   = 0x15,
	MCTL    = 0x16,
	INSTRST = 0x17,
	CTL1    = 0x18,
	CTL2    = 0x19,
	LDTH    = 0x1A,
	PDTH    = 0x1B,
	PW      = 0x1C,
	LT      = 0x1D,
	TW      = 0x1E
};

enum Direction: uint8_t
{
	_X = 0x00,
	_Y = 0x02,
	_Z = 0x04
};
//TODO: make program scream when device dropped

enum MCTLflags: uint8_t
{
	//operating mode
	_STANDBY = 0x00,
	_MEASUREMENT = 0x01,
	_LEVELDET = 0x02,
	_PULSEDET = 0x03,
	
	//gravity sensitivity
	_8g = 0x00,
	_2g = 0x04,
	_4g = 0x08, 

	//interface configuration
	_DRPD = 0x40,  //if set, disable outputting ready signal to INT1
	_SPI3W = 0x20, //if set, use 3 wire SPI, else use 4 wire

	//misc
	_STON = 0x10 //self-test (1 = enabled)
};

class MMA7455
{

	private:

		//pins (bitmasks)
		const uint8_t cs, clk, sdio;
		//GPIO register references
		volatile uint8_t &port;
		volatile uint8_t &ddr; 
		volatile uint8_t &pin; 

		//internal functions for MMA7455 register manipulation
		void startOperation(uint8_t readWrite);
		uint8_t readReg(uint8_t addr);
		void writeReg(uint8_t addr, byte val);
		void writeAddr(uint8_t addr);

	public:
		MMA7455(uint8_t cs, uint8_t clk, uint8_t sdio, volatile uint8_t& port):
			cs(cs), clk(clk), sdio(sdio), port(port), ddr(*(&port - 1)), pin(*(&port - 2))
		{
			//set relevant pins as output
			ddr |= cs | clk ;
		}
		//get MMA7455 status register value
		uint8_t getStatus() { return readReg(STATUS);}
		void setMode(uint8_t mode = _MEASUREMENT | _2g | _SPI3W | _DRPD);
		uint8_t getMode();
		int8_t getShortAccel(uint8_t d);
		void setOffset(uint8_t d, uint16_t offset);
		//get the rough acceleration in a given direction (answer in m/s^2 float)
		float getRoughAccelF(uint8_t d) { return 9.8 * getShortAccel(d) / 64.; }
		

};


#endif


