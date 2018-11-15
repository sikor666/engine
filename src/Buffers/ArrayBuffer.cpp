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
    // Utworzenie i kompilacja shadera wierzcho³ków.
    vertex_shader_point = sb7::shader::load("../ext/engine/media/shaders/engine.1.vs.glsl", GL_VERTEX_SHADER);
    vertex_shader_triangle = sb7::shader::load("../ext/engine/media/shaders/engine.2.vs.glsl", GL_VERTEX_SHADER);
    // Utworzenie i kompilacja shadera sterowania teselacj¹.
    tessellation_control_shader = sb7::shader::load("../ext/engine/media/shaders/engine.tcs.glsl", GL_TESS_CONTROL_SHADER);
    // Utworzenie i kompilacja shadera wyliczenia teselacji.
    tessellation_evaluation_shader = sb7::shader::load("../ext/engine/media/shaders/engine.tes.glsl", GL_TESS_EVALUATION_SHADER);
    // Utworzenie i kompilacja shadera geometrii.
    geometry_shader = sb7::shader::load("../ext/engine/media/shaders/engine.gs.glsl", GL_GEOMETRY_SHADER);
    // Utworzenie i kompilacja shadera fragmentów.
    fragment_shader = sb7::shader::load("../ext/engine/media/shaders/engine.fs.glsl", GL_FRAGMENT_SHADER);

    // Utworzenie programu, dodanie shaderów i ich po³¹czenie.
    // tworzy obiekt programu, do którego mo¿na do³¹czyæ obiekty shaderów;
    program = glCreateProgram();

    // do³¹cza obiekt shadera do obiektu programu;
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

    // ³¹czy w jedn¹ ca³oœæ wszystkie dodane obiekty shaderów;
    glLinkProgram(program);

    // Usuniêcie shaderów, bo znajduj¹ siê ju¿ w programie.
    // usuwa obiekt shadera; po do³¹czeniu shadera do obiektu programu program
    // zawiera kod binarny i sam shader nie jest ju¿ potrzebny.
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
    // To dane, które umieœcimy w obiekcie bufora.
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
           // Parametr attribindex to indeks atrybutu wierzcho³ka.Jako wejœcie dla shadera wierzcho³ków mo¿na
           // zdefiniowaæ wiele ró¿nych atrybutów, a nastêpnie odnosiæ siê do nich na podstawie indeksu
        4, // Specifies the number of components per generic vertex attribute.Must be 1, 2, 3, 4.
           // Additionally, the symbolic constant GL_BGRA is accepted by glVertexAttribPointer.The initial value is 4.
           // Parametr size to liczba komponentów zapamiêtanych w buforze dla ka¿dego wierzcho³ka.
        GL_FLOAT, // Parametr type to typ danych, który zazwyczaj jest jednym z typów z tabeli 5.3.
        GL_FALSE, // Parametr normalized informuje OpenGL, czy dane w buforze nale¿y poddaæ normalizacji (przeskalowaæ
                  // do zakresu od 0 do 1) przed przekazaniem do shadera wierzcho³ków, czy te¿ pozostawiæ niezmienione.
                  // Parametr jest ignorowany dla danych zmiennoprzecinkowych, ale dla typów takich jak
                  // GL_UNSIGNED_BYTE lub GL_INT ma znaczenie.Jeœli na przyk³ad dane GL_UNSIGNED_BYTE zostan¹ poddane
                  // normalizacji, zostan¹ podzielone przez 255 (maksymalna dopuszczalna wartoœæ dla bajta bez
                  // znaku) przed przekazaniem do shadera wierzcho³ków jako dane zmiennoprzecinkowe.Oznacza to,
                  // ¿e shader bêdzie korzysta³ z danych w zakresie od 0 do 1. W wypadku danych bez normalizacji nast¹pi
                  // rzutowanie na wartoœci zmiennoprzecinkowe, wiêc shader otrzyma dane w zakresie od 0 do 255.
                  // Nie nast¹pi jednak ¿adne skalowanie wartoœci.
        0 /*sizeof(GLfloat) * 4*/, // Parametr stride informuje OpenGL, ile bajtów znajduje siê miêdzy pocz¹tkiem danych
                                   // jednego wierzcho³ka a pocz¹tkiem danych nastêpnego.Wartoœæ tê mo¿na ustawiæ na 0,
                                   // aby OpenGL samodzielnie wyliczy³ odstêp na podstawie wartoœci size i type.
        NULL // Specifies a offset of the first component of the first generic vertex attribute in the array in the data
             // store of the buffer currently bound to the GL_ARRAY_BUFFER target. The initial value is 0.
    );
    glEnableVertexAttribArray(0);
}

void ArrayBuffer::readNamedBuffers()
{
    GLuint buffer[3];

    // To dane, które umieœcimy w obiekcie bufora.
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

    // Utworzenie buforów.
    glCreateBuffers(3, &buffer[0]);
    // Inicjalizacja pierwszego bufora.
    glNamedBufferStorage(buffer[0], sizeof(position_data), position_data, 0);
    // Dowi¹zanie do tablicy wierzcho³ków — zerowe przesuniêcie, krok = sizeof(vec4).
    glVertexArrayVertexBuffer(vao, position_location, buffer[0], 0, sizeof(GLfloat) * 4);
    // Poinformowanie OpenGL o formacie atrybutu.
    glVertexArrayAttribFormat(vao, position_location, 4, GL_FLOAT, GL_FALSE, 0);
    // Poinformowanie OpenGL, które dowi¹zanie bufora wierzcho³ka wykorzystaæ dla atrybutu.
    glVertexArrayAttribBinding(vao, position_location, position_location);
    // W³¹czenie atrybutu.
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
    // Po skorzystaniu z danych obiektu bufora do wype³nienia atrybutu wierzcho³ka mo¿na wy³¹czyæ
    // atrybut, stosuj¹c funkcjê glDisableVertexAttribArray(), której prototyp ma postaæ :
    // Po wy³¹czeniu atrybutu wierzcho³ka wartoœæ ponownie stanie siê statyczna i do jej przekazania do
    // shadera trzeba bêdzie zastosowaæ funkcjê glVertexAttrib*().
    glDisableVertexAttribArray(2);
}

void ArrayBuffer::interleavedAttributes()
{
    struct vertex
    {
        // po³o¿enie
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

    // Konfiguracja dwóch atrybutów wierzcho³ka — najpierw po³o¿enie.
    glVertexArrayAttribBinding(vao, position_location, 0);
    glVertexArrayAttribFormat(vao, position_location, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, x));
    glEnableVertexArrayAttrib(vao, position_location);

    // Nastêpnie kolory.
    glVertexArrayAttribBinding(vao, color_location, 0);
    glVertexArrayAttribFormat(vao, color_location, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, r));
    glEnableVertexArrayAttrib(vao, color_location);

    // Na koñcu dowi¹zanie jedynego bufora do obiektu tablicy wierzcho³ków.
    glVertexArrayVertexBuffer(vao, 0, buffer, 0, sizeof(float) * 6);
}
