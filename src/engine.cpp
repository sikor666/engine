#include "engine.h"

#include <iostream>

#include <SDL.h>
#include <glad.h>

// Window management
bool quit = false; // exit program
SDL_Window *window = nullptr;
SDL_GLContext glContext;
SDL_Event event;

// Window parameters
char title[] = "First Window"; // window's title
short unsigned int wHeight = 600;
short unsigned int wWidth = 800;
short unsigned int initialPosX = 100;
short unsigned int initialPosY = 100;

GLuint rendering_program;
GLuint vertex_array_object;

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

void OpenGLSet() // set up OpenGL
{
    GLint vpWidth, vpHeight;
    SDL_GL_GetDrawableSize(window, &vpWidth, &vpHeight);
}

GLuint compile_shaders(void)
{
    //GLuint vertex_shader_point;
    GLuint vertex_shader_triangle;
    GLuint fragment_shader;
    GLuint program;

    // Kod Ÿród³owy shadera wierzcho³ków dla pojedynczego punktu.
    static const GLchar * vertex_shader_source_point[] =
    {
    "#version 450 core \n"
    " \n"
    "void main(void) \n"
    "{ \n"
    " gl_Position = vec4(-0.6, 0.3, -1.0, 1.0); \n"
    "} \n"
    };

    // Kod Ÿród³owy shadera wierzcho³ków dla trójk¹ta.
    static const GLchar * vertex_shader_source_triangle[] =
    {
    "#version 450 core \n"
    " \n"
    "void main(void) \n"
    "{ \n"
    "   // Deklaracja zaszytej na sztywno tablicy wierzcho³ków. \n"
    "   const vec4 vertices[3] = vec4[3](vec4(0.25, -0.25, 0.5, 1.0), \n"
    "                                    vec4(-0.25, -0.25, 0.5, 1.0), \n"
    "                                    vec4(0.25, 0.25, 0.5, 1.0)); \n"
    " \n"
    "   // Wykorzystanie indeksu gl_VertexID. \n"
    "   gl_Position = vertices[gl_VertexID]; \n"
    "} \n"
    };

    // Kod Ÿród³owy shadera fragmentów.
    static const GLchar * fragment_shader_source[] =
    {
    "#version 450 core \n"
    " \n"
    "out vec4 color; \n"
    " \n"
    "void main(void) \n"
    "{ \n"
    " color = vec4(0.0, 0.8, 1.0, 1.0); \n"
    "} \n"
    };

    // Utworzenie i kompilacja shadera wierzcho³ków.
    // tworzy pusty obiekt shadera gotowy do przyjêcia
    // kodu Ÿród³owego przeznaczonego do kompilacji;
    // vertex_shader_point = glCreateShader(GL_VERTEX_SHADER);
    // przekazuje kod Ÿród³owy shadera do obiektu shadera (tworzy jego kopiê);
    // glShaderSource(vertex_shader_point, 1, vertex_shader_source_point, NULL);
    // kompiluje kod Ÿród³owy zawarty w obiekcie shadera;
    // glCompileShader(vertex_shader_point);

    // Utworzenie i kompilacja shadera wierzcho³ków.
    vertex_shader_triangle = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_triangle, 1, vertex_shader_source_triangle, NULL);
    glCompileShader(vertex_shader_triangle);

    // Utworzenie i kompilacja shadera fragmentów.
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    // Utworzenie programu, dodanie shaderów i ich po³¹czenie.
    // tworzy obiekt programu, do którego mo¿na do³¹czyæ obiekty shaderów;
    program = glCreateProgram();
    // do³¹cza obiekt shadera do obiektu programu;
    // glAttachShader(program, vertex_shader_point);
    glAttachShader(program, vertex_shader_triangle);
    glAttachShader(program, fragment_shader);
    // ³¹czy w jedn¹ ca³oœæ wszystkie dodane obiekty shaderów;
    glLinkProgram(program);

    // Usuniêcie shaderów, bo znajduj¹ siê ju¿ w programie.
    // usuwa obiekt shadera; po do³¹czeniu shadera do obiektu programu program
    // zawiera kod binarny i sam shader nie jest ju¿ potrzebny.
    // glDeleteShader(vertex_shader_point);
    glDeleteShader(vertex_shader_triangle);
    glDeleteShader(fragment_shader);

    return program;
}

void engine::startup()
{
    rendering_program = compile_shaders();

    // Przed narysowaniem czegokolwiek trzeba jeszcze
    // utworzyæ tak zwany VAO (ang.Vertex Array Object),
    // czyli obiekt tablicy wierzcho³ków.To obiekt
    // reprezentuj¹cy etap pobierania wierzcho³ków w OpenGL,
    // stanowi¹cy materia³ wejœciowy dla shadera wierzcho³ków
    glCreateVertexArrays(1, &vertex_array_object);

    // Powi¹zanie tablicy wierzcho³ków z kontekstem
    glBindVertexArray(vertex_array_object);
}

void engine::shutdown()
{
    glDeleteVertexArrays(1, &vertex_array_object);
    glDeleteProgram(rendering_program);
}

// Funkcja renderuj¹ca
void engine::render(double currentTime)
{
    // Wyczyszczenie okna kolorem
    const GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f,
                              (float)cos(currentTime) * 0.5f + 0.5f, 0.0f, 1.0f };
    glClearBufferfv(GL_COLOR, 0, color);

    // U¿ycie utworzonego wczeœniej obiektu programu.
    glUseProgram(rendering_program);

    // Funkcja ustawia œrednicê punktu w pikselach na zadany rozmiar.
    // OpenGL gwarantuje obs³ugê przynajmniej rozmiaru 64 piksele.
    glPointSize(64.0f);

    // Narysowanie jednego punktu.
    // Funkcja glDrawArrays() wysy³a wierzcho³ki do potoku OpenGL.
    // Dla ka¿dego wierzcho³ka zostanie wykonany shader wierzcho³ka. 
    // glDrawArrays(GL_POINTS, 0, 1);

    // Narysowanie trójk¹ta
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

int engine::run()
{
    init(); // Init SDL2 Glad

    startup();

    // Main loop
    while (!quit)
    {
        double sec = SDL_GetTicks() / 1000.0f;
        render(sec);

        std::cout << sec << std::endl;

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
                default:
                    break;
                }
                break;
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
