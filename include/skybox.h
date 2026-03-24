#ifndef SKYBOX_H
#define SKYBOX_H

#include <SDL2/SDL_opengl.h>

typedef struct {
    GLuint texture_id;
} Skybox;

void init_skybox(Skybox* skybox, const char* filename);
void draw_skybox(Skybox* skybox);

#endif