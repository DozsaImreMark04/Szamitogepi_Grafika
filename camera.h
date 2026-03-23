#ifndef CAMERA_H
#define CAMERA_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
typedef struct {
    float x, y, z;
    float yaw, pitch;
} Camera;
void init_camera(Camera* camera);
void handle_camera_input(Camera* camera, const Uint8* state, int mouse_x, int mouse_y, double dt);
void set_view_matrix(Camera* camera);
#endif