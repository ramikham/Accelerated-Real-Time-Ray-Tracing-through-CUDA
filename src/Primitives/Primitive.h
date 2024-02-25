//
// Created by Rami on 9/20/2023.
//

#ifndef CUDA_RAY_TRACER_PRIMITIVE_H
#define CUDA_RAY_TRACER_PRIMITIVE_H

#include "../Utilities.h"
#include "../Accelerators/AABB.h"

class Material;         // pre-definition of class "Material"

/// Reference: Fundamentals of Computer Graphics: Section 4.4.3 - Ray Intersection in Software
// ----------------------------------------------------------------------
struct Intersection_Information {
    // The ray/primitive intersection routine needs to return intersection
    // information at the point of intersection. We use a structure for that,
    // and we pass it around via the intersection() function.

    point3D p;                              // intersection point
    Vec3D normal;                           // surface normal at intersection
    double t;                               // intersection t
    bool front_face;                        // did the ray intersect the front face of the primitive?
    std::shared_ptr<Material> mat_ptr;      // pointer to the material of the primitive

    // Function to make the normal always point out against the ray.
    inline void set_face_normal(const Ray& r, const Vec3D& outward_normal) {
        if (dot_product(r.get_ray_direction(), outward_normal) > 0.0) {
            normal = -outward_normal;
            front_face = false;
        } else {
            normal = outward_normal;
            front_face = true;
        }
    }
};

// A parent class to all other primitives
// ----------------------------------------------------------------------
class Primitive {
public:
    virtual ~Primitive()=default;
    virtual bool intersection(const Ray& r, double t_0, double t_1, Intersection_Information& intersection_info) const = 0;
    virtual bool has_bounding_box(double time_0, double time_1, AABB& surrounding_AABB) const = 0;
    virtual double PDF_value(const point3D& o, const Vec3D& v) const { return 0.0; }
    virtual Vec3D random(const Vec3D& o) const { return Vec3D(1,0,0); }
};

// Supporting Functions for constructing the different BVH classes
// -----------------------------------------------------------------------

// Supporting Functions for the BVH Class
// -----------------------------------------------------------------------
inline bool compare_AABBs(const std::shared_ptr<Primitive> a, const std::shared_ptr<Primitive> b, int axis) {
    // Compares two primitives based on the minimum coordinates of their bounding boxes along a specified axis.

  //  assert(axis >= 0 && axis <= 2);

    AABB box_a;
    AABB box_b;

    if (!a->has_bounding_box(0,0, box_a) || !b->has_bounding_box(0,0, box_b)) {
        std::cerr << "NO BOUNDING BOX";
        exit(0);
    }

    // Compare by the minimum coordinate of the bounding boxes
    return box_a.get_min().V[axis] < box_b.get_min().V[axis];
}

bool box_x_compare(const std::shared_ptr<Primitive> a, const std::shared_ptr<Primitive> b){
    // Utilizes compare_AABBs() to compare the primitives a and b along the x-axis.

    return compare_AABBs(a, b, 0);
}

bool box_y_compare(const std::shared_ptr<Primitive> a, const std::shared_ptr<Primitive> b){
    // Utilizes compare_AABBs() to compare the primitives a and b along the y-axis.

    return compare_AABBs(a, b, 1);
}

bool box_z_compare(const std::shared_ptr<Primitive> a, const std::shared_ptr<Primitive> b){
    // Utilizes compare_AABBs() to compare the primitives a and b along the z-axis.

    return compare_AABBs(a, b, 2);
}

// Supporting Functions for the BVH_Max_Coordinate Class
// -----------------------------------------------------------------------
inline bool compare_AABBs_max_coord(const std::shared_ptr<Primitive> a, const std::shared_ptr<Primitive> b, int axis) {
    // Compares two primitives based on the maximum coordinates of their bounding boxes along a specified axis.

    assert(axis >= 0 && axis <= 2);

    AABB box_a;
    AABB box_b;

    if (!a->has_bounding_box(0,0, box_a) || !b->has_bounding_box(0,0, box_b)) {
        std::cerr << "NO BOUNDING BOX";
        exit(0);
    }

    // Sort by max coordinate
    return box_a.get_max().V[axis] < box_b.get_max().V[axis];
}

bool box_x_compare_max_coord(const std::shared_ptr<Primitive> a, const std::shared_ptr<Primitive> b){
    // Utilizes compare_AABBs_max_coord() to compare the primitives a and b along the x-axis.

    return compare_AABBs_max_coord(a, b, 0);
}

bool box_y_compare_max_coord(const std::shared_ptr<Primitive> a, const std::shared_ptr<Primitive> b){
    // Utilizes compare_AABBs_max_coord() to compare the primitives a and b along the y-axis.

    return compare_AABBs_max_coord(a, b, 1);
}

bool box_z_compare_max_coord(const std::shared_ptr<Primitive> a, const std::shared_ptr<Primitive> b){
    // Utilizes compare_AABBs_max_coord() to compare the Hittables a and b along the z-axis.

    return compare_AABBs_max_coord(a, b, 2);
}

// Supporting Functions for the BVH_Centroid_Coordinate Class
// -----------------------------------------------------------------------
inline bool compare_AABBs_centroid_coord(const std::shared_ptr<Primitive> a, const std::shared_ptr<Primitive> b, int axis) {
    // Compares two primitives based on the centroid coordinates of their bounding boxes along a specified axis.

    assert(axis >= 0 && axis <= 2);

    AABB box_a;            // AABB of object a
    AABB box_b;            // AABB of object b

    if (!a->has_bounding_box(0,0, box_a) || !b->has_bounding_box(0,0, box_b)) {
        std::cerr << "NO BOUNDING BOX";
        exit(0);
    }

    // Calculate centroids
    point3D centroid_a = 0.5 * (box_a.get_min() + box_a.get_max());
    point3D centroid_b = 0.5 * (box_b.get_min() + box_b.get_max());

    // Compare centroids
    return centroid_a.V[axis] < centroid_b.V[axis];
}

bool box_x_compare_centroid_coord(const std::shared_ptr<Primitive> a, const std::shared_ptr<Primitive> b){
    // Utilizes compare_AABBs_centroid_coord() to compare the primitives a and b along the x-axis.

    return compare_AABBs_centroid_coord(a, b, 0);
}

bool box_y_compare_centroid_coord(const std::shared_ptr<Primitive> a, const std::shared_ptr<Primitive> b){
    // Utilizes compare_AABBs_centroid_coord() to compare the primitives a and b along the y-axis.

    return compare_AABBs_centroid_coord(a, b, 1);
}

bool box_z_compare_centroid_coord(const std::shared_ptr<Primitive> a, const std::shared_ptr<Primitive> b){
    // Utilizes compare_AABBs_centroid_coord() to compare the primitives a and b along the z-axis.

    return compare_AABBs_centroid_coord(a, b, 2);
}

#endif //CUDA_RAY_TRACER_PRIMITIVE_H
