//
// Created by Rami on 9/23/2023.
//

#ifndef CUDA_RAY_TRACER_PRIMITIVES_GROUP_H
#define CUDA_RAY_TRACER_PRIMITIVES_GROUP_H

#include "Primitive.h"

/// Reference: [2]
/// ----------------------------------------------------------------------
class Primitives_Group : public Primitive {
public:
    // Constructor
    // -------------------------------------------------------------------
    Primitives_Group() {}
    Primitives_Group(std::shared_ptr<Primitive> o) { add_primitive_to_list(o); };

    // Overloaded Function
    // -------------------------------------------------------------------
    bool intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const override;
    bool has_bounding_box(double time_0, double time_1, Axis_Aligned_Bounding_Box &surrounding_AABB) const override;

    // Helper Functions
    // -------------------------------------------------------------------
    void add_primitive_to_list(std::shared_ptr<Primitive> o) { primitives_list.push_back(o); }
    void empty_primitives_list() { primitives_list.clear(); }
public:
    std::vector<std::shared_ptr<Primitive>> primitives_list;        // primitives group container
};

bool Primitives_Group::intersection(const Ray &r, double t_0, double t_1,
                                    Intersection_Information &intersection_info) const {
    Intersection_Information current_info;
    bool intersection_flag = false;
    double t_min = t_1;

    for (auto & o : primitives_list) {
        if (o->intersection(r, t_0, t_min, current_info)) {
            intersection_flag = true;
            t_min = current_info.t;
            intersection_info = current_info;
        }
    }

    return intersection_flag;
}

bool
Primitives_Group::has_bounding_box(double time_0, double time_1, Axis_Aligned_Bounding_Box &surrounding_AABB) const {
    if (primitives_list.empty())
        return false;

    Axis_Aligned_Bounding_Box temp_box;
    bool first_box = true;

    for (const auto& obj : primitives_list){
        if (!obj->has_bounding_box(time_0, time_1, surrounding_AABB))
            return false;
        if (first_box)
            surrounding_AABB = temp_box;
        else
            surrounding_AABB = construct_surrounding_box(surrounding_AABB, temp_box);
        first_box = false;
    }

    return true;
}


#endif //CUDA_RAY_TRACER_PRIMITIVES_GROUP_H
