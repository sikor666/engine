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

void UniformBuffer::interleavedAttributes()
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

    std::cout << "\nIndeksy sk³adowych bloku TransformBlock" << std::endl;
    std::cout << "TransformBlock.scale: " << uniformIndices[0] << std::endl;
    std::cout << "TransformBlock.translation: " << uniformIndices[1] << std::endl;
    std::cout << "TransformBlock.rotation: " << uniformIndices[2] << std::endl;
    std::cout << "TransformBlock.projection_matrix: " << uniformIndices[3] << std::endl;

    std::cout << "\nPrzesuniêcia sk³adowych bloku TransformBlock" << std::endl;
    std::cout << "TransformBlock.scale: " << uniformOffsets[0] << std::endl;
    std::cout << "TransformBlock.translation: " << uniformOffsets[1] << std::endl;
    std::cout << "TransformBlock.rotation: " << uniformOffsets[2] << std::endl;
    std::cout << "TransformBlock.projection_matrix: " << uniformOffsets[3] << std::endl;

    std::cout << "\nOdstêpy sk³adowych tablicy (dotyczy tablicy obrotów)" << std::endl;
    std::cout << "TransformBlock.rotation[0]: " << arrayStrides[0] << std::endl;
    std::cout << "TransformBlock.rotation[1]: " << arrayStrides[1] << std::endl;
    std::cout << "TransformBlock.rotation[2]: " << arrayStrides[2] << std::endl;
    std::cout << "TransformBlock.rotation[3]: " << arrayStrides[3] << std::endl;

    std::cout << "\nOdstêpy sk³adowych macierzy (macierz projection_matrix)" << std::endl;
    std::cout << "TransformBlock.projection_matrix[0]: " << matrixStrides[0] << std::endl;
    std::cout << "TransformBlock.projection_matrix[1]: " << matrixStrides[1] << std::endl;
    std::cout << "TransformBlock.projection_matrix[2]: " << matrixStrides[2] << std::endl;
    std::cout << "TransformBlock.projection_matrix[3]: " << matrixStrides[3] << std::endl;

    // Alokacja pamiêci dla bufora (pamiêtaj o zwolnieniu pamiêci).
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

    // Wiemy, ¿e TransformBlock.scale znajduje siê uniformOffsets[0] bajtów
    // w g³êbi bloku, wiêc o tyle bajtów mo¿emy przesun¹æ wskaŸnik na bufor,
    // aby wstawiæ wartoœæ skali.
    *((float *)(buffer + uniformOffsets[0])) = 3.0f;

    // Umieszczenie w nastêpuj¹cych po sobie obszarach pamiêci wartoœci GLfloat
    // stanowi¹cych czêœæ wektora vec4.
    ((float *)(buffer + uniformOffsets[1]))[0] = 0.2f;
    ((float *)(buffer + uniformOffsets[1]))[1] = 0.1f;
    ((float *)(buffer + uniformOffsets[1]))[2] = 0.0f;
    ((float *)(buffer + uniformOffsets[1]))[3] = 0.0f;

    // Wartoœæ TransformBlock.rotations[0] znajduje siê uniformOffsets[2] bajtów
    // od pocz¹tku bufora. Ka¿dy element tablicy znajduje siê w wielokrotnoœci
    // arrayStrides[2] bajtów od pocz¹tku tablicy.
    const GLfloat rotations[] = { 30.0f, 40.0f, 60.0f };
    unsigned int offset = uniformOffsets[2];
    for (int n = 0; n < 3; n++)
    {
        *((float *)(buffer + offset)) = rotations[n];
        offset += arrayStrides[2];
    }

    // Pierwsza kolumna TransformBlock.projection_matrix znajduje siê
    // uniformOffsets[3] bajtów od pocz¹tku bufora. Kolumny s¹ u³o¿one
    // matrixStride[3] bajtów od siebie i w zasadzie s¹ typu vec4.
    // Pamiêtaj, ¿e macierz w przyk³adzie stosuje pierwszeñstwo kolumn.
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
