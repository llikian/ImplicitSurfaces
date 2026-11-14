/***************************************************************************************************
 * @file  Texture.hpp
 * @brief Declaration of the Texture class
 **************************************************************************************************/

#pragma once

#include <filesystem>

/**
 * @class Texture
 * @brief Creates a texture and assigns an image's data to it. Can then be bound.
 */
class Texture {
public:
    /**
     * @brief Default constructor. Initializes the texture id to 0, the default texture.
     */
    Texture();

    /**
     * @brief Copy constructor.
     * @warning The responsibility of freeing the texture goes to the user, so if multiple copies of
     * the same texture exist, be sure that all copies are no longer in use before freeing.
     * @param texture The texture to copy.
     */
    Texture(const Texture& texture);

    /**
     * @brief Copy operator.
     * @warning The responsibility of freeing the texture goes to the user, so if multiple copies of
     * the same texture exist, be sure that all copies are no longer in use before freeing.
     * Additionally, if this instance of the Texture class already had a texture (id != 0) and you
     * no longer wish to use that texture, be sure to call the free methodbeforehand.
     * @param texture The texture to copy.
     */
    Texture& operator=(const Texture& texture);

    /**
     * @brief Creates a texture.
     */
    void init();

    /**
     * @brief Deletes the texture.
     */
    void free();

    /**
     * @brief Creates a texture with the specified, width, height, data and format.
     * @warning The responsibility of freeing the texture goes to the user, so if this instance of
     * the Texture class already had an active texture (id != 0) and you no longer wish to use that
     * texture, be sure to call the free method beforehand.
     * @param internal_format The texture's internal format.
     * @param format The image data's format.
     * @param type The image data's type.
     * @param width The image's width.
     * @param height The image's height.
     * @param data The texture's image data.
     */
    void create(unsigned int internal_format,
                unsigned int format,
                unsigned int type,
                unsigned int width,
                unsigned int height,
                const void* data);

    /**
     * @brief Creates a 1*1px texture with a specific color.
     * @warning The responsibility of freeing the texture goes to the user, so if this instance of
     * the Texture class already had an active texture (id != 0) and you no longer wish to use that
     * texture, be sure to call the free method beforehand.
     * @param r The r component of the texture's color.
     * @param g The g component of the texture's color.
     * @param b The b component of the texture's color.
     */
    void create(unsigned char r, unsigned char g, unsigned char b);

    /**
     * @brief Binds the texture to a specifc texture unit.
     * @param texture_unit The opengl texture unit ID.
     */
    void bind(unsigned int texture_unit = 0) const;

    /**
     * @return Whether the texture is the default texture (id == 0).
     */
    bool is_default_texture() const;

    /**
     * @return The texture's id.
     */
    unsigned int get_id() const;

    /**
     * @return Whether the texture has transparency.
     */
    bool has_transparency() const;

private:
    unsigned int id;         ///< Texture id.
    bool b_has_transparency; ///< Whether the texture has transparency.
};
