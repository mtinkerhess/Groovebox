#include "Constants.h"
#include "Voice/Envelope.h"
#include "GrooveboxWindow/GrooveboxWindow.h"
#include "Voice/Oscillator.h"
#include "Sequence.h"
#include "SequencerStream.h"
#include "Util.h"
#include "Input/InputSystem.h"

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

using namespace std;
using namespace sf;

int main()
{
    std::array<Sequence, Constants::NumTracks> sequences{};

    for (int i = 0; i < Constants::NumTracks; ++i)
    {
        sequences[i] = Sequence(16);
    }

    sequences[0].GetStep(0).SetOn(true);
    sequences[0].GetStep(2).SetOn(true);
    sequences[0].GetStep(4).SetOn(true);
    sequences[0].GetStep(6).SetOn(true);
    sequences[0].GetStep(8).SetOn(true);
    sequences[0].GetStep(10).SetOn(true);
    sequences[0].GetStep(12).SetOn(true);
    sequences[0].GetStep(14).SetOn(true);

    SequencerStream stream(sequences);

    sf::RenderWindow window{ sf::VideoMode(1920, 1080), "Groovebox" };
    window.setVerticalSyncEnabled(true);

    GrooveboxWindow grooveboxWindow(window, stream, sequences);

    stream.play();

    sf::Clock clock;
    sf::Time lastTime = clock.getElapsedTime();

    sf::Time deltaTimeAccumulator;
    int tickCount = 0;

    InputSystem& inputSystem = InputSystem::Instance();

    // run the program as long as the window is open
    while (window.isOpen())
    {
        sf::Time now = clock.getElapsedTime();
        sf::Time deltaTime = now - lastTime;
        lastTime = now;

        stream.Update(deltaTime.asSeconds());
        inputSystem.Update(deltaTime.asSeconds());
        grooveboxWindow.Tick(deltaTime.asSeconds());

        deltaTimeAccumulator += deltaTime;
        ++tickCount;

        while (deltaTimeAccumulator >= sf::seconds(1.0f))
        {
            cout << "FPS: " << tickCount << endl;
            tickCount = 0;
            deltaTimeAccumulator -= sf::seconds(1.0f);
        }
    }

    stream.OnApplicationQuit();

    return 0;
}