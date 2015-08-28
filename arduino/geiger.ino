/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   geiger.ino                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abouvier <abouvier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/08/04 23:30:42 by abouvier          #+#    #+#             */
/*   Updated: 2015/08/25 22:01:54 by abouvier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
		Serial.println(buffer.sum() * (60. / size), 0);
		count -= value;
		past = now;
	}
}
