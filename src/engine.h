#pragma once

#include "exported.h"

#include <memory>

class EXPORTED engine
{
public:
    engine();
    ~engine();

    int run();

private:
    void init(int& vpWidth, int& vpHeight);
    void info();
    void error(const char *mes);

    void startup();
    void render(double currentTime);
    void shutdown();

private:
    // Window parameters
    const char *title = "First Window"; // window's title
    const short unsigned int wWidth = 800;
    const short unsigned int wHeight = 600;
    const short unsigned int initialPosX = 100;
    const short unsigned int initialPosY = 100;

    std::unique_ptr<class Scene> scene;
};
