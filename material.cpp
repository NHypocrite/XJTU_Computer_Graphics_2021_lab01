#include "material.h"
// 随机浮点数生成
float get_random_float(float min, float max) {
  static std::random_device dev;
  static std::mt19937 rng(dev());
  static std::uniform_real_distribution<float> dist(
      min, max); // distribution in range [min, max]
  return dist(rng);
}
// 三维随机矩阵生成
Vec3 get_random_Vec3(float min, float max){
  return Vec3(get_random_float(min,max),get_random_float(min,max),get_random_float(min,max));
}

Vec3 random_in_unit_sphere() {
// 随机在单位球上采样光线
  while (true) {
    auto p = get_random_Vec3(-1,1);
    if (p.squared_length() >= 1) continue;
    return p;  
  }
}

Vec3 random_unit_vector() {
// 归一化的采样
    return unit_vector(random_in_unit_sphere());
}

Vec3 random_sample_hemisphere(const Vec3 &normal) {
// 随机在单位半球上采样光线
  Vec3 in_unit_sphere = random_in_unit_sphere();
  if (dot(in_unit_sphere, normal) > 0.0) 
    return in_unit_sphere;
  else
    return -in_unit_sphere;

}
Vec3 reflect(const Vec3 &v, Vec3 &n) {  
// v:输入光线方向
// n:物体表面法线
// 返回反射光线的位置
  return v - 2*dot(v,n)*n;
}

Vec3 refract(Vec3 &v, Vec3 &n, float ior) {
// ior:物体的折射率
// 返回一个折射后的光线
  auto cos_theta = fmin(dot(-v, n), 1.0);
  // 垂直方向
  Vec3 r_out_perp =  ior * (v + cos_theta*n);
  // 平行方向
  Vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.squared_length())) * n;
  return r_out_perp + r_out_parallel;
}

float schlick_reflectance(float cos, float ior) {
  // Use Schlick's approximation for reflectance.
  auto r0 = (1-ior) / (1+ior);
  r0 = r0*r0;
  return r0 + (1-r0)*pow((1 - cos),5);
}

Material::Material(Vec3 albedo_, MaterialType type_) {
  albedo = albedo_;
  type = type_;
}

Material::Material(Vec3 albedo_, float fuzz_, MaterialType type_) {
  albedo = albedo_;
  type = type_;
  fuzz = std::min<float>(fuzz_, 1);
}

Material::Material(float ior_, MaterialType type_) {
  ior = ior_;
  type = type_;
}

Material::~Material() {}

bool Material::scatter(const Ray &r, hit_record &rec, Ray &scattered,Vec3 &attenu) {
// rec ：光线与物体相交的点的信息
// scattered：光线与物体作用之后的反射方向
// attenu ：能量衰减
  switch (type) {

  case Diffuse: {
    auto scatter_direction = rec.normal + random_unit_vector();
    // p->s: = rec.p + rec.normal + random_in_square
    // 如果随机生成的向量在所有维度上都非常接近于零，会出错，拦截这样的可能
    if (scatter_direction.near_zero())
      scatter_direction = rec.normal;
    //漫反射的光线
    scattered = Ray(rec.p, scatter_direction);
    //能量衰减系数
    attenu = albedo;
    return true;
    break;
  }

  case Metal: {
    // r:入射光线方向
    // fuzz：模糊参数
    Vec3 reflected = reflect(unit_vector(r.Direction()), rec.normal);
    scattered = Ray(rec.p, reflected + fuzz*random_in_unit_sphere());
    attenu = albedo;
    return (dot(scattered.Direction(), rec.normal) > 0);
    // return true;
    break;
  }

  case Dielectric: {
    // 玻璃不吸收任何光，故衰减系数始终为1
    attenu = Vec3(1.0, 1.0, 1.0);
    // 检查入射方向是从那边向那边
    rec.set_face_normal(r,rec.normal);
    double refraction_ratio = rec.front_face ? (1.0/ior) : ior;
    Vec3 unit_direction = unit_vector(r.Direction());
    double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0;   //全反射条件
    Vec3 direction;
    // 除了全反射（必定折射的光线外），正常光线可能反射也可能折射，限度为随机数：
    if (cannot_refract || schlick_reflectance(cos_theta, refraction_ratio) > get_random_float(0,1))
      direction = reflect(unit_direction, rec.normal);
    else
      direction = refract(unit_direction, rec.normal, refraction_ratio);

    scattered = Ray(rec.p, direction);

    return true;
    break;
  }

  default: {
    std::cout << "Unsurportted Material type!" << std::endl;
    return false;
    break;
  }
  }
}