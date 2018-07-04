#pragma once
#include <SoftwareSerial.h>

namespace JollyGood
{
	class SerialConnection
	{
	public:
		virtual int read() = 0;
		virtual int available() = 0;
		virtual void print(const char * text) = 0;
	};

	class DefaultSerialConnection : public SerialConnection
	{
	public:
		int available() { return Serial.available(); }
		int read() { return Serial.read(); }
		void print(const char * text) { return Serial.print(text); }
	};

	class SoftwareSerialConnection : public SerialConnection
	{
	public:
		SoftwareSerialConnection(SoftwareSerial & serial) : serial(serial) {}
		int available() { return serial.available(); }
		int read() { return serial.read(); }
		void print(const char * text) { return serial.print(text); }
	private:
		SoftwareSerial & serial;
	};

	class SerialCommandConnection
	{
	public:
		SerialCommandConnection(SerialConnection & serial, const char *(*handler)(const char * cmd)) : serial(serial), handler(handler), index(0) {}

		void Tick()
		{
			if (serial.available())
			{
				cmd[index] = serial.read();

				if (cmd[index] == '\n')
				{
					cmd[index + 1] = 0;
					handler(cmd);
					index = 0;
				}
				else if (index >= sizeof(cmd) - 1)
				{
					index = 0;
				}
				else
				{
					index++;
				}
			}
		}

	private:
		SerialConnection & serial;
		const char * (*handler)(const char * cmd);
		char cmd[64];
		int index;
	};
}