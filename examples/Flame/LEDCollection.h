#pragma once
#include <FastLED.h>

namespace JollyGood
{
	class LEDCollection
	{
	protected:
		LEDCollection(int count, CRGB * leds) : count(count), leds(leds) {}
	public:
		int Count() const { return count; }
		void Set(int index, CRGB value) { leds[index] = value; }
		CRGB & operator[] (int index) { return leds[index]; }

		template<template<uint8_t DATA_PIN, EOrder RGB_ORDER> class CHIPSET, uint8_t DATA_PIN, EOrder RGB_ORDER>
		void Setup()
		{
			FastLED.addLeds<WS2811, DATA_PIN, RGB_ORDER>(leds, count).setCorrection(TypicalLEDStrip);
			FastLED.setBrightness(255);
		}
	private:
		int count;
		CRGB * leds;
	};

	template<int COUNT>
	class NLEDCollection : public LEDCollection
	{
	public:
		NLEDCollection() : LEDCollection(COUNT, leds) {}
	private:
		CRGB leds[COUNT];
	};
}