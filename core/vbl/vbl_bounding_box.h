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
    initialized_ = false;
  }

  //: Incorporate 2d point x, y
  void update(const T& x, const T& y) {
    T tmp[DIM] = {x,y};
    update(tmp);
  }

  //: Incorporate DIM-d point
  void update(const T* point) {
    if (!initialized_) {
      initialized_ = true;
      for(int i = 0; i < DIM; ++i)
	min_[i] = max_[i] = point[i];
    } else {
      for(int i = 0; i < DIM; ++i) {
	if (point[i] < min_[i]) min_[i] = point[i];
	if (point[i] > max_[i]) max_[i] = point[i];
      }
    }
  }

  //: Reset to infinitesimal
  void reset() {
    initialized_ = false;
  }
  
  ostream& print(ostream& s) const;

  const T* get_min() const { return min_; }
  const T* get_max() const { return max_; }

  T* get_min() { return min_; }
  T* get_max() { return max_; }
  
  const T& get_xmin() const { return min_[0]; }
  const T& get_xmax() const { return max_[0]; }
  const T& get_ymin() const { return min_[1]; }
  const T& get_ymax() const { return max_[1]; }

protected:
  // Data Members--------------------------------------------------------------
  bool initialized_;
  T min_[DIM];
  T max_[DIM];

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

