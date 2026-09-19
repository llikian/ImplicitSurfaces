/***************************************************************************************************
 * @file  mat4.hpp
 * @brief Declaration of the mat4 struct
 **************************************************************************************************/

#pragma once

#include "vec3.hpp"
#include "vec4.hpp"

/**
 * @struct mat4
 * @brief Represents a 4 by 4 matrix. Even if the elements are accessed
 */
struct mat4 {
    /**
     * @brief Constructs a mat4 with all components equal to 0.
     */
    mat4() = default;

    /**
     * @brief Constructs a mat4 with a specific value for each component.
     * @param v00, v01, v02, v03 The values of the components of the first row.
     * @param v10, v11, v12, v13 The values of the components of the second row.
     * @param v20, v21, v22, v23 The values of the components of the third row.
     * @param v30, v31, v32, v33 The values of the components of the fourth row.
     */
    mat4(float v00, float v01, float v02, float v03,
         float v10, float v11, float v12, float v13,
         float v20, float v21, float v22, float v23,
         float v30, float v31, float v32, float v33);

    /**
     * @brief Constructs a mat4 with a specific value for each component of the 3 first columns
     * and rows. The fourth column and row are the same as the identity matrix.
     * @param v00, v01, v02 The values of the first three components of the first row.
     * @param v10, v11, v12 The values of the first three components of the second row.
     * @param v20, v21, v22 The values of the first three components of the third row.
     */
    mat4(float v00, float v01, float v02,
         float v10, float v11, float v12,
         float v20, float v21, float v22);

    /**
     * @brief Constructs a mat4 which is the identity matrix multiplied by a scalar.
     * Each value of the diagonal is equal to the scalar and the rest is zeros.
     * @param scalar The value for the components on the diagonal.
     */
    explicit mat4(float scalar);

    /**
     * @brief Accesses an element of the mat4.
     * @param row The row's index.
     * @param column The column's index.
     * @return A reference to the element.
     */
    float& operator()(uint8_t row, uint8_t column) { return (&columns[column].x)[row]; }

    /**
     * @brief Accesses an element of the mat4.
     * @param row The row's index.
     * @param column The column's index.
     * @return A const reference to the element.
     */
    const float& operator()(uint8_t row, uint8_t column) const { return (&columns[column].x)[row]; }

    /**
     * @brief Access a column of the mat4.
     * @param column The column's index.
     * @return A reference to the column.
     */
    vec4& operator[](uint8_t column) { return columns[column]; }

    /**
     * @brief Access a column of the mat4.
     * @param column The column's index.
     * @return A const reference to the column.
     */
    const vec4& operator[](uint8_t column) const { return columns[column]; }

private:
    vec4 columns[4]; ///< The columns of the mat4.
};

/**
 * @brief Writes the components of the given mat4 to the output stream in the format:\n
 * ( v00 ; v01 ; v02 ; v03 )\n
 * ( v10 ; v11 ; v12 ; v13 )\n
 * ( v20 ; v21 ; v22 ; v23 )\n
 * ( v30 ; v31 ; v32 ; v33 )\n
 * @param stream The output stream to write to.
 * @param mat The mat4 to write to the stream.
 * @return A reference to the output stream after writing the mat4.
 */
std::ostream& operator <<(std::ostream& stream, const mat4& mat);

/**
 * @brief Adds the second mat4's components to the first's.
 * @param left The left operand. Will store the result.
 * @param right The right operand.
 */
void operator +=(mat4& left, const mat4& right);

/**
 * @brief Subtracts the second mat4's components from the first's.
 * @param left The left operand. Will store the result.
 * @param right The right operand.
 */
void operator -=(mat4& left, const mat4& right);

/**
 * @brief Adds a scalar to all of a mat4's components.
 * @param mat The mat4.
 * @param scalar The scalar.
 */
void operator +=(mat4& mat, float scalar);

/**
 * @brief Subtracts all of a mat4's components by a scalar.
 * @param mat The mat4.
 * @param scalar The scalar.
 */
void operator -=(mat4& mat, float scalar);

/**
 * @brief Multiplies a scalar to all of a mat4's components.
 * @param mat The mat4.
 * @param scalar The scalar.
 */
void operator *=(mat4& mat, float scalar);

/**
 * @brief Divides all of a mat4's components by a scalar.
 * @param mat The mat4.
 * @param scalar The scalar.
 */
void operator /=(mat4& mat, float scalar);

/** @brief Adds a mat4's components to another's.
 *  @param left The left operand.
 *  @param right The right operand.
 *  @return The component-wise sum of the two mat4.
 */
mat4 operator +(const mat4& left, const mat4& right);

/** @brief Subtracts a mat4's components by another's.
 *  @param left The left operand.
 *  @param right The right operand.
 *  @return The component-wise subtraction of the first mat4 by the second.
 */
mat4 operator -(const mat4& left, const mat4& right);

/** @brief Multiplies a mat4 by another.
 *  @param left The left operand.
 *  @param right The right operand.
 *  @return The product of the two mat4.
 */
mat4 operator *(const mat4& left, const mat4& right);

/** @brief Adds a scalar to each of a mat4's components.
 *  @param mat The mat4.
 *  @param scalar The scalar.
 *  @return The component-wise sum of a mat4 by a scalar.
 */
mat4 operator +(const mat4& mat, float scalar);

/** @brief Subtracts each of a mat4's components by a scalar.
 *  @param mat The mat4.
 *  @param scalar The scalar.
 *  @return The component-wise subtraction of a mat4 by a scalar.
 */
mat4 operator -(const mat4& mat, float scalar);

/** @brief Multiplies each of a mat4's components by a scalar.
 *  @param mat The mat4.
 *  @param scalar The scalar.
 *  @return The component-wise product of a mat4 by a scalar.
 */
mat4 operator *(const mat4& mat, float scalar);

/** @brief Multiplies each of a mat4's components by a scalar.
 *  @param scalar The scalar.
 *  @param mat The mat4.
 *  @return The component-wise product of a mat4 by a scalar.
 */
mat4 operator *(float scalar, const mat4& mat);

/** @brief Divides each of a mat4's components by a scalar.
 *  @param mat The mat4.
 *  @param scalar The scalar.
 *  @return The component-wise division of a mat4 by a scalar.
 */
mat4 operator /(const mat4& mat, float scalar);

/**
 * @brief Multiplies a mat4 by a 3-component vector interpreted as the 4x1 column mat4 (vec.x, vec.y, vec.z, 0.0).
 * @param mat The mat4.
 * @param vec The vector.
 * @return The vector formed with the values in the first 3 rows of the result of mat * (vec.x, vec.y, vec.z, 0.0).
 */
vec3 operator*(const mat4& mat, const vec3& vec);

/**
 * @brief Multiplies a mat4 by a 4-component vector interpreted as a 4x1 column mat4.
 * @param mat The mat4.
 * @param vec The vector.
 * @return The vector formed with the values in the 4 rows of mat * vec.
 */
vec4 operator*(const mat4& mat, const vec4& vec);
