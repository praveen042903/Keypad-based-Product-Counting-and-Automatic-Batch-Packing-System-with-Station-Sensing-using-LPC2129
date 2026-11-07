#include <lpc21xx.h>
#include "keypad.h"
#define buzzer 1<<21
#define motor 1<<17
#define gled 1<<12
#define rled 1<<13
#define sw1 1<<11                                      

volatile unsigned int count = 0;
volatile unsigned char ircount = 0;
int batch=0,pac=0,flag=0,flag1=0;
void EINT0_ISR(void) __irq;
void EINT1_ISR(void) __irq;


int main()
{
PINSEL0=0X00;
PINSEL1|=1<<0;
PINSEL0|=1<<29;
IODIR0|=gled|rled|motor|buzzer;
IOSET0=buzzer;
LCD_INIT();

VICIntSelect=0x00;
VICVectCntl0=(0x20)|14;
VICVectAddr0=(int)EINT0_ISR;
VICVectCntl1=(0x20)|15;
VICVectAddr1=(int)EINT1_ISR;
EXTMODE=0X03;
EXTPOLAR=0X00;
VICIntEnable=(1<<14)|(1<<15);

LCD_COMMAND(0X80);
LCD_STRING((unsigned char *)"PRODUCT COUNTING    SYSTEM");
delay_ms(1000);
LCD_COMMAND(0X01);
LCD_STRING((unsigned char *)"PRESS THE SWITCH    TO ON ");
delay_ms(1000);
while(1)
{
label:
if((IOPIN0&sw1)==0)
{   IOSET0=gled;
// IOCLR0=rled|gled;
LCD_COMMAND(0X01);
LCD_COMMAND(0X80);
LCD_STRING((unsigned char *)"BATCH NO:");
batch=keyscan();
LCD_INTEGER(batch);
LCD_COMMAND(0XC0);
LCD_STRING((unsigned char *)"NO OF PACKS:");
pac=keyscan();
LCD_INTEGER(pac);
delay_ms(1000);
LCD_COMMAND(0X01);
LCD_COMMAND(0X80);
LCD_STRING((unsigned char *)"MACHINE STARTED");
delay_ms(2000);
IOSET0=motor;
LCD_COMMAND(0X01);
LCD_COMMAND(0X80);
LCD_STRING((unsigned char *)"WAITING FOR ");
LCD_COMMAND(0Xc0);
LCD_STRING((unsigned char *)"PRODUCTS");
while(count<pac)
{

IOCLR0=rled;
if(T1TC>5)
{
IOCLR0=motor;
IOCLR0=buzzer;
LCD_COMMAND(0X01);
LCD_COMMAND(0X80);
LCD_STRING((unsigned char *)"PACK NOT TRACED");
LCD_COMMAND(0Xc0);
LCD_STRING((unsigned char *)"RESTART");
goto label;
}
else if(flag1)
{
flag1=0;
LCD_COMMAND(0X01);
LCD_COMMAND(0X80);
LCD_STRING((unsigned char *)"BATCH :");
LCD_INTEGER(batch);
LCD_COMMAND(0Xc0);
LCD_STRING((unsigned char *)"NO OF PACKS:");  
LCD_INTEGER(count);
LCD_STRING((unsigned char *)"/");
LCD_INTEGER(pac);
}
else if(flag)
{
flag=0;
LCD_COMMAND(0X01);
LCD_COMMAND(0X80);
LCD_STRING((unsigned char *)"BATCH :");
LCD_INTEGER(batch);
LCD_COMMAND(0Xc0);
LCD_STRING((unsigned char *)"NO OF PACKS:");  
LCD_INTEGER(count);
LCD_STRING((unsigned char *)"/");
LCD_INTEGER(pac);
}
} IOSET0=rled;
IOCLR0=gled;
IOCLR0=motor;
IOCLR0=buzzer;
LCD_COMMAND(0X01);
LCD_COMMAND(0X80);
LCD_STRING((unsigned char *)"BATCH :");
LCD_INTEGER(batch);
LCD_COMMAND(0Xc4);
LCD_STRING((unsigned char *)"COMPLETED");
delay_ms(5000);
LCD_COMMAND(0X01);
LCD_STRING((unsigned char *)"PRESS THE SWITCH    TO ON ");

}
}
}

void EINT0_ISR(void) __irq  
{
    ircount=1;          
    EXTINT = 0x01;
T1TCR=0X03;
T1PR=15000000-1;
T1TCR=0X01;
flag1=1;
    VICVectAddr = 0;
}

void EINT1_ISR(void) __irq  
{
if (ircount)
{  
if(T1TC<=5)          
count++;
ircount=0;            
T1TCR=0X00;
flag=1;
    }
EXTINT = 0x02;
    VICVectAddr = 0;
}
