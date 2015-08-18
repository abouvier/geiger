/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   geiger.ino                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouvier <abouvier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/08/04 23:30:42 by abouvier          #+#    #+#             */
/*   Updated: 2015/08/17 06:46:47 by abouvier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Timer.h>
#include <CircularBuffer.h>
#include <MovingAverage.h>
#include <limits.h> // UINT_MAX = (word)-1

const int pin = 2;
const int interrupt = 0;
const int speed = 9600;

const int size = 60;

Timer timer;
volatile word count;
MovingAverage<word, size> buffer;
MovingAverage<unsigned long, 4> history;
unsigned long cpm[2];

void setup()
{
	Serial.begin(speed);
	pinMode(pin, INPUT_PULLUP);
	attachInterrupt(interrupt, pulse, FALLING);
	//timer.every(10000, longCycle);
	timer.every(1000, shortCycle);
}

void loop()
{
	timer.update();
	/*Serial.println(cpm[0]);
	delay(1);*/
}

static void pulse()
{
	//while (digitalRead(pin) == LOW); // risque de boucle infinie ?
	if (count < UINT_MAX / size)
		count++;
}

static void shortCycle()
{
	buffer.push(const_cast<word &>(count));
	cpm[0] = buffer.sum();
	/*cpm[0] = buffer.sum() * 6;
	if (!history.empty())
		cpm[0] = (cpm[0] + cpm[1]) / 2;*/
	count -= buffer.back();
	Serial.println(cpm[0]);
}

static void longCycle()
{
	history.push(cpm[0]);
	cpm[1] = history.average();
}
