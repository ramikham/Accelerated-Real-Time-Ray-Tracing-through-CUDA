//
// Created by Rami on 2/3/2024.
//

#ifndef CUDA_RAY_TRACER_BVH_MIDPOINT_PARTITION_H
#define CUDA_RAY_TRACER_BVH_MIDPOINT_PARTITION_H

#include "../Utilities.h"
#include "../Primitives/Primitive.h"
#include "../Primitives/Primitives_Group.h"

/// Reference: Fundamentals of Computer Graphics - Section 12.3.2: Hierarchical Bounding Boxes
class BVH_Midpoint_Partition : public Primitive {
public:
    // Constructors
    // -----------------------------------------------------------------------
    BVH_Midpoint_Partition();

    /// Reference: Fundamentals of Computer Graphics - Section 12.3.2: Hierarchical Bounding Boxes
    BVH_Midpoint_Partition (const Primitives_Group &list) :
            BVH_Midpoint_Partition(list,  0.0, 0.0, 0) {}

    const int MAX_RECURSION_DEPTH = 2000;  // Set an appropriate limit

    BVH_Midpoint_Partition(const Primitives_Group& A, double time0, double time1, int AXIS, int depth = 0) {
        // Generate an axis at random
        AXIS = random_int_in_range(0,2);
        if (depth > MAX_RECURSION_DEPTH || A.primitives_list.empty()) {
            // If recursion depth limit is reached or no primitives, construct a leaf node
            left = right = nullptr;
            return;
        }

        auto N = A.primitives_list.size();

        if (N == 1) {
            left = right = A.primitives_list[0];
        } else if (N == 2){
            left = A.primitives_list[0];
            right = A.primitives_list[1];
        } else {
            auto m = find_midpoint_of_A(A, time0, time1, AXIS);
            auto pair = partition_around_midpoint(A.primitives_list, m, AXIS);

            // Recursive calls with increased depth
            left = std::make_shared<BVH_Midpoint_Partition>(pair.first, time0, time1, AXIS, depth + 1);
            right = std::make_shared<BVH_Midpoint_Partition>(pair.second, time0, time1, AXIS, depth + 1);
        }

        AABB box_left, box_right;
        if (left && right && left->has_bounding_box(time0, time1, box_left) && right->has_bounding_box(time0, time1, box_right)) {
            BBOX = construct_surrounding_box(box_left, box_right);
        }
    }

    // Overloaded Functions
    // -----------------------------------------------------------------------
    /// Reference: Fundamentals of Computer Graphics - Section 12.3.2: Hierarchical Bounding Boxes
    bool intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const override {
        // Test whether the box of the BVH node is intersected by the ray r
        if (BBOX.intersection(r, t_0, t_1)) {
            Intersection_Information l_rec, r_rec;

            bool left_hit = (left != nullptr) && left->intersection(r, t_0, t_1, l_rec);
            bool right_hit = (right != nullptr) && right->intersection(r, t_0, t_1, r_rec);

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

public:

    point3D find_midpoint_of_A(const Primitives_Group& A, double time0, double time1, int AXIS) const {
        if (A.primitives_list.empty())
            return point3D(0.0,0.0,0.0);

        AABB A_bbox;
        A.has_bounding_box(time0, time1, A_bbox);

        // Calculate hte midpoint along AXIS
        double midpoint_value = 0.5 * (A_bbox.get_min()[AXIS] + A_bbox.get_max()[AXIS]);

        point3D midpoint;
        midpoint[AXIS] = midpoint_value;
      //  std::cout << midpoint << std::endl;
        return midpoint;
    }

    std::pair<Primitives_Group, Primitives_Group>
    partition_around_midpoint(const std::vector<std::shared_ptr<Primitive>> &A, const point3D& m, int AXIS) {
        Primitives_Group left_group;
        Primitives_Group right_group;

        for (const auto& obj : A) {
            AABB obj_box;
            obj->has_bounding_box(0.0, 0.0, obj_box);
            point3D centroid = 0.5 * (obj_box.get_min() + obj_box.get_max());

            if (centroid[AXIS] < m[AXIS]) {
                left_group.add_primitive_to_list(obj);
            } else {
                right_group.add_primitive_to_list(obj);
            }
        }

        return std::make_pair(left_group, right_group);
    }
    // Data Members
    // -----------------------------------------------------------------------
    std::shared_ptr<Primitive> left;        // left-child node
    std::shared_ptr<Primitive> right;       // right-child node
    AABB BBOX;                              // bounding box
};


#endif //CUDA_RAY_TRACER_BVH_MIDPOINT_PARTITION_H
