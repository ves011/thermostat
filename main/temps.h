/*
 * temps.h
 *
 *  Created on: Jan 8, 2024
 *      Author: viorel_serbu
 */

#ifndef MAIN_TEMPS_H_
#define MAIN_TEMPS_H_

#include <inttypes.h>

#define MAX_TEMP_DEVICES   (10)
#define DS18B20_RESOLUTION   (DS18B20_RESOLUTION_12_BIT)
#define DS_SAMPLE_PERIOD        (1000)   // milliseconds


extern float	target_temp, hist_temp, freeze_temp;
extern int		on_off_time, maint_time;
extern QueueHandle_t temp_mon_queue;;

int register_temps();
int do_temp(int argc, char **argv);
void get_temp(int idx, float *temperature);
uint64_t get_t_hwadd(int idx);
/*
#define NTEMP		9

#define HWADDR0		0xE800000053E95828L
#define HWADDR1		0x3800000052B44428L
#define HWADDR2		0x3C000000525D8228L
#define HWADDR3		0xBB0000005259F228L
#define HWADDR4		0x2F00000053FCEA28L
#define HWADDR5		0x32000000530F0E28L
#define HWADDR6		0x7E00000053EB6328L
#define HWADDR7		0x5700000052401F28L
#define HWADDR8		0xBB00000051FB0F28L

#define ACOEF0		1 
#define ACOEF1		1.0438		
#define ACOEF2		0.9872		
#define ACOEF3 		1.0375		
#define ACOEF4		1.0061		
#define ACOEF5		1.0248
#define ACOEF6		1.0328
#define ACOEF7		1.0453
#define ACOEF8		0.9765

#define BCOEF0		0 
#define BCOEF1		-0.9763		
#define BCOEF2		 0.432		
#define BCOEF3 		-0.7275		
#define BCOEF4		-0.065		
#define BCOEF5		-0.2649
#define BCOEF6		-0.854
#define BCOEF7		-0.9038
#define BCOEF8		 0.9674
*/

#define NTEMP		11

#define HWADDR0	0x3800000052B44428L	
#define ACOEF0	1
#define BCOEF0	0
	
#define HWADDR1	0x3C000000525D8228L
#define ACOEF1	0.9427
#define BCOEF1	1.533
		
#define HWADDR2	0xBB0000005259F228L		
#define ACOEF2	0.9833
#define BCOEF2	0.7467

#define HWADDR3	0x2F00000053FCEA28L
#define ACOEF3	0.9691
#define BCOEF3	0.7039
		
#define HWADDR4	0x32000000530F0E28L
#define ACOEF4	0.9781
#define BCOEF4	0.8286
		
#define HWADDR5	0x7E00000053EB6328L
#define ACOEF5	0.9737
#define BCOEF5	0.5858
	
#define HWADDR6	0xBB00000051FB0F28L
#define ACOEF6	0.932
#define BCOEF6	2.0708
		
#define HWADDR7	0x5700000052401F28L
#define ACOEF7	0.9982
#define BCOEF7	0.9994

#define HWADDR8	0xE800000053e95828L
#define ACOEF8	1
#define BCOEF8	0
/*
#define HWADDR9	0xAE00000052b44d28L
#define ACOEF9	1
#define BCOEF9	0
*/
#define HWADDR9	0xAC000000C06EFD28L
#define ACOEF9	1
#define BCOEF9	0

#define HWADDR10	0xDC000000BC081F28L
#define ACOEF10	1
#define BCOEF10	0



/*
#define HWADDR10	0xe23c01f095f74d28L
#define ACOEF10	1
#define BCOEF10	0
*/

typedef struct
	{
	uint64_t hwaddr;
	float a;
	float b;
	}tcor_t;


#endif /* MAIN_TEMPS_H_ */
