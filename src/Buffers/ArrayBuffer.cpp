#include "ArrayBuffer.h"

void ArrayBuffer::compileShaders()
{
    GLuint compute_shader;
    GLuint vertex_shader_point;
    GLuint vertex_shader_triangle;
    GLuint tessellation_control_shader;
    GLuint tessellation_evaluation_shader;
    GLuint geometry_shader;
    GLuint fragment_shader;

    // Utworzenie i kompilacja shadera obliczeniowego.
    compute_shader = sb7::shader::load("../ext/engine/media/shaders/engine.cs.glsl", GL_COMPUTE_SHADER);
    // Utworzenie i kompilacja shadera wierzcho�k�w.
    vertex_shader_point = sb7::shader::load("../ext/engine/media/shaders/engine.1.vs.glsl", GL_VERTEX_SHADER);
    vertex_shader_triangle = sb7::shader::load("../ext/engine/media/shaders/engine.2.vs.glsl", GL_VERTEX_SHADER);
    // Utworzenie i kompilacja shadera sterowania teselacj�.
    tessellation_control_shader = sb7::shader::load("../ext/engine/media/shaders/engine.tcs.glsl", GL_TESS_CONTROL_SHADER);
    // Utworzenie i kompilacja shadera wyliczenia teselacji.
    tessellation_evaluation_shader = sb7::shader::load("../ext/engine/media/shaders/engine.tes.glsl", GL_TESS_EVALUATION_SHADER);
    // Utworzenie i kompilacja shadera geometrii.
    geometry_shader = sb7::shader::load("../ext/engine/media/shaders/engine.gs.glsl", GL_GEOMETRY_SHADER);
    // Utworzenie i kompilacja shadera fragment�w.
    fragment_shader = sb7::shader::load("../ext/engine/media/shaders/engine.fs.glsl", GL_FRAGMENT_SHADER);

    // Utworzenie programu, dodanie shader�w i ich po��czenie.
    // tworzy obiekt programu, do kt�rego mo�na do��czy� obiekty shader�w;
    program = glCreateProgram();

    // do��cza obiekt shadera do obiektu programu;
    // glAttachShader(program, compute_shader);

    if (keyboard.isPress(SDLK_v))
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

    // ��czy w jedn� ca�o�� wszystkie dodane obiekty shader�w;
    glLinkProgram(program);

    // Usuni�cie shader�w, bo znajduj� si� ju� w programie.
    // usuwa obiekt shadera; po do��czeniu shadera do obiektu programu program
    // zawiera kod binarny i sam shader nie jest ju� potrzebny.
    glDeleteShader(compute_shader);
    glDeleteShader(vertex_shader_point);
    glDeleteShader(vertex_shader_triangle);
    glDeleteShader(tessellation_control_shader);
    glDeleteShader(tessellation_evaluation_shader);
    glDeleteShader(geometry_shader);
    glDeleteShader(fragment_shader);
}

void ArrayBuffer::readPositionBuffer()
{
    // To dane, kt�re umie�cimy w obiekcie bufora.
    static const float vertex_data[] =
    {
         0.25, -0.25, 0.5, 1.0,
         -0.25, -0.25, 0.5, 1.0,
         0.25, 0.25, 0.5, 1.0,

         -0.3, 0.25, 0.5, 1.0,
         -0.3, -0.25, 0.5, 1.0,
         0.2, 0.25, 0.5, 1.0
    };

    GLuint buffer;

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(vertex_data),
        vertex_data,
        GL_STATIC_DRAW);
    glVertexAttribPointer(
        0, // Specifies the index of the generic vertex attribute to be modified.
           // Parametr attribindex to indeks atrybutu wierzcho�ka.Jako wej�cie dla shadera wierzcho�k�w mo�na
           // zdefiniowa� wiele r�nych atrybut�w, a nast�pnie odnosi� si� do nich na podstawie indeksu
        4, // Specifies the number of components per generic vertex attribute.Must be 1, 2, 3, 4.
           // Additionally, the symbolic constant GL_BGRA is accepted by glVertexAttribPointer.The initial value is 4.
           // Parametr size to liczba komponent�w zapami�tanych w buforze dla ka�dego wierzcho�ka.
        GL_FLOAT, // Parametr type to typ danych, kt�ry zazwyczaj jest jednym z typ�w z tabeli 5.3.
        GL_FALSE, // Parametr normalized informuje OpenGL, czy dane w buforze nale�y podda� normalizacji (przeskalowa�
                  // do zakresu od 0 do 1) przed przekazaniem do shadera wierzcho�k�w, czy te� pozostawi� niezmienione.
                  // Parametr jest ignorowany dla danych zmiennoprzecinkowych, ale dla typ�w takich jak
                  // GL_UNSIGNED_BYTE lub GL_INT ma znaczenie.Je�li na przyk�ad dane GL_UNSIGNED_BYTE zostan� poddane
                  // normalizacji, zostan� podzielone przez 255 (maksymalna dopuszczalna warto�� dla bajta bez
                  // znaku) przed przekazaniem do shadera wierzcho�k�w jako dane zmiennoprzecinkowe.Oznacza to,
                  // �e shader b�dzie korzysta� z danych w zakresie od 0 do 1. W wypadku danych bez normalizacji nast�pi
                  // rzutowanie na warto�ci zmiennoprzecinkowe, wi�c shader otrzyma dane w zakresie od 0 do 255.
                  // Nie nast�pi jednak �adne skalowanie warto�ci.
        0 /*sizeof(GLfloat) * 4*/, // Parametr stride informuje OpenGL, ile bajt�w znajduje si� mi�dzy pocz�tkiem danych
                                   // jednego wierzcho�ka a pocz�tkiem danych nast�pnego.Warto�� t� mo�na ustawi� na 0,
                                   // aby OpenGL samodzielnie wyliczy� odst�p na podstawie warto�ci size i type.
        NULL // Specifies a offset of the first component of the first generic vertex attribute in the array in the data
             // store of the buffer currently bound to the GL_ARRAY_BUFFER target. The initial value is 0.
    );
    glEnableVertexAttribArray(0);
}

void ArrayBuffer::readNamedBuffers()
{
    GLuint buffer[3];

    // To dane, kt�re umie�cimy w obiekcie bufora.
    static const GLfloat position_data[] =
    {
         0.25, -0.25, 0.5, 1.0,
        -0.25, -0.25, 0.5, 1.0,
         0.25,  0.25, 0.5, 1.0,

        -0.3,   0.25, 0.5, 1.0,
        -0.3,  -0.25, 0.5, 1.0,
         0.2,   0.25, 0.5, 1.0
    };

    GLfloat offset_data[] = { 0.3f, -0.6f, 0.5f, 0.0f };
    GLfloat color_data[] = { 0.5f, 0.5f, 0.0f, 0.0f };

    GLint position_location = glGetAttribLocation(program, "position");
    GLint offset_location = glGetAttribLocation(program, "offset");
    GLint color_location = glGetAttribLocation(program, "color");

    // Utworzenie bufor�w.
    glCreateBuffers(3, &buffer[0]);
    // Inicjalizacja pierwszego bufora.
    glNamedBufferStorage(buffer[0], sizeof(position_data), position_data, 0);
    // Dowi�zanie do tablicy wierzcho�k�w � zerowe przesuni�cie, krok = sizeof(vec4).
    glVertexArrayVertexBuffer(vao, position_location, buffer[0], 0, sizeof(GLfloat) * 4);
    // Poinformowanie OpenGL o formacie atrybutu.
    glVertexArrayAttribFormat(vao, position_location, 4, GL_FLOAT, GL_FALSE, 0);
    // Poinformowanie OpenGL, kt�re dowi�zanie bufora wierzcho�ka wykorzysta� dla atrybutu.
    glVertexArrayAttribBinding(vao, position_location, position_location);
    // W��czenie atrybutu.
    glEnableVertexAttribArray(0); // glEnableVertexArrayAttrib(vertex_array_object, 0);

    // Przeprowadzenie podobnej inicjalizacji dla drugiego bufora.
    glNamedBufferStorage(buffer[1], sizeof(offset_data), offset_data, 0);
    glVertexArrayVertexBuffer(vao, offset_location, buffer[1], 0, sizeof(GLfloat) * 4);
    glVertexArrayAttribFormat(vao, offset_location, 4, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, offset_location, offset_location);
    //glEnableVertexAttribArray(1); 
    glDisableVertexAttribArray(1);

    // Przeprowadzenie podobnej inicjalizacji dla trzeciego bufora.
    glNamedBufferStorage(buffer[2], sizeof(color_data), color_data, 0);
    glVertexArrayVertexBuffer(vao, color_location, buffer[2], 0, sizeof(GLfloat) * 4);
    glVertexArrayAttribFormat(vao, color_location, 4, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, color_location, color_location);
    // glEnableVertexAttribArray(2);
    // Po skorzystaniu z danych obiektu bufora do wype�nienia atrybutu wierzcho�ka mo�na wy��czy�
    // atrybut, stosuj�c funkcj� glDisableVertexAttribArray(), kt�rej prototyp ma posta� :
    // Po wy��czeniu atrybutu wierzcho�ka warto�� ponownie stanie si� statyczna i do jej przekazania do
    // shadera trzeba b�dzie zastosowa� funkcj� glVertexAttrib*().
    glDisableVertexAttribArray(2);
}

void ArrayBuffer::interleavedAttributes()
{
    struct vertex
    {
        // po�o�enie
        float x;
        float y;
        float z;
        // kolor
        float r;
        float g;
        float b;
    };

    GLuint buffer;

    GLint position_location = glGetAttribLocation(program, "position");
    GLint color_location = glGetAttribLocation(program, "color");

    static const vertex vertices[] =
    {
        { 0.25, -0.25, 0.5,     1.0, 0.0, 0.0},
        {-0.25, -0.25, 0.5,     0.0, 1.0, 0.0},
        { 0.25,  0.25, 0.5,     0.0, 0.0, 1.0},

        {-0.3,   0.25, 0.5,     0.0, 0.0, 1.0},
        {-0.3,  -0.25, 0.5,     0.0, 1.0, 0.0},
        { 0.2,   0.25, 0.5,     1.0, 0.0, 0.0}
    };

    // Alokacja i inicjalizacja obiektu bufora.
    glCreateBuffers(1, &buffer);
    glNamedBufferStorage(buffer, sizeof(vertices), vertices, 0);

    // Konfiguracja dw�ch atrybut�w wierzcho�ka � najpierw po�o�enie.
    glVertexArrayAttribBinding(vao, position_location, 0);
    glVertexArrayAttribFormat(vao, position_location, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, x));
    glEnableVertexArrayAttrib(vao, position_location);

    // Nast�pnie kolory.
    glVertexArrayAttribBinding(vao, color_location, 0);
    glVertexArrayAttribFormat(vao, color_location, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, r));
    glEnableVertexArrayAttrib(vao, color_location);

    // Na ko�cu dowi�zanie jedynego bufora do obiektu tablicy wierzcho�k�w.
    glVertexArrayVertexBuffer(vao, 0, buffer, 0, sizeof(float) * 6);
}
