#ifndef pair_float_int_h_
#define pair_float_int_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

#include <vcl/vcl_iostream.h>

struct pair_float_int {
  float f;
  int i;

  pair_float_int() {}
  pair_float_int(float f_, int i_):f(f_),i(i_) {}

};

inline
bool operator==(const pair_float_int& p1, const pair_float_int& p2) 
{ return p1.i == p2.i; }

inline
ostream& operator<<(ostream& s, const pair_float_int& p) 
{ return s << "[ " << p.i << " , " << p.f << "]\n";}

int pair_float_int_compare_ascend(pair_float_int const& p1, pair_float_int const& p2);

#endif
