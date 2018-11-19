#include "engine.h"

#include <shader.h>

#include <iostream>
#include <memory>

#include <SDL.h>
#include <glad.h>

#include "Scene.h"

// Window management
SDL_Window *window = nullptr;
SDL_GLContext glContext = nullptr;
SDL_Event event;

void engine::init()
{
    // Init SDL2 SDL_INIT_VIDEO - for video initialisation only
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        error("Could not initialize SDL");

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
        error("Could not create window");

    // Create OpenGL context
    glContext = SDL_GL_CreateContext(window);
    if (glContext == nullptr)
        error("Could not create the OpenGL context");

    // Load OpenGL functions glad SDL
    gladLoadGLLoader(SDL_GL_GetProcAddress);

    // V-Sync
    SDL_GL_SetSwapInterval(1);

    // set up OpenGL ... FIXME
    GLint vpWidth, vpHeight;
    SDL_GL_GetDrawableSize(window, &vpWidth, &vpHeight);
}

void engine::info()
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

void engine::error(const char *mes)
{
    fprintf(stderr, "%s: %s\n", mes, SDL_GetError());

    SDL_ClearError();

    exit(1);
}

void engine::startup()
{
    scene->startup();
}

void engine::shutdown()
{
    scene->shutdown();
}

// Funkcja renderująca
void engine::render(double currentTime)
{
    scene->render(currentTime);
}

int engine::run()
{
    info();

    startup();

    bool exit = false;

    // Main loop
    while (!exit)
    {
        double sec = SDL_GetTicks() / 1000.0f;
        render(sec);

        SDL_GL_SwapWindow(window); // swap buffers
        while (SDL_PollEvent(&event)) // handle events
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                scene->onMouseButton(event.button.button, SDL_MOUSEBUTTONDOWN);
                break;
            case SDL_MOUSEBUTTONUP:
                scene->onMouseButton(event.button.button, SDL_MOUSEBUTTONUP);
                break;
            case SDL_MOUSEMOTION:
                scene->onMouseMove(event.motion.x, event.motion.y);
                break;
            case SDL_KEYDOWN:
                scene->onKey(event.key.keysym.sym, SDL_KEYDOWN);

                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    exit = true;
                    break;
                default:
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

engine::engine()
{
    init(); // Init SDL2 Glad

    scene = std::make_unique<Scene>();
}

engine::~engine()
{
}
