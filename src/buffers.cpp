#include "buffers.h"

void read_position_buffer()
{
    GLuint position_buffer;

    // To dane, które umieścimy w obiekcie bufora.
    static const float vertex_data[] =
    {
         0.25, -0.25, 0.5, 1.0,
         -0.25, -0.25, 0.5, 1.0,
         0.25, 0.25, 0.5, 1.0,

         -0.3, 0.25, 0.5, 1.0,
         -0.3, -0.25, 0.5, 1.0,
         0.2, 0.25, 0.5, 1.0
    };

    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(vertex_data),
        vertex_data,
        GL_STATIC_DRAW);
    glVertexAttribPointer(
        0, // Specifies the index of the generic vertex attribute to be modified.
           // Parametr attribindex to indeks atrybutu wierzchołka.Jako wejście dla shadera wierzchołków można
           // zdefiniować wiele różnych atrybutów, a następnie odnosić się do nich na podstawie indeksu
        4, // Specifies the number of components per generic vertex attribute.Must be 1, 2, 3, 4.
           // Additionally, the symbolic constant GL_BGRA is accepted by glVertexAttribPointer.The initial value is 4.
           // Parametr size to liczba komponentów zapamiętanych w buforze dla każdego wierzchołka.
        GL_FLOAT, // Parametr type to typ danych, który zazwyczaj jest jednym z typów z tabeli 5.3.
        GL_FALSE, // Parametr normalized informuje OpenGL, czy dane w buforze należy poddać normalizacji (przeskalować
                  // do zakresu od 0 do 1) przed przekazaniem do shadera wierzchołków, czy też pozostawić niezmienione.
                  // Parametr jest ignorowany dla danych zmiennoprzecinkowych, ale dla typów takich jak
                  // GL_UNSIGNED_BYTE lub GL_INT ma znaczenie.Jeśli na przykład dane GL_UNSIGNED_BYTE zostaną poddane
                  // normalizacji, zostaną podzielone przez 255 (maksymalna dopuszczalna wartość dla bajta bez
                  // znaku) przed przekazaniem do shadera wierzchołków jako dane zmiennoprzecinkowe.Oznacza to,
                  // że shader będzie korzystał z danych w zakresie od 0 do 1. W wypadku danych bez normalizacji nastąpi
                  // rzutowanie na wartości zmiennoprzecinkowe, więc shader otrzyma dane w zakresie od 0 do 255.
                  // Nie nastąpi jednak żadne skalowanie wartości.
        0 /*sizeof(GLfloat) * 4*/, // Parametr stride informuje OpenGL, ile bajtów znajduje się między początkiem danych
                                   // jednego wierzchołka a początkiem danych następnego.Wartość tę można ustawić na 0,
                                   // aby OpenGL samodzielnie wyliczył odstęp na podstawie wartości size i type.
        NULL // Specifies a offset of the first component of the first generic vertex attribute in the array in the data
             // store of the buffer currently bound to the GL_ARRAY_BUFFER target. The initial value is 0.
    );
    glEnableVertexAttribArray(0);
}

void read_named_buffers(GLuint program, GLuint vao)
{
    GLuint buffer[3];

    // To dane, które umieścimy w obiekcie bufora.
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
    // Dowiązanie do tablicy wierzchołków — zerowe przesunięcie, krok = sizeof(vec4).
    glVertexArrayVertexBuffer(vao, position_location, buffer[0], 0, sizeof(GLfloat) * 4);
    // Poinformowanie OpenGL o formacie atrybutu.
    glVertexArrayAttribFormat(vao, position_location, 4, GL_FLOAT, GL_FALSE, 0);
    // Poinformowanie OpenGL, które dowiązanie bufora wierzchołka wykorzystać dla atrybutu.
    glVertexArrayAttribBinding(vao, position_location, position_location);
    // Włączenie atrybutu.
    glEnableVertexAttribArray(0); // glEnableVertexArrayAttrib(vertex_array_object, 0);

    // Przeprowadzenie podobnej inicjalizacji dla drugiego bufora.
    glNamedBufferStorage(buffer[1], sizeof(offset_data), offset_data, 0);
    glVertexArrayVertexBuffer(vao, offset_location, buffer[1], 0, sizeof(GLfloat) * 4);
    glVertexArrayAttribFormat(vao, offset_location, 4, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, offset_location, offset_location);
    glEnableVertexAttribArray(1); // glDisableVertexAttribArray(1);

    // Przeprowadzenie podobnej inicjalizacji dla trzeciego bufora.
    glNamedBufferStorage(buffer[2], sizeof(color_data), color_data, 0);
    glVertexArrayVertexBuffer(vao, color_location, buffer[2], 0, sizeof(GLfloat) * 4);
    glVertexArrayAttribFormat(vao, color_location, 4, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, color_location, color_location);
    glEnableVertexAttribArray(2);
    // Po skorzystaniu z danych obiektu bufora do wypełnienia atrybutu wierzchołka można wyłączyć
    // atrybut, stosując funkcję glDisableVertexAttribArray(), której prototyp ma postać :
    // Po wyłączeniu atrybutu wierzchołka wartość ponownie stanie się statyczna i do jej przekazania do
    // shadera trzeba będzie zastosować funkcję glVertexAttrib*().
    // glDisableVertexAttribArray(2);
}

void interleaved_attributes(GLuint program, GLuint vao)
{
    struct vertex
    {
        // położenie
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

    // Konfiguracja dwóch atrybutów wierzchołka — najpierw położenie.
    glVertexArrayAttribBinding(vao, position_location, 0);
    glVertexArrayAttribFormat(vao, position_location, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, x));
    glEnableVertexArrayAttrib(vao, position_location);

    // Następnie kolory.
    glVertexArrayAttribBinding(vao, color_location, 0);
    glVertexArrayAttribFormat(vao, color_location, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, r));
    glEnableVertexArrayAttrib(vao, color_location);

    // Na końcu dowiązanie jedynego bufora do obiektu tablicy wierzchołków.
    glVertexArrayVertexBuffer(vao, 0, buffer, 0, sizeof(float) * 6);
}
