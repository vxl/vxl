// This is brl/bbas/bsol/bsol_intrinsic_curve_2d.h
#ifndef bsol_intrinsic_curve_2d_h_
#define bsol_intrinsic_curve_2d_h_
//*****************************************************************************
//:
// \file
// \brief Generic intrinsic curve in 2D that has intrinsic curvature, d_theta, d_s defined.
//   Note that to make the definition intrinsic,
//   I didn't inherent it from bdgl_polyline_2d; instead, I inherent it
//   from vsol_curve_2d.
//   For closed curve, refer to bdgl_intrinsic_closed_curve_2d.
//
// This class inherits from vsol_curve_2d.
//
// \author MingChing Chang
// \date   2003-11-14
//
// \verbatim
//  Modifications
//   2003-11-14 MingChing Chang - Modified from the code of Thomas Sebastian
//   2004-10-25 Ozge Can Ozcanli - Modified to add close curve support
//   2004-11-01 Ozge Can Ozcanli - Added method upsample that interpolates new samples linearly according to arclength
// \endverbatim
//*****************************************************************************

#include <vector>
#include <iostream>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vcl_compiler.h>
#include <vcl_cassert.h>

#define ZERO_TOLERANCE 1E-1 // used in bsol_intrinsic_curve_2d.cxx

//: General intrinsic curve class

class bsol_intrinsic_curve_2d : public vsol_curve_2d
{
 protected:
  //***************************************************************************
  // Data members

  //: Pointer to list of vsol_point_2d smart pointers
  std::vector<vsol_point_2d_sptr> *storage_;
  //: First point of the curve : just to conform to vsol_curve_2d standard
  vsol_point_2d_sptr p0_;
  //: Last point of the curve
  vsol_point_2d_sptr p1_;
  //: Arclength measured from the p0_
  std::vector<double> arcLength_;
  //: Arclength of the current segment i to i+1
  std::vector<double> s_;
  //: Normalized arclength measured from the p0_
  //  Normalized arclength of the whole curve is 1.
  std::vector<double> normArcLength_;
  //: Length of the intrinsic curve
  double length_;
  //: dx of the intrinsic curve
  std::vector<double> dx_;
  //: dy of the intrinsic curve
  std::vector<double> dy_;
  //: Curvature of the intrinsic curve
  std::vector<double> curvature_;
  //: Bending angle of the intrinsic curve
  std::vector<double> angle_;
  //: Total curvature of the intrinsic curve
  double totalCurvature_;
  //: Total angle change of the intrinsic curve
  double totalAngleChange_;

  bool isOpen_; // true - open, false - closed

 public:
  //***************************************************************************
  // Initialization

  //: Default Constructor
  bsol_intrinsic_curve_2d();
  //: Constructor from a std::vector of points
  bsol_intrinsic_curve_2d(const std::vector<vsol_point_2d_sptr> &new_vertices);
  //: Constructor from a vsol_polyline_2d_sptr
  bsol_intrinsic_curve_2d(const vsol_polyline_2d_sptr poly);
  bsol_intrinsic_curve_2d(const vsol_polygon_2d_sptr poly);

  //: Copy constructor
  bsol_intrinsic_curve_2d(const bsol_intrinsic_curve_2d &other);
  //: Destructor
  virtual ~bsol_intrinsic_curve_2d();
  //: Clone `this': creation of a new object and initialization
  // See Prototype pattern
  virtual vsol_spatial_object_2d* clone(void) const;

  //: Return a platform independent string identifying the class
  std::string is_a() const { return std::string("bsol_intrinsic_curve_2d"); }

  //***************************************************************************
  // Access

  bool isOpen(void) const { return isOpen_; }
  //: Return the first point of `this';  pure virtual of vsol_curve_2d
  virtual vsol_point_2d_sptr p0() const { return p0_; }
  //: Return the last point of `this';   pure virtual of vsol_curve_2d
  virtual vsol_point_2d_sptr p1() const { return p1_; }
  //: Is `i' a valid index for the list of vertices ?
  bool valid_index(unsigned int i) const { return i<storage_->size(); }
  //: Return vertex `i'
  vsol_point_2d_sptr vertex(const int i) const {
    assert(valid_index(i));
    return (*storage_)[i];
  }
  //: Return x coord of vertex `i'
  double x (const int i) const {
    assert(valid_index(i));
    return (*storage_)[i]->x();
  }
  //: Return y coord of vertex `i'
  double y (const int i) const {
    assert(valid_index(i));
    return (*storage_)[i]->y();
  }
  //: Return the number of vertices
  int size(void) const { return (int)storage_->size(); }
  //: Return the total arclength from vertex `0' to vertex `i'
  double arcLength (const int i) const {
    assert (valid_index(i));
    return arcLength_[i];
  }
  //: Return the total arclength of the current vertex `i-1' to vertex `i'
  double s (const int i) const {
    assert (valid_index(i));
    return s_[i];
  }
  //: Return the normalized arclength from vertex `0' to vertex `i'
  double normArcLength (const int i) const;
  //: Return the length of the intrinsic curve
  virtual double length (void) const { return length_; }
  //: Return the dx vertex `i-1' to vertex `i'
  double dx (const int i) const {
    assert(valid_index(i));
    return (*storage_)[i]->x() - (*storage_)[i-1]->x();
  }
  //: Return the dy vertex `i-1' to vertex `i'
  double dy (const int i) const {
    assert(valid_index(i));
    return (*storage_)[i]->y() - (*storage_)[i-1]->y();
  }
  //: Return the curvature of vertex `i'
  double curvature (const int i) const;
  //: Return the angle of vertex `i'
  double angle (const int i) const;

  //: Return the total curvature of the intrinsic curve
  double totalCurvature (void) const { return totalCurvature_; }
  //: Return the total angle change of the intrinsic curve
  double totalAngleChange (void) const { return totalAngleChange_; }

  //***************************************************************************
  // Comparison

  //: Has `this' the same points than `other' in the same order ?
  virtual bool operator==(const bsol_intrinsic_curve_2d &other) const;
  virtual bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d
  //: Has `this' the same points than `other' in the same order ?
  inline bool operator!=(const bsol_intrinsic_curve_2d &o) const { return !operator==(o); }

 protected:

  // Internal status setting functions

  //: Computing the properties of the curve.
  void computeDerivatives();
  void computeCurvatures();
  void computeArcLength();
  void computeAngles();

 public:

  //***************************************************************************
  // Status setting

  void setOpen(bool flag) { isOpen_ = flag; }

  //: Set the first point of the curve
  virtual void set_p0 (const vsol_point_2d_sptr &new_p0);
  //: Set the last point of the curve
  virtual void set_p1 (const vsol_point_2d_sptr &new_p1);
  //: Compute intrinsic properties.
  //  Note that if you even call the other modifying function with the
  //  flag bRecomputeProperties set to false, remember to call this function
  //  to recompute intrinsic properties of this curve.
  void computeProperties();

  //: Delete all points of the intrinsic curve
  void clear();
  //: Add another point to the end of the intrinsic curve
  void add_vertex (const vsol_point_2d_sptr &new_p, bool bRecomputeProterties=false);
  //: Add another point to the end of the intrinsic curve
  void add_vertex (double x, double y) {
    vsol_point_2d_sptr newpoint = new vsol_point_2d (x, y);
    add_vertex (newpoint, false);
  }
  //: Remove one vertex from the intrinsic curve
  void remove_vertex (const int i, bool bRecomputeProterties=false);
  //: Modify one vertex of the intrinsic curve
  void modify_vertex (const int i, double x, double y, bool bRecomputeProterties=false);
  //: Insert one vertex to position `i' of the intrinsic curve
  //  Note that it insert the vertex into `i-1'
  void insert_vertex (int i, double x, double y, bool bRecomputeProterties=false);

  void readCONFromFile(std::string fileName);

  //: Added by Ozge Can Ozcanli to upsample the curve
  bool upsample(int new_size);

  //***************************************************************************
  // Basic operations

  //: Compute the bounding box of `this'
  virtual void compute_bounding_box(void) const;

  //: output description to stream
  inline void describe(std::ostream &strm, int blanking=0) const {
    if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
    strm << "<bsol_intrinsic_curve_2d "
      // << static_cast<vsol_curve_2d>(*this) << ' '
         << "p0 = " << *p0_ << ", p1 = " << *p1_ << " >\n";
  }
};

#endif // bsol_intrinsic_curve_2d_h_
