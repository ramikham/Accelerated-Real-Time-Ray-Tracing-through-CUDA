//
// Created by Rami on 3/26/2024.
//

#ifndef CUDA_RAY_TRACER_BVH_PARALLEL_H
#define CUDA_RAY_TRACER_BVH_PARALLEL_H

#include "../Utilities.h"
#include "../Primitives/Primitive.h"
#include "../Primitives/Primitives_Group.h"

class BVH_Parallel: public Primitive {
public:
    // Constructors
    // -----------------------------------------------------------------------
    BVH_Parallel(const Primitives_Group &list) :
            BVH_Parallel(list.primitives_list, 0) {}

    BVH_Parallel(const std::vector<std::shared_ptr<Primitive>>& src_objects, int axis_ctr) {
        auto objects = src_objects;

        int axis = axis_ctr % 3;                          // keep rotating between the axes

        auto comparator = (axis == 0) ? box_x_compare_centroid_coord
                                      : (axis == 1) ? box_y_compare_centroid_coord
                                                    : box_z_compare_centroid_coord;

        size_t size = objects.size();

        if (size == 1) {
            left = right = objects[0];
        } else if (size == 2) {
            if (comparator(objects[0], objects[1])) {
                left = objects[0];
                right = objects[1];
            } else {
                left = objects[1];
                right = objects[0];
            }
        } else {
            auto m = size / 2;

            std::nth_element(objects.begin(), objects.begin() + m, objects.end(), comparator);

            // Don't send the full lists, just send partial copies of them
            auto first_half = std::vector<std::shared_ptr<Primitive>>(objects.begin(), objects.begin() + m);
            auto second_half = std::vector<std::shared_ptr<Primitive>>(objects.begin() + m, objects.end());

            // Invoke recursion
#pragma omp parallel
#pragma omp single nowait
            {
#pragma omp task
                left = std::make_shared<BVH_Parallel>(first_half, axis_ctr + 1);
#pragma omp task
                right = std::make_shared<BVH_Parallel>(second_half, axis_ctr + 1);
            };
        }
        AABB box_left, box_right;

        if (  !left->has_bounding_box(0.0, 0.0, box_left)
              || !right->has_bounding_box(0.0, 0.0, box_right)
                )
            std::cerr << "No bounding box in bvh_node constructor.\n";

        BBOX = construct_surrounding_box(box_left, box_right);
    }

    // Overridden Functions
    // -----------------------------------------------------------------------
    bool intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const override {
        /* A faster intersection method [1]. I will stick to this rather than my implementation below. */
        if (!BBOX.intersection(r, t_0, t_1))
            return false;

        bool hit_left = left->intersection(r, t_0, t_1, intersection_info);
        bool hit_right = right->intersection(r, t_0, hit_left ? intersection_info.t : t_1, intersection_info);

        return hit_left || hit_right;
    }

    bool has_bounding_box(double time_0, double time_1, AABB &surrounding_AABB) const override {
        surrounding_AABB = BBOX;
        return true;
    }
public:
    // Data Members
    // -----------------------------------------------------------------------
    std::shared_ptr<Primitive> left;        // left-child node
    std::shared_ptr<Primitive> right;       // right-child node
    AABB BBOX;                              // bounding box of the BVH node
};

#endif //CUDA_RAY_TRACER_BVH_PARALLEL_H