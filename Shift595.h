#ifndef SHIFT595_H
#define SHIFT595_H

#include "AT84_common.h"

enum BitOrder
{
	_MSBFIRST = 0x80,
	_LSBFIRST = 0x01
};

//class for 74hc595 shift register
class Shift595
{
	private:
		//pin masks
		const uint8_t clk, lat, data;
		//GPIO port reference
		volatile uint8_t &port;
	public:
		Shift595(uint8_t clk, uint8_t lat, uint8_t data, volatile uint8_t &port):
			clk(clk), lat(lat), port(port), data(data) 
		{
			//set pins as outputs
			*(&port - 1) |= clk | lat | data;

		}
		
		//shift out and latch an 8-bit value to the outputs
		void shiftOut(uint8_t value, BitOrder order=_LSBFIRST);

		//unnecessarily cleaner way of calling shiftOut
		uint8_t operator=(uint8_t val) { shiftOut(val); return val; }
};

#endif
