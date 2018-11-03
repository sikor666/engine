#include "engine.h"

#include <cstdio>
#include <cstdlib>

#include <SDL.h>
#include <glad.h>

// Window management
bool quit = false; // exit program
SDL_Window *window = NULL;
SDL_GLContext glContext;
SDL_Event event;

// Window parameters
char title[] = "First Window"; // window's title
short unsigned int wHeight = 600;
short unsigned int wWidth = 800;
short unsigned int initialPosX = 100;
short unsigned int initialPosY = 100;

// OpenGL
GLuint shaderProgram;

GLuint VAO; // vertex array object
GLuint VBO; // vertex buffer object

GLuint vertices = 3;
GLfloat vVertices[] = { 0.0f, -0.5f, 0.0f,
                       -0.5f, 0.5f, 0.0f,
                        0.5f, 0.5f, 0.0f };

void _sdlError(const char *mes)
{
    fprintf(stderr, "%s: %s\n", mes, SDL_GetError());
    exit(1);
}

void Init()
{
    // Init SDL2     SDL_INIT_VIDEO - for video initialisation only
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

    if (window == NULL)
        _sdlError("Could not create window");

    // Create OpenGL context
    glContext = SDL_GL_CreateContext(window);
    if (glContext == NULL)
        _sdlError("Could not create the OpenGL context");

    // Load OpenGL functions glad SDL
    gladLoadGLLoader(SDL_GL_GetProcAddress);

    // V-Sync
    SDL_GL_SetSwapInterval(1);

    // Disable depth test and face culling.
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void PreLoop()
{
    // Check OpenGL properties
    printf("Vendor:          %s\n", glGetString(GL_VENDOR));
    printf("Renderer:        %s\n", glGetString(GL_RENDERER));
    printf("Version OpenGL:  %s\n", glGetString(GL_VERSION));
    printf("Version GLSL:    %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void LoadShaders()
{
    // Shaders
    const char* vertexShaderSource = "#version 450 core\n"
        "layout (location = 0) in vec3 position;\n"
        "void main()\n"
        "{\n"
        "gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 450 core\n"
        "out vec4 color;\n"
        "void main()\n"
        "{\n"
        "color = vec4(0.8f, 0.6f, 0.9f, 1.0f);\n"
        "}\n\0";

    // Build compile VERTEX_SHADER
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check for compile errors VERTEX_SHADER
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, sizeof(infoLog), NULL, infoLog);
        printf("Error: vertex shader compilation error: %s\n", infoLog);
    }

    // Build compile FRAGMENT_SHADER
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check for compile errors FRAGMENT_SHADER
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, sizeof(infoLog), NULL, infoLog);
        printf("Error: fragment shader compilation error: %s\n", infoLog);
    }

    // Link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // glBindFragDataLocation(shaderProgram, 0, "color");
    glLinkProgram(shaderProgram);

    // Check the linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
        printf("Error: linking error: %s\n", infoLog);
    }

    // Link shaders with application
    GLuint position = 0;
    glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Delete shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void OpenGLSet() // set up OpenGL
{
    GLint vpWidth, vpHeight;
    SDL_GL_GetDrawableSize(window, &vpWidth, &vpHeight);
    glViewport(0, 0, vpWidth, vpHeight);

    LoadShaders();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // Bind the Vertex Array Object first, then bind and set Vertex Buffers and attribute pointers
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // ?
    glBindVertexArray(0);

}

void DeleteGLTrash()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void DeleteSDLTrash()
{
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
}

int engine::run()
{
    Init(); // Init SDL2 Glad

    PreLoop(); // To do before loop (display graphics card info)

    OpenGLSet(); // set up OpenGL

    // Main loop
    while (!quit)
    {
        glClearColor(0.6f, 0.8f, 0.9f, 1.0f); // Clear the color buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw functions
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices);
        glBindVertexArray(0);

        SDL_GL_SwapWindow(window); // swap buffers
        while (SDL_PollEvent(&event)) // handle events
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                case SDLK_r:
                    break;
                case SDLK_g:
                    break;
                case SDLK_b:
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }
    }

    // To do before exit the program
    DeleteGLTrash();
    DeleteSDLTrash();
    SDL_Quit();

    return 0;
}
