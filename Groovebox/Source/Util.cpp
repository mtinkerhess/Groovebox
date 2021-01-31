#include "Util.h"

bool Util::IsShiftPressed()
{
	return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);
}

double Util::ConvertPitchToFrequency(double pitch)
{
	return Constants::Pitch0Frequency * pow(2.0, pitch / 12.0);
}