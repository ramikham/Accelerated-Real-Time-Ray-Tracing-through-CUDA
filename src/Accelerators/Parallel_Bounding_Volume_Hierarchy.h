//
// Created by Rami on 1/23/2024.
//

#ifndef CUDA_RAY_TRACER_PARALLEL_BOUNDING_VOLUME_HIERARCHY_H
#define CUDA_RAY_TRACER_PARALLEL_BOUNDING_VOLUME_HIERARCHY_H

#include "../Utilities.h"
#include "../Primitives/Primitive.h"
#include "../Primitives/Primitives_Group.h"
/*

/// Reference: Fundamentals of Computer Graphics - Section 12.3.2
class Parallel_Bounding_Volume_Hierarchy : public Primitive {
public:
    // Constructors
    // -----------------------------------------------------------------------
    Parallel_Bounding_Volume_Hierarchy();
    Parallel_Bounding_Volume_Hierarchy(const Primitives_Group &list) :
            Parallel_Bounding_Volume_Hierarchy(list.primitives_list, 0, list.primitives_list.size(), 0.0, 0.0) {}
    Parallel_Bounding_Volume_Hierarchy(const std::vector<std::shared_ptr<Primitive>> &src_objects,
                              size_t start, size_t end, double time0, double time1);

    // Overloaded Functions
    // -----------------------------------------------------------------------
    bool intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const override;
    bool has_bounding_box(double time_0, double time_1, Axis_Aligned_Bounding_Box &surrounding_AABB) const override;

public:
    // Constructors
    // -----------------------------------------------------------------------
    std::shared_ptr<Primitive> left;        // left-child node
    std::shared_ptr<Primitive> right;       // right-child node
    Axis_Aligned_Bounding_Box BBOX;         // bounding box
};

/// This is a straightforward of the pseudocode of Section 12.3.2
bool Parallel_Bounding_Volume_Hierarchy::intersection(const Ray &r, double t_0, double t_1,
                                             Intersection_Information &intersection_info) const {
    /// Test whether the box of the BVH node is intersected by the ray r
    if (BBOX.intersection(r, t_0, t_1)) {
        Intersection_Information l_rec, r_rec;

        bool left_hit = left->intersection(r, t_0, t_1, l_rec);
        bool right_hit = right->intersection(r, t_0, t_1, r_rec);

        if (left_hit && right_hit) {
            if (l_rec.t < r_rec.t)
                intersection_info = l_rec;
            else
                intersection_info = r_rec;
            return true;
        } else if (left_hit) {
            intersection_info = l_rec;
            return true;
        } else if (right_hit) {
            intersection_info = r_rec;
            return true;
        } else
            return false;
    } else
        return false;
}

bool Parallel_Bounding_Volume_Hierarchy::has_bounding_box(double time_0, double time_1,
                                                 Axis_Aligned_Bounding_Box &surrounding_AABB) const {
    surrounding_AABB = BBOX;
    return true;
}

Parallel_Bounding_Volume_Hierarchy::Parallel_Bounding_Volume_Hierarchy(const std::vector<std::shared_ptr<Primitive>> &src_objects,
                                                     size_t start, size_t end, double time0, double time1) {

    /// Build a tree for a BV hierarchy. The tree shall be binary,
    /// roughly balanced, and have the boxes of sibling subtrees
    /// overlap as little as possible

    omp_set_num_threads(16);
    auto objects = src_objects;     // create a modifiable array of the source scene objects

    // Generate an axis at random
    int axis = random_int_in_range(0,2);

    // Get the respective comparator function
    auto comparator = (axis == 0) ? box_x_compare
                                  : (axis == 1) ? box_y_compare
                                                : box_z_compare;

    size_t N = end - start;         // length of the list

    // Build the tree
    if (N == 1) {
        // Set both the left and right pointers of the current node to the single object
        left = right = objects[start];
    } else if (N == 2) {
        // Compare the two objects using the axis comparator
        if (comparator(objects[start], objects[start+1])) {
            left = objects[start];
            right = objects[start+1];
        } else {
            left = objects[start+1];
            right = objects[start];
        }
    } else {
        // Sort objects using the respective comparator
        std::sort(objects.begin() + start, objects.begin() + end, comparator);

        // Recursively construct a new BVH node for each half of the sorted range
        auto mid = start + N / 2;
        //   left = std::make_shared<BVH>(objects, start, mid, time0, time1);
        //  right = std::make_shared<BVH>(objects, mid, end, time0, time1);
        /*
#pragma omp task
        {
          //  std::cout << "Accessed by = " << omp_get_thread_num() << std::endl;
            left = std::make_shared<Parallel_Bounding_Volume_Hierarchy>(objects, start, mid, time0, time1);
        };

#pragma omp task
        right = std::make_shared<Parallel_Bounding_Volume_Hierarchy>(objects, mid, end, time0, time1);
    }
*/

/*
#pragma omp parallel
#pragma omp single nowait
        {
#pragma omp task
            left = std::make_shared<Parallel_Bounding_Volume_Hierarchy>(objects, start, mid, time0, time1);
#pragma omp task
            right = std::make_shared<Parallel_Bounding_Volume_Hierarchy>(objects, mid, end, time0, time1);
        }
    }

    Axis_Aligned_Bounding_Box box_left, box_right;

    if (  !left->has_bounding_box(time0, time1, box_left)
          || !right->has_bounding_box(time0, time1, box_right)
            )
        std::cerr << "No bounding box in bvh_node constructor.\n";

    BBOX = construct_surrounding_box(box_left, box_right);
}
*/
#endif //CUDA_RAY_TRACER_PARALLEL_BOUNDING_VOLUME_HIERARCHY_H
