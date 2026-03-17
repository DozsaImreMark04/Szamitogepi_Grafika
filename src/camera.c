#define _USE_MATH_DEFINES
#include <math.h>
#include <SDL2/SDL_opengl.h>
#include "camera.h"

void init_camera(Camera* camera) {
    camera->x = 0.0f;
    camera->y = 1.7f;
    camera->z = 5.0f;
    camera->yaw = 0.0f;
    camera->pitch = 0.0f;
}

void handle_camera_input(Camera* camera, const Uint8* state, int mouse_x, int mouse_y, double dt) {
    float speed = 10.0f * dt;
    float sensitivity = 0.2f;

    camera->yaw -= mouse_x * sensitivity;
    camera->pitch -= mouse_y * sensitivity;

    if (camera->pitch > 89.0f) camera->pitch = 89.0f;
    if (camera->pitch < -89.0f) camera->pitch = -89.0f;

    float yaw = -camera->yaw * (M_PI / 180.0f);

    float forward_x = sinf(yaw);
    float forward_z = -cosf(yaw);

    float right_x = cosf(yaw);
    float right_z = sinf(yaw);

    if (state[SDL_SCANCODE_W]) {
        camera->x += forward_x * speed;
        camera->z += forward_z * speed;
    }

    if (state[SDL_SCANCODE_S]) {
        camera->x -= forward_x * speed;
        camera->z -= forward_z * speed;
    }

    if (state[SDL_SCANCODE_A]) {
        camera->x -= right_x * speed;
        camera->z -= right_z * speed;
    }

    if (state[SDL_SCANCODE_D]) {
        camera->x += right_x * speed;
        camera->z += right_z * speed;
    }
}

void set_view_matrix(Camera* camera) {
    glRotatef(-camera->pitch, 1.0f, 0.0f, 0.0f);
    glRotatef(-camera->yaw, 0.0f, 1.0f, 0.0f);
    glTranslatef(-camera->x, -camera->y, -camera->z);
}