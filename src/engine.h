#pragma once

#include "exported.h"

class EXPORTED engine
{
public:
    int run();

private:
    void render(unsigned __int32 currentTime);
};
