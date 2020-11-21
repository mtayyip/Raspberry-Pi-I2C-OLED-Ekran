//
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "oled96.h"
#include "drawExample.h"

//--------------------------
static void handler (int signum)
{
	if (signum == SIGINT) 
	{
		oledFill(255); oledShutdown();
		exit(0);
	}
}

//--------------------------
void usage(char *arg)
{
	printf("Usage : %s <I2C Device> <I2C Address> <options>\r\n \t<options:>\r\n",arg);
	printf("\t1 : String Example\r\n \t2 : Shape Example\r\n\r\n");
	printf("\tExample : %s /dev/i2c-1 0x3C 1\r\n",arg);
	printf("\tPress CTRL-C to Close the Application\r\n");
}
//--------------------------
void getTemp(char* output)
{
    float systemp, millideg;
    FILE *thermal;
    int n;
    

	thermal = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    n = fscanf(thermal, "%f", &millideg);
    fclose(thermal);
    systemp = (int)(millideg / 10.0f) / 100.0f;
    sprintf(output, "%.2fC", systemp);
}

//--------------------------
void getFreq(char* output)
{
    int cpuMhz, cpuFreqKhz;
    FILE *freq;
    int n;
  
	freq = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq", "r");
    n = fscanf(freq, "%d", &cpuFreqKhz);
    fclose(freq);
    cpuMhz = (int)(cpuFreqKhz / 1000);
    sprintf(output, "%4dMhz", cpuMhz);
}

//--------------------------
void getTime(char* output)
{
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	sprintf(output, "%02d:%02d:%02d",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

//--------------------------
int main(int argc, char *argv[])
{

	if(argc !=4)
	{
		usage(argv[0]);
		return -1;
	}

    struct sigaction act;
    memset (&act, '\0', sizeof(act));
    act.sa_handler = &handler;
    if (sigaction(SIGINT, &act, NULL) < 0) 
    {
        perror ("sigaction");
        return -1;
    }

	char temp[10],timeVal[10],freqVal[10];
	int i=oledInit(argv[1], strtol(argv[2],NULL,16), 0, 1);
	int y=0;

	if (i == 0)
	{
		printf("Successfully opened I2C bus %s\n", argv[1]);
		switch(atoi(argv[3]))
		{
			case 1:
				printf("String Example\r\n");
				oledFill(0); // fill with white
				while(1)
				{
					stringExample();

					getTemp(temp);
					oledWriteString(0,1,"CPU Temp",FONT_BIG);
					oledWriteString(1,4,temp,FONT_BIG);
					sleep(2); oledFill(0);

					getTime(timeVal);
					oledWriteString(0,1,"Cur.Time",FONT_BIG);
					oledWriteString(0,4,timeVal,FONT_BIG);
					sleep(2); oledFill(0);

					getFreq(freqVal);
					oledWriteString(0,1,"CPU Freq",FONT_BIG);
					oledWriteString(0,4,freqVal,FONT_BIG);
					sleep(2); oledFill(0);
				}
			break;

			case 2:
				printf("Shape Example\r\n");
				oledFill(255); // fill with black
				while(1)
				{
					drawSquare();	 sleep(2); oledFill(255);
					drawRectangle(); sleep(2); oledFill(255);
					drawX(); 		 sleep(2); oledFill(255);
					drawTriangle();  sleep(2); oledFill(255);
				}
			break;
			
			default:
			break;
		}
	}
	else
	{
		printf("Unable to initialize I2C bus\r\n");
	}

	return 0;
} 
