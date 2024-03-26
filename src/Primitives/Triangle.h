//
// Created by Rami on 2/4/2024.
//

#ifndef CUDA_RAY_TRACER_TRIANGLE_H
#define CUDA_RAY_TRACER_TRIANGLE_H

#include "Primitive.h"
static int num_calls_triangle_intersection = 0;
// A Triangle class that includes the following ray/triangle intersection algorithms:
//          1. Möller–Trumbore ray-triangle intersection algorithm
//          2. // TODO: Badouel ray-triangle intersection algorithm
//          3. Snyder & Barr ray-triangle intersection algorithm
class Triangle : public Primitive {
public:
    // Constructor
    // -----------------------------------------------------------------------
    Triangle(const point3D &a, const point3D &b, const point3D &c, std::shared_ptr<Material> triangle_material)
    : a(a), b(b), c(c), triangle_material(triangle_material) {}

    // Overloaded Function
    // -------------------------------------------------------------------
    bool intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const override {
        // NOTE: When measuring runtime, don't call this function; instead, paste the intersection code here.

        // return Snyder_Barr_ray_triangle_intersection(r, t_0, t_1, intersection_info);
        return Moller_Trumbore_ray_triangle_intersection(r, t_0, t_1, intersection_info);
    }

    /// Reference: Ray Tracing Complex Models Containing Surface Tessellations
    bool Snyder_Barr_ray_triangle_intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const {
        // Snyder & Barr ray-triangle intersection algorithm

        // Get Ray details
        // ----
        Vec3D e = r.get_ray_origin();
        Vec3D d = r.get_ray_direction();

        // Get Triangle's details
        Vec3D B_A = b - a;          // (b - a), an edge
        Vec3D C_A = c - a;           // (c - a), an edge

        // Set variable's for Cramer's Rule
        double A = a.x() - b.x();
        double D = a.x() - c.x();
        double G = d.x();

        double B = a.y() - b.y();
        double E = a.y() - c.y();
        double H  = d.y();

        double C = a.z() - b.z();
        double F = a.z() - c.z();
        double I = d.z();

        // Compute M
        double EI = E * I;
        double HF = H * F;

        double GF = G * F;
        double DI = D * I;

        double DH = D * H;
        double EG = E * G;

        double M = A * (EI - HF) + B * (GF - DI) + C * (DH - EG);

        // Compute t
        double J = a.x() - e.x();
        double K = a.y() - e.y();
        double L = a.z() - e.z();

        double AK = A * K;
        double JB = J * B;

        double JC = J * C;
        double AL = A * L;

        double BL = B * L;
        double KC = K * C;

        double t = -(F * (AK - JB) + E * (JC - AL) + D * (BL - KC)) / M;

        // Check for visibility in [t_0,t_1]
        if (t < t_0 || t > t_1)
            return false;

        // Compute GAMMA
        double GAMMA = (I * (AK - JB) + H * (JC - AL) + G * (BL - KC)) / M;

        // Check GAMMA's range
        if (GAMMA < 0 || GAMMA > 1)
            return false;

        // Compute BETA
        double BETA = (J * (EI - HF) + K * (GF - DI) + L * (DH - EG)) / M;

        // Check BETA's range
        if (BETA < 0 || BETA > 1 - GAMMA)
            return false;

        // An intersection must happen, so update all intersection information
        intersection_info.t = t;
        intersection_info.p = r.at(t);
        Vec3D n = cross_product(B_A, C_A);

        intersection_info.set_face_normal(r, unit_vector(n));
        intersection_info.mat_ptr = triangle_material;

        return true;
    }

    /// Reference: Fast, Minimum Storage Ray/Triangle Intersection
    bool Moller_Trumbore_ray_triangle_intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const {
        // Möller–Trumbore ray-triangle intersection algorithm
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

        // NOTE: newly added -> Check for visibility in [t_0,t_1]
        if (t < t_0 || t > t_1)
            return false;

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
        // Does the triangle have a bounding box?

        Vec3D EPS(epsilon, epsilon, epsilon);
        surrounding_AABB =  AABB(
                min(a, min(b, c)) - EPS,
                max(a, max(b, c)) + EPS
        );

        return true;
    }

    double PDF_value(const point3D &o, const Vec3D &v) const override {
        // Calculate the PDF value, the likelihood of sampling a random direction on the triangle

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

    /// Reference: https://blogs.sas.com/content/iml/2020/10/19/random-points-in-triangle.html
    Vec3D random(const Vec3D &o) const override {
        // Generates a random direction within the triangle based on importance sampling

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

public:
    // Data Members
    // -----------------------------------------------------------------------
    point3D a,b,c;                                          // vertices of the triangle
    std::shared_ptr<Material> triangle_material;            // triangle's material

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
    // TODO: DEPRECATED !!
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

            // NEW
            point3D(190,90,190);

            x += -0.3;
            y += 0.1;
           // z += 190;

            // Apply scaling
            x *= 25.0;
            y *= 25.0;
            z *= 25.0;

            double theta =-59.5;
            double tempX = x;
            double tempZ = z;

            x = std::cos(theta) * tempX - std::sin(theta) * tempZ;
            z = std::sin(theta) * tempX + std::cos(theta) * tempZ;


            vertices.emplace_back(x, y, z);
        //    std::cout << x << " " << y << " " << z << " " << std::endl;
        } else if (token == "f") {
            // Face
            int v1, v2, v3;
            iss >> v1 >> v2 >> v3;      // DANGER! Ensure obj file follows this for faces

            // Indices in OBJ files start from 1. In C++ they start from 0.
            v1--; v2--; v3--;

            // Pad vertices by epsilon
            point3D padded_v1 = vertices[v1];
            point3D padded_v2 = vertices[v2];
            point3D padded_v3 = vertices[v3];

         //   std::cout << padded_v1 << " " << vertices[v2] << " " << vertices[v3] << " " << std::endl;

            // Create the triangle using the vertices
            triangles.emplace_back(padded_v1, padded_v2, padded_v3, material);
        }
    }
    obj_file.close();
}

void load_model(const std::string& file_name, std::vector<point3D>& vertices,
                std::vector<Triangle>& triangles, std::vector<std::shared_ptr<Material>>& materials) {
    // TODO: ADD TRANSFORMATIONS

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

void load_model(const std::string& file_name,std::vector<point3D>& vertices,
                std::vector<Triangle>& triangles, const std::shared_ptr<Material>& material,
                Vec3D& displacement, double& scale_factor, double& angle_of_rotation){
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

            // Apply scaling
            x *= scale_factor;
            y *= scale_factor;
            z *= scale_factor;

            double theta = angle_of_rotation;  // - 59.5
            double tempX = x;
            double tempZ = z;

            // Z-angle rotation
            x = std::cos(theta) * tempX - std::sin(theta) * tempZ;
            z = std::sin(theta) * tempX + std::cos(theta) * tempZ;


            x += displacement.x();
            y += displacement.y();
            z += displacement.z();

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

void load_model(const std::string& file_name,std::vector<point3D>& vertices,
                std::vector<Triangle>& triangles, const std::shared_ptr<Material>& material,
                Vec3D& displacement, double& scale_factor, double& X_angle_of_rotation, double& Y_angle_of_rotation, double& Z_angle_of_rotation){
    Y_angle_of_rotation = degrees_to_radians(Y_angle_of_rotation);
    Z_angle_of_rotation = degrees_to_radians(Z_angle_of_rotation);
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

            // Apply scaling
            // -------------------------------------------------------------------------------
            x *= scale_factor;
            y *= scale_factor;
            z *= scale_factor;

            // Apply rotation
            // -------------------------------------------------------------------------------

            // Z-axis rotation
            double theta_Z = Z_angle_of_rotation;
            double tempX = x;
            double tempY = y;

            x = std::cos(theta_Z) * tempX - std::sin(theta_Z) * tempY;
            y = std::sin(theta_Z) * tempX + std::cos(theta_Z) * tempY;

            // Y-axis rotation
            double theta_Y = Y_angle_of_rotation;  // - 59.5
            tempX = x;
            double tempZ = z;

            x = std::cos(theta_Y) * tempX + std::sin(theta_Y) * tempZ;
            z = -std::sin(theta_Y) * tempX + std::cos(theta_Y) * tempZ;

            // X-axis rotation
            double theta_X = X_angle_of_rotation;
            tempY = y;
            tempZ = z;

            y = std::cos(theta_X) * tempY - std::sin(theta_X) * tempZ;
            z = std::sin(theta_X) * tempX + std::cos(theta_X) * tempZ;



            // Apply Translation
            // -------------------------------------------------------------------------------
            x += displacement.x();
            y += displacement.y();
            z += displacement.z();

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



#endif //CUDA_RAY_TRACER_TRIANGLE_H
