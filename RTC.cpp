#include "Arduino.h"
#include "Wire.h"
#include "RTC.h"

	RTC::RTC() {}
	
	float RTC::temp() {
		byte a = readadr(0x11);
		byte b = readadr(0x12);
		if (a >> 7) {      //if qnegative temp
			a = ~a + 0b1;  //2's complement
			b = ~b + 0b1;
			return a + b * 0.25;
		}
		else {
			return a + b * 0.25;
		}
	}
	//reading from an adress of the DS3231
	byte RTC::readadr(byte adr) {
		String s = "";
		Wire.beginTransmission(adress);
		Wire.write(adr);
		Wire.requestFrom(adress, 1);    // request 1 byte from slave device
		while (Wire.available())    // slave may send less than requested
		{
			s += Wire.read();    // receive a byte as character
		}
		Wire.endTransmission();
		byte a = stringTobyte(s);
		return a;
	}
	byte RTC::stringTobyte(String s) {
		return s.toInt();
	}
	
	//returns current second
	int RTC::seconds() {
		byte s = readadr(0x00);
		byte a = (s >> 4) * 10;
		byte b = s & 0b00001111;
		return (int)a + (int)b;
	}
	
	//returns current minute
	int RTC::minutes() {
		byte s = readadr(0x01);
		byte a = (s >> 4) * 10;
		byte b = s & 0x0F;
		return (int)a + (int)b;
	}
	
	//returns current hour
	int RTC::hours() {
		byte s = readadr(0x02);
		byte a = (s >> 4) * 10;
		byte b = s & 0x0F;
		return (int)a + (int)b;
	}
	
	//returns current weekday
	int RTC::weekdays() {
		byte s = readadr(0x03);
		return s;
	}
	
	//returns current day
	int RTC::days() {
		byte s = readadr(0x04);
		int a = (s >> 4) * 10;
		int b = s & 0x0F;
		return (int)a + (int)b;
	}
	
	//returns current month
	int RTC::months() {
		byte s = readadr(0x05);
		byte a = ((s << 1) >> 5) * 10;
		byte b = s & 0x0F;
		return (int)a + (int)b;
	}
	
	//returns current year
	int RTC::years() {
		byte s = readadr(0x06);
		byte a = (s >> 4) * 10;
		byte b = s & 0x0F;
		return(int)a + (int)b;
	}
	//convert byte/int to bcd
	byte RTC::byteTobcd(int a) {
		int b = a % 10;
		a = (a / 10) << 4 ;
		byte f = a | b;
		return f;
	}
	
	//set Alarm 1
	//mode =  {A1M4, A1M3, A1M2, A1M1, DY/DT}
	void RTC::setA1 (byte second, byte minute, byte hour, byte dayofweek, byte day, bool mode[] ){
		
		second = byteTobcd(second);			//converting byte to bcd
		minute = byteTobcd(minute);
		hour = byteTobcd(hour);
		dayofweek = byteTobcd(dayofweek);
		day = byteTobcd(day);
		hour &= ~(1 << 6);			//settting 24 hour mode
		
		byte ctrreg = readadr(0x0E);
		ctrreg |= 0b00011101;				//Set A1F bit
		Wire.beginTransmission(adress); //Write CTRL REG
		Wire.write(0x0E);
		Wire.write(ctrreg);
		Wire.endTransmission();
		
		if (mode[3]) {					//setting DY/DT
			day |= (1 << 6);
		}
		else if (!mode[3]) {
			dayofweek &= ~(1 << 6);
		}
		
		if (mode[0] && mode[1] && mode[2] && mode[3]) {//modyfiying inputs according to mode
			second |= (1 << 7);
			minute |= (1 << 7);
			hour |= (1 << 7);
			dayofweek = (1 << 7);
			day = (1 << 7);
		}
		else if (mode[0] && mode[1] && mode[2] && !mode[3]) {
			second &= ~(1 << 7);
			minute |= (1 << 7);
			hour |= (1 << 7);
			dayofweek |= (1 << 7);
			day |= (1 << 7);
		}
		else if (mode[0] && mode[1] && !mode[2] && !mode[3]) {
			second &= ~(1 << 7);
			minute &= ~(1 << 7);
			hour |= (1 << 7);
			dayofweek |= (1 << 7);
			day |= (1 << 7);
		}
		else if (mode[0] && !mode[1] && !mode[2] && !mode[3]) {
			second &= ~(1 << 7);
			minute &= ~(1 << 7);
			hour &= ~(1 << 7);
			dayofweek |= (1 << 7);
			day |= (1 << 7);
		}
		else if (!mode[0] && !mode[1] && !mode[2] && !mode[3]) {
			second &= ~(1 << 7);
			minute &= ~(1 << 7);
			hour &= ~(1 << 7);
			dayofweek &= ~(1 << 7);
			day &= ~(1 << 7);
		}
		Wire.beginTransmission(adress);
		Wire.write(0x07);
		Wire.write(second);
		Wire.write(minute);
		Wire.write(hour);
		if (mode[3]) {
			Wire.write(day);
		}
		else Wire.write(dayofweek);
		Wire.endTransmission();
	}

	//set Alarm 2
	//mode =  {A2M4, A2M3, A2M2, DY/DT}
	void RTC::setA2 ( byte minute, byte hour, byte dayofweek, byte day, bool mode[]  ) {
		minute = byteTobcd(minute);
		hour = byteTobcd(hour);
		dayofweek = byteTobcd(dayofweek);
		day = byteTobcd(day);
		hour &= ~(1 << 6);

		byte ctrreg = readadr(0x0E);
		ctrreg |= 0b00011110;			//Set A2F bit
		Wire.beginTransmission(adress); //Write CTRL REG
		Wire.write(0x0E);
		Wire.write(ctrreg);
		Wire.endTransmission();

		if (mode[3]) {					//setting DY/DT
			day |= (1 << 6);
		}
		else if (!mode[3]) {
			dayofweek &= ~(1 << 6);
		}

		if (mode[0] && mode[1] && mode[2]) {	//modyfiying inputs according to mode
			minute |= (1 << 7);
			hour |= (1 << 7);
			dayofweek = (1 << 7);
			day = (1 << 7);
		}
		else if (mode[0] && mode[1] && !mode[2]) {
			minute &= ~(1 << 7);
			hour |= (1 << 7);
			dayofweek = (1 << 7);
			day = (1 << 7);
		}
		else if (mode[0] && !mode[1] && !mode[2]) {
			minute &= ~(1 << 7);
			hour &= ~(1 << 7);
			dayofweek = (1 << 7);
			day = (1 << 7);
		}
		else if (!mode[0] && !mode[1] && !mode[2]) {
			minute &= ~(1 << 7);
			hour &= ~(1 << 7);
			dayofweek &= ~(1 << 7);
			day &= ~(1 << 7);
		}
		Wire.beginTransmission(adress);
		Wire.write(0x0B);
		Wire.write(minute);
		Wire.write(hour);
		if (mode[3]) {
			Wire.write(day);
		}
		else Wire.write(dayofweek);
		Wire.endTransmission();


	}

	//returns string of current date and time : DD/MM/YY hh:mm:ss 
	String RTC::getdate() {
		String date =  (String)this->seconds() + "/" + (String)this->days() + "/" + (String)this->months()+ " ";
		return  date;
	}
	String RTC::gettime() {
		String time =  (String)this->years()+ ":" + (String)this->hours() + ":" + (String)this->minutes();
		return  time;
	}

	void RTC::setdate(byte second, byte minute, byte hour, byte dayofweek, byte day, byte month, byte year) {
		second = byteTobcd(second);
		minute = byteTobcd(minute);
		hour = byteTobcd(hour);
		dayofweek = byteTobcd(dayofweek);
		day = byteTobcd(day);
		month = byteTobcd(month);
		
		hour &= ~(1 << 6);
		Wire.beginTransmission(0x68);
		Wire.write(0x00);
		Wire.write(second);
		Wire.write(minute);
		Wire.write(hour);
		Wire.write(dayofweek);
		Wire.write(day);
		Wire.write(month);
		Wire.write(year);
		Wire.endTransmission();

	}
	bool RTC::isA1() {
		byte a = readadr(0x0F);
		a = ((a << 7) >> 7);
		return (bool) a;
	}
	bool RTC::isA2() {
		byte a = readadr(0x0F);
		a = ((a << 6) >> 7);
		return (bool) a;
	}
	void RTC::resetA1(){
		byte a = readadr(0x0F);
		a &= ~(0x01);
		Wire.beginTransmission(adress);
		Wire.write(0x0F);
		Wire.write(0x01);
		Wire.endTransmission();
	}
	void RTC::resetA2() {
		byte a = readadr(0x0F);
		a &= ~(0x02);
		Wire.beginTransmission(adress);
		Wire.write(0x0F);
		Wire.write(0x02);
		Wire.endTransmission();
	}
	void RTC::disableA1() {
		byte a = readadr(0x0E);
		a &= ~(0x01);
		Wire.beginTransmission(adress);
		Wire.write(0x0E);
		Wire.write(a);
		Wire.endTransmission();
	}
	void RTC::disableA2() {
		byte a = readadr(0x0E);
		a &= ~(0x02);
		Wire.beginTransmission(adress);
		Wire.write(0x0E);
		Wire.write(a);
		Wire.endTransmission();
	}
	void RTC::setSQW(int RS1, int RS2) {
		byte a = readadr(0x0E);
		byte b = (RS1 << 3);
		byte c = (RS2 << 4);
		a |= b;
		a |= c;
		a &= ~(1 << 2); //INTCN disable
		a &= ~(1 << 6);
		Wire.beginTransmission(adress);
		Wire.write(0x0E);
		Wire.write(a);
		Wire.endTransmission();

	}
	
	