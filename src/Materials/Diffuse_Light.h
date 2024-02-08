//
// Created by Rami on 1/28/2024.
//

#ifndef CUDA_RAY_TRACER_DIFFUSE_LIGHT_H
#define CUDA_RAY_TRACER_DIFFUSE_LIGHT_H

#include "Material.h"

class Diffuse_Light : public Material {
public:
    Diffuse_Light(Color light_color) : light_color(light_color) {}

    Diffuse_Light(Color& lightColor, double& area, double& x_min, double& x_max,
                  double& y_min, double& y_max, double& z_min, double& z_max) :
                  light_color(light_color), area(area), x_min(x_min), x_max(x_max),
                  y_min(y_min), y_max(y_max), z_min(z_min), z_max(z_max) {}


    Diffuse_Light(Color light_color, double area, double x_min, double x_max, double y_min, double y_max, double z_min, double z_max) {
        // I think I can improve this.

        this->light_color = light_color;
        this->area = area;
        this->x_min = x_min;
        this->x_max = x_max;
        this->y_min = y_min;
        this->y_max = y_max;
        this->z_min = z_min;
        this->z_max = z_max;
    }

    bool illumination(const Ray &incident_ray, const Intersection_Information &intersection_info, Color &shading_color,
                      Ray &scattered_ray, MATERIAL_TYPE &type, double &pdf, std::shared_ptr<PDF>& surface_pdf_ptr) const override {
        return false;
    }

    Color emitted(const point3D &p, const Intersection_Information& intersection_information) override {
        if (!intersection_information.front_face)
            return Color(0,0,0);
        return light_color;
    }

    // Getters
    // -----------------------------------------------------------------------
    double get_area() const {
        // Get the area of the light

        return area;
    }

    // Supporting Functions
    // -----------------------------------------------------------------------
    point3D sample_position_XZ_Rectangle() const {
        // Importance Sampling: Get random (x,y,z) on an XZ_Rectangle

        double x = random_double(x_min, x_max);
        double y = random_double(y_min, y_max);
        double z = random_double(z_min, z_max);

        return point3D(x, y, z);
    }
private:
    // Supporting Functions
    // -----------------------------------------------------------------------

    Color light_color;                                  // color of the light
    double area;                                        // area of light; needed for importance sampling
    double x_min, x_max, y_min, y_max, z_min, z_max;    // coordinates of the primitive representing the light
};

#endif //CUDA_RAY_TRACER_DIFFUSE_LIGHT_H
