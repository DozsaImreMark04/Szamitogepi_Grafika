#ifndef MODEL_H
#define MODEL_H

#include <SDL2/SDL_opengl.h>

typedef struct { float x, y, z; } Vertex;
typedef struct { float u, v; } TexCoord;
typedef struct { int v[3], t[3], n[3]; } Face;

typedef struct {
    Vertex* vertices;
    TexCoord* texcoords;
    Vertex* normals;
    Face* faces;
    int vertex_count, texcoord_count, normal_count, face_count;
    GLuint texture_id;
} Model;

void load_model(Model* model, const char* filename);
void draw_model(Model* model);
void free_model(Model* model);

#endif