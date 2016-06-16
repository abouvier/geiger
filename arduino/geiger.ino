// geiger.ino - Geiger counter for Arduino
// Copyright (C) 2015-2016  abouvier <abouvier@student.42.fr>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <Arduino.h>
#include <CircularBuffer.h>
#include <MovingAverage.h>
#include <limits.h>

volatile unsigned long cps;
MovingAverage<volatile unsigned long, 60> cpm;
unsigned long timer;

static void pulse()
{
	if (cps < ULONG_MAX / 60)
		cps++;
}

void setup()
{
	Serial.begin(9600);
	pinMode(2, INPUT_PULLUP);
	attachInterrupt(0, pulse, FALLING);
	timer = millis();
}

void loop()
{
	if (millis() - timer >= 1000)
	{
		cpm.push(cps);
		Serial.println(cpm.sum() * (60 / cpm.size()));
		cps -= cpm.back();
		timer += 1000;
	}
}
