#ifndef _decls_h_
#define _decls_h_

#ifdef __GNUG__
#pragma interface
#endif

// point[23]d.h
template <class type> class tmpl_point2D;
typedef tmpl_point2D<float>  Point2D;
typedef tmpl_point2D<int>    intPoint2D;
typedef tmpl_point2D<float>  floatPoint2D;
typedef tmpl_point2D<double> doublePoint2D;

template <class T> struct fast_array2d;

#endif
