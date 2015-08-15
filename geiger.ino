/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   geiger.ino                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouvier <abouvier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/08/04 23:30:42 by abouvier          #+#    #+#             */
/*   Updated: 2015/08/16 00:45:56 by abouvier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Timer.h>
#include <CircularBuffer.h>
#include <MovingAverage.h>
#include <limits.h> // UINT_MAX = (word)-1

const int pin = 2;
const int interrupt = 0;
const int speed = 9600;

const int size = 10;

Timer timer;
volatile word count;
MovingAverage<word, size> buffer;
MovingAverage<unsigned long, 4> history;
unsigned long cpm;
unsigned long cpm2;

void setup()
{
	Serial.begin(speed);
	pinMode(pin, INPUT_PULLUP);
	attachInterrupt(interrupt, pulse, FALLING);
	timer.every(10000, tik);
	timer.every(1000, tok);
}

void loop()
{
	timer.update();
	Serial.println(cpm);
	delay(1);
}

static void pulse()
{
	//while (digitalRead(pin) == LOW); // risque de boucle infinie ?
	if (count < UINT_MAX / size)
		count++;
}

static void tik()
{
	history.push(buffer.average());
	cpm2 = history.average();
}

static void tok()
{
	buffer.push(const_cast<word &>(count));
	cpm = (60 * buffer.average() + 15 * cpm2) / 20;
	count -= buffer.back();
}
