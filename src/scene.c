#include "scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
    for (int i = 0; i < scene->tree_count; i++) {
        glPushMatrix();
            glTranslatef(scene->trees[i].x, scene->trees[i].y, scene->trees[i].z);
            glScalef(scene->trees[i].scale, scene->trees[i].scale, scene->trees[i].scale);
            draw_model(tree_model);
        glPopMatrix();
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

void free_scene(Scene* scene) { free(scene->trees); }