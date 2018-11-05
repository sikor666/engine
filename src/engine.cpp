#include "engine.h"

#include <iostream>

#include <SDL.h>
#include <glad.h>

// SDLK_ESCAPE: exit program
// SDLK_t: trójkąt z teselacją
// SDLK_g: trójkąt po teselacji i dodaniu shadera geometrii
// SDLK_b: powrót do stanu początkowego
struct KeyboardController
{
public:
    void keyPress(SDL_Keycode k) { key = k; }
    bool isPress(SDL_Keycode k) { return k == key; }

private:
    SDL_Keycode key = SDLK_b;
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

GLuint compile_shaders(void)
{
    GLuint compute_shader;
    //GLuint vertex_shader_point;
    GLuint vertex_shader_triangle;
    GLuint tessellation_control_shader;
    GLuint tessellation_evaluation_shader;
    GLuint geometry_shader;
    GLuint fragment_shader;
    GLuint program;

    // Kod źródłowy shadera obliczeniowego, który nic nie robi.
    // Shader informuje OpenGL, że wielkość lokalnej grupy to 32 na 32 elementy.
    static const GLchar * compute_shader_source[] =
    {
    "#version 430 core                                  \n"
    "                                                   \n"
    "#extension GL_ABC_nowa_funkcja : enable            \n"
    "#extension GL_DEF_nowa_funkcja : require           \n"
    "                                                   \n"
    "layout(local_size_x = 32, local_size_y = 32) in;   \n"
    "                                                   \n"
    "void main(void)                                    \n"
    "{                                                  \n"
    "    // Nic nie rób.                                \n"
    "}                                                  \n"
    };

    // Kod źródłowy shadera wierzchołków dla pojedynczego punktu.
    static const GLchar * vertex_shader_source_point[] =
    {
    "#version 430 core                              \n"
    "                                               \n"
    "void main(void)                                \n"
    "{                                              \n"
    "   gl_Position = vec4(-0.6, 0.3, -1.0, 1.0);   \n"
    "}                                              \n"
    };

    // Kod źródłowy shadera wierzchołków dla trójkąta.
    static const GLchar * vertex_shader_source_triangle[] =
    {
    "#version 430 core                                                          \n"
    "                                                                           \n"
    "// ′offset′ i ′color′ to wejściowe atrybuty wierzchołka.                   \n"
    "layout (location = 0) in vec4 offset;                                      \n"
    "layout (location = 1) in vec4 color;                                       \n"
    "                                                                           \n"
    "// vs_color to wartość wyjściowa do przekazania do następnego shadera.     \n"
    "out vec4 vs_color;                                                         \n"
    "                                                                           \n"
    "void main(void)                                                            \n"
    "{                                                                          \n"
    "   // Deklaracja zaszytej na sztywno tablicy wierzchołków.                 \n"
    "   const vec4 vertices[3] = vec4[3](vec4( 0.25, -0.25, 0.5, 1.0),          \n"
    "                                    vec4(-0.25, -0.25, 0.5, 1.0),          \n"
    "                                    vec4( 0.25,  0.25, 0.5, 1.0));         \n"
    "                                                                           \n"
    "   const vec4 colors[] = vec4[3](vec4(1.0, 0.0, 0.0, 1.0),                 \n"
    "                                 vec4(0.0, 1.0, 0.0, 1.0),                 \n"
    "                                 vec4(0.0, 0.0, 1.0, 1.0));                \n"
    "                                                                           \n"
    "   // Wykorzystanie indeksu gl_VertexID.                                   \n"
    "   // Dodaj ′offset′ do umieszczonej na sztywno pozycji.                   \n"
    "   gl_Position = vertices[gl_VertexID] + offset;                           \n"
    "                                                                           \n"
    "   // Przekazanie do vs_color stałej wartości.                             \n"
    "   vs_color = colors[gl_VertexID] + color * 0.6;                           \n"
    "}                                                                          \n"
    };

    // Kod źródłowy shadera sterowania teselacją.
    static const GLchar * tessellation_control_shader_source[] =
    {
    "#version 430 core                                                              \n"
    "                                                                               \n"
    "layout (vertices = 3) out;                                                     \n"
    "                                                                               \n"
    "in vec4 vs_color[];                                                            \n"
    "patch out vec4 patch_color;                                                    \n"
    "                                                                               \n"
    "void main(void)                                                                \n"
    "{                                                                              \n"
    "   // Tylko, jeśli to wywołanie o identyfikatorze 0…                           \n"
    "   if (gl_InvocationID == 0)                                                   \n"
    "   {                                                                           \n"
    "       gl_TessLevelInner[0] = 5.0;                                             \n"
    "       gl_TessLevelOuter[0] = 5.0;                                             \n"
    "       gl_TessLevelOuter[1] = 5.0;                                             \n"
    "       gl_TessLevelOuter[2] = 5.0;                                             \n"
    "   }                                                                           \n"
    "                                                                               \n"
    "   // Zawsze kopiuj wejście na wyjście.                                        \n"
    "   gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;   \n"
    "   patch_color = vs_color[gl_InvocationID];                                    \n"
    "}                                                                              \n"
    };

    // Kod źródłowy shadera wyliczenia teselacji.
    static const GLchar * tessellation_evaluation_shader_source[] =
    {
    "#version 430 core                                          \n"
    "                                                           \n"
    "layout (triangles, equal_spacing, cw) in;                  \n"
    "                                                           \n"
    "patch in vec4 patch_color;                                 \n"
    "out vec4 vs_color;                                         \n"
    "                                                           \n"
    "void main(void)                                            \n"
    "{                                                          \n"
    "   gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position +  \n"
    "                  gl_TessCoord.y * gl_in[1].gl_Position +  \n"
    "                  gl_TessCoord.z * gl_in[2].gl_Position);  \n"
    "                                                           \n"
    "   vs_color = patch_color;                                 \n"
    "}                                                          \n"
    };

    // Kod źródłowy shadera geometrii.
    static const GLchar * geometry_shader_source[] =
    {
    "#version 430 core                              \n"
    "                                               \n"
    "layout (triangles) in;                         \n"
    "layout (points, max_vertices = 3) out;         \n"
    "                                               \n"
    "in vec4 vs_color[];                            \n"
    "out vec4 patch_color;                          \n"
    "                                               \n"
    "void main(void)                                \n"
    "{                                              \n"
    "   int i;                                      \n"
    "   for (i = 0; i < gl_in.length(); i++)        \n"
    "   {                                           \n"
    "       gl_Position = gl_in[i].gl_Position;     \n"
    "       patch_color = vs_color[i];              \n"
    "       EmitVertex();                           \n"
    "   }                                           \n"
    "   EndPrimitive();                             \n"
    "}                                              \n"
    };

    // Kod źródłowy shadera fragmentów.
    static const GLchar * fragment_shader_source[] =
    {
    "#version 430 core                                                              \n"
    "                                                                               \n"
    "// Dane z poprzedniego shadera                                                 \n"
    "in vec4 vs_color;                                                              \n"
    "in vec4 patch_color;                                                           \n"
    "                                                                               \n"
    "// Wynik kierowany do bufora ramki.                                            \n"
    "out vec4 color;                                                                \n"
    "                                                                               \n"
    "void main(void)                                                                \n"
    "{                                                                              \n"
    "   if (vs_color == vec4(0.0, 0.0, 0.0, 1.0))                                   \n"
    "   {                                                                           \n"
    "       color = patch_color;                                                    \n"
    "   }                                                                           \n"
    "   else                                                                        \n"
    "   {                                                                           \n"
    "       color = vs_color;                                                       \n"
    "   }                                                                           \n"
    "                                                                               \n"
//  "   color = vec4(sin(gl_FragCoord.x * 0.25) * 0.5 + 0.5,                        \n"
//  "                cos(gl_FragCoord.y * 0.25) * 0.5 + 0.5,                        \n"
//  "                sin(gl_FragCoord.x * 0.15) *                                   \n"
//  "                cos(gl_FragCoord.y * 0.15), 1.0) - color;                      \n"
    "}                                                                              \n"
    };

    // Utworzenie i kompilacja shadera obliczeniowego.
    compute_shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute_shader, 1, compute_shader_source, NULL);
    glCompileShader(compute_shader);

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

    // Utworzenie i kompilacja shadera sterowania teselacją.
    tessellation_control_shader = glCreateShader(GL_TESS_CONTROL_SHADER);
    glShaderSource(tessellation_control_shader, 1, tessellation_control_shader_source, NULL);
    glCompileShader(tessellation_control_shader);

    // Utworzenie i kompilacja shadera wyliczenia teselacji.
    tessellation_evaluation_shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
    glShaderSource(tessellation_evaluation_shader, 1, tessellation_evaluation_shader_source, NULL);
    glCompileShader(tessellation_evaluation_shader);

    // Utworzenie i kompilacja shadera geometrii.
    geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry_shader, 1, geometry_shader_source, NULL);
    glCompileShader(geometry_shader);

    // Utworzenie i kompilacja shadera fragmentów.
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    // Utworzenie programu, dodanie shaderów i ich połączenie.
    // tworzy obiekt programu, do którego można dołączyć obiekty shaderów;
    program = glCreateProgram();

    // dołącza obiekt shadera do obiektu programu;
    // glAttachShader(program, compute_shader);

    // glAttachShader(program, vertex_shader_point);
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
    // glDeleteShader(vertex_shader_point);
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
    glPointSize(6.9f);

    // Aby zobaczyć wynik działania mechanizmu teselacji, trzeba poinformować OpenGL, żeby rysował
    // jedynie zarysy wynikowych trójkątów.
    if (!keyboard.isPress(SDLK_b)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Funkcja glDrawArrays() wysyła wierzchołki do potoku OpenGL.
    // Dla każdego wierzchołka zostanie wykonany shader wierzchołka. 

    // Narysowanie jednego punktu.
    // glDrawArrays(GL_POINTS, 0, 1);

    // Narysowanie trójkąta
    if (!keyboard.isPress(SDLK_b)) glDrawArrays(GL_PATCHES, 0, 3);
    else glDrawArrays(GL_TRIANGLES, 0, 3);
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
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    keyboard.keyPress(SDLK_ESCAPE);
                    break;
                case SDLK_t:
                    keyboard.keyPress(SDLK_t);
                    rendering_program = compile_shaders();
                    break;
                case SDLK_g:
                    keyboard.keyPress(SDLK_g);
                    rendering_program = compile_shaders();
                    break;
                case SDLK_b:
                    keyboard.keyPress(SDLK_b);
                    rendering_program = compile_shaders();
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
