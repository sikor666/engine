#pragma once

#include <SDL.h>
#include <glad.h>

#include <shader.h>

#include <cmath>

#include "Screen.h"
#include "KeyboardController.h"

class ArrayBuffer : public Screen
{
public:
    ArrayBuffer(KeyboardController& keyboard_) : keyboard(keyboard_)
    {
        keyboard.keyPress(SDLK_b);
    }

    ~ArrayBuffer()
    {
    }

    virtual void startup()
    {
        // Gdy ju� uda si� okre�li� kierunek zwrotu tr�jk�ta, OpenGL mo�e pomin�� tr�jk�ty zwr�cone przodem,
        // ty�em lub nawet oba rodzaje.Domy�lnie OpenGL renderuje wszystkie tr�jk�ty niezale�nie od
        // sposobu ich zwrotu.Aby w��czy� usuwanie zb�dnych tr�jk�t�w, wywo�aj funkcj� glEnable() z
        // warto�ci� sta�ej GL_CULL_FACE.Domy�lnie OpenGL usunie tr�jk�ty skierowane ty�em.Aby zmieni� rodzaj
        // usuwanych tr�jk�t�w, wywo�aj funkcj� glCullFace() i przeka� jej warto�� GL_FRONT, GL_BACK lub
        // GL_FRONT_AND_BACK.
        // glEnable(GL_CULL_FACE);
        // glCullFace(GL_FRONT);

        compileShaders();

        // Teselacja w OpenGL dzia�a poprzez rozbicie powierzchni wysokiego poziomu nazywanych p�atami
        // (ang.patch) na punkty, linie i tr�jk�ty.Ka�dy p�at sk�ada si� z pewnej liczby punkt�w steruj�cych
        // (ang.control points).Ich liczb� konfiguruje si�, wywo�uj�c funkcj� glPatchParameteri() z 
        // parametrem pname ustawionym na GL_PATCH_VERTICES i parametrem value ustawionym na liczb�
        // punkt�w maj�cych tworzy� p�at.
        // Domy�lnie liczba punkt�w steruj�cych na p�at wynosi 3. Je�li w�a�nie taka liczba punkt�w jest
        // niezb�dna(jak to ma miejsce w przyk�adowej aplikacji), nie trzeba tej funkcji w og�le wywo�ywa�.
        // Maksymalna liczba punkt�w steruj�cych dla pojedynczego p�ata zale�y od implementacji sterownik�w,
        // ale OpenGL gwarantuje, �e nie b�dzie mniejsza ni� 32.
        // glPatchParameteri(GL_PATCH_VERTICES, 3);

        // Przed narysowaniem czegokolwiek trzeba jeszcze
        // utworzy� tak zwany VAO (ang.Vertex Array Object),
        // czyli obiekt tablicy wierzcho�k�w.To obiekt
        // reprezentuj�cy etap pobierania wierzcho�k�w w OpenGL,
        // stanowi�cy materia� wej�ciowy dla shadera wierzcho�k�w
        glGenVertexArrays(1, &vao);

        // Powi�zanie tablicy wierzcho�k�w z kontekstem
        glBindVertexArray(vao);

        if (keyboard.isPress(SDLK_v)) mode = (mode + 1) % 3;

        switch (mode)
        {
        case 0:
            readPositionBuffer();
            break;
        case 1:
            readNamedBuffers();
            break;
        case 2:
            interleavedAttributes();
            break;
        }
    }

    virtual void render(double currentTime)
    {
        // Wyczyszczenie okna kolorem
        const GLfloat disco[] = { (float)sin(currentTime) * 0.5f + 0.5f,
                                  (float)cos(currentTime) * 0.5f + 0.5f, 0.0f, 1.0f };
        const GLfloat blue[] = { 0.2f, 0.5f, 0.8f, 1.0f };
        glClearBufferfv(GL_COLOR, 0, blue);

        // U�ycie utworzonego wcze�niej obiektu programu.
        glUseProgram(program);

        GLfloat circle[] = { (float)sin(currentTime) * 0.5f,
                             (float)cos(currentTime) * 0.6f, 0.0f, 0.0f };

        GLfloat offset[] = { (float)cos(currentTime) * 0.5f,
                             (float)cos(currentTime) * 0.6f, 0.0f, 0.0f };

        GLfloat colore[] = { (float)cos(currentTime) * 0.5f + 0.5f,
                             (float)sin(currentTime) * 0.5f + 0.5f, 0.0f, 0.0f };

        // Aktualizacja warto�ci atrybutu wej�ciowego 0.
        glVertexAttrib4fv(1, offset);

        // Aktualizacja warto�ci atrybutu wej�ciowego 1.
        glVertexAttrib4fv(2, colore);

        // Funkcja ustawia �rednic� punktu w pikselach na zadany rozmiar.
        // OpenGL gwarantuje obs�ug� przynajmniej rozmiaru 64 piksele.
        glPointSize(6.9f);

        // Aby zobaczy� wynik dzia�ania mechanizmu teselacji, trzeba poinformowa� OpenGL, �eby rysowa�
        // jedynie zarysy wynikowych tr�jk�t�w.
        if (!keyboard.isPress(SDLK_b)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Funkcja glDrawArrays() wysy�a wierzcho�ki do potoku OpenGL.
        // Dla ka�dego wierzcho�ka zostanie wykonany shader wierzcho�ka. 

        // Narysowanie jednego punktu.
        // glDrawArrays(GL_POINTS, 0, 1);

        // Narysowanie tr�jk�t�w
        if (!keyboard.isPress(SDLK_b) && !keyboard.isPress(SDLK_v)) glDrawArrays(GL_PATCHES, 0, 6);
        else glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    virtual void shutdown()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(program);
        //glDeleteBuffers(1, &buffer);
    }

private:
    void compileShaders();

    void readPositionBuffer();
    void readNamedBuffers();
    void interleavedAttributes();

private:
    GLuint          program;
    GLuint          vao;
    //GLuint          buffer;
    int mode = 0;

    KeyboardController& keyboard;
};
