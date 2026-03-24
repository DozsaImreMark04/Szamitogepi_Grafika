#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wattributes"
#endif
#include "camera.h"
#include "model.h"
#include "texture.h"
#include "scene.h"
#include "skybox.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define RAIN_COUNT 420

typedef struct {
    float x;
    float y;
    float z;
    float speed;
} RainDrop;

static void draw_box(float sx, float sy, float sz, float r, float g, float b) {
    glColor3f(r, g, b);
    glPushMatrix();
    glScalef(sx, sy, sz);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, 0.5f, -0.5f); glVertex3f(0.5f, 0.5f, -0.5f); glVertex3f(0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(-0.5f, -0.5f, 0.5f); glVertex3f(0.5f, -0.5f, 0.5f); glVertex3f(0.5f, -0.5f, -0.5f);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f(0.5f, 0.5f, 0.5f); glVertex3f(0.5f, -0.5f, 0.5f);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(0.5f, -0.5f, -0.5f); glVertex3f(0.5f, 0.5f, -0.5f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(-0.5f, 0.5f, -0.5f); glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, -0.5f, 0.5f);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, -0.5f); glVertex3f(0.5f, -0.5f, 0.5f); glVertex3f(0.5f, 0.5f, 0.5f); glVertex3f(0.5f, 0.5f, -0.5f);
    glEnd();
    glPopMatrix();
}

static void draw_bench(float x, float z, float yaw_deg) {
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(yaw_deg, 0.0f, 1.0f, 0.0f);

    draw_box(3.2f, 0.10f, 0.55f, 0.44f, 0.24f, 0.10f);
    glPushMatrix(); glTranslatef(0.0f, 0.45f, -0.22f); draw_box(3.2f, 0.10f, 0.45f, 0.40f, 0.22f, 0.09f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.0f, 0.75f, -0.38f); draw_box(3.2f, 0.10f, 0.35f, 0.40f, 0.22f, 0.09f); glPopMatrix();

    glPushMatrix(); glTranslatef(-1.35f, -0.28f, -0.18f); draw_box(0.12f, 0.55f, 0.12f, 0.20f, 0.20f, 0.20f); glPopMatrix();
    glPushMatrix(); glTranslatef( 1.35f, -0.28f, -0.18f); draw_box(0.12f, 0.55f, 0.12f, 0.20f, 0.20f, 0.20f); glPopMatrix();
    glPushMatrix(); glTranslatef(-1.35f, -0.28f,  0.18f); draw_box(0.12f, 0.55f, 0.12f, 0.20f, 0.20f, 0.20f); glPopMatrix();
    glPushMatrix(); glTranslatef( 1.35f, -0.28f,  0.18f); draw_box(0.12f, 0.55f, 0.12f, 0.20f, 0.20f, 0.20f); glPopMatrix();

    glPopMatrix();
    glEnable(GL_TEXTURE_2D);
}

static void draw_path(void) {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
    glColor3f(0.55f, 0.50f, 0.44f);
    glVertex3f(-3.0f, -0.030f, 28.0f);
    glVertex3f(3.0f, -0.030f, 28.0f);
    glVertex3f(3.0f, -0.030f, -58.0f);
    glVertex3f(-3.0f, -0.030f, -58.0f);

    glColor3f(0.52f, 0.47f, 0.40f);
    glVertex3f(0.0f, -0.029f, -8.0f);
    glVertex3f(38.0f, -0.029f, -2.5f);
    glVertex3f(38.0f, -0.029f, 2.5f);
    glVertex3f(0.0f, -0.029f, -3.0f);
    glEnd();

    glEnable(GL_LIGHTING);
}

static int point_on_path(float x, float z) {
    int on_main = (x > -3.2f && x < 3.2f && z > -58.5f && z < 28.5f);

    /* Branch corridor approximated by a line strip with width. */
    float z_line = -8.0f + (x / 38.0f) * 5.5f;
    int on_branch = (x > -0.5f && x < 38.5f && z > (z_line - 3.0f) && z < (z_line + 3.0f));

    return on_main || on_branch;
}

static void move_trees_off_path(Scene* scene) {
    for (int i = 0; i < scene->tree_count; i++) {
        if (!point_on_path(scene->trees[i].x, scene->trees[i].z)) {
            continue;
        }

        float dir = (scene->trees[i].x >= 0.0f) ? 1.0f : -1.0f;
        if (fabsf(scene->trees[i].x) < 0.5f) {
            dir = 1.0f;
        }

        int guard = 0;
        while (point_on_path(scene->trees[i].x, scene->trees[i].z) && guard < 64) {
            scene->trees[i].x += dir * 0.45f;
            guard++;
        }
    }
}

static void add_extra_trees(Scene* scene) {
    const SceneObject extra[] = {
        { -8.0f, 0.0f, -6.0f, 0.95f },
        {  9.0f, 0.0f, -4.0f, 1.10f },
        { 16.0f, 0.0f, -12.0f, 0.85f },
        { -15.0f, 0.0f, -24.0f, 1.00f },
        {  4.0f, 0.0f, -44.0f, 1.15f },
        { -6.0f, 0.0f, 10.0f, 0.90f },
        { 12.0f, 0.0f, 14.0f, 0.80f },
        { -18.0f, 0.0f, 8.0f, 1.05f }
    };
    int extra_count = (int)(sizeof(extra) / sizeof(extra[0]));
    int new_count = scene->tree_count + extra_count;
    SceneObject* resized = (SceneObject*)realloc(scene->trees, sizeof(SceneObject) * new_count);
    if (!resized) {
        return;
    }

    scene->trees = resized;
    for (int i = 0; i < extra_count; i++) {
        scene->trees[scene->tree_count + i] = extra[i];
    }
    scene->tree_count = new_count;
}

static float rand_range(float min_v, float max_v) {
    float t = (float)rand() / (float)RAND_MAX;
    return min_v + t * (max_v - min_v);
}

static void reset_drop(RainDrop* d, const Camera* camera) {
    d->x = camera->x + rand_range(-55.0f, 55.0f);
    d->z = camera->z + rand_range(-55.0f, 55.0f);
    d->y = rand_range(12.0f, 60.0f);
    d->speed = rand_range(12.0f, 28.0f);
}

static void init_rain(RainDrop* drops, const Camera* camera) {
    for (int i = 0; i < RAIN_COUNT; i++) {
        reset_drop(&drops[i], camera);
    }
}

static void update_rain(RainDrop* drops, const Camera* camera, float dt) {
    for (int i = 0; i < RAIN_COUNT; i++) {
        drops[i].y -= drops[i].speed * dt;
        if (drops[i].y < 0.1f) {
            reset_drop(&drops[i], camera);
        }
    }
}

static void draw_rain(const RainDrop* drops) {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_LINES);
    for (int i = 0; i < RAIN_COUNT; i++) {
        glColor4f(0.75f, 0.82f, 0.92f, 0.50f);
        glVertex3f(drops[i].x, drops[i].y, drops[i].z);
        glColor4f(0.75f, 0.82f, 0.92f, 0.10f);
        glVertex3f(drops[i].x, drops[i].y + 0.9f, drops[i].z);
    }
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

static void draw_transparent_water(float time_s) {
    const float cx = 36.0f;
    const float cz = 18.0f;
    const float r = 11.0f;
    const int segments = 72;
    const float base_y = -0.035f;

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.10f, 0.30f, 0.52f, 0.42f);
    glVertex3f(cx, base_y, cz);
    glColor4f(0.20f, 0.45f, 0.68f, 0.28f);
    for (int i = 0; i <= segments; i++) {
        float a = (float)i / (float)segments * 2.0f * (float)M_PI;
        float wave = 0.45f * sinf(6.0f * a + time_s * 1.15f) + 0.2f * sinf(11.0f * a - time_s * 0.85f);
        float rr = r + wave;
        float y = base_y + 0.007f * sinf(4.0f * a + time_s * 1.35f);
        glVertex3f(cx + cosf(a) * rr, y, cz + sinf(a) * rr);
    }
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor4f(0.75f, 0.90f, 0.98f, 0.30f);
    for (int i = 0; i < segments; i++) {
        float a = (float)i / (float)segments * 2.0f * (float)M_PI;
        float wave = 0.45f * sinf(6.0f * a + time_s * 1.15f) + 0.2f * sinf(11.0f * a - time_s * 0.85f);
        float rr = r + wave;
        float y = base_y + 0.012f + 0.006f * sinf(4.0f * a + time_s * 1.35f);
        glVertex3f(cx + cosf(a) * rr, y, cz + sinf(a) * rr);
    }
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

    /* Pebble ring around the pond edge to separate water from ground. */
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    for (int i = 0; i < segments; i++) {
        float a = (float)i / (float)segments * 2.0f * (float)M_PI;
        float jitter = 0.22f * sinf(17.0f * a);
        float pr = r + 0.65f + jitter;
        float px = cx + cosf(a) * pr;
        float pz = cz + sinf(a) * pr;
        float pebble_size = 0.24f + 0.08f * (0.5f + 0.5f * sinf(11.0f * a));
        float gray = 0.42f + 0.20f * (0.5f + 0.5f * cosf(13.0f * a));

        glBegin(GL_TRIANGLE_FAN);
        glColor3f(gray, gray, gray * 0.95f);
        glVertex3f(px, 0.03f, pz);
        for (int k = 0; k <= 10; k++) {
            float ak = (float)k / 10.0f * 2.0f * (float)M_PI;
            float ex = px + cosf(ak) * pebble_size * (0.85f + 0.15f * sinf(a * 7.0f));
            float ez = pz + sinf(ak) * pebble_size * (0.85f + 0.15f * cosf(a * 5.0f));
            glVertex3f(ex, 0.03f, ez);
        }
        glEnd();
    }

    glEnable(GL_LIGHTING);
}

static void show_help(SDL_Window* window) {
    const char* help_text =
        "Park Projekt - Kezeles\n\n"
        "Mozgas: W/A/S/D\n"
        "Nezes: eger\n"
        "Fenyero: + / -\n"
        "Fenymozgatas: nyilak (X/Z), R/F (Y)\n"
        "Automata feny korpalya ki/be: O\n"
        "Fa mozgatasa: J=bal, L=jobb, I=fel, K=le\n"
        "Kodhatas suruseg: 9 (novel), 8 (csokkent)\n"
        "Kod ki/be: G\n"
        "Eso ki/be: P\n"
        "Kivalasztott fa valtas: T\n"
        "Sugo: F1\n"
        "Kilepes: ESC";

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Hasznalati utmutato", help_text, window);
}

int main(int, char**) {

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
    tree_model.texture_id = create_checker_texture();
    
    Scene scene;
    load_scene(&scene, "assets/data.csv");
    add_extra_trees(&scene);
    move_trees_off_path(&scene);
    Skybox skybox;
    bool running = true;
    uint32_t last_ticks = SDL_GetTicks();
    float brightness = 1.0f;
    float light_angle = 0.0f;
    float fog_density = 0.005f;
    bool fog_enabled = true;
    bool auto_light = true;
    bool rain_enabled = true;
    float light_x = 100.0f;
    float light_y = 80.0f;
    float light_z = 0.0f;
    int selected_tree = 0;
    RainDrop rain[RAIN_COUNT];
    srand((unsigned int)SDL_GetTicks());
    init_rain(rain, &camera);

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
                if (event.key.keysym.sym == SDLK_F1) show_help(window);
                if (event.key.keysym.sym == SDLK_o) auto_light = !auto_light;
                if (event.key.keysym.sym == SDLK_g) fog_enabled = !fog_enabled;
                if (event.key.keysym.sym == SDLK_p) rain_enabled = !rain_enabled;
                if (event.key.keysym.sym == SDLK_9 || event.key.keysym.sym == SDLK_KP_9) fog_density += 0.001f;
                if (event.key.keysym.sym == SDLK_8 || event.key.keysym.sym == SDLK_KP_8) fog_density -= 0.001f;
                if (event.key.keysym.sym == SDLK_t && scene.tree_count > 0) {
                    selected_tree = (selected_tree + 1) % scene.tree_count;
                }
            }
            if (event.type == SDL_MOUSEMOTION) { mouse_x = event.motion.xrel; mouse_y = event.motion.yrel; }
        }

        if (brightness < 0.0f) brightness = 0.0f;
        if (brightness > 2.0f) brightness = 2.0f;
        if (fog_density < 0.0005f) fog_density = 0.0005f;
        if (fog_density > 0.03f) fog_density = 0.03f;

        const Uint8* state = SDL_GetKeyboardState(NULL);

        float light_speed = 50.0f * (float)dt;
        if (state[SDL_SCANCODE_LEFT]) light_x -= light_speed;
        if (state[SDL_SCANCODE_RIGHT]) light_x += light_speed;
        if (state[SDL_SCANCODE_UP]) light_z -= light_speed;
        if (state[SDL_SCANCODE_DOWN]) light_z += light_speed;
        if (state[SDL_SCANCODE_R]) light_y += light_speed;
        if (state[SDL_SCANCODE_F]) light_y -= light_speed;

        if (scene.tree_count > 0) {
            float obj_speed = 6.0f * (float)dt;
            if (state[SDL_SCANCODE_J]) scene.trees[selected_tree].x -= obj_speed;
            if (state[SDL_SCANCODE_L]) scene.trees[selected_tree].x += obj_speed;
            if (state[SDL_SCANCODE_I]) scene.trees[selected_tree].z -= obj_speed;
            if (state[SDL_SCANCODE_K]) scene.trees[selected_tree].z += obj_speed;
        }

        if (auto_light) {
            light_angle += 0.2f * (float)dt;
            light_x = cosf(light_angle) * 100.0f;
            light_z = sinf(light_angle) * 100.0f;
        }

        if (fog_enabled) {
            glEnable(GL_FOG);
            glFogf(GL_FOG_DENSITY, fog_density);
        } else {
            glDisable(GL_FOG);
        }

        float light_pos[] = { light_x, light_y, light_z, 1.0f };
        float light_diff[] = { brightness, brightness, brightness, 1.0f };
        float light_amb[] = { 0.5f, 0.5f, 0.5f, 1.0f };
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        set_view_matrix(&camera);
        
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diff);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);

        float old_x = camera.x;
        float old_z = camera.z;
        handle_camera_input(&camera, state, mouse_x, mouse_y, dt);
        if (check_collision_aabb(&scene, camera.x, camera.z, 0.25f)) {
            camera.x = old_x;
            camera.z = old_z;
        }
        
        draw_skybox(&skybox);
        draw_path();
        draw_scene_shadows(&scene);
        draw_transparent_water((float)current_ticks / 1000.0f);
        draw_bench(4.1f, -12.0f, -90.0f);
        draw_scene_with_selection(&scene, &tree_model, selected_tree);
        if (rain_enabled) {
            update_rain(rain, &camera, (float)dt);
            draw_rain(rain);
        }
        
        SDL_GL_SwapWindow(window);
    }
    if (tree_model.texture_id != 0) {
        glDeleteTextures(1, &tree_model.texture_id);
    }

    free_scene(&scene); free_model(&tree_model);
    SDL_GL_DeleteContext(context); SDL_DestroyWindow(window); SDL_Quit();
    return 0;
}