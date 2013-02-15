#include "vbl_edge.h"
bool operator<(const vbl_edge& e0, const vbl_edge& e1){
  return e0.w_ < e1.w_;
}
