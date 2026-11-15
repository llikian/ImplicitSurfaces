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

float attenuation_polynomiale(float distance_sqr) {
    return pow2(1.0f - distance_sqr) * (1.0f - 4.0f * distance_sqr / 9.0f);
}

Blob::Blob(float scale) : scale(scale) {
}

void Blob::precompute(std::size_t& count) {
    ++count;
};
