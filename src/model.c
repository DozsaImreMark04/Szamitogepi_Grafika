#include "model.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void load_model(Model* model, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return;
    char line[256];
    int v_idx = 0, t_idx = 0, n_idx = 0, f_idx = 0;
    model->vertex_count = model->texcoord_count = model->normal_count = model->face_count = 0;
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "v ", 2) == 0) model->vertex_count++;
        else if (strncmp(line, "vt ", 3) == 0) model->texcoord_count++;
        else if (strncmp(line, "vn ", 3) == 0) model->normal_count++;
        else if (strncmp(line, "f ", 2) == 0) model->face_count++;
    }
    model->vertices = malloc(model->vertex_count * sizeof(Vertex));
    model->texcoords = malloc(model->texcoord_count * sizeof(TexCoord));
    model->normals = malloc(model->normal_count * sizeof(Vertex));
    model->faces = malloc(model->face_count * sizeof(Face));
    rewind(file);
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "v ", 2) == 0) {
            sscanf(line, "v %f %f %f", &model->vertices[v_idx].x, &model->vertices[v_idx].y, &model->vertices[v_idx].z);
            v_idx++;
        } else if (strncmp(line, "vt ", 3) == 0) {
            sscanf(line, "vt %f %f", &model->texcoords[t_idx].u, &model->texcoords[t_idx].v);
            t_idx++;
        } else if (strncmp(line, "vn ", 3) == 0) {
            sscanf(line, "vn %f %f %f", &model->normals[n_idx].x, &model->normals[n_idx].y, &model->normals[n_idx].z);
            n_idx++;
        } else if (strncmp(line, "f ", 2) == 0) {
            sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", 
                &model->faces[f_idx].v[0], &model->faces[f_idx].t[0], &model->faces[f_idx].n[0],
                &model->faces[f_idx].v[1], &model->faces[f_idx].t[1], &model->faces[f_idx].n[1],
                &model->faces[f_idx].v[2], &model->faces[f_idx].t[2], &model->faces[f_idx].n[2]);
            f_idx++;
        }
    }
    fclose(file);
}

void draw_model(Model* model) {
    glDisable(GL_TEXTURE_2D);
    glShadeModel(GL_FLAT); // Kikapcsoljuk a színátmenetet
    
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < model->face_count; i++) {
        // A háromszög színe az első vertex magassága alapján dől el
        int first_vi = model->faces[i].v[0] - 1;
        
        if (model->vertices[first_vi].y > 2.8f) {
            glColor3f(0.0f, 0.7f, 0.0f); 
        } else {
            glColor3f(0.3f, 0.15f, 0.03f); 
        }

        for (int j = 0; j < 3; j++) {
            int vi = model->faces[i].v[j] - 1;
            int ni = model->faces[i].n[j] - 1;
            glNormal3f(model->normals[ni].x, model->normals[ni].y, model->normals[ni].z);
            glVertex3f(model->vertices[vi].x, model->vertices[vi].y, model->vertices[vi].z);
        }
    }
    glEnd();
    
    glShadeModel(GL_SMOOTH); 
    glEnable(GL_TEXTURE_2D);
}

void free_model(Model* model) {
    free(model->vertices); free(model->texcoords);
    free(model->normals); free(model->faces);
}