// This is vxl/vgl/vgl_ellipse_scan_iterator.h
#ifndef vgl_ellipse_scan_iterator_h_
#define vgl_ellipse_scan_iterator_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Amitha Perera
// \date   31 August 2001

#include <vgl/vgl_region_scan_iterator.h>

//: Scan convert an ellipse
//  The ellipse is parameterised by (\a xc, \a yc) (the centre), by \a
//  a and \a b (the radii along the principle axes) and by \a theta,
//  the rotation of the main axis (in radians) about the centre of the
//  ellipse w.r.t\. the horizontal direction (X-axis).
//
//  Scan lines are horizontal lines intersecting the ellipse interior.
//  For a convex region like an ellipse, such a scan line is fully determined
//  by the two end points (startx(),scany()) and (endx(),scany()).
//
class vgl_ellipse_scan_iterator : public vgl_region_scan_iterator
{
 public:
  //: Constructor
  //  The ellipse is parameterised by (\a xc, \a yc) (the centre), by \a
  //  rx and \a ry (the radii along the principle axes) and by \a theta,
  //  the rotation of the main axis (in radians) about the centre of the
  //  ellipse w.r.t\. the horizontal direction (X-axis).
  vgl_ellipse_scan_iterator( double xc, double yc, double rx, double ry, double theta );

  //: Destructor
  virtual ~vgl_ellipse_scan_iterator();

  //: Resets the scan iterator to before the first scan line
  //  After calling this function, next() needs to be called before
  //  startx() and endx() form a valid scan line.
  virtual void reset();

  //: Tries to moves to the next scan line.
  //  Returns false if there are no more scan lines.
  virtual bool next();

  //: y-coordinate of the current scan line.
  virtual int  scany() const { return y_; }

  //: Returns starting x-value of the current scan line.
  virtual int  startx() const { return start_x_; }

  //: Returns ending x-value of the current scan line.
  virtual int  endx() const { return end_x_; }

 private:
  //: Parameters of the ellipse being scan converted.
  //  Centre, squared radii, and angle of rotation about the centre.
  double xc_, yc_, rx_, ry_, theta_;

  //: Current scan line
  int y_;

  //: Final scan line
  int min_y_;

  //: Start of current scan line
  int start_x_;

  //: End of current scan line
  int end_x_;
};

#endif // vgl_ellipse_scan_iterator_h_
