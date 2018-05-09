#include "BIT.h"
#include "struct.h"
#include "IOctl.h"
////////////////////////////////////////////////////////////
/*
unsigned char  BIT_Check()
{
unsigned char i = 0, bit = 0;
    //  µçÑ¹³¬²î
    if(get_Fhase_A_status() == OPEN)
    	{
    	  
    	  if(abs(g_Phase_Voltage[0]/10 - 220) >= VOLTAGE_ALW)
		   	{
		   	   bit =( bit &0xf7) + 0x08;  //bit3
		   	}
    	}
	  if(get_Fhase_B_status() == OPEN)
    	{
    	  
    	  if(abs(g_Phase_Voltage[1]/10 - 220) >= VOLTAGE_ALW)
		   	{
		   	   bit =( bit &0xf7) + 0x08;  //bit3
		   	}
    	}
	  if(get_Fhase_C_status() == OPEN)
    	{
    	  
           if(abs(g_Phase_Voltage[2]/10 - 220) >= VOLTAGE_ALW)
		   	{
		   	   bit =( bit &0xf7) + 0x08;  //bit3
		   	}
    	}
    //µç±í¹ÊÕÏÅĞ¶Ï
////////////////////////////////////////////////////////////////////////////
   	if(get_Fhase_A_status() == CLOSE)
		{
             if(g_Phase_Amp[0] > CURRENT_THRESHOLD)
             	{
             	  bit =( bit &0xfb) + 0x04;  //bit2
             	}
		}
	if(get_Fhase_B_status() == CLOSE)
		{
             if(g_Phase_Amp[1] > CURRENT_THRESHOLD)
             	{
             	  bit =( bit &0xfb) + 0x04;  //bit2
             	}
		}
	if(get_Fhase_C_status() == CLOSE)
		{
             if(g_Phase_Amp[2] > CURRENT_THRESHOLD)
             	{
             	  bit =( bit &0xfb) + 0x04;  //bit2
             	}
		}
    
////////////////////////////////////////////////////////////////////////////
	//¼ÌµçÆ÷¹ÊÕÏÅĞ¶Ï
	if(get_Fhase_A_status() == CLOSE)
		{
             if(g_Phase_Voltage[0] > VOLTAGE_THRESHOLD)
             	{
             	  bit =( bit &0xfd) + 0x02;  //bit1
             	}
		}
	if(get_Fhase_B_status() == CLOSE)
		{
             if(g_Phase_Voltage[1] > VOLTAGE_THRESHOLD)
             	{
             	  bit =( bit &0xfd) + 0x02;  //bit1
             	}
		}
	if(get_Fhase_C_status() == CLOSE)
		{
             if(g_Phase_Voltage[2] > VOLTAGE_THRESHOLD)
             	{
             	  bit =( bit &0xfd) + 0x02;  //bit1
             	}
		}
////////////////////////////////////////////////////////////////////////////
	//´¥ÃşÆÁ¹ÊÕÏÅĞ¶Ï
	if(g_touchscreen_timeout_2000ms_counter > 30)
	    {
	       bit =( bit &0xfe) + 0x01;  //bit0
		}

////////////////////////////////////////////////////////////////////////////
	return bit;
}
*/
