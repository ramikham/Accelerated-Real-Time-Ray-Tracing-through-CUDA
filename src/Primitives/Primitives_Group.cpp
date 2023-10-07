//
// Created by Rami on 9/23/2023.
//

#include "Primitives_Group.h"

Primitives_Group::Primitives_Group(std::shared_ptr<Primitive> o) {
    primitives_list.push_back(o);
}

void Primitives_Group::add_primitive_to_list(std::shared_ptr<Primitive> o) {
    primitives_list.push_back(o);
}

void Primitives_Group::empty_primitives_list() {
    primitives_list.clear();
}

bool Primitives_Group::intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const {
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
