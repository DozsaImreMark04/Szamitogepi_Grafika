#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <stdbool.h>
#include <math.h>
#include "camera.h"
#include "model.h"
#include "texture.h"
#include "scene.h"
#include "skybox.h"

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Park Projekt", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_FOG);
    float fog_col[] = {0.7f, 0.8f, 0.9f, 1.0f};
    glFogfv(GL_FOG_COLOR, fog_col);
    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogf(GL_FOG_DENSITY, 0.005f);

    glClearColor(0.7f, 0.8f, 0.9f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 1280.0/720.0, 0.01, 2000.0);

    Camera camera;
    init_camera(&camera);
    Model tree_model;
    load_model(&tree_model, "assets/models/simple_low-poly_tree.obj");
    
    Scene scene;
    load_scene(&scene, "assets/data.csv");
    Skybox skybox;
    bool running = true;
    uint32_t last_ticks = SDL_GetTicks();
    float brightness = 1.0f;
    float light_angle = 0.0f;

    while (running) {
        uint32_t current_ticks = SDL_GetTicks();
        double dt = (current_ticks - last_ticks) / 1000.0;
        last_ticks = current_ticks;
        SDL_Event event;
        int mouse_x = 0, mouse_y = 0;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) running = false;
                if (event.key.keysym.sym == SDLK_PLUS || event.key.keysym.sym == SDLK_KP_PLUS) brightness += 0.1f;
                if (event.key.keysym.sym == SDLK_MINUS || event.key.keysym.sym == SDLK_KP_MINUS) brightness -= 0.1f;
            }
            if (event.type == SDL_MOUSEMOTION) { mouse_x = event.motion.xrel; mouse_y = event.motion.yrel; }
        }

        light_angle += 0.2f * dt;
        float light_pos[] = { cosf(light_angle)*100.0f, 80.0f, sinf(light_angle)*100.0f, 1.0f };
        float light_diff[] = { brightness, brightness, brightness, 1.0f };
        float light_amb[] = { 0.5f, 0.5f, 0.5f, 1.0f };
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        set_view_matrix(&camera);
        
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diff);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);

        handle_camera_input(&camera, SDL_GetKeyboardState(NULL), mouse_x, mouse_y, dt);
        
        draw_skybox(&skybox);
        draw_scene(&scene, &tree_model);
        
        SDL_GL_SwapWindow(window);
    }
    free_scene(&scene); free_model(&tree_model);
    SDL_GL_DeleteContext(context); SDL_DestroyWindow(window); SDL_Quit();
    return 0;
}