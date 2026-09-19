/***************************************************************************************************
 * @file  mat4.cpp
 * @brief Implementation of the mat4 struct
 **************************************************************************************************/

#include "maths/mat4.hpp"

#include "maths/geometry.hpp"

mat4::mat4(float v00, float v01, float v02, float v03,
           float v10, float v11, float v12, float v13,
           float v20, float v21, float v22, float v23,
           float v30, float v31, float v32, float v33)
    : columns{
        vec4(v00, v10, v20, v30),
        vec4(v01, v11, v21, v31),
        vec4(v02, v12, v22, v32),
        vec4(v03, v13, v23, v33)
    } { }

mat4::mat4(float v00, float v01, float v02,
           float v10, float v11, float v12,
           float v20, float v21, float v22)
    : columns{
        vec4(v00, v10, v20, 0.0f),
        vec4(v01, v11, v21, 0.0f),
        vec4(v02, v12, v22, 0.0f),
        vec4(0.0f, 0.0f, 0.0f, 1.0f)
    } { }

mat4::mat4(float scalar)
    : columns{
        vec4(scalar, 0.0f, 0.0f, 0.0f),
        vec4(0.0f, scalar, 0.0f, 0.0f),
        vec4(0.0f, 0.0f, scalar, 0.0f),
        vec4(0.0f, 0.0f, 0.0f, scalar)
    } { }

std::ostream& operator <<(std::ostream& stream, const mat4& mat) {
    for(int i = 0 ; i < 4 ; ++i) {
        stream << "( ";

        for(int j = 0 ; j < 3 ; ++j) {
            stream << ' ' << mat(i, j) << " ; ";
        }

        stream << mat(i, 3) << " )\n";
    }
    return stream;
}

void operator +=(mat4& left, const mat4& right) {
    for(int i = 0 ; i < 4 ; ++i) {
        for(int j = 0 ; j < 4 ; ++j) {
            left(i, j) += right(i, j);
        }
    }
}

void operator -=(mat4& left, const mat4& right) {
    for(int i = 0 ; i < 4 ; ++i) {
        for(int j = 0 ; j < 4 ; ++j) {
            left(i, j) -= right(i, j);
        }
    }
}

void operator +=(mat4& mat, float scalar) {
    for(int i = 0 ; i < 4 ; ++i) {
        for(int j = 0 ; j < 4 ; ++j) {
            mat(i, j) += scalar;
        }
    }
}

void operator -=(mat4& mat, float scalar) {
    for(int i = 0 ; i < 4 ; ++i) {
        for(int j = 0 ; j < 4 ; ++j) {
            mat(i, j) -= scalar;
        }
    }
}

void operator *=(mat4& mat, float scalar) {
    for(int i = 0 ; i < 4 ; ++i) {
        for(int j = 0 ; j < 4 ; ++j) {
            mat(i, j) *= scalar;
        }
    }
}

void operator /=(mat4& mat, float scalar) {
    for(int i = 0 ; i < 4 ; ++i) {
        for(int j = 0 ; j < 4 ; ++j) {
            mat(i, j) /= scalar;
        }
    }
}

mat4 operator +(const mat4& left, const mat4& right) {
    mat4 result;

    for(int i = 0 ; i < 4 ; ++i) {
        for(int j = 0 ; j < 4 ; ++j) {
            result(i, j) = left(i, j) + right(i, j);
        }
    }

    return result;
}

mat4 operator -(const mat4& left, const mat4& right) {
    mat4 result;

    for(int i = 0 ; i < 4 ; ++i) {
        for(int j = 0 ; j < 4 ; ++j) {
            result(i, j) = left(i, j) - right(i, j);
        }
    }

    return result;
}

mat4 operator *(const mat4& left, const mat4& right) {
    return mat4(
        left(0, 0) * right(0, 0) + left(0, 1) * right(1, 0) + left(0, 2) * right(2, 0) + left(0, 3) * right(3, 0),
        left(0, 0) * right(0, 1) + left(0, 1) * right(1, 1) + left(0, 2) * right(2, 1) + left(0, 3) * right(3, 1),
        left(0, 0) * right(0, 2) + left(0, 1) * right(1, 2) + left(0, 2) * right(2, 2) + left(0, 3) * right(3, 2),
        left(0, 0) * right(0, 3) + left(0, 1) * right(1, 3) + left(0, 2) * right(2, 3) + left(0, 3) * right(3, 3),
        left(1, 0) * right(0, 0) + left(1, 1) * right(1, 0) + left(1, 2) * right(2, 0) + left(1, 3) * right(3, 0),
        left(1, 0) * right(0, 1) + left(1, 1) * right(1, 1) + left(1, 2) * right(2, 1) + left(1, 3) * right(3, 1),
        left(1, 0) * right(0, 2) + left(1, 1) * right(1, 2) + left(1, 2) * right(2, 2) + left(1, 3) * right(3, 2),
        left(1, 0) * right(0, 3) + left(1, 1) * right(1, 3) + left(1, 2) * right(2, 3) + left(1, 3) * right(3, 3),
        left(2, 0) * right(0, 0) + left(2, 1) * right(1, 0) + left(2, 2) * right(2, 0) + left(2, 3) * right(3, 0),
        left(2, 0) * right(0, 1) + left(2, 1) * right(1, 1) + left(2, 2) * right(2, 1) + left(2, 3) * right(3, 1),
        left(2, 0) * right(0, 2) + left(2, 1) * right(1, 2) + left(2, 2) * right(2, 2) + left(2, 3) * right(3, 2),
        left(2, 0) * right(0, 3) + left(2, 1) * right(1, 3) + left(2, 2) * right(2, 3) + left(2, 3) * right(3, 3),
        left(3, 0) * right(0, 0) + left(3, 1) * right(1, 0) + left(3, 2) * right(2, 0) + left(3, 3) * right(3, 0),
        left(3, 0) * right(0, 1) + left(3, 1) * right(1, 1) + left(3, 2) * right(2, 1) + left(3, 3) * right(3, 1),
        left(3, 0) * right(0, 2) + left(3, 1) * right(1, 2) + left(3, 2) * right(2, 2) + left(3, 3) * right(3, 2),
        left(3, 0) * right(0, 3) + left(3, 1) * right(1, 3) + left(3, 2) * right(2, 3) + left(3, 3) * right(3, 3)
    );
}

mat4 operator +(const mat4& mat, float scalar) {
    mat4 result;

    for(int i = 0 ; i < 4 ; ++i) {
        for(int j = 0 ; j < 4 ; ++j) {
            result(i, j) = mat(i, j) + scalar;
        }
    }

    return result;
}

mat4 operator -(const mat4& mat, float scalar) {
    mat4 result;

    for(int i = 0 ; i < 4 ; ++i) {
        for(int j = 0 ; j < 4 ; ++j) {
            result(i, j) = mat(i, j) - scalar;
        }
    }

    return result;
}

mat4 operator *(const mat4& mat, float scalar) {
    mat4 result;

    for(int i = 0 ; i < 4 ; ++i) {
        for(int j = 0 ; j < 4 ; ++j) {
            result(i, j) = mat(i, j) * scalar;
        }
    }

    return result;
}

mat4 operator *(float scalar, const mat4& mat) {
    mat4 result;

    for(int i = 0 ; i < 4 ; ++i) {
        for(int j = 0 ; j < 4 ; ++j) {
            result(i, j) = scalar * mat(i, j);
        }
    }

    return result;
}

mat4 operator /(const mat4& mat, float scalar) {
    mat4 result;

    for(int i = 0 ; i < 4 ; ++i) {
        for(int j = 0 ; j < 4 ; ++j) {
            result(i, j) = mat(i, j) / scalar;
        }
    }

    return result;
}

vec3 operator*(const mat4& mat, const vec3& vec) {
    return vec3(
        mat(0, 0) * vec.x + mat(0, 1) * vec.y + mat(0, 2) * vec.z,
        mat(1, 0) * vec.x + mat(1, 1) * vec.y + mat(1, 2) * vec.z,
        mat(2, 0) * vec.x + mat(2, 1) * vec.y + mat(2, 2) * vec.z
    );
}

vec4 operator*(const mat4& mat, const vec4& vec) {
    return vec4(
        mat(0, 0) * vec.x + mat(0, 1) * vec.y + mat(0, 2) * vec.z + mat(0, 3) * vec.w,
        mat(1, 0) * vec.x + mat(1, 1) * vec.y + mat(1, 2) * vec.z + mat(1, 3) * vec.w,
        mat(2, 0) * vec.x + mat(2, 1) * vec.y + mat(2, 2) * vec.z + mat(2, 3) * vec.w,
        mat(3, 0) * vec.x + mat(3, 1) * vec.y + mat(3, 2) * vec.z + mat(3, 3) * vec.w
    );
}
