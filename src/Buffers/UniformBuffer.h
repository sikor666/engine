#pragma once

#include <SDL.h>
#include <glad.h>

#include <shader.h>

#include <cmath>

#include "../KeyboardController.h"

class UniformBuffer
{
public:
    UniformBuffer(KeyboardController& keyboard_) : keyboard(keyboard_)
    {
        keyboard.keyPress(SDLK_v);
    }

    virtual void startup()
    {
        // Gdy ju¿ uda siê okreœliæ kierunek zwrotu trójk¹ta, OpenGL mo¿e pomin¹æ trójk¹ty zwrócone przodem,
        // ty³em lub nawet oba rodzaje.Domyœlnie OpenGL renderuje wszystkie trójk¹ty niezale¿nie od
        // sposobu ich zwrotu.Aby w³¹czyæ usuwanie zbêdnych trójk¹tów, wywo³aj funkcjê glEnable() z
        // wartoœci¹ sta³ej GL_CULL_FACE.Domyœlnie OpenGL usunie trójk¹ty skierowane ty³em.Aby zmieniæ rodzaj
        // usuwanych trójk¹tów, wywo³aj funkcjê glCullFace() i przeka¿ jej wartoœæ GL_FRONT, GL_BACK lub
        // GL_FRONT_AND_BACK.
        // glEnable(GL_CULL_FACE);
        // glCullFace(GL_FRONT);

        compileShaders();

        // Teselacja w OpenGL dzia³a poprzez rozbicie powierzchni wysokiego poziomu nazywanych p³atami
        // (ang.patch) na punkty, linie i trójk¹ty.Ka¿dy p³at sk³ada siê z pewnej liczby punktów steruj¹cych
        // (ang.control points).Ich liczbê konfiguruje siê, wywo³uj¹c funkcjê glPatchParameteri() z 
        // parametrem pname ustawionym na GL_PATCH_VERTICES i parametrem value ustawionym na liczbê
        // punktów maj¹cych tworzyæ p³at.
        // Domyœlnie liczba punktów steruj¹cych na p³at wynosi 3. Jeœli w³aœnie taka liczba punktów jest
        // niezbêdna(jak to ma miejsce w przyk³adowej aplikacji), nie trzeba tej funkcji w ogóle wywo³ywaæ.
        // Maksymalna liczba punktów steruj¹cych dla pojedynczego p³ata zale¿y od implementacji sterowników,
        // ale OpenGL gwarantuje, ¿e nie bêdzie mniejsza ni¿ 32.
        // glPatchParameteri(GL_PATCH_VERTICES, 3);

        // Przed narysowaniem czegokolwiek trzeba jeszcze
        // utworzyæ tak zwany VAO (ang.Vertex Array Object),
        // czyli obiekt tablicy wierzcho³ków.To obiekt
        // reprezentuj¹cy etap pobierania wierzcho³ków w OpenGL,
        // stanowi¹cy materia³ wejœciowy dla shadera wierzcho³ków
        glGenVertexArrays(1, &vao);

        // Powi¹zanie tablicy wierzcho³ków z kontekstem
        glBindVertexArray(vao);

        interleavedAttributes();
        fillUniformBlock();
        printUniformBlocks();
    }

    virtual void render(double currentTime)
    {
        // Wyczyszczenie okna kolorem
        const GLfloat disco[] = { (float)sin(currentTime) * 0.5f + 0.5f,
                                  (float)cos(currentTime) * 0.5f + 0.5f, 0.0f, 1.0f };
        const GLfloat blue[] = { 0.2f, 0.5f, 0.8f, 1.0f };
        glClearBufferfv(GL_COLOR, 0, blue);

        // U¿ycie utworzonego wczeœniej obiektu programu.
        glUseProgram(program);

        GLfloat circle[] = { (float)sin(currentTime) * 0.5f,
                             (float)cos(currentTime) * 0.6f, 0.0f, 0.0f };

        GLfloat offset[] = { (float)cos(currentTime) * 0.5f,
                             (float)cos(currentTime) * 0.6f, 0.0f, 0.0f };

        GLfloat colore[] = { (float)cos(currentTime) * 0.5f + 0.5f,
                             (float)sin(currentTime) * 0.5f + 0.5f, 0.0f, 0.0f };

        //Jeœli funkcja glGetUniformLocation() zwróci wartoœæ - 1, oznacza to, ¿e w danym programie nie
        //ma zmiennej o podanej nazwie.Warto pamiêtaæ, ¿e nawet gdy shader skompiluje siê prawid³owo,
        //zmienna uniform mo¿e „znikn¹æ” z programu, jeœli nie zostanie wykorzystana bezpoœrednio w przynajmniej
        //jednym z shaderów do³¹czonych do programu(i to nawet wtedy, gdy jawnie wskazano
        //w deklaracji jej po³o¿enie).Najczêœciej nie trzeba siê przejmowaæ optymalizacj¹ zmiennych uniform,
        //które s¹ zadeklarowane, ale nieu¿ywane, bo kompilator sam zajmie siê spraw¹.Nazwy zmiennych
        //w shaderach s¹ czu³e na wielkoœæ liter, wiêc w zapytaniach o po³o¿enie trzeba uwa¿aæ na to, ¿eby u¿yæ
        //w³aœciwej lokalizacji.
        GLint discoLocation = glGetUniformLocation(program, "disco");
        glUniform4fv(discoLocation, 1, circle);

        // Aktualizacja wartoœci atrybutu wejœciowego 0.
        glVertexAttrib4fv(1, offset);

        // Aktualizacja wartoœci atrybutu wejœciowego 1.
        glVertexAttrib4fv(2, colore);

        // Funkcja ustawia œrednicê punktu w pikselach na zadany rozmiar.
        // OpenGL gwarantuje obs³ugê przynajmniej rozmiaru 64 piksele.
        glPointSize(6.9f);

        // Aby zobaczyæ wynik dzia³ania mechanizmu teselacji, trzeba poinformowaæ OpenGL, ¿eby rysowa³
        // jedynie zarysy wynikowych trójk¹tów.
        if (!keyboard.isPress(SDLK_b)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Funkcja glDrawArrays() wysy³a wierzcho³ki do potoku OpenGL.
        // Dla ka¿dego wierzcho³ka zostanie wykonany shader wierzcho³ka. 

        // Narysowanie jednego punktu.
        // glDrawArrays(GL_POINTS, 0, 1);

        // Narysowanie trójk¹tów
        if (keyboard.isPress(SDLK_b) || keyboard.isPress(SDLK_v)) glDrawArrays(GL_TRIANGLES, 0, 6);
        else glDrawArrays(GL_PATCHES, 0, 6); 
    }

    virtual void shutdown()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(program);
        //glDeleteBuffers(1, &buffer);
    }

private:
    void compileShaders();

    void interleavedAttributes();

    void fillUniformBlock();
    void printUniformBlocks();

private:
    GLuint          program;
    GLuint          vao;
    //GLuint          buffer;

    KeyboardController& keyboard;
};
