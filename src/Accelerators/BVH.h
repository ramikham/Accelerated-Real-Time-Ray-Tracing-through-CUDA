//
// Created by Rami on 10/17/2023.
//

#ifndef CUDA_RAY_TRACER_BVH_H
#define CUDA_RAY_TRACER_BVH_H

#include "../Utilities.h"
#include "../Primitives/Primitive.h"
#include "../Primitives/Primitives_Group.h"

// An enum to specify the split strategy when constructing the BVH
// -----------------------------------------------------------------------
enum SPLIT_STRATEGY {
    // Deterministic
    // -----------------------------------------------------------------------
    MIN_COORDINATE_SORT,            // sort by the object min coordinate along AXIS
    MAX_COORDINATE_SORT,            // sort by the object max coordinate along AXIS
    CENTROID_SORT,                  // sort by the object center along AXIS
    MIDPOINT_PARTITION,             // partition around the midpoint along AXIS
    MIN_VOLUMES_SUM_HEURISTIC,      // minimize the sum of the volumes of the bboxes of the two subtrees heuristic
    SURFACE_AREA_HEURISTIC,         // minimize the surface area heuristic
};

// Supporting Functions
// -----------------------------------------------------------------------
inline bool compare_AABBs(const std::shared_ptr<Primitive> a, const std::shared_ptr<Primitive> b, int axis, SPLIT_STRATEGY split_strategy) {
    // Compares two PRIMITIVES based on their bounding boxes along a specified axis.

    assert(axis >= 0 && axis <= 2);

    AABB box_a;            // AABB of object a
    AABB box_b;            // AABB of object b

    if (!a->has_bounding_box(0,0,box_a) || !b->has_bounding_box(0,0,box_b))
        std::cerr << "NO BOUNDING BOX IN BVH_NODE CONSTRUCTOR.\n";

    // Compare based on the splitting strategy
    if (split_strategy == MIN_COORDINATE_SORT)
        return box_a.get_min().V[axis] < box_b.get_min().V[axis];
    else if (split_strategy == MAX_COORDINATE_SORT) {
        // TODO: to be implemented
        std::cout << "ds" << std::endl;
    }

    else if (split_strategy == CENTROID_SORT)
        // TODO: to be implemented
        ;
    else if (split_strategy == MIDPOINT_PARTITION)
        // TODO: to be implemented
        ;
    else if (split_strategy == MIN_VOLUMES_SUM_HEURISTIC)
        // TODO: to be implemented
        ;
    else if (split_strategy == SURFACE_AREA_HEURISTIC)
        // TODO: to be implemented
        ;
}

bool box_x_compare(const std::shared_ptr<Primitive> a, const std::shared_ptr<Primitive> b, SPLIT_STRATEGY split_strategy){
    // Utilizes box_compare() to compare the primitives a and b along the x-axis.

    return compare_AABBs(a, b, 0, split_strategy);
}

bool box_y_compare(const std::shared_ptr<Primitive> a, const std::shared_ptr<Primitive> b, SPLIT_STRATEGY split_strategy){
    // Utilizes box_compare() to compare the primitives a and b along the y-axis.

    return compare_AABBs(a, b, 1, split_strategy);
}

bool box_z_compare(const std::shared_ptr<Primitive> a, const std::shared_ptr<Primitive> b, SPLIT_STRATEGY split_strategy){
    // Utilizes box_compare() to compare the Hittables a and b along the z-axis.

    return compare_AABBs(a, b, 2, split_strategy);
}

/// Reference: Fundamentals of Computer Graphics - Section 12.3.2: Hierarchical Bounding Boxes
class BVH : public Primitive {
public:
    // Constructors
    // -----------------------------------------------------------------------
    BVH();

    /// Reference: Fundamentals of Computer Graphics - Section 12.3.2: Hierarchical Bounding Boxes
    BVH(const Primitives_Group &list, SPLIT_STRATEGY split_strategy) :
            BVH(list.primitives_list, 0, list.primitives_list.size(), 0.0, 0.0, split_strategy) {}

    /// Reference: xxx
    BVH(const std::vector<std::shared_ptr<Primitive>> &src_objects,
        size_t start, size_t end, double time0, double time1, SPLIT_STRATEGY split_strategy) {
        // Builds a tree for a BVH.

        // A modifiable array of the source scene objects
        auto objects = src_objects;

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
            if (comparator(objects[start], objects[start+1], split_strategy)) {
                left = objects[start];
                right = objects[start+1];
            } else {
                left = objects[start+1];
                right = objects[start];
            }
        } else {
            // Sort objects using the respective comparator
            std::sort(objects.begin() + start, objects.begin() + end,
                      [comparator, split_strategy](const std::shared_ptr<Primitive>& a, const std::shared_ptr<Primitive>& b) {
                          return comparator(a, b, split_strategy);
                      });

            // Recursively construct a new BVH node for each half of the sorted range
            auto mid = start + N/2;
            left = std::make_shared<BVH>(objects, start, mid, time0, time1, split_strategy);
            right = std::make_shared<BVH>(objects, mid, end, time0, time1, split_strategy);
        }

        AABB box_left, box_right;

        if (  !left->has_bounding_box(time0, time1, box_left)
              || !right->has_bounding_box(time0, time1, box_right)
                )
            std::cerr << "No bounding box in bvh_node constructor.\n";

        BBOX = construct_surrounding_box(box_left, box_right);
    };

    // Overloaded Functions
    // -----------------------------------------------------------------------
    /// Reference: Fundamentals of Computer Graphics - Section 12.3.2: Hierarchical Bounding Boxes
    bool intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const override {
        // Test whether the box of the BVH node is intersected by the ray r

        if (BBOX.intersection(r, t_0, t_1)) {
            Intersection_Information l_rec, r_rec;

            bool left_hit = left->intersection(r, t_0, t_1, l_rec);
            bool right_hit = right->intersection(r, t_0, t_1, r_rec);

            if (left_hit and right_hit) {
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
    };
    bool has_bounding_box(double time_0, double time_1, AABB &surrounding_AABB) const override {
        surrounding_AABB = BBOX;
        return true;
    };

    // Supporting Functions
    // -----------------------------------------------------------------------
    void min_coord_sort_BVH();                      // TODO: TO BE IMPLEMENTED
    void max_coord_sort_BVH();                      // TODO: TO BE IMPLEMENTED
    void centroid_sort_BVH();                       // TODO: TO BE IMPLEMENTED
    void midpoint_partition_BVH();                  // TODO: TO BE IMPLEMENTED
    void min_volume_sum_heuristic_BVH();            // TODO: TO BE IMPLEMENTED
    void surface_area_heuristic_BVH();              // TODO: TO BE IMPLEMENTED
public:
    // Data Members
    // -----------------------------------------------------------------------
    std::shared_ptr<Primitive> left;        // left-child node
    std::shared_ptr<Primitive> right;       // right-child node
    AABB BBOX;         // bounding box
    SPLIT_STRATEGY split_strategy;          // split strategy
};
#endif //CUDA_RAY_TRACER_BVH_H