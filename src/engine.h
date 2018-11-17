#pragma once

#include "exported.h"

class EXPORTED engine
{
public:
    engine();
    ~engine();

    int run();

private:
    void init();
    void info();
    void error(const char *mes);

    void startup();
    void render(double currentTime);
    void shutdown();

private:
    // Window parameters
    const char *title = "First Window"; // window's title
    short unsigned int wWidth = 800;
    short unsigned int wHeight = 600;
    short unsigned int initialPosX = 100;
    short unsigned int initialPosY = 100;
};
