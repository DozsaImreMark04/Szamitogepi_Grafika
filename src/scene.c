#include "scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void draw_scene_shadows(Scene* scene) {
    const int segments = 18;

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < scene->tree_count; i++) {
        float r = 0.65f * scene->trees[i].scale;

        glBegin(GL_TRIANGLE_FAN);
        glColor4f(0.05f, 0.05f, 0.05f, 0.35f);
        glVertex3f(scene->trees[i].x, 0.01f, scene->trees[i].z);

        glColor4f(0.05f, 0.05f, 0.05f, 0.00f);
        for (int s = 0; s <= segments; s++) {
            float a = (float)s / (float)segments * 2.0f * (float)M_PI;
            float x = scene->trees[i].x + cosf(a) * r;
            float z = scene->trees[i].z + sinf(a) * r;
            glVertex3f(x, 0.01f, z);
        }
        glEnd();
    }

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void load_scene(Scene* scene, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return;
    char line[100];
    fgets(line, sizeof(line), file);
    scene->tree_count = 0;
    while (fgets(line, sizeof(line), file)) scene->tree_count++;
    scene->trees = malloc(sizeof(SceneObject) * scene->tree_count);
    rewind(file);
    fgets(line, sizeof(line), file);
    for (int i = 0; i < scene->tree_count; i++) {
        fscanf(file, "%f,%f,%f,%f\n", &scene->trees[i].x, &scene->trees[i].y, &scene->trees[i].z, &scene->trees[i].scale);
    }
    fclose(file);
}

void draw_scene(Scene* scene, Model* tree_model) {
    draw_scene_with_selection(scene, tree_model, -1);
}

void draw_scene_with_selection(Scene* scene, Model* tree_model, int selected_index) {
    for (int i = 0; i < scene->tree_count; i++) {
        glPushMatrix();
            glTranslatef(scene->trees[i].x, scene->trees[i].y, scene->trees[i].z);
            glScalef(scene->trees[i].scale, scene->trees[i].scale, scene->trees[i].scale);
            draw_model(tree_model);
        glPopMatrix();
    }
    
    /* Rajzolj kijelölési jelölőt a kiválasztott fa körül. */
    if (selected_index >= 0 && selected_index < scene->tree_count) {
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        float sel_x = scene->trees[selected_index].x;
        float sel_z = scene->trees[selected_index].z;
        float sel_radius = 1.2f * scene->trees[selected_index].scale;
        
        glBegin(GL_LINE_LOOP);
        glColor4f(1.0f, 1.0f, 0.0f, 0.8f); /* Sárga körül */
        for (int j = 0; j < 32; j++) {
            float angle = (float)j / 32.0f * 2.0f * (float)M_PI;
            float x = sel_x + cosf(angle) * sel_radius;
            float z = sel_z + sinf(angle) * sel_radius;
            glVertex3f(x, 0.3f, z);
        }
        glEnd();
        
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
    }
}

int check_collision(Scene* scene, float x, float z, float radius) {
    for (int i = 0; i < scene->tree_count; i++) {
        float dx = x - scene->trees[i].x;
        float dz = z - scene->trees[i].z;
        float dist = sqrtf(dx*dx + dz*dz);
        if (dist < (radius + 0.5f * scene->trees[i].scale)) return 1;
    }
    return 0;
}

int check_collision_aabb(Scene* scene, float x, float z, float half_extent) {
    for (int i = 0; i < scene->tree_count; i++) {
        float half = 0.45f * scene->trees[i].scale;
        float min_x = scene->trees[i].x - half;
        float max_x = scene->trees[i].x + half;
        float min_z = scene->trees[i].z - half;
        float max_z = scene->trees[i].z + half;

        float cam_min_x = x - half_extent;
        float cam_max_x = x + half_extent;
        float cam_min_z = z - half_extent;
        float cam_max_z = z + half_extent;

        int overlap_x = (cam_max_x > min_x) && (cam_min_x < max_x);
        int overlap_z = (cam_max_z > min_z) && (cam_min_z < max_z);
        if (overlap_x && overlap_z) return 1;
    }
    return 0;
}

void free_scene(Scene* scene) { free(scene->trees); }