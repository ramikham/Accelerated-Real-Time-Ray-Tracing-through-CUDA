//
// Created by Rami on 3/27/2024.
//

#ifndef CUDA_RAY_TRACER_TEXTURE_H
#define CUDA_RAY_TRACER_TEXTURE_H

#include "../Utilities.h"

class Texture {
public:
    virtual ~Texture() = default;
    virtual Color value_at(double u, double v, const point3D& p) const = 0;
};

class Constant_Color : public Texture {
public:
    // Constructor
    // -----------------------------------------------------------------------
    Constant_Color(Color color) : constant_color(color) {}

    Constant_Color(double r, double g, double b) : constant_color(Color(r, g, b)) {}

    // Overloaded Function
    // -----------------------------------------------------------------------
    Color value_at(double u, double v, const point3D &p) const override {
        // A constant color texture just returns the color itself.
        // So if the user inputs a red color, the texture is just a
        // constant red color.

        return constant_color;
    }

private:
    // Data Members
    // -----------------------------------------------------------------------
    Color constant_color;
};

/// Reference:                     - Fundamentals of Computer Graphics - Section 11.1.1
class Stripe_Texture : public Texture {
public:
    // Constructor
    // -----------------------------------------------------------------------
    Stripe_Texture(const std::shared_ptr<Texture>& first_color, const std::shared_ptr<Texture>& second_color) : c0(first_color), c1(second_color) {}

    // Overloaded Function
    // -----------------------------------------------------------------------
    Color value_at(double u, double v, const point3D &p) const override {
        // Given two colors c0,c1, we can make a stripe color out of them using
        // an oscillating function to switch between the two colors.
        if (sin(p.x()) > 0) {
            //std::cout << "here c0" << std::endl;
            return c0->value_at(u, v, p);
        }
        else {
            //std::cout << "Here c1" << std::endl;
            return c1->value_at(u, v, p);
        }
    }

private:
    // Data Members
    // -----------------------------------------------------------------------
    std::shared_ptr<Texture> c0;        // first color
    std::shared_ptr<Texture> c1;        // second color
};
#endif //CUDA_RAY_TRACER_TEXTURE_H
