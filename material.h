#pragma once

#include "hitable.h"
#include "ray.h"
#include "vec3.h"
#include <float.h>
#include <random>

// struct hit_record;

enum MaterialType { Diffuse, Metal, Dielectric };

class Material {
public:
  // diffuse material constructor
  Material(Vec3 albedo, MaterialType type);
  // metal material constructor
  Material(Vec3 albedo, float fuzz, MaterialType type);
  // dielectric material construtor
  Material(float ior,MaterialType type);
  ~Material();

  bool scatter(const Ray &r, hit_record &rec, Ray &scattered, Vec3 &attenu);

  MaterialType type;
  Vec3 albedo;
  // 取值范围[0,1].控制金属光泽,值越小,越接近全反射,值越大,越接近漫反射
  float fuzz;
  float ior; // 折射率
};
