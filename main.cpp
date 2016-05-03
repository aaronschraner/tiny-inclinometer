#include "Shift595.h"
#include "MMA7455.h"

uint8_t abs(int8_t x)
{
	return x >= 0 ? x : -x;
}
float abs(float x)
{
	return x >= 0 ? x : -x;
}
//binary digits for 7-segment display
const uint8_t SEGDIGITS[] = {
	0b11111100,
	0b01100000, 
	0b11011010, 
	0b11110010, 
	0b01100110, 
	0b10110110, 
	0b10111110, 
	0b11100000, 
	0b11111110, 
	0b11110110, 
	0b11101110, 
	0b00111110,
	0b10011100, 
	0b01111010,
	0b10011110, 
	0b10001110};

int main()
{
	//configure display pins as outputs
	DDRA |= 0x7F;
	//instantiate shift register (for controlling display)
	// clock - PA2
	// latch - PA1
	// data - PA0
	Shift595 myshift(0x04, 0x02, 0x01, PORTA);

	//instantiate accelerometer
	// chip select - PB2
	// clock - PB0
	// data (SDIO) - PB1
	MMA7455 accel(0x04, 0x01, 0x02, PORTB);
	
	//configure the accelerometer to measure continuously,
	//  ranging from +/-2G of acceleration, using 3 wire SPI
	accel.setMode(_MEASUREMENT | _2g | _SPI3W | _DRPD);

	//calibrate accelerometer
	accel.setOffset(_X, 12);

	//average 8 samples before displaying
	const int samples = 8;
	//pins for display common cathodes (PORTA)
	const uint8_t colenable[] = { 0x08, 0x10, 0x20, 0x40 }; 
	float xsample;
	float zsample;
	float dval;
	float absdv;
	uint8_t disps[4];
	while(true)
	{
		xsample = 0;
		zsample = 0;
		for(uint16_t x=0; x<samples; x++)
		{
			//take the sum of the samples
			xsample += accel.getRoughAccelF(_X);
			zsample += accel.getRoughAccelF(_Z);
			if(x == samples - 1)
			{
				//convert the sum of samples into an average (arithmetic mean)
				xsample /= samples;
				zsample /= samples;

				//convert the average acceleration in X, Z to an angle relative
				// to vertical 
				float newval = asin( xsample / 
						sqrt(zsample * zsample + xsample * xsample)
						) * 180 / M_PI ; 

				//fix the sign of the angle
				newval *= xsample / abs(xsample);

				//store the angle's character representation for easy displaying
				dval = newval;
				absdv = abs(dval);
				disps[0] = xsample < 0 ? 0x02 : 0;
				disps[1] = SEGDIGITS[(int)(absdv /  10) % 10];
				disps[2] = SEGDIGITS[(int)(absdv *   1) % 10] | 0x01;
				disps[3] = SEGDIGITS[(int)(absdv *  10) % 10];
			}
			for(uint8_t z=0; z<32; z++)
				for(uint8_t i=0; i<4 ; i++)
				{
					//turn off all digits
					PORTA = PORTA & 0x87;

					//shift out the i'th character display code
					myshift = ~disps[i];

					//turn on the i'th digit
					PORTA = (PORTA & 0x87) | colenable[i];

					//wait a bit
					_delay_ms(2);
				}
		}
	}
}
