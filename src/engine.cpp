#include "engine.h"

#include "ArrayBuffer.h"
#include "UniformBuffer.h"
#include "CubeBuffer.h"
#include "TriangleTexture2D.h"
#include "WindowTextureKTX.h"
#include "SimpleTextureCoords.h"

#include "KeyboardController.h"

#include <shader.h>

#include <iostream>
#include <memory>

#include <SDL.h>
#include <glad.h>

// Window management
SDL_Window *window = nullptr;
SDL_GLContext glContext;
SDL_Event event;

KeyboardController keyboard;
std::unique_ptr<Screen> screen = std::make_unique<SimpleTextureCoords>();

// Window parameters
char title[] = "First Window"; // window's title
short unsigned int wWidth = 800;
short unsigned int wHeight = 600;
short unsigned int initialPosX = 100;
short unsigned int initialPosY = 100;

void _sdlError(const char *mes)
{
    fprintf(stderr, "%s: %s\n", mes, SDL_GetError());

    SDL_ClearError();

    exit(1);
}

void init()
{
    // Init SDL2 SDL_INIT_VIDEO - for video initialisation only
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        _sdlError("Could not initialize SDL");

    // Set attributes
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create window
    if (false)
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);
    else
        window = SDL_CreateWindow(title, initialPosX, initialPosY, wWidth, wHeight, SDL_WINDOW_OPENGL);

    if (window == nullptr)
        _sdlError("Could not create window");

    // Create OpenGL context
    glContext = SDL_GL_CreateContext(window);
    if (glContext == nullptr)
        _sdlError("Could not create the OpenGL context");

    // Load OpenGL functions glad SDL
    gladLoadGLLoader(SDL_GL_GetProcAddress);

    // V-Sync
    SDL_GL_SetSwapInterval(1);
}

void info()
{
    // Check OpenGL properties
    printf("Vendor:                 %s\n", glGetString(GL_VENDOR));
    printf("Renderer:               %s\n", glGetString(GL_RENDERER));
    printf("Version OpenGL:         %s\n", glGetString(GL_VERSION));
    printf("Version GLSL:           %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    GLint numExtension = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtension);
    printf("Number of extensions:   %d\n", numExtension);
}

void OpenGLSet() // set up OpenGL
{
    GLint vpWidth, vpHeight;
    SDL_GL_GetDrawableSize(window, &vpWidth, &vpHeight);
}

void engine::startup()
{
    screen->startup();
}

void engine::shutdown()
{
    screen->shutdown();
}

// Funkcja renderująca
void engine::render(double currentTime)
{
    screen->render(currentTime);
}

int engine::run()
{
    init(); // Init SDL2 Glad
    info();

    startup();

    // Main loop
    while (!keyboard.isPress(SDLK_ESCAPE))
    {
        double sec = SDL_GetTicks() / 1000.0f;
        render(sec);

        SDL_GL_SwapWindow(window); // swap buffers
        while (SDL_PollEvent(&event)) // handle events
        {
            switch (event.type)
            {
            case SDL_QUIT:
                keyboard.keyPress(SDLK_ESCAPE);
                break;
            case SDL_KEYDOWN:
                keyboard.keyPress(event.key.keysym.sym);
                switch (event.key.keysym.sym)
                {
                case SDLK_1:
                    screen->shutdown();
                    screen = std::make_unique<ArrayBuffer>(keyboard);
                    screen->startup();
                    break;
                case SDLK_2:
                    screen->shutdown();
                    screen = std::make_unique<UniformBuffer>(keyboard);
                    screen->startup();
                    break;
                case SDLK_3:
                    screen->shutdown();
                    screen = std::make_unique<CubeBuffer>();
                    screen->startup();
                    break;
                case SDLK_4:
                    screen->shutdown();
                    screen = std::make_unique<TriangleTexture2D>();
                    screen->startup();
                    break;
                case SDLK_5:
                    screen->shutdown();
                    screen = std::make_unique<WindowTextureKTX>();
                    screen->startup();
                    break;
                case SDLK_6:
                    screen->shutdown();
                    screen = std::make_unique<SimpleTextureCoords>();
                    screen->startup();
                    break;
                default:
                    screen->shutdown();
                    screen->startup();
                    break;
                }
            default:
                break;
            }
        }
    }

    shutdown();

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
