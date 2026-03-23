#include <SDL2/SDL_opengl.h>
#include <stdbool.h>
#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint create_checker_texture(void) {
    /* Simple 4x4 checker texture as fallback when no image asset is available. */
    static const unsigned char pixels[4 * 4 * 3] = {
        170, 170, 170,   135, 135, 135,   170, 170, 170,   135, 135, 135,
        135, 135, 135,   170, 170, 170,   135, 135, 135,   170, 170, 170,
        170, 170, 170,   135, 135, 135,   170, 170, 170,   135, 135, 135,
        135, 135, 135,   170, 170, 170,   135, 135, 135,   170, 170, 170
    };

    GLuint tid = 0;
    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_2D, tid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 4, 4, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    return tid;
}

GLuint load_texture(const char* filename) {
    int w, h, c;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename, &w, &h, &c, 0);
    if (!data) return 0;
    GLuint tid;
    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_2D, tid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLenum fmt = (c == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    return tid;
}