// geiger.ino - Geiger counter for Arduino
// Copyright (C) 2015-2016  Alexandre Bouvier <abouvier@student.42.fr>
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

#include "geiger.h"
#include <Arduino.h>
#include <CircularBuffer.h>
#include <MovingAverage.h>
#include <TinyTimer.h>
#include <limits.h>

volatile unsigned long cps;
MovingAverage<volatile unsigned long, CYCLE_SIZE> cpm;
TinyTimer<millis> timers[2];

static void count()
{
	if (cps < ULONG_MAX / 60)
		cps++;
}

static void buffer()
{
	noInterrupts();
	cpm.push(cps);
	cps = 0;
	interrupts();
}

static void print()
{
	Serial.println(cpm.sum() * (60 / cpm.capacity()));
}

void setup()
{
	Serial.begin(SERIAL_SPEED);
	pinMode(GEIGER_PIN, INPUT_PULLUP);
	attachInterrupt(GEIGER_INTERRUPT, count, FALLING);
	timers[1].init(print, PRINT_DELAY);
	timers[0].init(buffer, 1000);
}

void loop()
{
	TinyTimer<millis>::update(timers);
}
