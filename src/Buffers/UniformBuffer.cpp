#include "UniformBuffer.h"

#include <iostream>
#include <vector>

void UniformBuffer::compileShaders()
{
    GLuint compute_shader;
    GLuint vertex_shader_point;
    GLuint vertex_shader_triangle;
    GLuint tessellation_control_shader;
    GLuint tessellation_evaluation_shader;
    GLuint geometry_shader;
    GLuint fragment_shader;

    // Utworzenie i kompilacja shadera obliczeniowego.
    compute_shader = sb7::shader::load("../ext/engine/media/shaders/engine.1.vs.glsl", GL_COMPUTE_SHADER);
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

void UniformBuffer::interleavedAttributes()
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

void UniformBuffer::fillUniformBlock()
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

    std::cout << "\nIndeksy sk�adowych bloku TransformBlock" << std::endl;
    std::cout << "TransformBlock.scale: " << uniformIndices[0] << std::endl;
    std::cout << "TransformBlock.translation: " << uniformIndices[1] << std::endl;
    std::cout << "TransformBlock.rotation: " << uniformIndices[2] << std::endl;
    std::cout << "TransformBlock.projection_matrix: " << uniformIndices[3] << std::endl;

    std::cout << "\nPrzesuni�cia sk�adowych bloku TransformBlock" << std::endl;
    std::cout << "TransformBlock.scale: " << uniformOffsets[0] << std::endl;
    std::cout << "TransformBlock.translation: " << uniformOffsets[1] << std::endl;
    std::cout << "TransformBlock.rotation: " << uniformOffsets[2] << std::endl;
    std::cout << "TransformBlock.projection_matrix: " << uniformOffsets[3] << std::endl;

    std::cout << "\nOdst�py sk�adowych tablicy (dotyczy tablicy obrot�w)" << std::endl;
    std::cout << "TransformBlock.rotation[0]: " << arrayStrides[0] << std::endl;
    std::cout << "TransformBlock.rotation[1]: " << arrayStrides[1] << std::endl;
    std::cout << "TransformBlock.rotation[2]: " << arrayStrides[2] << std::endl;
    std::cout << "TransformBlock.rotation[3]: " << arrayStrides[3] << std::endl;

    std::cout << "\nOdst�py sk�adowych macierzy (macierz projection_matrix)" << std::endl;
    std::cout << "TransformBlock.projection_matrix[0]: " << matrixStrides[0] << std::endl;
    std::cout << "TransformBlock.projection_matrix[1]: " << matrixStrides[1] << std::endl;
    std::cout << "TransformBlock.projection_matrix[2]: " << matrixStrides[2] << std::endl;
    std::cout << "TransformBlock.projection_matrix[3]: " << matrixStrides[3] << std::endl;

    // Alokacja pami�ci dla bufora (pami�taj o zwolnieniu pami�ci).
    const GLfloat matrix[] =
    {
        1.0f, 2.0f, 3.0f, 4.0f,
        9.0f, 8.0f, 7.0f, 6.0f,
        2.0f, 4.0f, 6.0f, 8.0f,
        1.0f, 3.0f, 5.0f, 7.0f
    };

    GLint maxUniformBlockSize;
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformBlockSize);
    std::cout << "\nMax uniform block size: " << maxUniformBlockSize << std::endl;

    GLuint bufferSize = uniformOffsets[3] + sizeof(matrix);
    std::cout << "Uniform block size: " << bufferSize << std::endl;
    unsigned char * buffer = (unsigned char *)malloc(bufferSize);

    // Wiemy, �e TransformBlock.scale znajduje si� uniformOffsets[0] bajt�w
    // w g��bi bloku, wi�c o tyle bajt�w mo�emy przesun�� wska�nik na bufor,
    // aby wstawi� warto�� skali.
    *((float *)(buffer + uniformOffsets[0])) = 3.0f;

    // Umieszczenie w nast�puj�cych po sobie obszarach pami�ci warto�ci GLfloat
    // stanowi�cych cz�� wektora vec4.
    ((float *)(buffer + uniformOffsets[1]))[0] = 0.2f;
    ((float *)(buffer + uniformOffsets[1]))[1] = 0.1f;
    ((float *)(buffer + uniformOffsets[1]))[2] = 0.0f;
    ((float *)(buffer + uniformOffsets[1]))[3] = 0.0f;

    // Warto�� TransformBlock.rotations[0] znajduje si� uniformOffsets[2] bajt�w
    // od pocz�tku bufora. Ka�dy element tablicy znajduje si� w wielokrotno�ci
    // arrayStrides[2] bajt�w od pocz�tku tablicy.
    const GLfloat rotations[] = { 30.0f, 40.0f, 60.0f };
    unsigned int offset = uniformOffsets[2];
    for (int n = 0; n < 3; n++)
    {
        *((float *)(buffer + offset)) = rotations[n];
        offset += arrayStrides[2];
    }

    // Pierwsza kolumna TransformBlock.projection_matrix znajduje si�
    // uniformOffsets[3] bajt�w od pocz�tku bufora. Kolumny s� u�o�one
    // matrixStride[3] bajt�w od siebie i w zasadzie s� typu vec4.
    // Pami�taj, �e macierz w przyk�adzie stosuje pierwsze�stwo kolumn.
    for (int i = 0; i < 4; i++)
    {
        GLuint offset = uniformOffsets[3] + matrixStrides[3] * i;
        for (int j = 0; j < 4; j++)
        {
            *((float *)(buffer + offset)) = matrix[i * 4 + j];
            offset += sizeof(GLfloat);
        }
    }

    GLint transformLocation = glGetUniformBlockIndex(program, "TransformBlock");
    std::cout << "\nTransformBlock location: " << transformLocation << std::endl;

    GLint maxUniformBufferBindings;
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUniformBufferBindings);
    std::cout << "Max uniform buffer bindings: " << maxUniformBufferBindings << std::endl;

    GLuint uniformBlockBinding = maxUniformBufferBindings - 1;
    glUniformBlockBinding(program, transformLocation, uniformBlockBinding);

    GLuint uniformBuffer;
    glGenBuffers(1, &uniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, bufferSize, NULL, GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlockBinding, uniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, bufferSize, buffer);

    free(buffer);
}

void UniformBuffer::printUniformBlocks()
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
