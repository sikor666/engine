#include "engine.h"
#include "buffers.h"

#include <shader.h>

#include <iostream>

#include <SDL.h>
#include <glad.h>

// SDLK_ESCAPE: exit program
// SDLK_1: 49
// SDLK_t: trójkąt z teselacją 116
// SDLK_g: trójkąt po teselacji i dodaniu shadera geometrii 103
// SDLK_b: powrót do stanu początkowego 98
struct KeyboardController
{
public:
    void keyPress(SDL_Keycode k) { key = k; }
    bool isPress(SDL_Keycode k) { return k == key; }

private:
    SDL_Keycode key = SDLK_1;
} keyboard;

// Window management
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

GLuint compile_shaders()
{
    GLuint compute_shader;
    GLuint vertex_shader_point;
    GLuint vertex_shader_triangle;
    GLuint tessellation_control_shader;
    GLuint tessellation_evaluation_shader;
    GLuint geometry_shader;
    GLuint fragment_shader;
    GLuint program;

    // Utworzenie i kompilacja shadera obliczeniowego.
    compute_shader = sb7::shader::load("../ext/engine/media/shaders/engine.1.vs.glsl", GL_COMPUTE_SHADER);
    // Utworzenie i kompilacja shadera wierzchołków.
    vertex_shader_point = sb7::shader::load("../ext/engine/media/shaders/engine.1.vs.glsl", GL_VERTEX_SHADER);
    vertex_shader_triangle = sb7::shader::load("../ext/engine/media/shaders/engine.2.vs.glsl", GL_VERTEX_SHADER);
    // Utworzenie i kompilacja shadera sterowania teselacją.
    tessellation_control_shader = sb7::shader::load("../ext/engine/media/shaders/engine.tcs.glsl", GL_TESS_CONTROL_SHADER);
    // Utworzenie i kompilacja shadera wyliczenia teselacji.
    tessellation_evaluation_shader = sb7::shader::load("../ext/engine/media/shaders/engine.tes.glsl", GL_TESS_EVALUATION_SHADER);
    // Utworzenie i kompilacja shadera geometrii.
    geometry_shader = sb7::shader::load("../ext/engine/media/shaders/engine.gs.glsl", GL_GEOMETRY_SHADER);
    // Utworzenie i kompilacja shadera fragmentów.
    fragment_shader = sb7::shader::load("../ext/engine/media/shaders/engine.fs.glsl", GL_FRAGMENT_SHADER);

    // Utworzenie programu, dodanie shaderów i ich połączenie.
    // tworzy obiekt programu, do którego można dołączyć obiekty shaderów;
    program = glCreateProgram();

    // dołącza obiekt shadera do obiektu programu;
    // glAttachShader(program, compute_shader);

    if (keyboard.isPress(SDLK_1))
        glAttachShader(program, vertex_shader_point);
    else
        glAttachShader(program, vertex_shader_triangle);

    if (!keyboard.isPress(SDLK_b) && (keyboard.isPress(SDLK_t) || keyboard.isPress(SDLK_g)))
    {
        glAttachShader(program, tessellation_control_shader);
        glAttachShader(program, tessellation_evaluation_shader);

        if (keyboard.isPress(SDLK_g))
            glAttachShader(program, geometry_shader);
    }

    glAttachShader(program, fragment_shader);

    // łączy w jedną całość wszystkie dodane obiekty shaderów;
    glLinkProgram(program);

    // Usunięcie shaderów, bo znajdują się już w programie.
    // usuwa obiekt shadera; po dołączeniu shadera do obiektu programu program
    // zawiera kod binarny i sam shader nie jest już potrzebny.
    glDeleteShader(compute_shader);
    glDeleteShader(vertex_shader_point);
    glDeleteShader(vertex_shader_triangle);
    glDeleteShader(tessellation_control_shader);
    glDeleteShader(tessellation_evaluation_shader);
    glDeleteShader(geometry_shader);
    glDeleteShader(fragment_shader);

    return program;
}

void engine::startup()
{
    // Gdy już uda się określić kierunek zwrotu trójkąta, OpenGL może pominąć trójkąty zwrócone przodem,
    // tyłem lub nawet oba rodzaje.Domyślnie OpenGL renderuje wszystkie trójkąty niezależnie od
    // sposobu ich zwrotu.Aby włączyć usuwanie zbędnych trójkątów, wywołaj funkcję glEnable() z
    // wartością stałej GL_CULL_FACE.Domyślnie OpenGL usunie trójkąty skierowane tyłem.Aby zmienić rodzaj
    // usuwanych trójkątów, wywołaj funkcję glCullFace() i przekaż jej wartość GL_FRONT, GL_BACK lub
    // GL_FRONT_AND_BACK.
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT);

    rendering_program = compile_shaders();

    // Teselacja w OpenGL działa poprzez rozbicie powierzchni wysokiego poziomu nazywanych płatami
    // (ang.patch) na punkty, linie i trójkąty.Każdy płat składa się z pewnej liczby punktów sterujących
    // (ang.control points).Ich liczbę konfiguruje się, wywołując funkcję glPatchParameteri() z 
    // parametrem pname ustawionym na GL_PATCH_VERTICES i parametrem value ustawionym na liczbę
    // punktów mających tworzyć płat.
    // Domyślnie liczba punktów sterujących na płat wynosi 3. Jeśli właśnie taka liczba punktów jest
    // niezbędna(jak to ma miejsce w przykładowej aplikacji), nie trzeba tej funkcji w ogóle wywoływać.
    // Maksymalna liczba punktów sterujących dla pojedynczego płata zależy od implementacji sterowników,
    // ale OpenGL gwarantuje, że nie będzie mniejsza niż 32.
    // glPatchParameteri(GL_PATCH_VERTICES, 3);

    // Przed narysowaniem czegokolwiek trzeba jeszcze
    // utworzyć tak zwany VAO (ang.Vertex Array Object),
    // czyli obiekt tablicy wierzchołków.To obiekt
    // reprezentujący etap pobierania wierzchołków w OpenGL,
    // stanowiący materiał wejściowy dla shadera wierzchołków
    glGenVertexArrays(1, &vertex_array_object);

    // Powiązanie tablicy wierzchołków z kontekstem
    glBindVertexArray(vertex_array_object);

    //read_position_buffer();
    //read_named_buffers(rendering_program, vertex_array_object);
    interleaved_attributes(rendering_program, vertex_array_object);

    fillUniformBlock(rendering_program);
    printUniformBlocks(rendering_program);
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
    const GLfloat disco[] = { (float)sin(currentTime) * 0.5f + 0.5f,
                              (float)cos(currentTime) * 0.5f + 0.5f, 0.0f, 1.0f };
    const GLfloat blue[] = { 0.2f, 0.5f, 0.8f, 1.0f };
    glClearBufferfv(GL_COLOR, 0, blue);

    // Użycie utworzonego wcześniej obiektu programu.
    glUseProgram(rendering_program);

    GLfloat circle[] = { (float)sin(currentTime) * 0.5f,
                         (float)cos(currentTime) * 0.6f, 0.0f, 0.0f };

    GLfloat offset[] = { (float)cos(currentTime) * 0.5f,
                         (float)cos(currentTime) * 0.6f, 0.0f, 0.0f };

    GLfloat colore[] = { (float)cos(currentTime) * 0.5f + 0.5f,
                         (float)sin(currentTime) * 0.5f + 0.5f, 0.0f, 0.0f };

    //Jeśli funkcja glGetUniformLocation() zwróci wartość - 1, oznacza to, że w danym programie nie
    //ma zmiennej o podanej nazwie.Warto pamiętać, że nawet gdy shader skompiluje się prawidłowo,
    //zmienna uniform może „zniknąć” z programu, jeśli nie zostanie wykorzystana bezpośrednio w przynajmniej
    //jednym z shaderów dołączonych do programu(i to nawet wtedy, gdy jawnie wskazano
    //w deklaracji jej położenie).Najczęściej nie trzeba się przejmować optymalizacją zmiennych uniform,
    //które są zadeklarowane, ale nieużywane, bo kompilator sam zajmie się sprawą.Nazwy zmiennych
    //w shaderach są czułe na wielkość liter, więc w zapytaniach o położenie trzeba uważać na to, żeby użyć
    //właściwej lokalizacji.
    GLint discoLocation = glGetUniformLocation(rendering_program, "disco");
    glUniform4fv(discoLocation, 1, circle);

    // Aktualizacja wartości atrybutu wejściowego 0.
    glVertexAttrib4fv(1, offset);

    // Aktualizacja wartości atrybutu wejściowego 1.
    glVertexAttrib4fv(2, colore);

    // Funkcja ustawia średnicę punktu w pikselach na zadany rozmiar.
    // OpenGL gwarantuje obsługę przynajmniej rozmiaru 64 piksele.
    glPointSize(6.9f);

    // Aby zobaczyć wynik działania mechanizmu teselacji, trzeba poinformować OpenGL, żeby rysował
    // jedynie zarysy wynikowych trójkątów.
    if (!keyboard.isPress(SDLK_b)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Funkcja glDrawArrays() wysyła wierzchołki do potoku OpenGL.
    // Dla każdego wierzchołka zostanie wykonany shader wierzchołka. 

    // Narysowanie jednego punktu.
    // glDrawArrays(GL_POINTS, 0, 1);

    // Narysowanie trójkątów
    if (keyboard.isPress(SDLK_1)) glDrawArrays(GL_TRIANGLES, 0, 6);
    else
        if (!keyboard.isPress(SDLK_b)) glDrawArrays(GL_PATCHES, 0, 6);
        else glDrawArrays(GL_TRIANGLES, 0, 6);
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
                rendering_program = compile_shaders();
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
