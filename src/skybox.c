#include "skybox.h"
#include <SDL2/SDL_opengl.h>

void draw_skybox(Skybox* skybox) {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_FOG);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);

    glBegin(GL_QUADS);
    glColor3f(0.5f, 0.7f, 0.9f);
    glVertex3f(-1000, -100, -1000);
    glVertex3f(1000, -100, -1000);
    glColor3f(0.2f, 0.4f, 0.6f);
    glVertex3f(1000, 1000, -1000);
    glVertex3f(-1000, 1000, -1000);

    glVertex3f(-1000, -100, 1000);
    glVertex3f(1000, -100, 1000);
    glVertex3f(1000, 1000, 1000);
    glVertex3f(-1000, 1000, 1000);

    glVertex3f(-1000, -100, -1000);
    glVertex3f(-1000, -100, 1000);
    glVertex3f(-1000, 1000, 1000);
    glVertex3f(-1000, 1000, -1000);

    glVertex3f(1000, -100, -1000);
    glVertex3f(1000, -100, 1000);
    glVertex3f(1000, 1000, 1000);
    glVertex3f(1000, 1000, -1000);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.1f, 0.3f, 0.1f);
    glVertex3f(-1000, -0.01f, -1000);
    glVertex3f(1000, -0.01f, -1000);
    glVertex3f(1000, -0.01f, 1000);
    glVertex3f(-1000, -0.01f, 1000);
    glEnd();

    glDepthMask(GL_TRUE);
    glEnable(GL_FOG);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
}