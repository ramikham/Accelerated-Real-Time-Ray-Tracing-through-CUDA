//
// Created by Rami on 2/4/2024.
//

#ifndef CUDA_RAY_TRACER_TRIANGLE_H
#define CUDA_RAY_TRACER_TRIANGLE_H

#include "Primitive.h"

// A Triangle class that includes the following ray/triangle intersection algorithms:
//          1. Möller–Trumbore ray-triangle intersection algorithm
//          2. // TODO: Badouel ray-triangle intersection algorithm
//          3. // TODO: Snyder & Barr ray-triangle intersection algorithm
class Triangle : public Primitive {
public:
    Triangle(const point3D &a, const point3D &b, const point3D &c, std::shared_ptr<Material> triangle_material)
    : a(a), b(b), c(c), triangle_material(triangle_material) {}

    /*
    bool intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const override {
        double t = dot_product((r_0 - r.get_ray_origin()), n)/dot_product(r.get_ray_direction(), n);
        // Check t in range
        if (t < t_0 || t_1 < t)
            return false;
        // t is in range, sweet, lets now check that the point is in the triangle
        point3D p = r.at(t);
        // First a check from v0
        Vec3D v0_v1 = b - a;
        Vec3D v0_v2 = c - a;
        Vec3D v0_p = p - a;
        double v0_angle = angle_between(v0_v1, v0_v2);
        double v0_p_angle = angle_between(v0_v1, v0_p);
        if (v0_p_angle > v0_angle)
            return false;
        // Check from v1
        Vec3D v1_v0 = a - b;
        Vec3D v1_v2 = c - b;
        Vec3D v1_p = p - b;
        double v1_angle = angle_between(v1_v2, v1_v0);
        double v1_p_angle = angle_between(v1_v2, v1_p);
        if (v1_p_angle > v1_angle)
            return false;
        // Check from v2
        Vec3D v2_v0 = a - c;
        Vec3D v2_v1 = b - c;
        Vec3D v2_p = p - c;
        double v2_angle = angle_between(v2_v0, v2_v1);
        double v2_p_angle = angle_between(v2_v0, v2_p);
        if (v2_p_angle > v2_angle)
            return false;
        // Passed this far must be in triangle
        // Record hit data
        intersection_info.t = t;
        intersection_info.p = p; // Where on the ray did the intersection occur
        intersection_info.set_face_normal(r, unit_vector(n)); // Set the normal
        intersection_info.mat_ptr = triangle_material; // Set material
        // Yes it did hit
        return true;
    }
*/

    bool intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const override {
        constexpr float epsilon = std::numeric_limits<float>::epsilon();

        Vec3D edge_1 = b - a;
        Vec3D edge_2 = c - a;
        Vec3D ray_cross_e2 = cross_product(r.get_ray_direction(), edge_2);
        double D = dot_product(edge_1, ray_cross_e2);

        // Front face culling
        if (D > -epsilon && D < epsilon)
            return false;  // parallel ray

        double inv_D = 1.0 / D;
        Vec3D s = r.get_ray_origin() - a;
        double u = inv_D * dot_product(s, ray_cross_e2);

        if (u < 0 || u > 1)
            return false;

        Vec3D s_cross_e1 = cross_product(s, edge_1);
        double v = inv_D * dot_product(r.get_ray_direction(), s_cross_e1);

        if (v < 0 || u + v > 1)
            return false;

        double t = inv_D * dot_product(edge_2, s_cross_e1);

        if (t > epsilon) {
            // Ray intersects
            intersection_info.t = t;
            intersection_info.p = r.at(t);
            Vec3D n = cross_product(edge_1, edge_2);

            intersection_info.set_face_normal(r, unit_vector(n));
            intersection_info.mat_ptr = triangle_material;

            return true;
        } else
            return false;
    }

    bool has_bounding_box(double time_0, double time_1, AABB &surrounding_AABB) const override {
        // Increase the size by a very small epsilon
        constexpr double epsilon = std::numeric_limits<double>::epsilon();

        // Find the minimum and maximum coordinates along each axis
        double min_x = fmin(fmin(a.x(), b.x()), c.x()) - epsilon;
        double max_x = fmax(fmax(a.x(), b.x()), c.x()) + epsilon;
        double min_y = fmin(fmin(a.y(), b.y()), c.y()) - epsilon;
        double max_y = fmax(fmax(a.y(), b.y()), c.y()) + epsilon;
        double min_z = fmin(fmin(a.z(), b.z()), c.z()) - epsilon;
        double max_z = fmax(fmax(a.z(), b.z()), c.z()) + epsilon;

        // Create the bounding box
        surrounding_AABB = AABB(point3D(min_x, min_y, min_z), point3D(max_x, max_y, max_z));

        return true;
    }

    double PDF_value(const point3D &o, const Vec3D &v) const override {
        Ray r(o, v);
        Intersection_Information intersection_info;
        if (!this->intersection(Ray(o, v), 0.001, infinity, intersection_info))
            return 0;

        double cost_theta_I = dot_product(-v, intersection_info.normal);
        if (cost_theta_I <= 0.0f)
            return 0;

        double dis_sq = (r.at(intersection_info.t) - o).length_squared();
        return dis_sq / (cost_theta_I * area());
    }

    Vec3D random(const Vec3D &o) const override {
        double u1 = random_double();
        double u2 = random_double();

        if (u1 + u2 > 1.0) {
            u1 = 1.0 - u1;
            u2 = 1.0 - u2;
        }

        Vec3D e1 = b - a;
        Vec3D e2 = c - a;

        Vec3D w = u1*e1 + u2*e2;

        return w + a - o;
    }
private:
    // Data Members
    // -----------------------------------------------------------------------
    point3D a,b,c;                                          // vertices of the triangle
    std::shared_ptr<Material> triangle_material;            // triangle's material
//    Vec3D normal;

    double area() const {
        // Calculate the area of the triangle (you may use a more accurate formula)
        Vec3D edge1 = b - a;
        Vec3D edge2 = c - a;
        return 0.5 * cross_product(edge1, edge2).length();
    }

};

// Functions to facilitate loading Meshes from OBJ Files
// -----------------------------------------------------------------------
void load_model(const std::string& file_name,std::vector<point3D>& vertices,
                std::vector<Triangle>& triangles, const std::shared_ptr<Material>& material){
    std::ifstream obj_file(file_name);

    if (!obj_file.is_open()) {
        std::cerr << "ERROR: UNABLE TO OPEN OBJ FILE " << file_name << std::endl;
        return;
    }

    std::string line;
    while (std::getline(obj_file, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "v") {
            // Vertex
            double x, y, z;
            iss >> x >> y >> z;
            vertices.emplace_back(x, y, z);
        //    std::cout << x << " " << y << " " << z << " " << std::endl;
        } else if (token == "f") {
            // Face
            int v1, v2, v3;
            iss >> v1 >> v2 >> v3;      // DANGER! Ensure obj file follows this for faces

            // Indices in OBJ files start from 1. In C++ they start from 0.
            v1--; v2--; v3--;

            // Pad vertices by epsilon
            point3D padded_v1 = vertices[v1] + Vec3D(epsilon, epsilon, epsilon);
            point3D padded_v2 = vertices[v2] + Vec3D(epsilon, epsilon, epsilon);
            point3D padded_v3 = vertices[v3] + Vec3D(epsilon, epsilon, epsilon);

         //   std::cout << padded_v1 << " " << vertices[v2] << " " << vertices[v3] << " " << std::endl;

            // Create the triangle using the vertices
            triangles.emplace_back(padded_v1, padded_v2, padded_v3, material);
        }
    }
    obj_file.close();
}

void load_model(const std::string& file_name, std::vector<point3D>& vertices,
                std::vector<Triangle>& triangles, std::vector<std::shared_ptr<Material>>& materials) {
    int material_ctr = 0;

    std::ifstream obj_file(file_name);

    if (!obj_file.is_open()) {
        std::cerr << "ERROR: UNABLE TO OPEN OBJ FILE " << file_name << std::endl;
        return;
    }

    std::string line;
    while (std::getline(obj_file, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "v") {
            // Vertex
            double x, y, z;
            iss >> x >> y >> z;
            vertices.emplace_back(x, y, z);
        } else if (token == "f") {
            // Face
            int v1, v2, v3;
            iss >> v1 >> v2 >> v3;

            // Indices in OBJ files start from 1. In C++ they start from 0.
            v1--; v2--; v3--;

            // Pad vertices by epsilon
            point3D padded_v1 = vertices[v1] + Vec3D(epsilon, epsilon, epsilon);
            point3D padded_v2 = vertices[v2] + Vec3D(epsilon, epsilon, epsilon);
            point3D padded_v3 = vertices[v3] + Vec3D(epsilon, epsilon, epsilon);

            // Retrieve the material for this face
            std::shared_ptr<Material> face_material = materials[material_ctr];
            material_ctr++;

            // Create the triangle using the vertices and assign the material
            triangles.emplace_back(padded_v1, padded_v2, padded_v3, face_material);
        }
    }
    obj_file.close();
}

#endif //CUDA_RAY_TRACER_TRIANGLE_H
