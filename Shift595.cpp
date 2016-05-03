#include "Shift595.h"
void Shift595::shiftOut(uint8_t value, BitOrder order )
{
	for(uint8_t x = order; x; x = (order==_MSBFIRST) ? x>>1 : x<<1)
	{
		port = port & ~clk;
		if ( x & value)
			port = port | data; 
		else
			port = port & ~data; 
		port |= clk;
	}
	port &= ~lat;
	port |=  lat;
	port &= ~data;
}
