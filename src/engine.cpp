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

    // Kod źródłowy shadera wierzchołków dla pojedynczego punktu.
    static const GLchar * vertex_shader_source_point[] =
    {
    "#version 450 core                              \n"
    "                                               \n"
    "void main(void)                                \n"
    "{                                              \n"
    "   gl_Position = vec4(-0.6, 0.3, -1.0, 1.0);   \n"
    "}                                              \n"
    };

    // Kod źródłowy shadera wierzchołków dla trójkąta.
    static const GLchar * vertex_shader_source_triangle[] =
    {
    "#version 450 core                                                          \n"
    "                                                                           \n"
    "// ′offset′ i ′color′ to wejściowe atrybuty wierzchołka.                   \n"
    "layout (location = 0) in vec4 offset;                                      \n"
    "layout (location = 1) in vec4 color;                                       \n"
    "                                                                           \n"
    "// Deklaracja VS_OUT jako deklaracja wyjściowego bloku interfejsu.         \n"
    "out VS_OUT                                                                 \n"
    "{                                                                          \n"
    "   vec4 color; // Wysłanie ′color′ do następnego etapu.                    \n"
    "} vs_out;                                                                  \n"
    "                                                                           \n"
    "void main(void)                                                            \n"
    "{                                                                          \n"
    "   // Deklaracja zaszytej na sztywno tablicy wierzchołków.                 \n"
    "   const vec4 vertices[3] = vec4[3](vec4( 0.25, -0.25, 0.5, 1.0),          \n"
    "                                    vec4(-0.25, -0.25, 0.5, 1.0),          \n"
    "                                    vec4( 0.25,  0.25, 0.5, 1.0));         \n"
    "                                                                           \n"
    "   // Wykorzystanie indeksu gl_VertexID.                                   \n"
    "   // Dodaj ′offset′ do wbudowanej zmiennej (ang. built - in variable)     \n"
    "   gl_Position = vertices[gl_VertexID] + offset;                           \n"
    "                                                                           \n"
    "   // Przekazanie do vs_color stałej wartości.                             \n"
    "   vs_out.color = color;                                                   \n"
    "}                                                                          \n"
    };

    // Kod źródłowy shadera fragmentów.
    static const GLchar * fragment_shader_source[] =
    {
    "#version 450 core                                                              \n"
    "                                                                               \n"
    "// Deklaracja VS_OUT jako wejściowy blok interfejsu.                           \n"
    "in VS_OUT                                                                      \n"
    "{                                                                              \n"
    "   vec4 color; // Odebranie koloru z poprzedniego etapu.                       \n"
    "} fs_in;                                                                       \n"
    "                                                                               \n"
    "// Wynik kierowany do bufora ramki.                                            \n"
    "out vec4 color;                                                                \n"
    "                                                                               \n"
    "void main(void)                                                                \n"
    "{                                                                              \n"
    "   // Proste przypisanie danych koloru z shadera wierzchołków do bufora ramki. \n"
    "   color = fs_in.color;                                                        \n"
    "}                                                                              \n"
    };

    // Utworzenie i kompilacja shadera wierzchołków.
    // tworzy pusty obiekt shadera gotowy do przyjęcia
    // kodu źródłowego przeznaczonego do kompilacji;
    // vertex_shader_point = glCreateShader(GL_VERTEX_SHADER);
    // przekazuje kod źródłowy shadera do obiektu shadera (tworzy jego kopię);
    // glShaderSource(vertex_shader_point, 1, vertex_shader_source_point, NULL);
    // kompiluje kod źródłowy zawarty w obiekcie shadera;
    // glCompileShader(vertex_shader_point);

    // Utworzenie i kompilacja shadera wierzchołków.
    vertex_shader_triangle = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_triangle, 1, vertex_shader_source_triangle, NULL);
    glCompileShader(vertex_shader_triangle);

    // Utworzenie i kompilacja shadera fragmentów.
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    // Utworzenie programu, dodanie shaderów i ich połączenie.
    // tworzy obiekt programu, do którego można dołączyć obiekty shaderów;
    program = glCreateProgram();
    // dołącza obiekt shadera do obiektu programu;
    // glAttachShader(program, vertex_shader_point);
    glAttachShader(program, vertex_shader_triangle);
    glAttachShader(program, fragment_shader);
    // łączy w jedną całość wszystkie dodane obiekty shaderów;
    glLinkProgram(program);

    // Usunięcie shaderów, bo znajdują się już w programie.
    // usuwa obiekt shadera; po dołączeniu shadera do obiektu programu program
    // zawiera kod binarny i sam shader nie jest już potrzebny.
    // glDeleteShader(vertex_shader_point);
    glDeleteShader(vertex_shader_triangle);
    glDeleteShader(fragment_shader);

    return program;
}

void engine::startup()
{
    rendering_program = compile_shaders();

    // Przed narysowaniem czegokolwiek trzeba jeszcze
    // utworzyć tak zwany VAO (ang.Vertex Array Object),
    // czyli obiekt tablicy wierzchołków.To obiekt
    // reprezentujący etap pobierania wierzchołków w OpenGL,
    // stanowiący materiał wejściowy dla shadera wierzchołków
    glCreateVertexArrays(1, &vertex_array_object);

    // Powiązanie tablicy wierzchołków z kontekstem
    glBindVertexArray(vertex_array_object);
}

void engine::shutdown()
{
    glDeleteVertexArrays(1, &vertex_array_object);
    glDeleteProgram(rendering_program);
}

// Funkcja renderująca
void engine::render(double currentTime)
{
    // Wyczyszczenie okna kolorem
    const GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f,
                              (float)cos(currentTime) * 0.5f + 0.5f, 0.0f, 1.0f };
    glClearBufferfv(GL_COLOR, 0, color);

    // Użycie utworzonego wcześniej obiektu programu.
    glUseProgram(rendering_program);

    GLfloat offset[] = { (float)cos(currentTime) * 0.5f,
                         (float)cos(currentTime) * 0.6f, 0.0f, 0.0f };

    GLfloat colore[] = { (float)cos(currentTime) * 0.5f + 0.5f,
                         (float)sin(currentTime) * 0.5f + 0.5f, 0.0f, 0.0f };

    // Aktualizacja wartości atrybutu wejściowego 0.
    glVertexAttrib4fv(0, offset);

    // Aktualizacja wartości atrybutu wejściowego 1.
    glVertexAttrib4fv(1, colore);

    // Funkcja ustawia średnicę punktu w pikselach na zadany rozmiar.
    // OpenGL gwarantuje obsługę przynajmniej rozmiaru 64 piksele.
    // glPointSize(64.0f);

    // Narysowanie jednego punktu.
    // Funkcja glDrawArrays() wysyła wierzchołki do potoku OpenGL.
    // Dla każdego wierzchołka zostanie wykonany shader wierzchołka. 
    // glDrawArrays(GL_POINTS, 0, 1);

    // Narysowanie trójkąta
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
