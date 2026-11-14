/***************************************************************************************************
 * @file  BezierBlob.cpp
 * @brief Implementation of the BezierBlob class
 **************************************************************************************************/

#include "blobs/BezierBlob.hpp"

void BezierBlob::precompute(std::size_t& count) {
    Blob::precompute(count);

    precomputed_values.resize(spheres_count);
    std::vector<float> radii(spheres_count);

    aabb.pmin = vec3(INFINITY);
    aabb.pmax = vec3(-INFINITY);
    float length = 0.0f;

    for(int i = 0; i < spheres_count; ++i) {
        float t = static_cast<float>(i) / (spheres_count - 1);
        radii[i] = std::lerp(radius_start, radius_end, t);
        precomputed_values[i].center = evaluate(t);
        precomputed_values[i].inv_radius_sqr = 1.0f / (radii[i] * radii[i]);
        precomputed_values[i].color = lerp(color_start, color_end, t);

        aabb.pmin = min(aabb.pmin, precomputed_values[i].center - radii[i]);
        aabb.pmax = max(aabb.pmax, precomputed_values[i].center + radii[i]);

        if(i > 0) {
            length += std::sqrt(length2(precomputed_values[i].center - precomputed_values[i - 1].center));
        }
    }

    float spacing = length / (spheres_count - 1);
    for(int i = 0; i < spheres_count; ++i) {
        precomputed_values[i].weight = 15.0f * spacing / (16.0f * radii[i]);
    }
}

[[nodiscard]] float BezierBlob::potential(const vec3& point) const {
    float sum = 0.0f;
    for(int i = 0; i < spheres_count; ++i) {
        float q = length2(point - precomputed_values[i].center) * precomputed_values[i].inv_radius_sqr;
        if(q < 1.0f) {
            sum += precomputed_values[i].weight * attenuation(q);
        }
    }
    return scale * sum;
}

[[nodiscard]] Sample BezierBlob::sample(const vec3& point) const {
    float sum = 0.0f;
    vec3 color_sum(0.0f);

    for(int i = 0; i < spheres_count; ++i) {
        float q = length2(point - precomputed_values[i].center) * precomputed_values[i].inv_radius_sqr;
        if(q < 1.0f) {
            const float contribution = precomputed_values[i].weight * attenuation(q);
            sum += contribution;
            color_sum += contribution * precomputed_values[i].color;
        }
    }

    if(sum <= 0.0f) {
        return Sample(0.0f, color_start);
    }

    return Sample(scale * sum, color_sum / sum);
}

vec3 BezierBlob::evaluate(float t) {
    float t1 = 1.0f - t;
    return (t1 * t1 * t1) * p0 + (3.0f * t1 * t1 * t) * p1 + (3.0f * t1 * t * t) * p2 + (t * t * t) * p3;
}

