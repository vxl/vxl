//-*- c++ -*-------------------------------------------------------------------
#ifndef vbl_bounding_box_h_
#define vbl_bounding_box_h_
#ifdef __GNUC__
#pragma interface
#endif
// Author: awf@robots.ox.ac.uk
// Created: 17 Mar 00

#include <vcl/vcl_iosfwd.h>

//: A class to hold and update a bounding box.
//  Save valuable time not writing
//    if (x > xmax).....
template <class T, int DIM>
class vbl_bounding_box {
public:
  // -- Construct an empty bounding box.
  vbl_bounding_box() {
    _initialized = false;
  }

  //: Incorporate 2d point x, y
  void update(const T& x, const T& y) {
    T tmp[DIM] = {x,y};
    update(tmp);
  }

  //: Incorporate DIM-d point
  void update(const T* point) {
    if (!_initialized) {
      _initialized = true;
      for(int i = 0; i < DIM; ++i)
	_min[i] = _max[i] = point[i];
    } else {
      for(int i = 0; i < DIM; ++i) {
	if (point[i] < _min[i]) _min[i] = point[i];
	if (point[i] > _max[i]) _max[i] = point[i];
      }
    }
  }

  //: Reset to infinitesimal
  void reset() {
    _initialized = false;
  }
  
  ostream& print(ostream& s) const;

  const T* get_min() const { return _min; }
  const T* get_max() const { return _max; }

  T* get_min() { return _min; }
  T* get_max() { return _max; }
  
  const T& get_xmin() const { return _min[0]; }
  const T& get_xmax() const { return _max[0]; }
  const T& get_ymin() const { return _min[1]; }
  const T& get_ymax() const { return _max[1]; }

protected:
  // Data Members--------------------------------------------------------------
  bool _initialized;
  T _min[DIM];
  T _max[DIM];

  // Helpers-------------------------------------------------------------------
};

// VC50 has trouble with this
#if !defined (WIN32) && !defined(__SUNPRO_CC)
template <class T, int DIM>
ostream& operator << (ostream& s, const vbl_bounding_box<T,DIM>& bbox) { return bbox.print(s); }
#else
template <class T>
ostream& operator << (ostream& s, const vbl_bounding_box<T,2>& bbox) { return bbox.print(s); }
template <class T>
ostream& operator << (ostream& s, const vbl_bounding_box<T,3>& bbox) { return bbox.print(s); }
#endif

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vbl_bounding_box.

