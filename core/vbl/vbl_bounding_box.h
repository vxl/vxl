#ifndef vbl_bounding_box_h_
#define vbl_bounding_box_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vbl/vbl_bounding_box.h

//:
// \file
// \brief Contains a bounding box class
// \author awf@robots.ox.ac.uk, Created: 17 Mar 00
//
// \verbatim
// Modifications
//970807 AWF Initial version.
//     07 Mar 01 stewart@cs.rpi.edu added "inside" functions
// PDA (Manchester) 21/03/2001: Tidied up the documentation and 
// \endverbatim


#include <vcl_iosfwd.h>

//: A class to hold and update a bounding box.
//  Save valuable time not writing
//    if (x > xmax).....

#if defined(VCL_SUNPRO_CC_50)
// Non-type template parameters are not allowed for function templates.
#endif

template <int DIM>
struct vbl_bounding_box_DIM { enum { value = DIM }; };

template <class T, class DIM_>
struct vbl_bounding_box_base
{
  //: Construct an empty bounding box.
  vbl_bounding_box_base() : initialized_(false) { }

  //: Incorporate 2d point x, y
  void update(T const& x, T const& y) {
    T tmp[2] = {x,y};
    update(tmp);
  }

  //: Incorporate 3d point x, y, z
  void update(T const& x, T const& y, T const& z) {
    T tmp[3] = {x,y,z};
    update(tmp);
  }

  //: return dimension.
  int dimension() const { return DIM_::value; }

  //: Incorporate DIM-d point
  void update(T const* point) {
    if (!initialized_) {
      initialized_ = true;
      for(int i = 0; i < dimension(); ++i)
        min_[i] = max_[i] = point[i];
    } else {
      for(int i = 0; i < dimension(); ++i) {
        if (point[i] < min_[i]) min_[i] = point[i];
        if (point[i] > max_[i]) max_[i] = point[i];
      }
    }
  }

  //: Reset to empty
  void reset() {
    initialized_ = false;
  }

  //:
  bool empty() const {
    return initialized_ == false;
  }

  //:  is a 2D point inside the bounding box
  bool inside( const T& x, const T& y) const {
    return
      min_[0] <= x && x <= max_[0] &&
      min_[1] <= y && y <= max_[1];
  }

  //:  is a 3D point inside the bounding box
  bool inside( const T& x, const T& y, const T& z) const {
    return
      min_[0] <= x && x <= max_[0] &&
      min_[1] <= y && y <= max_[1] &&
      min_[2] <= z && z <= max_[2];
  }

  //:  inside test for arbitrary dimension
  bool inside(T const* point) {
    for( int i=0; i<dimension(); ++i )
      if( point[i] < min_[i] || max_[i] < point[i] )
        return false;
    return true;
  }

  //: return "volume".
  T volume() const {
    if (!initialized_) return T(0);
    T vol = 1;
    for (int i=0; i<dimension(); ++i)
      vol *= max_[i] - min_[i];
    return vol;
  }

  vcl_ostream& print(vcl_ostream& s) const;

  T const* get_min() const { return min_; }
  T const* get_max() const { return max_; }

  T      * get_min() { return min_; }
  T      * get_max() { return max_; }

  T const& get_xmin() const { return min_[0]; }
  T const& get_xmax() const { return max_[0]; }
  T const& get_ymin() const { return min_[1]; }
  T const& get_ymax() const { return max_[1]; }

  //#pragma dogma
  // There is no need for the data members to be private to
  // this class. After all, the STL pair<> template has its
  // two main members first, second public without causing
  // any problems.
//protected:
  bool initialized_;
  T min_[DIM_::value];
  T max_[DIM_::value];
};

template <class T, int DIM>
class vbl_bounding_box : public vbl_bounding_box_base<T, vbl_bounding_box_DIM<DIM> >
{
public:
};

//------------------------------------------------------------

template <class T, class DIM_>
inline // this is "operator \subseteq"
bool nested(vbl_bounding_box_base<T,DIM_> const &a, vbl_bounding_box_base<T,DIM_> const &b)
{
  for (int i=0; i<DIM_::value; ++i)
    if (a.min_[i] < b.min_[i] || a.max_[i] > b.max_[i])
      return false;
  return true;
}

template <class T, class DIM_>
inline
bool disjoint(vbl_bounding_box_base<T,DIM_> const &a, 
              vbl_bounding_box_base<T,DIM_> const &b)
{
  for (int i=0; i<DIM_::value; ++i)
    if (a.min_[i] > b.max_[i] || a.max_[i] < b.min_[i])
      return true;
  return false;
}

template <class T, class DIM_>
inline
bool meet(vbl_bounding_box_base<T,DIM_> const &a, 
          vbl_bounding_box_base<T,DIM_> const &b)
{ return ! disjoint(a, b); }

// VC50 has trouble with this
template <class T, class DIM_>
vcl_ostream& operator << (vcl_ostream& s, const vbl_bounding_box_base<T,DIM_>& bbox)
 { return bbox.print(s); }

#endif // vbl_bounding_box_h_
