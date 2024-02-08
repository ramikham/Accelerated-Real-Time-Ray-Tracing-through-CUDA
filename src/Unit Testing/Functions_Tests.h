//
// Created by Rami on 2/8/2024.
//

#ifndef CUDA_RAY_TRACER_FUNCTIONS_TESTS_H
#define CUDA_RAY_TRACER_FUNCTIONS_TESTS_H

#include "../Utilities.h"
#include "../Primitives/Sphere.h"

bool correct_sphere_intersection(Vec3D center, double radius, const Ray& r, double t_0, double t_1) {
    // Get the A, B, C of the quadratic equation
    Vec3D OC = r.get_ray_origin() - center;
    auto A = r.get_ray_direction().length_squared();
    auto half_B = dot_product(OC, r.get_ray_direction());           // half_B is a shortcut
    auto C = OC.length_squared() - radius*radius;

    // Calculate the quadratic equation discriminant.
    auto discriminant = half_B * half_B - A * C;
    // auto discriminant = fma(half_B, half_B, -A * C);

    // If the discriminant is negative, the ray misses the sphere.
    if (discriminant < 0) return false;

    // Calculate the square root of the discriminant.
    double sqrt_discriminant = sqrt(discriminant);

    // Since t > 0 is part of the ray definition, we examine the two
    // roots. The smaller, positive real root is the one that is closest
    // to the intersection distance on the ray.
    double intersection_t = (-half_B - sqrt_discriminant) / A;       // first root
    if (intersection_t <= t_0 || t_1 <= intersection_t) {
        // first root not in range [t_0,t_1], so calculate
        // the second root.
        intersection_t = (-half_B + sqrt_discriminant) / A;

        // Check if second root is also not in the range [t_0,t_1].
        if (intersection_t <= t_0 || t_1 <= intersection_t)
            return false;
    }

    // We know the ray intersects the sphere, so we should update the
    // intersection information
    std::cout << "Correct Function - Intersection t: " << intersection_t << std::endl;
    std::cout << "Correct Function - Ray at t: " << r.at(intersection_t) << std::endl;

    return true;
}

bool new_sphere_intersection(Vec3D center, double radius, const Ray& r, double t_min, double t_max) {
    bool flag_outside_sphere = false;
    double R2 = radius*radius;
    Vec3D OC  = center - r.get_ray_origin();
    double L_2_OC = dot_product(OC, OC);
    flag_outside_sphere = false;
    if (L_2_OC >= R2) {
        // RAY ORIGINATED OURSIDE THE SPJERE
        flag_outside_sphere = true;
    }

    double t_ca = dot_product(OC, unit_vector(r.get_ray_direction()));

    if (t_ca < 0) {
        if (flag_outside_sphere)
            return false;
    }
    double t_2_hc = R2 - L_2_OC + (t_ca * t_ca);

    if (t_2_hc < 0) {
        return false;
    }

    std::cout << "1. " <<  (t_ca - sqrt(t_2_hc)) / r.get_ray_direction().length() << std::endl;
    std::cout << "2. " << (t_ca + sqrt(t_2_hc)) / r.get_ray_direction().length() << std::endl;

    double intersection_t;
    if (flag_outside_sphere)
        intersection_t =  (t_ca - sqrt(t_2_hc)) / r.get_ray_direction().length();
    else
        intersection_t = (t_ca + sqrt(t_2_hc)) / r.get_ray_direction().length();

    if (intersection_t <= t_min || t_max <= intersection_t) {
        return false;
    }
    std::cout << "New Function - Intersection at: " << intersection_t << std::endl;
    std::cout << "New Function - Ray at t: " << r.at(intersection_t) << std::endl;
    return true;

}

void test_sphere_intersection() {
    // Test case 1
    std::cout << "TEST 1: BEGIN" << std::endl;
    Ray r1(Vec3D(1, -2, -1), Vec3D(1, 2, 4));
    Sphere S1(Vec3D(3, 0, 5), 3, nullptr);
    correct_sphere_intersection(Vec3D(3, 0, 5), 3, r1, 0.001, infinity);
    new_sphere_intersection(Vec3D(3, 0, 5), 3, r1, 0.001, infinity);
    std::cout << "TEST 1: END\n" << std::endl;

    // Test case 2
    std::cout << "TEST 2: BEGIN" << std::endl;
    Ray r2(Vec3D(-3, 3, 5), Vec3D(1, 2, 4));
    Sphere S2(Vec3D(-3, 3, 5), 3, nullptr);
    correct_sphere_intersection(Vec3D(-3, 3, 5), 3, r2, 0.001, infinity);
    new_sphere_intersection(Vec3D(-3, 3, 5), 3, r2, 0.001, infinity);
    std::cout << "TEST 2: END\n" << std::endl;

    // Test case 3
    std::cout << "TEST 3: BEGIN" << std::endl;
    Ray r3(Vec3D(0.2, 0.5, 0.1), Vec3D(1, 2, 4));
    Sphere S3(Vec3D(0.2, 0.5, 0.1), 1, nullptr);
    correct_sphere_intersection(Vec3D(0.2, 0.5, 0.1), 1, r3, 0.001, infinity);
    new_sphere_intersection(Vec3D(0.2, 0.5, 0.1), 1, r3, 0.001, infinity);
    std::cout << "TEST 3: END\n" << std::endl;

    // Test case 4
    std::cout << "TEST 4: BEGIN" << std::endl;
    Ray r4(Vec3D(0.09, 0.03, 0.1), Vec3D(1, 2, 4));
    Sphere S4(Vec3D(0.2, 0.02, 0.1), 1, nullptr);
    correct_sphere_intersection(Vec3D(0.2, 0.5, 0.1), 1, r4, 0.001, infinity);
    new_sphere_intersection(Vec3D(0.2, 0.5, 0.1), 1, r4, 0.001, infinity);
    std::cout << "TEST 4: END\n" << std::endl;
}

bool correct_triangle_intersection(const Ray& r, point3D a, point3D b, point3D c, double t_0, double t_1) {
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
        Vec3D n = cross_product(edge_1, edge_2);
        std::cout << "Correct Intersection Time: t = " << t << std::endl;
        std::cout << "Correct Normal: N = " << n;
        std::cout << "Correct Intersection Point: p = " << r.at(t) << std::endl;
        return true;
    } else {
        std::cout << "NO::INTERSECTION" << std::endl;
        return false;
    }
}

bool new_triangle_intersection(const Ray& r, point3D a, point3D b, point3D c, double t_0, double t_1) {
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
    if (t < t_0 || t > t_1) {
        std::cout << "NO::INTERSECTION" << std::endl;
        return false;
    }

    // Compute GAMMA
    double GAMMA = (I * (AK - JB) + H * (JC - AL) + G * (BL - KC)) / M;

    // Check GAMMA's range
    if (GAMMA < 0 || GAMMA > 1) {
        std::cout << "NO::INTERSECTION" << std::endl;
        return false;
    }

    // Compute BETA
    double BETA = (J * (EI - HF) + K * (GF - DI) + L * (DH - EG)) / M;

    // Check BETA's range
    if (BETA < 0 || BETA > 1 - GAMMA) {
        std::cout << "NO::INTERSECTION" << std::endl;
        return false;
    }
    // An intersection must happen
    if (t > epsilon) {
        Vec3D n = cross_product(B_A, C_A);
        std::cout << "New Function Intersection Time: t = " << t << std::endl;
        std::cout << "New Function Normal: N = " << n;
        std::cout << "New Function Intersection Point: p = " << r.at(t) << std::endl;
    }
    return true;
}

void test_triangle_intersection() {
    // Test case 1
    std::cout << "TEST 1: BEGIN" << std::endl;
    Ray r1(Vec3D(0,0,-2), Vec3D(0,0,1));
    correct_triangle_intersection(r1, point3D(-1, -1, 0), point3D(1,-1,0), point3D(0,1,0), 0.001, infinity);
    new_triangle_intersection(r1, point3D(-1, -1, 0), point3D(1,-1,0), point3D(0,1,0), 0.001, infinity);
    std::cout << "TEST 1: END\n" << std::endl;

    // Test case 2
    std::cout << "TEST 2: BEGIN" << std::endl;
    Ray r2(Vec3D(-3, 3, 5), Vec3D(1, 2, 4));
    correct_triangle_intersection(r2, point3D(0, 0, -1), point3D(0,1,0), point3D(1,0,0), 0.001, infinity);
    new_triangle_intersection(r2, point3D(0, 0, -1), point3D(0,1,0), point3D(1,0,0), 0.001, infinity);
    std::cout << "TEST 2: END\n" << std::endl;

    // Test case 3
    std::cout << "TEST 3: BEGIN" << std::endl;
    Ray r3(Vec3D(0, -2, 0), Vec3D(0, 1, 0));
    correct_triangle_intersection(r3, point3D(-1,0,0), point3D(0,1,0), point3D(1,1,0), 0.001, infinity);
    new_triangle_intersection(r3, point3D(-1,0,0), point3D(0,1,0), point3D(1,1,0), 0.001, infinity);
    std::cout << "TEST 3: END\n" << std::endl;

    // Test case 4
    std::cout << "TEST 4: BEGIN" << std::endl;
    Ray r4(Vec3D(0.09, 0.03, 0.1), Vec3D(1, 2, 4));
    correct_triangle_intersection(r4, point3D(0,0,0), point3D(1,0,0), point3D(0,1,0), 0.001, infinity);
    new_triangle_intersection(r4, point3D(0,0,0), point3D(1,0,0), point3D(0,1,0), 0.001, infinity);
    std::cout << "TEST 4: END\n" << std::endl;
}


#endif //CUDA_RAY_TRACER_FUNCTIONS_TESTS_H
