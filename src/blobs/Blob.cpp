/***************************************************************************************************
 * @file  Blob.cpp
 * @brief Implementation of the Blob class
 **************************************************************************************************/

#include "blobs/Blob.hpp"

float attenuation_wyvill(float distance_sqr, int n) {
    float base = 1.0f - distance_sqr;
    float result = 1.0f;

    for(int i = 0; i < n; ++i) {
        result *= base;
    }

    return result;
}

float attenuation_wyvill2(float distance_sqr) {
    float base = 1.0f - distance_sqr;
    return base * base;
}

Blob::Blob(float scale) : scale(scale) {
}
