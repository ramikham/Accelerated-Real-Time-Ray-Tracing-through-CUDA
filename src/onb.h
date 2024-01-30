//
// Created by Rami on 1/28/2024.
//

#ifndef CUDA_RAY_TRACER_ONB_H
#define CUDA_RAY_TRACER_ONB_H

#include "Utilities.h"

class onb {
public:
    onb() {}

    Vec3D operator[](int i) const { return axis[i]; }
    Vec3D& operator[](int i) { return axis[i]; }

    Vec3D u() const { return axis[0]; }
    Vec3D v() const { return axis[1]; }
    Vec3D w() const { return axis[2]; }

    Vec3D local(double a, double b, double c) const {
        return a*u() + b*v() + c*w();
    }

    Vec3D local(const Vec3D& a) const {
        std::cout << "INSIDE HIS FUNCTION: " << u() << " , " << v() << " , " << w() << std::endl;
        return a.x()*u() + a.y()*v() + a.z()*w();
    }

    void build_from_w(const Vec3D& w) {
       // std::cout << w;
        Vec3D unit_w = unit_vector(w);
        Vec3D a = (fabs(unit_w.x()) > 0.9) ? Vec3D(0,1,0) : Vec3D(1,0,0);
        Vec3D v = unit_vector(cross_product(unit_w, a));
        Vec3D u = cross_product(unit_w, v);
        axis[0] = u;
        axis[1] = v;
        axis[2] = unit_w;
    }

public:
    Vec3D axis[3];
};

#endif //CUDA_RAY_TRACER_ONB_H
