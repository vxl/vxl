#ifndef vgl_clip_h_
#define vgl_clip_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vgl_clip
// .INCLUDE vgl/vgl_clip.h
// .FILE vgl_clip.cxx
// .SECTION Author
//  fsm@robots.ox.ac.uk
//

//: clips away the portion where ax+by+c<0. return false if nothing left.
//template <class double>
bool vgl_clip_lineseg_to_line(double &x1, double &y1, // line segment start
                              double &x2, double &y2, // and end.
                              double a, double b, double c);

// clip line ax+by+c=0 to given box. return false if no intersection.
//template <class double>
bool vgl_clip_line_to_box(double a, double b, double c, // line equation ax+by+c=0.
                          double x1,double y1, // coordinates of
                          double x2,double y2, // box corners.
                          double &bx, double &by,  // clipped line
                          double &ex, double &ey); // segment.

#endif // vgl_clip_h_
