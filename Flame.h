#pragma once
#include <LEDCollection.h>

namespace JollyGood
{
	template<int COUNT>
	class Flame
	{
	public:
		Flame(LEDCollection & leds, uint8_t sparking, uint8_t cooling, uint32_t period, bool fromZero = true)
			: leds(leds), sparking(sparking), cooling(cooling), period(period), hueShift(0), fromZero(fromZero) {}

		void Tick()
		{
			uint32_t now = millis();
			if (now - previousTick > period)
			{
				previousTick = now;

				// heat travels up
				for (uint8_t i = COUNT - 1; i > 2; i--)
					heat[i] = (heat[i - 1] + heat[i - 2] * 2) / 3;

				// spark new heat at bottom
				if (random8() < sparking)
				{
					uint8_t spark = random8(4);
					heat[spark] = qadd8(heat[spark], random(160, 255));
				}

				for (uint8_t i = 0; i < COUNT; i++)
				{
					// Map heat
					FlameColor(leds[fromZero ? i : leds.Count() - i], heat[i]);

					// Cool down
					heat[i] = qsub8(heat[i], random8(0, cooling + 2));
				}
			}
		}

		void FlameColor(CRGB & color, uint8_t temperature)
		{
			// Map into three simple thirds
			uint8_t t192 = scale8_video(temperature, 191);
			uint8_t h, s, v;
			uint8_t heatramp = t192 & 0x3F; // 0..63
			switch (t192 & 0xC0)
			{
			case 0x00:
				h = 0;
				s = 255;
				v = heatramp << 2;
				break;
			case 0x40:
				h = heatramp;
				s = 255;
				v = 255;
				break;
			case 0x80:
				h = 64;
				s = 255-(heatramp << 2);
				v = 255;
				break;
			}
			color.setHSV((h + hueShift)%256, s, v);
		}
	private:
		uint8_t heat[COUNT];
		uint8_t sparking;
		uint8_t cooling;
		uint32_t period;

		uint32_t previousTick;
		LEDCollection & leds;

		bool fromZero;
	public:
		uint8_t hueShift;
	};
}

