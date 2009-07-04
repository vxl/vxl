#include "boxm_triangle_scan_iterator.h"
//:
// \file
#include <vcl_vector.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>


//: constructor
boxm_triangle_scan_iterator::boxm_triangle_scan_iterator(double *verts_x, double *verts_y, unsigned int v0, unsigned int v1, unsigned int v2)
: tri_it_()
{
  tri_it_.a.x = verts_x[v0] - 0.5;
  tri_it_.a.y = verts_y[v0] - 0.5;
  tri_it_.b.x = verts_x[v1] - 0.5;
  tri_it_.b.y = verts_y[v1] - 0.5;
  tri_it_.c.x = verts_x[v2] - 0.5;
  tri_it_.c.y = verts_y[v2] - 0.5;
}


//: Resets the scan iterator to before the first scan line
//  After calling this function, next() needs to be called before
//  startx() and endx() form a valid scan line.
void boxm_triangle_scan_iterator::reset()
{
  tri_it_.reset();
}

//: Tries to move to the next scan line.
//  Returns false if there are no more scan lines.
bool boxm_triangle_scan_iterator::next()
{
  return tri_it_.next();
}

//: y-coordinate of the current scan line.
int boxm_triangle_scan_iterator::scany() const
{
  return tri_it_.scany();
}

//: Returns starting x-value of the current scan line.
//  startx() should be smaller than endx(), unless the scan line is empty
int boxm_triangle_scan_iterator::startx() const
{
  return tri_it_.startx();
}

//: Returns ending x-value of the current scan line.
//  endx() should be larger than startx(), unless the scan line is empty
int  boxm_triangle_scan_iterator::endx() const
{
  return tri_it_.endx() + 1;
}

