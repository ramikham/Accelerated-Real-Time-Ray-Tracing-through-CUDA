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
    Primitives_Group(std::shared_ptr<Primitive> o);

    // Overloaded Function
    // -------------------------------------------------------------------
    bool intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const override;

    // Helper Functions
    // -------------------------------------------------------------------
    void add_primitive_to_list(std::shared_ptr<Primitive> o);
    void empty_primitives_list();
public:
    std::vector<std::shared_ptr<Primitive>> primitives_list;        // primitives group container
};


#endif //CUDA_RAY_TRACER_PRIMITIVES_GROUP_H
