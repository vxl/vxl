// This is core/vbl/vbl_bounding_box.h
#ifndef vbl_bounding_box_h_
#define vbl_bounding_box_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Contains a bounding box class
// \author awf@robots.ox.ac.uk
// \date   17 Mar 00
//
// \verbatim
// Modifications
// 970807 AWF Initial version.
// 07 Mar 2001 stewart@cs.rpi.edu added "inside" functions
// 21 Mar 2001 PDA (Manchester)   Tidied up the documentation
// 13 Jul 2001 Peter Vanroose     bug fix in inside() when box is empty
// \endverbatim

#include <vcl_iosfwd.h>
#include <vcl_cassert.h>

#if defined(VCL_SUNPRO_CC_50)
// Non-type template parameters are not allowed for function templates.
#endif


#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Helper class for vbl_bounding_box
template <int DIM>
struct vbl_bounding_box_DIM { enum { value = DIM }; };
#endif // DOXYGEN_SHOULD_SKIP_THIS

template <class T, class DIM_>
class vbl_bounding_box_base
{
 public:
  //: Construct an empty bounding box.
  inline vbl_bounding_box_base() : initialized_(false) { }

  //: Incorporate 1d point x
  inline void update(T const& x) {
    assert(DIM_::value == 1);
    update(&x);
  }

  //: Incorporate 2d point x, y
  inline void update(T const& x, T const& y) {
    assert(DIM_::value == 2);
    T tmp[2] = {x,y};
    update(tmp);
  }

  //: Incorporate 3d point x, y, z
  inline void update(T const& x, T const& y, T const& z) {
    assert(DIM_::value == 3);
    T tmp[3] = {x,y,z};
    update(tmp);
  }

  //: return dimension.
  inline int dimension() const { return DIM_::value; }

  //: Incorporate DIM-d point
  inline void update(T const* point) {
    if (!initialized_) {
      initialized_ = true;
      for (int i = 0; i < dimension(); ++i)
        min_[i] = max_[i] = point[i];
    } else {
      for (int i = 0; i < dimension(); ++i) {
        if (point[i] < min_[i]) min_[i] = point[i];
        if (point[i] > max_[i]) max_[i] = point[i];
      }
    }
  }

  //: Reset to empty
  inline void reset() { initialized_ = false; }

  //: Return initialisation status
  inline bool empty() const { return !initialized_; }

  //:  is a 2D point inside the bounding box
  inline bool inside( const T& x, const T& y) const {
    assert (DIM_::value == 2);
    return
      initialized_ &&
      min_[0] <= x && x <= max_[0] &&
      min_[1] <= y && y <= max_[1];
  }

  //:  is a 3D point inside the bounding box
  inline bool inside( const T& x, const T& y, const T& z) const {
    assert (DIM_::value == 3);
    return
      initialized_ &&
      min_[0] <= x && x <= max_[0] &&
      min_[1] <= y && y <= max_[1] &&
      min_[2] <= z && z <= max_[2];
  }

  //:  inside test for arbitrary dimension
  inline bool inside(T const* point) {
    if (!initialized_) return false;
    for ( int i=0; i<dimension(); ++i )
      if ( point[i] < min_[i] || max_[i] < point[i] )
        return false;
    return true;
  }

  //: return "volume".
  inline T volume() const {
    if (!initialized_) return T(0);
    T vol = 1;
    for (int i=0; i<dimension(); ++i)
      vol *= max_[i] - min_[i];
    return vol;
  }

  vcl_ostream& print(vcl_ostream& s) const;

  inline T const* min() const { return min_; }
  inline T const* max() const { return max_; }

  inline T      * min() { return min_; }
  inline T      * max() { return max_; }

  inline T const& xmin() const { return min_[0]; }
  inline T const& xmax() const { return max_[0]; }
  inline T const& ymin() const { assert(DIM_::value >= 2); return min_[1]; }
  inline T const& ymax() const { assert(DIM_::value >= 2); return max_[1]; }
  inline T const& zmin() const { assert(DIM_::value >= 3); return min_[2]; }
  inline T const& zmax() const { assert(DIM_::value >= 3); return max_[2]; }

  //#pragma dogma
  // There is no need for the data members to be private to
  // this class. After all, the STL pair<> template has its
  // two main members first, second public without causing
  // any problems.
  // BUT...
  //#pragma practically
  // The main difference here is that min_ and max_ have to satisfy certain
  // constraints: e.g., min_[0] must be <= max_[0].  Hence we want write access
  // to the data members to go through update() and empty() which can enforce
  // this.  Also, the use of initialized_ is a bit tricky: it indicates whether
  // the bounding box is empty or not.  This information could have been stored
  // in the other data members instead (e.g. by setting min_[0] > max_[0])
  // but that was not the design choice.  Hence, leaving the really private
  // (because subject to removal) data member initialized_ freely accessible
  // is a very bad idea...
  // HUH???
  //#pragma reality_check
  // But those members are still public; they are exposed by min() and max(),
  // except the client must use the odd "bb.min()[i]" instead of "bb.min[i]".
  // I predict the response to this observation will be the removal of the two
  // non-const versions.

 private:
  bool initialized_;
  T min_[DIM_::value];
  T max_[DIM_::value];
};

//: A class to hold and update a bounding box.
//  Save valuable time not writing
// \code
//    if (x > xmax).....
// \endcode

template <class T, int DIM>
class vbl_bounding_box : public vbl_bounding_box_base<T, vbl_bounding_box_DIM<DIM> >
{
 public:
};

//------------------------------------------------------------

//: this is "operator \subseteq"
template <class T, class DIM_>
inline
bool nested(vbl_bounding_box_base<T,DIM_> const &a, vbl_bounding_box_base<T,DIM_> const &b)
{
  for (int i=0; i<DIM_::value; ++i)
    if (a.min()[i] < b.min()[i] || a.max()[i] > b.max()[i])
      return false;
  return true;
}

//: is the intersection empty?
template <class T, class DIM_>
inline
bool disjoint(vbl_bounding_box_base<T,DIM_> const &a,
              vbl_bounding_box_base<T,DIM_> const &b)
{
  for (int i=0; i<DIM_::value; ++i)
    if (a.min()[i] > b.max()[i] || a.max()[i] < b.min()[i])
      return true;
  return false;
}

//: is the intersection nonempty?
template <class T, class DIM_>
inline
bool meet(vbl_bounding_box_base<T,DIM_> const &a,
          vbl_bounding_box_base<T,DIM_> const &b)
{ return ! disjoint(a, b); }

// VC50 has trouble with this
template <class T, class DIM_>
vcl_ostream& operator << (vcl_ostream& s, const vbl_bounding_box_base<T,DIM_>& bbox);

#endif // vbl_bounding_box_h_
