/*
  fsm@robots.ox.ac.uk
*/
struct X {
  int x;
  X(int x_) : x(x_) { }
  bool operator==(X const &that) const { return x == that.x; }
  bool operator< (X const &that) const { return x <  that.x; }
};

#include <vcl/vcl_rel_ops.h>

int function()
{
  X x(2), y(3);
  if (x == y) return 1;
  if (x != y) return 2;
  if (x <  y) return 3;
  if (x >  y) return 4;
  if (x <= y) return 5;
  if (x >= y) return 6;
}

int main(int, char **)
{
  function();
  return 0;
}

VCL_REL_OPS_INSTANTIATE(X);
