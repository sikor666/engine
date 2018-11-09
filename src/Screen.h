#pragma once

class Screen
{
public:
    Screen() = default;
    virtual ~Screen() = default;

    virtual void run()
    {
    }

    virtual void init()
    {
    }

    virtual void startup() = 0;
    virtual void render(double currentTime) = 0;
    virtual void shutdown() = 0;

    void setWindowTitle(const char * title)
    {
    }

    virtual void onResize(int w, int h)
    {
    }

    virtual void onKey(int key, int action)
    {
    }

    virtual void onMouseButton(int button, int action)
    {
    }

    virtual void onMouseMove(int x, int y)
    {
    }

    virtual void onMouseWheel(int pos)
    {
    }

    void getMousePosition(int& x, int& y)
    {
    }
};
