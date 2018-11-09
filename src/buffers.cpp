#include "buffers.h"

#include <iostream>
#include <vector>

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

void fillUniformBlock(GLuint program)
{
    static const GLchar * uniformNames[4] =
    {
        "TransformBlock.scale",
        "TransformBlock.translation",
        "TransformBlock.rotation",
        "TransformBlock.projection_matrix"
    };

    GLuint uniformIndices[4];
    GLint uniformOffsets[4];
    GLint arrayStrides[4];
    GLint matrixStrides[4];

    glGetUniformIndices(program, 4, uniformNames, uniformIndices);
    glGetActiveUniformsiv(program, 4, uniformIndices, GL_UNIFORM_OFFSET, uniformOffsets);
    glGetActiveUniformsiv(program, 4, uniformIndices, GL_UNIFORM_ARRAY_STRIDE, arrayStrides);
    glGetActiveUniformsiv(program, 4, uniformIndices, GL_UNIFORM_MATRIX_STRIDE, matrixStrides);

    std::cout << "\nIndeksy składowych bloku TransformBlock" << std::endl;
    std::cout << "TransformBlock.scale: " << uniformIndices[0] << std::endl;
    std::cout << "TransformBlock.translation: " << uniformIndices[1] << std::endl;
    std::cout << "TransformBlock.rotation: " << uniformIndices[2] << std::endl;
    std::cout << "TransformBlock.projection_matrix: " << uniformIndices[3] << std::endl;

    std::cout << "\nPrzesunięcia składowych bloku TransformBlock" << std::endl;
    std::cout << "TransformBlock.scale: " << uniformOffsets[0] << std::endl;
    std::cout << "TransformBlock.translation: " << uniformOffsets[1] << std::endl;
    std::cout << "TransformBlock.rotation: " << uniformOffsets[2] << std::endl;
    std::cout << "TransformBlock.projection_matrix: " << uniformOffsets[3] << std::endl;

    std::cout << "\nOdstępy składowych tablicy (dotyczy tablicy obrotów)" << std::endl;
    std::cout << "TransformBlock.rotation[0]: " << arrayStrides[0] << std::endl;
    std::cout << "TransformBlock.rotation[1]: " << arrayStrides[1] << std::endl;
    std::cout << "TransformBlock.rotation[2]: " << arrayStrides[2] << std::endl;
    std::cout << "TransformBlock.rotation[3]: " << arrayStrides[3] << std::endl;

    std::cout << "\nOdstępy składowych macierzy (macierz projection_matrix)" << std::endl;
    std::cout << "TransformBlock.projection_matrix[0]: " << matrixStrides[0] << std::endl;
    std::cout << "TransformBlock.projection_matrix[1]: " << matrixStrides[1] << std::endl;
    std::cout << "TransformBlock.projection_matrix[2]: " << matrixStrides[2] << std::endl;
    std::cout << "TransformBlock.projection_matrix[3]: " << matrixStrides[3] << std::endl;

    // Alokacja pamięci dla bufora (pamiętaj o zwolnieniu pamięci).
    const GLfloat matrix[] =
    {
        1.0f, 2.0f, 3.0f, 4.0f,
        9.0f, 8.0f, 7.0f, 6.0f,
        2.0f, 4.0f, 6.0f, 8.0f,
        1.0f, 3.0f, 5.0f, 7.0f
    };

    GLuint bufferSize = uniformOffsets[3] + sizeof(matrix);
    std::cout << "\nBuffer size: " << bufferSize << std::endl;
    unsigned char * buffer = (unsigned char *)malloc(bufferSize);

    // Wiemy, że TransformBlock.scale znajduje się uniformOffsets[0] bajtów
    // w głębi bloku, więc o tyle bajtów możemy przesunąć wskaźnik na bufor,
    // aby wstawić wartość skali.
    *((float *)(buffer + uniformOffsets[0])) = 3.0f;

    // Umieszczenie w następujących po sobie obszarach pamięci wartości GLfloat
    // stanowiących część wektora vec4.
    ((float *)(buffer + uniformOffsets[1]))[0] = 0.2f;
    ((float *)(buffer + uniformOffsets[1]))[1] = 0.1f;
    ((float *)(buffer + uniformOffsets[1]))[2] = 0.0f;
    ((float *)(buffer + uniformOffsets[1]))[3] = 0.0f;

    // Wartość TransformBlock.rotations[0] znajduje się uniformOffsets[2] bajtów
    // od początku bufora. Każdy element tablicy znajduje się w wielokrotności
    // arrayStrides[2] bajtów od początku tablicy.
    const GLfloat rotations[] = { 30.0f, 40.0f, 60.0f };
    unsigned int offset = uniformOffsets[2];
    for (int n = 0; n < 3; n++)
    {
        *((float *)(buffer + offset)) = rotations[n];
        offset += arrayStrides[2];
    }

    // Pierwsza kolumna TransformBlock.projection_matrix znajduje się
    // uniformOffsets[3] bajtów od początku bufora. Kolumny są ułożone
    // matrixStride[3] bajtów od siebie i w zasadzie są typu vec4.
    // Pamiętaj, że macierz w przykładzie stosuje pierwszeństwo kolumn.
    for (int i = 0; i < 4; i++)
    {
        GLuint offset = uniformOffsets[3] + matrixStrides[3] * i;
        for (int j = 0; j < 4; j++)
        {
            *((float *)(buffer + offset)) = matrix[i * 4 + j];
            offset += sizeof(GLfloat);
        }
    }

    //GLint transformLocation = glGetUniformBlockIndex(program, "TransformBlock");
    //std::cout << "\nTransformBlock location: " << transformLocation << std::endl;
    //GLint maxUniformBufferBindings;
    //glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUniformBufferBindings);
    //std::cout << "Max uniform buffer bindings: " << maxUniformBufferBindings << std::endl;
    //GLuint uniformBlockBinding = maxUniformBufferBindings - 1;
    //glUniformBlockBinding(program, transformLocation, uniformBlockBinding);
    //glBindBufferBase(GL_UNIFORM_BUFFER, transformLocation, uniformBlockBinding);

    GLuint uniform_buffer;
    glGenBuffers(1, &uniform_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer);
    glBufferData(GL_UNIFORM_BUFFER, bufferSize, NULL, GL_DYNAMIC_DRAW); //GL_STATIC_DRAW

    glBindBufferBase(GL_UNIFORM_BUFFER, 2, uniform_buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, bufferSize, buffer);

    free(buffer);
}

void printUniformBlocks(GLuint program)
{
    GLint numBlocks;
    GLint nameLen;

    std::vector<std::string> nameList;

    glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);
    nameList.reserve(numBlocks);

    std::cout << "\nFound " << numBlocks << " block in shader" << std::endl;

    for (int blockIx = 0; blockIx < numBlocks; blockIx++) {
        glGetActiveUniformBlockiv(program, blockIx, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);

        std::vector<GLchar> name;
        name.resize(nameLen);
        glGetActiveUniformBlockName(program, blockIx, nameLen, NULL, &name[0]);

        nameList.push_back(std::string());
        nameList.back().assign(name.begin(), name.end() - 1); //Remove the null terminator.
    }

    for (unsigned int il = 0; il < nameList.size(); il++) {
        std::cout << "Block name: " << nameList[il].c_str() << std::endl;
    }
}
