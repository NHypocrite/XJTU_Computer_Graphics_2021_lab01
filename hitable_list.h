#ifndef HITABLE_LIST_H
#define HITABLE_LIST_H

#include "hitable.h"
#include <vector>

class hitable_list : public hitable {
public:
  std::vector<hitable *> list;
  int list_size;

public:
  hitable_list() {}
  hitable_list(std::vector<hitable *> &l, int n) {
    list = l;
    list_size = n;
  }

  virtual bool hit(const Ray &r, float tmin, float tmax, hit_record &rec) const;
};

#endif // HITABLE_LIST_H