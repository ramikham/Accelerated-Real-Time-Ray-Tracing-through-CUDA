//
// Created by Rami on 9/20/2023.
//

#ifndef CUDA_RAY_TRACER_PRIMITIVE_H
#define CUDA_RAY_TRACER_PRIMITIVE_H

#include "../Utilities.h"

class Material;         // predef
/// The ray/primitive intersection routine needs to return intersection
/// information at the point of intersection. We use a structure for that,
/// and we pass it around via the intersection() function.
/// ----------------------------------------------------------------------
struct Intersection_Information {
    point3D p;              // intersection point
    Vec3D normal;           // surface normal at intersection
    double t;               // intersection t
    bool front_face;        // did the ray intersect the front face of the primitive?
    std::shared_ptr<Material> mat_ptr;      // pointer to the material of the primitive
    Material_Type material_type;
    Color surface_color;

    /// Function to make the normal always point out against the ray.
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

/// A parent class to all other primitives.
/// ----------------------------------------------------------------------
class Primitive {
public:
    virtual bool intersection(const Ray& r, double t_0, double t_1, Intersection_Information& intersection_info) const = 0;
    virtual ~Primitive()=default;
};

#endif //CUDA_RAY_TRACER_PRIMITIVE_H
