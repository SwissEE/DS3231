#ifndef RTC_h
#define RTC_h
#include "Arduino.h"
#include"Wire.h"

class RTC {
public:
	RTC();
	
	//I2C adress
	int adress = 0x68;
	

	//Time functions

	//returns current second (0-59)
	int seconds();
	//returns current minute (0-59)
	int minutes();
	//returns current hour (0-23)
	int hours();
	//returns current weekday (1-7)
	int weekdays();
	//returns current day of month (1-31)
	int days();
	//returns current month (1-12)
	int months();
	//returns current year (0-99)
	int years();
	//returns temperature 
	float temp();
	//setting the time of the DS3231
	void setdate(byte second, byte minute, byte hour, byte dayofweek, byte day, byte month, byte year); 
	//returns DD/MM/YY
	String getdate(); 
	//returns HH:MM:SS
	String gettime();
	
	
	//Alarmfunctions

	//Set time and mode of Alarm 1
	// mode = {A1M4, A1M3, A1M2, A1M1, DY/DT}
	// DY/DT A1M4 A1M3 A1M2 A1M1 Alarmrate
	//  X     1    1    1    1   Alarm once per second
	//  X     1    1    1    0   Alarm when seconds match
	//  X     1    1    0    0   Alarm when minutes and seconds match
	//  X     1    0    0    0   Alarm when hours, minutes, and seconds match
	//  0     0    0    0    0   Alarm when date, hours, minutes, and seconds match
	//  1     0    0    0    0   Alarm when day, hours, minutes, and seconds match
	void setA1(byte second, byte minute, byte hour, byte dayofweek, byte day, bool mode[]); 
	//Set time and mode of Alarm 2
	// mode = {A2M4, A2M3, A2M2, DY/DT}
	//DY/DT A2M4 A2M3 A2M2 Alarmrate
	//  X    1    1    1   Alarm once per minute(00 seconds of every minute)
	//  X    1    1    0   Alarm when minutes match
	//  X    1    0    0   Alarm when hours and minutes match
	//  0    0    0    0   Alarm when date, hours, and minutes match
	//  1    0    0    0   Alarm when day, hours, and minutes match
	void setA2(byte minute, byte hour, byte dayofweek, byte day, bool mode[]);
	
	//returns true if A1 flag (A1F) is set to 1
	bool isA1(); 
	//returns true if A2 flag (A2F) is set to 1
	bool isA2(); 

	//write AF1 bit to 0
	void resetA1(); 
	//write AF2 bit to 0
	void resetA2(); 

	//disables Alarm1 Interrupt (set A1IE to 0)
	void disableA1(); 
	//disables Alarm2 Interrupt (set A2IE to 0)
	void disableA2(); 
	
	//disables the Alarm Interrupt Capability
	//Ouputs Squarewave on the INT/SQW Pin
	//RS2 RS1   SQW OUTPUTFREQUENCY
	// 0   0		1Hz
	// 0   1		1.024kHz
	// 1   0		4.096kHz
	// 1   1		8.192kHz				
	void setSQW(int RS1, int RS2);
	
	
	//converts String to Byte
	byte stringTobyte(String s);
	//converts Byte to BCD
	byte byteTobcd(int a);
	//returns content of RTC register adress
	byte readadr(byte adr);
	
};



#endif