
// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>




#define _XTAL_FREQ 8000000

#define SLAVE1READ  3
#define SLAVE1WRITE  2

#define SLAVE2WRITE  4
#define SLAVE2READ  5

#define IMUWRITE 0xD0
#define IMUREAD 0xD1

#define SEND 15


#define EN RA0
#define RS RA1

#include "I2C.h"
#include "LCD8bit.h" 
#include "OsciladorLibs.h"
#include "mpu6050.h"
#include <stdlib.h>
#include <stdio.h>

//Funciones: 
unsigned int counter_read;
char voltage[5],counter[4],sensor[10];
int mpu1, mpu2,mpu,adc_read;
//int Gx;//Solo se medira el angulo en X (Gx)
float MPU,adc_value;
//float Xg;

void mpu_begin(void){
    __delay_ms(150);
    I2C_Master_Start();
    I2C_Master_Write(IMUWRITE);
    I2C_Master_Write(SMPLRT_DIV);	
	I2C_Master_Write(7);         
	I2C_Master_Stop();
    
    I2C_Master_Start();
    I2C_Master_Write(IMUWRITE);
    I2C_Master_Write(PWR_MGMT_1);	
	I2C_Master_Write(1);        
	I2C_Master_Stop();
    
    I2C_Master_Start();
    I2C_Master_Write(IMUWRITE);
    I2C_Master_Write(CONFIG);	
	I2C_Master_Write(0);        
	I2C_Master_Stop();
    
    I2C_Master_Start();
    I2C_Master_Write(IMUWRITE);
    I2C_Master_Write(GYRO_CONFIG);	
	I2C_Master_Write(0x18);        
	I2C_Master_Stop();
    
    I2C_Master_Start();
    I2C_Master_Write(IMUWRITE);
    I2C_Master_Write(INT_ENABLE);	
	I2C_Master_Write(1);        
	I2C_Master_Stop();
    }


void mpu_start(char mpu_register){
    I2C_Master_Start();
    I2C_Master_Write(IMUWRITE);
       //I2C_Master_Write(ACCEL_XOUT_H);
    I2C_Master_Write(mpu_register);
    I2C_Master_Stop();
    I2C_Master_Start();
    I2C_Master_Write(IMUREAD);
    }


void mpu_data_read(void){
        mpu_start(GYRO_XOUT_H); 
        //Para todos se usa H y L
        //TEMP_OUT_H
        //ACCEL_XOUT_H
		mpu1 = I2C_Master_Read(0);
		I2C_Master_Stop();
        
        mpu_start(GYRO_XOUT_L);
        mpu2 = I2C_Master_Read(0);
        I2C_Master_Stop();
    }
void setup(void){
    oscilator_begin(7);
    TRISA = 0;
    TRISB = 0; 
    ANSEL = 0;
    ANSELH = 0;
    PORTA = 0; 
    LCD8_begin();
    I2C_Master_Init(100000);
    mpu_begin();
    counter_read= 0;
    }
//char adc_slave, char counter_slave, char sensor_slave
void read_data(char counter_slave,char adc_slave){
    I2C_Master_Start();
    I2C_Master_Write(adc_slave);
    adc_read = I2C_Master_Read(0);
    I2C_Master_Stop();
    
    
    
    I2C_Master_Start();
    I2C_Master_Write(counter_slave);
    counter_read = I2C_Master_Read(0);
    I2C_Master_Stop();
    

    
   

    mpu_data_read();
    }



void main(void)
    {   
    setup();
    
    
    while(1){

        LCD8_clear();
 
    //SLAVE1READ, SLAVE2READ
 //Para temperatura: Xg = (Gx/340)+36.53;
 //Para aceleracion lineal: Xg = Gx/16384.0; 
//Para aceleracion angular: Xg= Gx/131.0;
    read_data(SLAVE1READ,SLAVE2READ);
    mpu = (mpu1<<8)|mpu2;
    adc_value = adc_read*0.019607;   
    MPU = (mpu/131.0);//Gx es el dato en bruto
    
    
     
  
    sprintf(voltage, "%.1f", adc_value);
    sprintf(counter, "%u",counter_read);
    sprintf(sensor, "%.2f  ",MPU);
    LCD8_set_cursor(2,1);
    LCD8_strWrite(voltage);
    LCD8_set_cursor(2,6);
    LCD8_strWrite(counter);
    LCD8_set_cursor(2,10);
    LCD8_strWrite(sensor);
    LCD8_set_cursor(1,1);
    LCD8_strWrite("ADC   NUM  IMU");
    __delay_ms(200);
    }
    }
