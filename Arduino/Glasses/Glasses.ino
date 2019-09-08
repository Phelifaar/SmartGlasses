#include <DS3231.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <GyverButton.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

int pulse, pulseData;
Time time, timeData;
char* dataData;
const uint64_t pipe = 0xE8E8F0F0E1FE;
bool isOn = 1;
byte page = 1;
String textPulse;

#define butt 8
Adafruit_PCD8544 display = Adafruit_PCD8544(3, 4, 5, 6, 7);
RF24 radio(9, 10);
GButton button(butt);
DS3231  rtc(SDA, SCL);

void setup() {
  display.begin();
  display.clearDisplay();
  display.display();
  display.setContrast(50);

  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.setChannel(9);
  radio.setPALevel(RF24_PA_HIGH); 
  radio.setDataRate(RF24_2MBPS);
  radio.powerUp();
  radio.startListening();

  pinMode(butt, INPUT_PULLUP);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void loop() {
	if (button.isHolded() && !isOn) on();
	if (button.isHolded() && isOn) off();
	if (button.isSingle()) page++;
	if (page > 2) page = 1;
	if (radio.available()) {
		radio.read(&pulseData, sizeof(pulseData));
		pulse = pulseData;
		radio.read(&timeData, sizeof(timeData));
		time.hour = timeData.hour;
		time.min = timeData.min;
		time.sec = timeData.sec;
	}
	video();
	pulseSensor(15);
}

void on() {
	wdt_enable(WDTO_15MS);
	radio.powerUp();
	isOn = 1;
}

void off() {
	sleep_mode();
	radio.powerDown();
	isOn = 0;
}

void video() {
	switch (page)
	{
	case 1:
		display.setTextColor(BLACK);
		display.setCursor(2, 2);
		display.setTextSize(2);
		display.print(time.hour + '/' + time.min + '/' + time.sec);
		display.display();
		break;
	case 2:
		display.setTextColor(BLACK);
		display.setCursor(2, 2);
		display.setTextSize(2);
		display.print(pulse);
		display.setCursor(4, 0);
		display.setTextSize(1);
		display.print(textPulse);
		display.display();
		break;
	}
}

void pulseSensor(byte old) {
	byte minPulse, maxPulse;
	if (old >= 10 && old < 15) {
		minPulse = 60;
		maxPulse = 100;
	}
	if (old >= 15 && old < 30) {
		minPulse = 60;
		maxPulse = 80;
	}
	if (old >= 30 && old < 40) {
		minPulse = 55;
		maxPulse = 95;
	}
	if (old >= 40 && old < 60) {
		minPulse = 50;
		maxPulse = 95;
	}
	if (old >= 60 && old < 80) {
		minPulse = 70;
		maxPulse = 90;
	}
	if (pulse <= minPulse + 5) textPulse = "Low pulse";
	if (pulse >= maxPulse - 5) textPulse = "High pulse";
	if (pulse <= maxPulse - 5 && pulse >= minPulse + 5) textPulse = "Normal pulse";
}
