/***************************************************************************************************
 * @file  Surface.cpp
 * @brief Implementation of the Surface class
 **************************************************************************************************/

#include "Surface.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <unordered_map>
#include <unordered_set>
#include "maths/functions.hpp"
#include "maths/geometry.hpp"
#include "utility/LifetimeLogger.hpp"
#include "marching_cube_tables.hpp"

[[nodiscard]] float Surface::implicit(const vec3& point) const {
    return THRESHOLD - root->potential(point);
}

[[nodiscard]] vec3 Surface::gradient(const vec3& pos) const {
    float x = implicit(vec3(pos[0] + EPSILON, pos[1], pos[2])) - implicit(vec3(pos[0] - EPSILON, pos[1], pos[2]));
    float y = implicit(vec3(pos[0], pos[1] + EPSILON, pos[2])) - implicit(vec3(pos[0], pos[1] - EPSILON, pos[2]));
    float z = implicit(vec3(pos[0], pos[1], pos[2] + EPSILON)) - implicit(vec3(pos[0], pos[1], pos[2] - EPSILON));

    return vec3(x, y, z) * (0.5f / EPSILON);
}

[[nodiscard]] vec3 Surface::normal(const vec3& pos) const {
    return normalize(gradient(pos));
}

[[nodiscard]] vec3 Surface::dichotomy(vec3 a, vec3 b, float va, float vb, float length) const {
    int ia = va > 0.0 ? 1 : -1;

    vec3 c = (vb * a - va * b) / (vb - va);

    while(length > EPSILON) {
        float vc = implicit(c);
        int ic = vc > 0.0f ? 1 : -1;
        if(ia + ic == 0) {
            b = c;
        } else {
            ia = ic;
            a = c;
        }
        length *= 0.5f;
        c = 0.5f * (a + b);
    }

    return c;
}

[[nodiscard]] Mesh Surface::compute_mesh(int n) {
    if(root == nullptr) { throw std::runtime_error("Can't create the mesh for an empty implicit surface"); }

    LifetimeLogger lifetime_logger("Mesh creation took: ");

    std::size_t blobs_count = 0;
    AABB region = root->get_aabb_and_blob_count(blobs_count);

    std::cout << "The scene contains " << blobs_count << " blobs.\n";

    std::vector<vec3> vertices;
    std::vector<int> triangles;

    vertices.reserve(20000);
    triangles.reserve(20000);

    int nv = 0;
    const int nx = n;
    const int ny = n;
    const int nz = n;

    // Clamped integer values
    const int nax = 0;
    const int nbx = nx;
    const int nay = 0;
    const int nby = ny;
    const int naz = 0;
    const int nbz = nz;

    const int size = nx * ny;

    // Intensities
    double* a = new double[size];
    double* b = new double[size];

    // Vertex
    vec3* u = new vec3[size];
    vec3* v = new vec3[size];

    // Edges
    int* eax = new int[size];
    int* eay = new int[size];
    int* ebx = new int[size];
    int* eby = new int[size];
    int* ez = new int[size];

    // Diagonal of a cell
    vec3 d = (region.pmax - region.pmin) / static_cast<float>(n - 1);

    double za = 0.0;

    // Compute field inside lower Oxy plane
    for(int i = nax; i < nbx; i++) {
        for(int j = nay; j < nby; j++) {
            u[i * ny + j] = region.pmin + vec3(i * d[0], j * d[1], za);
            a[i * ny + j] = implicit(u[i * ny + j]);
        }
    }

    // Compute straddling edges inside lower Oxy plane
    for(int i = nax; i < nbx - 1; i++) {
        for(int j = nay; j < nby; j++) {
            // We need a xor b, which can be implemented a == !b
            if(!((a[i * ny + j] < 0.0) == !(a[(i + 1) * ny + j] >= 0.0))) {
                vertices.push_back(
                    dichotomy(u[i * ny + j], u[(i + 1) * ny + j], a[i * ny + j], a[(i + 1) * ny + j], d[0]));
                eax[i * ny + j] = nv;
                nv++;
            }
        }
    }
    for(int i = nax; i < nbx; i++) {
        for(int j = nay; j < nby - 1; j++) {
            if(!((a[i * ny + j] < 0.0) == !(a[i * ny + (j + 1)] >= 0.0))) {
                vertices.push_back(
                    dichotomy(u[i * ny + j], u[i * ny + (j + 1)], a[i * ny + j], a[i * ny + (j + 1)], d[1]));
                eay[i * ny + j] = nv;
                nv++;
            }
        }
    }

    // Array for edge vertices
    int e[12];

    // For all layers
    for(int k = naz; k < nbz; k++) {
        double zb = za + d[2];
        for(int i = nax; i < nbx; i++) {
            for(int j = nay; j < nby; j++) {
                v[i * ny + j] = region.pmin + vec3(i * d[0], j * d[1], zb);
                b[i * ny + j] = implicit(v[i * ny + j]);
            }
        }

        // Compute straddling edges inside lower Oxy plane
        for(int i = nax; i < nbx - 1; i++) {
            for(int j = nay; j < nby; j++) {
                //   if (((b[i*ny + j] < 0.0) && (b[(i + 1)*ny + j] >= 0.0)) || ((b[i*ny + j] >= 0.0) && (b[(i + 1)*ny +
                //   j] < 0.0)))
                if(!((b[i * ny + j] < 0.0) == !(b[(i + 1) * ny + j] >= 0.0))) {
                    vertices.push_back(
                        dichotomy(v[i * ny + j], v[(i + 1) * ny + j], b[i * ny + j], b[(i + 1) * ny + j], d[0]));
                    ebx[i * ny + j] = nv;
                    nv++;
                }
            }
        }

        for(int i = nax; i < nbx; i++) {
            for(int j = nay; j < nby - 1; j++) {
                // if (((b[i*ny + j] < 0.0) && (b[i*ny + (j + 1)] >= 0.0)) || ((b[i*ny + j] >= 0.0) && (b[i*ny + (j +
                // 1)] < 0.0)))
                if(!((b[i * ny + j] < 0.0) == !(b[i * ny + (j + 1)] >= 0.0))) {
                    vertices.push_back(
                        dichotomy(v[i * ny + j], v[i * ny + (j + 1)], b[i * ny + j], b[i * ny + (j + 1)], d[1]));
                    eby[i * ny + j] = nv;
                    nv++;
                }
            }
        }

        // Create vertical straddling edges
        for(int i = nax; i < nbx; i++) {
            for(int j = nay; j < nby; j++) {
                // if ((a[i*ny + j] < 0.0) && (b[i*ny + j] >= 0.0) || (a[i*ny + j] >= 0.0) && (b[i*ny + j] < 0.0))
                if(!((a[i * ny + j] < 0.0) == !(b[i * ny + j] >= 0.0))) {
                    vertices.push_back(dichotomy(u[i * ny + j], v[i * ny + j], a[i * ny + j], b[i * ny + j], d[2]));
                    ez[i * ny + j] = nv;
                    nv++;
                }
            }
        }

        // Create mesh
        for(int i = nax; i < nbx - 1; i++) {
            for(int j = nay; j < nby - 1; j++) {
                int cubeindex = 0;
                if(a[i * ny + j] < 0.0) { cubeindex |= 1; }
                if(a[(i + 1) * ny + j] < 0.0) { cubeindex |= 2; }
                if(a[i * ny + j + 1] < 0.0) { cubeindex |= 4; }
                if(a[(i + 1) * ny + j + 1] < 0.0) { cubeindex |= 8; }
                if(b[i * ny + j] < 0.0) { cubeindex |= 16; }
                if(b[(i + 1) * ny + j] < 0.0) { cubeindex |= 32; }
                if(b[i * ny + j + 1] < 0.0) { cubeindex |= 64; }
                if(b[(i + 1) * ny + j + 1] < 0.0) { cubeindex |= 128; }

                // Cube is straddling the surface
                if((cubeindex != 255) && (cubeindex != 0)) {
                    e[0] = eax[i * ny + j];
                    e[1] = eax[i * ny + (j + 1)];
                    e[2] = ebx[i * ny + j];
                    e[3] = ebx[i * ny + (j + 1)];
                    e[4] = eay[i * ny + j];
                    e[5] = eay[(i + 1) * ny + j];
                    e[6] = eby[i * ny + j];
                    e[7] = eby[(i + 1) * ny + j];
                    e[8] = ez[i * ny + j];
                    e[9] = ez[(i + 1) * ny + j];
                    e[10] = ez[i * ny + (j + 1)];
                    e[11] = ez[(i + 1) * ny + (j + 1)];

                    for(int h = 0; mc_triangles_table[cubeindex][h] != -1; h += 3) {
                        triangles.push_back(e[mc_triangles_table[cubeindex][h + 0]]);
                        triangles.push_back(e[mc_triangles_table[cubeindex][h + 1]]);
                        triangles.push_back(e[mc_triangles_table[cubeindex][h + 2]]);
                    }
                }
            }
        }

        std::swap(a, b);

        za = zb;
        std::swap(eax, ebx);
        std::swap(eay, eby);
        std::swap(u, v);
    }

    delete[] a;
    delete[] b;
    delete[] u;
    delete[] v;

    delete[] eax;
    delete[] eay;
    delete[] ebx;
    delete[] eby;
    delete[] ez;

    Mesh mesh;
    mesh.set_primitive(MeshPrimitive::TRIANGLES);
    mesh.enable_attribute(ATTRIBUTE_NORMAL);

    mesh.reserve_vertices_and_indices(vertices.size(), triangles.size());
    std::vector normals(vertices.size(), vec3(0.0f));

    for(std::size_t i = 0; i + 2 < triangles.size(); i += 3) {
        mesh.add_triangle(triangles[i], triangles[i + 2], triangles[i + 1]);

        const vec3 normal = cross(vertices[triangles[i + 2]] - vertices[triangles[i]],
                                  vertices[triangles[i + 1]] - vertices[triangles[i]]);

        normals[triangles[i]] += normal;
        normals[triangles[i + 1]] += normal;
        normals[triangles[i + 2]] += normal;
    }

    for(std::size_t i = 0; i < vertices.size(); ++i) { mesh.add_vertex(vertices[i], normals[i]); }

    mesh.bind_buffers();

    std::cout << "Vertices: " << mesh.get_vertices_amount() << '\n';
    std::cout << "Triangles: " << mesh.get_indices_amount() / 3 << '\n';

    return mesh;
}
