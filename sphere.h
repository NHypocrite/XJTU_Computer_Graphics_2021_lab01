#ifndef SPHERE_H
#define SPHERE_H

#include "hitable.h"
#include "material.h"

class sphere : public hitable { // sphere为hitable的子类
public:
  sphere(){};
  sphere(Vec3 cen, float r, Material *m) : center(cen), Radius(r), mat(m) {}
  virtual bool hit(const Ray &r, float tmin, float tmax, hit_record &rec) const;

public:
  Vec3 center;
  float Radius;
  Material *mat;
};

#endif // SPHERE_H