#ifndef SCENE_H
#define SCENE_H

#include "model.h"

typedef struct {
    float x, y, z;
    float scale;
} SceneObject;

typedef struct {
    SceneObject* trees;
    int tree_count;
} Scene;

void load_scene(Scene* scene, const char* filename);
void draw_scene(Scene* scene, Model* tree_model);
void draw_scene_shadows(Scene* scene);
int check_collision(Scene* scene, float x, float z, float radius);
int check_collision_aabb(Scene* scene, float x, float z, float half_extent);
void free_scene(Scene* scene);

#endif