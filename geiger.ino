/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   geiger.ino                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouvier <abouvier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/08/04 23:30:42 by abouvier          #+#    #+#             */
/*   Updated: 2015/08/14 03:22:17 by abouvier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <EEPROM.h>
#include <limits.h>

const int pin = 2;
const int interrupt = 0;
const int speed = 9600;

int size;
int index;
unsigned long max;
unsigned long timer;
unsigned long buffer[60];
volatile unsigned long count;
unsigned long total;
unsigned long cpm;

static void load()
{
	index = 0;
	memset(buffer, 0, sizeof(buffer)); // garder les anciennes valeurs ?
	size = constrain(size, 1, 60);
	max = ULONG_MAX / size;
	if (count > max)
		count = max;
	total = 0;
	cpm = 0;
}

static void pulse()
{
	//while (digitalRead(pin) == LOW); // risque de boucle infinie ?
	if (count < max)
		count++;
}

void setup()
{
	Serial.begin(speed);
	pinMode(pin, INPUT_PULLUP);
	attachInterrupt(interrupt, pulse, FALLING);
	EEPROM.get(0, size);
	load();
}

void loop()
{
	unsigned long now = millis();

	if (now - timer >= 1000)
	{
		total -= buffer[index];
		buffer[index] = count;
		count -= buffer[index];
		total += buffer[index];
		if (total < ULONG_MAX / 60)
			cpm = (total * 60) / size;
		else
			cpm = max;
		if (++index >= size)
			index = 0;
		timer = now;
	}
	Serial.println(cpm);
	delay(1);
}

void serialEvent()
{
	size = Serial.parseInt();
	Serial.readStringUntil('\n');
	EEPROM.put(0, size);
	load();
}
