#include <PulseSensorPlayground.h>
#include <DS3231.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

int pulseData;
Time timeData;
const uint64_t pipe = 0xE8E8F0F0E1FE;
bool isOn = 1;
byte page = 1;
String textPulse;

#define PulseSensorPurplePin 8
RF24 radio(9, 10);
DS3231  rtc(SDA, SCL);

void setup() {
  radio.begin();
  radio.openWritingPipe(pipe);
  radio.setChannel(9);
  radio.setPALevel(RF24_PA_HIGH); 
  radio.setDataRate(RF24_2MBPS);
  radio.powerUp();
  radio.startListening();
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  rtc.setTime(12, 00, 00);
}

void loop() {
	for (byte i = 0; i < 1; i++)
	{
		wdt_enable(WDTO_500MS);
		sleep_mode();
		pulseData = analogRead(PulseSensorPurplePin);
		timeData = rtc.getTime();
		radio.write(&pulseData, sizeof(pulseData));
		radio.write(&timeData, sizeof(timeData));
		i = 0;
	}	
}
