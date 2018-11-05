#pragma once

#include "exported.h"

class EXPORTED engine
{
public:
    int run();

private:
    void startup();
    void render(double currentTime);
    void shutdown();
};
