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
float ratio;

static void pulse()
{
	//while (digitalRead(pin) == LOW); // risque de boucle infinie ?
	if (count < max)
		count++;
}

static void compute()
{
	if (total < ULONG_MAX / ratio)
		cpm = total * ratio;
	else
		cpm = max;
}

static void resize(int n)
{
	n = constrain(n, 1, 60);
	if (n < size)
	{
		if (n <= index)
		{
			size = index;
			index = 0;
		}
		for (int i = index; i < n; i++)
			buffer[i] = buffer[i + size - n];
		total = 0;
		for (int i = 0; i < n; i++)
			total += buffer[i];
	}
	else
	{
		for (int i = n; i > index;)
			buffer[--i] = --size < index ? 0 : buffer[size];
	}
	size = n;
	ratio = 60. / size;
	max = ULONG_MAX / size;
	if (count > max)
		count = max;
	compute();
}

void setup()
{
	int n;

	timer = millis();
	Serial.begin(speed);
	pinMode(pin, INPUT_PULLUP);
	attachInterrupt(interrupt, pulse, FALLING);
	EEPROM.get(0, n);
	resize(n);
}

void loop()
{
	unsigned long now = millis();

	if (now - timer >= 1000)
	{
		total -= buffer[index];
		buffer[index] = count;
		total += buffer[index];
		count -= buffer[index];
		if (++index >= size)
			index = 0;
		timer = now;
		compute();
	}
	Serial.println(cpm);
	delay(1);
}

void serialEvent()
{
	int n = Serial.parseInt();

	Serial.readStringUntil('\n');
	EEPROM.put(0, n);
	resize(n);
}
