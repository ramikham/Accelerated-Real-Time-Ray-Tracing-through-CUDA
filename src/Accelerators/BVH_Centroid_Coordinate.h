//
// Created by Rami on 2/11/2024.
//

#ifndef CUDA_RAY_TRACER_BVH_CENTROID_COORDINATE_H
#define CUDA_RAY_TRACER_BVH_CENTROID_COORDINATE_H


#include "../Utilities.h"
#include "../Primitives/Primitive.h"
#include "../Primitives/Primitives_Group.h"

/// Reference: Fundamentals of Computer Graphics - Section 12.3.2: Hierarchical Bounding Boxes
class BVH_Centroid_Coordinate : public Primitive {
public:
    // Constructors
    // -----------------------------------------------------------------------
    BVH_Centroid_Coordinate();

    /// Reference: Fundamentals of Computer Graphics - Section 12.3.2: Hierarchical Bounding Boxes
    BVH_Centroid_Coordinate(const Primitives_Group &list) :
            BVH_Centroid_Coordinate(list.primitives_list, 0, list.primitives_list.size(), 0.0, 0.0, 0) {}

    /// Reference: Fundamentals of Computer Graphics - Section 12.3.2: Hierarchical Bounding Boxes
    BVH_Centroid_Coordinate(const std::vector<std::shared_ptr<Primitive>> &src_objects,
                       size_t start, size_t end, double time0, double time1, int axis_ctr) {
        // Strategy: Sort by Centroid Coordinate.
        // Axis Choice: Rotational.

        auto objects = src_objects;     // create a modifiable array of the source scene objects

        // int axis = random_int_in_range(0,2);       // choose the axis at random, or ...
         int axis = axis_ctr % 3;                          // keep rotating between the axes

        // Get the respective comparator function
        auto comparator = (axis == 0) ? box_x_compare_centroid_coord
                                      : (axis == 1) ? box_y_compare_centroid_coord
                                                    : box_z_compare_centroid_coord;

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
            auto m = start + N/2;

            // Sort objects using the respective comparator...
            // std::sort(objects.begin() + start, objects.begin() + end, comparator);

            // ... or use partial sort (usually faster and does the job)
            // std::partial_sort(objects.begin() + start, objects.begin() + m, objects.begin() + end, comparator);

            // ... or use nth_element (fastest!)
             std::nth_element(objects.begin() + start, objects.begin() + m, objects.begin() + end, comparator);

            // Recursively construct the left and right subtrees
            left = std::make_shared<BVH_Centroid_Coordinate>(objects, start, m, time0, time1, axis_ctr + 1);
            right = std::make_shared<BVH_Centroid_Coordinate>(objects, m, end, time0, time1, axis_ctr + 1);
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
    /// Reference: [1] https://graphics.stanford.edu/~boulos/papers/efficient_notes.pdf
    /// Reference: [2] Fundamentals of Computer Graphics - Section 12.3.2: Hierarchical Bounding Boxes
    bool intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const override {
        // Tests whether the box of the BVH node is intersected by the ray r.

        /* A faster intersection method [1]. I will stick to this rather than my implementation below. */
        if (!BBOX.intersection(r, t_0, t_1))
            return false;

        bool hit_left = left->intersection(r, t_0, t_1, intersection_info);
        bool hit_right = right->intersection(r, t_0, hit_left ? intersection_info.t : t_1, intersection_info);

        return hit_left || hit_right;

        /* A slower intersection method [2] */
        /*
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
         return false;*/
    };
    bool has_bounding_box(double time_0, double time_1, AABB &surrounding_AABB) const override {
        surrounding_AABB = BBOX;
        return true;
    };

public:
    // Data Members
    // -----------------------------------------------------------------------
    std::shared_ptr<Primitive> left;        // left-child node
    std::shared_ptr<Primitive> right;       // right-child node
    AABB BBOX;         // bounding box
};


#endif //CUDA_RAY_TRACER_BVH_CENTROID_COORDINATE_H
