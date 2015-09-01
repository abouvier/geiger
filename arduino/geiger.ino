/*
	geiger.ino -- geiger counter for Arduino
	Copyright (C) 2015  abouvier <abouvier@student.42.fr>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <CircularBuffer.h>
#include <MovingAverage.h>
#include <limits.h>

const int pin = 2;
const int interrupt = 0;
const int speed = 9600;
const int size = 60;

volatile unsigned long count;
MovingAverage<unsigned long, size> buffer;
unsigned long past;

static void pulse()
{
	if (count < ULONG_MAX / size)
		count++;
}

void setup()
{
	Serial.begin(speed);
	pinMode(pin, INPUT_PULLUP);
	attachInterrupt(interrupt, pulse, FALLING);
	past = millis();
}

void loop()
{
	unsigned long now = millis();

	if (now - past >= 1000)
	{
		unsigned long value = count;

		buffer.push(value);
		Serial.println(buffer.sum() * (60. / size), 0); // possible overflow !
		count -= value;
		past = now;
	}
}
