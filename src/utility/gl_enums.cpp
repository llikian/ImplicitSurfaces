/***************************************************************************************************
 * @file  gl_enums.cpp
 * @brief Implementation of helper functions regarding OpenGL enums.
 **************************************************************************************************/

#include "utility/gl_enums.hpp"

#include <glad/glad.h>

const char* gl_internal_format_to_string(unsigned int internal_format) {
    switch(internal_format) {
        case GL_DEPTH_COMPONENT: return "GL_DEPTH_COMPONENT";
        case GL_DEPTH_STENCIL: return "GL_DEPTH_STENCIL";
        case GL_RED: return "GL_RED";
        case GL_RG: return "GL_RG";
        case GL_RGB: return "GL_RGB";
        case GL_RGBA: return "GL_RGBA";
        case GL_R8: return "GL_R8";
        case GL_R8_SNORM: return "GL_R8_SNORM";
        case GL_R16: return "GL_R16";
        case GL_R16_SNORM: return "GL_R16_SNORM";
        case GL_RG8: return "GL_RG8";
        case GL_RG8_SNORM: return "GL_RG8_SNORM";
        case GL_RG16: return "GL_RG16";
        case GL_RG16_SNORM: return "GL_RG16_SNORM";
        case GL_R3_G3_B2: return "GL_R3_G3_B2";
        case GL_RGB4: return "GL_RGB4";
        case GL_RGB5: return "GL_RGB5";
        case GL_RGB8: return "GL_RGB8";
        case GL_RGB8_SNORM: return "GL_RGB8_SNORM";
        case GL_RGB10: return "GL_RGB10";
        case GL_RGB12: return "GL_RGB12";
        case GL_RGB16_SNORM: return "GL_RGB16_SNORM";
        case GL_RGBA2: return "GL_RGBA2";
        case GL_RGBA4: return "GL_RGBA4";
        case GL_RGB5_A1: return "GL_RGB5_A1";
        case GL_RGBA8: return "GL_RGBA8";
        case GL_RGBA8_SNORM: return "GL_RGBA8_SNORM";
        case GL_RGB10_A2: return "GL_RGB10_A2";
        case GL_RGB10_A2UI: return "GL_RGB10_A2UI";
        case GL_RGBA12: return "GL_RGBA12";
        case GL_RGBA16: return "GL_RGBA16";
        case GL_SRGB8: return "GL_SRGB8";
        case GL_SRGB8_ALPHA8: return "GL_SRGB8_ALPHA8";
        case GL_R16F: return "GL_R16F";
        case GL_RG16F: return "GL_RG16F";
        case GL_RGB16F: return "GL_RGB16F";
        case GL_RGBA16F: return "GL_RGBA16F";
        case GL_R32F: return "GL_R32F";
        case GL_RG32F: return "GL_RG32F";
        case GL_RGB32F: return "GL_RGB32F";
        case GL_RGBA32F: return "GL_RGBA32F";
        case GL_R11F_G11F_B10F: return "GL_R11F_G11F_B10F";
        case GL_RGB9_E5: return "GL_RGB9_E5";
        case GL_R8I: return "GL_R8I";
        case GL_R8UI: return "GL_R8UI";
        case GL_R16I: return "GL_R16I";
        case GL_R16UI: return "GL_R16UI";
        case GL_R32I: return "GL_R32I";
        case GL_R32UI: return "GL_R32UI";
        case GL_RG8I: return "GL_RG8I";
        case GL_RG8UI: return "GL_RG8UI";
        case GL_RG16I: return "GL_RG16I";
        case GL_RG16UI: return "GL_RG16UI";
        case GL_RG32I: return "GL_RG32I";
        case GL_RG32UI: return "GL_RG32UI";
        case GL_RGB8I: return "GL_RGB8I";
        case GL_RGB8UI: return "GL_RGB8UI";
        case GL_RGB16I: return "GL_RGB16I";
        case GL_RGB16UI: return "GL_RGB16UI";
        case GL_RGB32I: return "GL_RGB32I";
        case GL_RGB32UI: return "GL_RGB32UI";
        case GL_RGBA8I: return "GL_RGBA8I";
        case GL_RGBA8UI: return "GL_RGBA8UI";
        case GL_RGBA16I: return "GL_RGBA16I";
        case GL_RGBA16UI: return "GL_RGBA16UI";
        case GL_RGBA32I: return "GL_RGBA32I";
        case GL_RGBA32UI: return "GL_RGBA32UI";
        case GL_COMPRESSED_RED: return "GL_COMPRESSED_RED";
        case GL_COMPRESSED_RG: return "GL_COMPRESSED_RG";
        case GL_COMPRESSED_RGB: return "GL_COMPRESSED_RGB";
        case GL_COMPRESSED_RGBA: return "GL_COMPRESSED_RGBA";
        case GL_COMPRESSED_SRGB: return "GL_COMPRESSED_SRGB";
        case GL_COMPRESSED_SRGB_ALPHA: return "GL_COMPRESSED_SRGB_ALPHA";
        case GL_COMPRESSED_RED_RGTC1: return "GL_COMPRESSED_RED_RGTC1";
        case GL_COMPRESSED_SIGNED_RED_RGTC1: return "GL_COMPRESSED_SIGNED_RED_RGTC1";
        case GL_COMPRESSED_RG_RGTC2: return "GL_COMPRESSED_RG_RGTC2";
        case GL_COMPRESSED_SIGNED_RG_RGTC2: return "GL_COMPRESSED_SIGNED_RG_RGTC2";
        case GL_COMPRESSED_RGBA_BPTC_UNORM: return "GL_COMPRESSED_RGBA_BPTC_UNORM";
        case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM: return "GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM";
        case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT: return "GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT";
        case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT: return "GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT";
        default: return "UNKNOWN INTERNAL FORMAT";
    }
}

const char* gl_format_to_string(unsigned int format) {
    switch(format) {
        case GL_RED: return "GL_RED";
        case GL_RG: return "GL_RG";
        case GL_RGB: return "GL_RGB";
        case GL_BGR: return "GL_BGR";
        case GL_RGBA: return "GL_RGBA";
        case GL_BGRA: return "GL_BGRA";
        case GL_RED_INTEGER: return "GL_RED_INTEGER";
        case GL_RG_INTEGER: return "GL_RG_INTEGER";
        case GL_RGB_INTEGER: return "GL_RGB_INTEGER";
        case GL_BGR_INTEGER: return "GL_BGR_INTEGER";
        case GL_RGBA_INTEGER: return "GL_RGBA_INTEGER";
        case GL_BGRA_INTEGER: return "GL_BGRA_INTEGER";
        case GL_STENCIL_INDEX: return "GL_STENCIL_INDEX";
        case GL_DEPTH_COMPONENT: return "GL_DEPTH_COMPONENT";
        case GL_DEPTH_STENCIL: return "GL_DEPTH_STENCIL";
        default: return "UNKNOWN FORMAT";
    }
}

const char* gl_uniform_type_to_string(unsigned int type) {
    switch(type) {
        case GL_FLOAT: return "float";
        case GL_FLOAT_VEC2: return "vec2";
        case GL_FLOAT_VEC3: return "vec3";
        case GL_FLOAT_VEC4: return "vec4";
        case GL_DOUBLE: return "double";
        case GL_DOUBLE_VEC2: return "dvec2";
        case GL_DOUBLE_VEC3: return "dvec3";
        case GL_DOUBLE_VEC4: return "dvec4";
        case GL_INT: return "int";
        case GL_INT_VEC2: return "ivec2";
        case GL_INT_VEC3: return "ivec3";
        case GL_INT_VEC4: return "ivec4";
        case GL_UNSIGNED_INT: return "unsigned int";
        case GL_UNSIGNED_INT_VEC2: return "uvec2";
        case GL_UNSIGNED_INT_VEC3: return "uvec3";
        case GL_UNSIGNED_INT_VEC4: return "uvec4";
        case GL_BOOL: return "bool";
        case GL_BOOL_VEC2: return "bvec2";
        case GL_BOOL_VEC3: return "bvec3";
        case GL_BOOL_VEC4: return "bvec4";
        case GL_FLOAT_MAT2: return "mat2";
        case GL_FLOAT_MAT3: return "mat3";
        case GL_FLOAT_MAT4: return "mat4";
        case GL_FLOAT_MAT2x3: return "mat2x3";
        case GL_FLOAT_MAT2x4: return "mat2x4";
        case GL_FLOAT_MAT3x2: return "mat3x2";
        case GL_FLOAT_MAT3x4: return "mat3x4";
        case GL_FLOAT_MAT4x2: return "mat4x2";
        case GL_FLOAT_MAT4x3: return "mat4x3";
        case GL_DOUBLE_MAT2: return "dmat2";
        case GL_DOUBLE_MAT3: return "dmat3";
        case GL_DOUBLE_MAT4: return "dmat4";
        case GL_DOUBLE_MAT2x3: return "dmat2x3";
        case GL_DOUBLE_MAT2x4: return "dmat2x4";
        case GL_DOUBLE_MAT3x2: return "dmat3x2";
        case GL_DOUBLE_MAT3x4: return "dmat3x4";
        case GL_DOUBLE_MAT4x2: return "dmat4x2";
        case GL_DOUBLE_MAT4x3: return "dmat4x3";
        case GL_SAMPLER_1D: return "sampler1D";
        case GL_SAMPLER_2D: return "sampler2D";
        case GL_SAMPLER_3D: return "sampler3D";
        case GL_SAMPLER_CUBE: return "samplerCube";
        case GL_SAMPLER_1D_SHADOW: return "sampler1DShadow";
        case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow";
        case GL_SAMPLER_1D_ARRAY: return "sampler1DArray";
        case GL_SAMPLER_2D_ARRAY: return "sampler2DArray";
        case GL_SAMPLER_1D_ARRAY_SHADOW: return "sampler1DArrayShadow";
        case GL_SAMPLER_2D_ARRAY_SHADOW: return "sampler2DArrayShadow";
        case GL_SAMPLER_2D_MULTISAMPLE: return "sampler2DMS";
        case GL_SAMPLER_2D_MULTISAMPLE_ARRAY: return "sampler2DMSArray";
        case GL_SAMPLER_CUBE_SHADOW: return "samplerCubeShadow";
        case GL_SAMPLER_BUFFER: return "samplerBuffer";
        case GL_SAMPLER_2D_RECT: return "sampler2DRect";
        case GL_SAMPLER_2D_RECT_SHADOW: return "sampler2DRectShadow";
        case GL_INT_SAMPLER_1D: return "isampler1D";
        case GL_INT_SAMPLER_2D: return "isampler2D";
        case GL_INT_SAMPLER_3D: return "isampler3D";
        case GL_INT_SAMPLER_CUBE: return "isamplerCube";
        case GL_INT_SAMPLER_1D_ARRAY: return "isampler1DArray";
        case GL_INT_SAMPLER_2D_ARRAY: return "isampler2DArray";
        case GL_INT_SAMPLER_2D_MULTISAMPLE: return "isampler2DMS";
        case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "isampler2DMSArray";
        case GL_INT_SAMPLER_BUFFER: return "isamplerBuffer";
        case GL_INT_SAMPLER_2D_RECT: return "isampler2DRect";
        case GL_UNSIGNED_INT_SAMPLER_1D: return "usampler1D";
        case GL_UNSIGNED_INT_SAMPLER_2D: return "usampler2D";
        case GL_UNSIGNED_INT_SAMPLER_3D: return "usampler3D";
        case GL_UNSIGNED_INT_SAMPLER_CUBE: return "usamplerCube";
        case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY: return "usampler2DArray";
        case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: return "usampler2DArray";
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE: return "usampler2DMS";
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "usampler2DMSArray";
        case GL_UNSIGNED_INT_SAMPLER_BUFFER: return "usamplerBuffer";
        case GL_UNSIGNED_INT_SAMPLER_2D_RECT: return "usampler2DRect";
        case GL_IMAGE_1D: return "image1D";
        case GL_IMAGE_2D: return "image2D";
        case GL_IMAGE_3D: return "image3D";
        case GL_IMAGE_2D_RECT: return "image2DRect";
        case GL_IMAGE_CUBE: return "imageCube";
        case GL_IMAGE_BUFFER: return "imageBuffer";
        case GL_IMAGE_1D_ARRAY: return "image1DArray";
        case GL_IMAGE_2D_ARRAY: return "image2DArray";
        case GL_IMAGE_2D_MULTISAMPLE: return "image2DMS";
        case GL_IMAGE_2D_MULTISAMPLE_ARRAY: return "image2DMSArray";
        case GL_INT_IMAGE_1D: return "iimage1D";
        case GL_INT_IMAGE_2D: return "iimage2D";
        case GL_INT_IMAGE_3D: return "iimage3D";
        case GL_INT_IMAGE_2D_RECT: return "iimage2DRect";
        case GL_INT_IMAGE_CUBE: return "iimageCube";
        case GL_INT_IMAGE_BUFFER: return "iimageBuffer";
        case GL_INT_IMAGE_1D_ARRAY: return "iimage1DArray";
        case GL_INT_IMAGE_2D_ARRAY: return "iimage2DArray";
        case GL_INT_IMAGE_2D_MULTISAMPLE: return "iimage2DMS";
        case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY: return "iimage2DMSArray";
        case GL_UNSIGNED_INT_IMAGE_1D: return "uimage1D";
        case GL_UNSIGNED_INT_IMAGE_2D: return "uimage2D";
        case GL_UNSIGNED_INT_IMAGE_3D: return "uimage3D";
        case GL_UNSIGNED_INT_IMAGE_2D_RECT: return "uimage2DRect";
        case GL_UNSIGNED_INT_IMAGE_CUBE: return "uimageCube";
        case GL_UNSIGNED_INT_IMAGE_BUFFER: return "uimageBuffer";
        case GL_UNSIGNED_INT_IMAGE_1D_ARRAY: return "uimage1DArray";
        case GL_UNSIGNED_INT_IMAGE_2D_ARRAY: return "uimage2DArray";
        case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE: return "uimage2DMS";
        case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY: return "uimage2DMSArray";
        case GL_UNSIGNED_INT_ATOMIC_COUNTER: return "atomic_uint";
        default: return "UNKNOWN UNIFORM TYPE";
    }
}
