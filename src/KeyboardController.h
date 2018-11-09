#pragma once

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
    SDL_Keycode key = SDLK_v;
};
