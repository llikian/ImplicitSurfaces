/***************************************************************************************************
 * @file  Texture.cpp
 * @brief Implementation of the Texture class
 **************************************************************************************************/

#include "assets/Texture.hpp"

#include "glad/glad.h"

/**
 * @brief Returns the internal_format and format corresponding to a specific amount of channels.
 * @param channels_amount The amount of channels.
 * @param srgb Whether the internal format should be SRGB.
 * @param internal_format The internal format corresponding to the amount of channels. Uses unsigned bytes as the type.
 * @param format The format corresponding to the amount of channels.
 */
static void get_formats_from_channels_amount(unsigned int channels_amount,
                                             bool srgb,
                                             unsigned int& internal_format,
                                             unsigned int& format) {
    switch(channels_amount) {
        case 3:
            internal_format = srgb ? GL_SRGB8 : GL_RGB8;
            format = GL_RGB;
            break;
        case 4:
            internal_format = srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
            format = GL_RGBA;
            break;
        case 1:
            internal_format = GL_R8;
            format = GL_RED;
            break;
        case 2:
            internal_format = GL_RG8;
            format = GL_RG;
            break;
        default: throw std::runtime_error("Unexpected amount of channels in image:" + std::to_string(channels_amount));
    }
}

Texture::Texture() : id(0), b_has_transparency(false) {
}

Texture::Texture(const Texture& texture) : id(texture.id), b_has_transparency(texture.has_transparency()) {
}

Texture& Texture::operator=(const Texture& texture) {
    id = texture.id;
    b_has_transparency = texture.b_has_transparency;
    return *this;
}

void Texture::init() {
    glGenTextures(1, &id);
}

void Texture::free() {
    if(id == 0) {
        return;
    }
    glDeleteTextures(1, &id);
    id = 0;
}

void Texture::create(unsigned int internal_format,
                     unsigned int format,
                     unsigned int type,
                     unsigned int width,
                     unsigned int height,
                     const void* data) {
    init();
    bind();

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    b_has_transparency =
        format == GL_RGBA || format == GL_BGRA || format == GL_RGBA_INTEGER || format == GL_BGRA_INTEGER;
}

void Texture::create(unsigned char r, unsigned char g, unsigned char b) {
    unsigned char color[3] { r, g, b };
    create(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, 1, 1, color);
}

void Texture::bind(unsigned int texture_unit) const {
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_2D, id);
}

bool Texture::is_default_texture() const {
    return id == 0;
}

unsigned int Texture::get_id() const {
    return id;
}

bool Texture::has_transparency() const {
    return b_has_transparency;
}
