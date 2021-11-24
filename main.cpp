#include "svpng.inc"
#include <iostream>

#include "camera.h"
#include "hitable_list.h"
#include "sphere.h"
#include <float.h>
#include <omp.h>
#include <random>
// #include <ctime>

#define W 800
#define H 400
const double infinity = std::numeric_limits<double>::infinity();

// normal shader
Vec3 color(const Ray &r, hitable *world) {
  hit_record rec;
  if (world->hit(r, 0.0, FLT_MAX, rec)) {
    //有撞点：即为球体，将球体颜色设置为球在该点的单位法向量的色彩表映射值
    return 0.5 *
           Vec3(rec.normal.x() + 1, rec.normal.y() + 1, rec.normal.z() + 1);
  } else {
    // 无撞点：将颜色设置为背景色
    Vec3 unit_direction = unit_vector(r.Direction());
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * Vec3(1.0, 1.0, 1.0) +
           t * Vec3(0.5, 0.7, 1.0); // white, light blue
  }
}

/******************** material shader ************************/
/**
 * @brief 光线追踪函数
 *
 * @param r: 光线
 * @param world： 场景中的物体集合
 * @param depth: 光线弹射次数，到达最大次数之后，返回0
 *
 */
Vec3 ray_color(const Ray &r, hitable *world, int depth) {
  hit_record rec;
  //如果光线到达反射次数,停止
  if (depth <= 0)
    return Vec3(0,0,0);

  //如果光线与物体相交,设置反射后的颜色
  if (world->hit(r, 0.001, infinity, rec)) {
    Ray scattered;
    Vec3 attenuation;
    if (rec.mat->scatter(r, rec, scattered, attenuation))
      return attenuation * ray_color(scattered, world, depth-1);
    return Vec3(0,0,0);
  }
  //如果光线与任何物体不相交,设置背景颜色
  Vec3 unit_direction = unit_vector(r.Direction());
  auto t = 0.5*(unit_direction.y() + 1.0);
  return (1.0-t)*Vec3(1.0, 1.0, 1.0) + t*Vec3(0.5, 0.7, 1.0);
}




unsigned char img[W * H * 3];

int main() {
  unsigned char *p = img;

  // 声明
  Camera cam;

  int SamplingRate = 100; //采样次数
  int Depth = 50; //光线弹射次数, 如果是一个空心玻璃球的话,弹射次数至少>4次，思考一下？

  /*将所有能够被撞击的物体信息保存在列表中*/
  std::vector<hitable *> list;
/*--------------------------------------------------------------------*/
  //散射球体 
  //设定材质
  // Material *diffuse_ground = new Material(Vec3(0.8, 0.8, 0.0), Diffuse);
  // Material *diffuse_center = new Material(Vec3(0.7, 0.3, 0.3), Diffuse);
  Material *diffuse_ground = new Material(Vec3(0.5, 0.5, 0.5), Diffuse);
  Material *diffuse_center = new Material(Vec3(0.5, 0.5, 0.5), Diffuse);
  //物体加入列表
  list.push_back(new sphere(Vec3(0, -100.5, -1), 100, diffuse_ground));
  list.push_back(new sphere(Vec3(0, 0, -1), 0.5, diffuse_center));
/*--------------------------------------------------------------------*/
  // // //金属球体
  // //设定材质
  // Material *diffuse_ground = new Material(Vec3(0.8, 0.8, 0.0), Diffuse);
  // Material *diffuse_center = new Material(Vec3(0.7, 0.3, 0.3), Diffuse);
  // Material *metal_left = new Material(Vec3(0.8, 0.8, 0.0), 0.3, Metal);
  // Material *metal_right = new Material(Vec3(0.8, 0.6, 0.2), 1.0, Metal);
    
  // //物体加入列表
  // list.push_back(new sphere(Vec3(0, -100.5, -1), 100, diffuse_ground));
  // list.push_back(new sphere(Vec3(0, 0, -1), 0.5, diffuse_center));
  // list.push_back(new sphere(Vec3(-1, 0, -1), 0.5, metal_left));
  // list.push_back(new sphere(Vec3(1, 0, -1), 0.5, metal_right));
/*--------------------------------------------------------------------*/
  // //透明球体
  // //设定材质
  // Material *diffuse_ground = new Material(Vec3(0.8, 0.8, 0.0), Diffuse);
  // Material *diffuse_center = new Material(Vec3(0.1, 0.2, 0.5), Diffuse);
  // Material *dielectric_left = new Material(1.5, Dielectric);
  // Material *metal_right = new Material(Vec3(0.8, 0.6, 0.2), 1.0, Metal);
    
  // //物体加入列表
  // list.push_back(new sphere(Vec3(0, -100.5, -1), 100, diffuse_ground));
  // list.push_back(new sphere(Vec3(0, 0, -1), 0.5, diffuse_center));
  // list.push_back(new sphere(Vec3(-1, 0, -1), 0.5, dielectric_left));
  // list.push_back(new sphere(Vec3(-1, 0, -1), -0.4, dielectric_left));    // 空心玻璃球
  // list.push_back(new sphere(Vec3(1, 0, -1), 0.5, metal_right));
/*--------------------------------------------------------------------*/
  //将物体组成的表加入碰撞列表
  hitable *world = new hitable_list(list, list.size());
  int tmp = 0;
  // // Windows随机数
  // srand((unsigned)time(NULL));
  #pragma omp parallel for     //接下来的for循环将被多线程执行
  for (int j = H - 1; j >= 0; j--) {
    // if(omp_get_thread_num()==1)
    if(j % 20 == 1)
    std::cerr << "\rComputing: " << ". .  .   .   ." << std::flush;
    else if(j % 20 == 11)
    std::cerr << "\rComputing: " << "              " << std::flush;
    for (int i = 0; i < W; i++) {
      int tmp=3*((399-j)*800+i);
      Vec3 col(0, 0, 0);
      for (int s = 0; s < SamplingRate; s++) {
        float u = float(i + drand48()) / float(W);
        float v = float(j + drand48()) / float(H);
        // // Windows随机数
        // float u = float(i + rand()) / float(W);
        // float v = float(j + rand()) / float(H);
        Ray r = cam.getRay(u, v);
        // col += color(r, world);
        col += ray_color(r, world, Depth);
      }
      col /= float(SamplingRate);
      auto scale = 1.0 / SamplingRate;

// #pragma omp critical
      // sqrt是伽马校正
      p[tmp] = int(255.99 * sqrt(col[0]));
      p[tmp+1] = int(255.99 * sqrt(col[1]));
      p[tmp+2] = int(255.99 * sqrt(col[2]));
      
      // std::cerr << "\rPixels remaining in this line: " << W-i <<' '<< std::flush; 
    }
  }

  svpng(fopen("diffuse.png", "wb"), W, H, img, 0);
  std::cout << "finished" << std::endl;

  //释放资源
  delete diffuse_center, diffuse_ground;
  // delete diffuse_center, diffuse_ground, metal_left, metal_right;
  // delete diffuse_center, diffuse_ground, dielectric_left, metal_right;
  return 0;
}
