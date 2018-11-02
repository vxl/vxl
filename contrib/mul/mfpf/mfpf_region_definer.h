#ifndef mfpf_region_definer_h_
#define mfpf_region_definer_h_
//:
// \file
// \brief Base for objects which generate regions from sets of points
// \author Tim Cootes

#include <string>
#include <iostream>
#include <memory>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mfpf/mfpf_region_form.h>
#include <vsl/vsl_binary_io.h>

constexpr unsigned mfpf_invalid_index = 99999;

//: Base for objects which generate regions from sets of points
//  Given a set of points, derived classes can generate a region.
//  Typically this is centred on one point or set of points, and
//  is aligned with reference to some other points.
//
//  Two key functions are provided: set_up(pts1), which defines
//  the region and its aspect ratio, and get_region(pts2), which
//  returns a box of the same aspect ratio, with its pose defined
//  by the points in pts2.  Two separate functions are required
//  because all returned regions should have the same aspect ratio,
//  since the boxes are to be used to initialise point_finders, which
//  require a fixed aspect.
class mfpf_region_definer
{
 public:

  //: Dflt ctor
  mfpf_region_definer();

  //: Destructor
  virtual ~mfpf_region_definer();

  //: Returns true if the region is centred on an input point
  virtual bool is_centred_on_pt() const = 0;

  //: Returns index of reference point on which the region is centred
  virtual unsigned ref_point_index() const = 0;

  //: Returns original index of reference point on which the region is centred
  virtual unsigned orig_ref_point_index() const = 0;

  //: Returns reference point for region
  //  Returns the point that would be returned by
  //  set_up(pts).pose().p(), but without changing internal
  //  state.
  virtual vgl_point_2d<double> get_ref_point(
             const std::vector<vgl_point_2d<double> >& pts) const = 0;

  //: Uses some subset of pts to define a region
  //  The pose for the region will be a translation + rotation,
  //  ie region.pose().u() is a unit length
  virtual mfpf_region_form set_up(
             const std::vector<vgl_point_2d<double> >& pts) = 0;

  //: Uses some subset of pts to define a new region
  //  The aspect ratio of the region will be the same as that
  //  from the last call to set_up. Only region.pose() will be
  //  different.  Thus the returned region.pose() can be used
  //  to define the pose for training an mfpf_point_finder,
  //  for instance.
  virtual mfpf_region_form get_region(
              const std::vector<vgl_point_2d<double> >& pts) const = 0;

  //: Replace each point index i with new_index[i]
  //  Allows for re-numbering of the points used.
  //  new_index[i]==mfpf_invalid_index indicates an invalid index
  //  Returns true if successful.
  virtual bool replace_index(const std::vector<unsigned>& new_index)=0;

  //: Initialise from a stream
  virtual bool set_from_stream(std::istream &is);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual std::string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual mfpf_region_definer* clone() const = 0;

  //: Print class to os
  virtual void print_summary(std::ostream& os) const =0;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const =0;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs) =0;

  //: Create a concrete object, from a text specification.
  static std::unique_ptr<mfpf_region_definer> create_from_stream(std::istream &is);
};

//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const mfpf_region_definer& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mfpf_region_definer& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mfpf_region_definer& b);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const mfpf_region_definer& b);

//: Stream output operator for class pointer
std::ostream& operator<<(std::ostream& os,const mfpf_region_definer* b);

//: Generate a new set of points from \p pts0 using set of definers
// \code
//  new_pts[i] = definer[i]->get_ref_point(pts0)
// \endcode
void mfpf_points_from_definers(
              const std::vector<mfpf_region_definer*>& definer,
              const std::vector<vgl_point_2d<double> >& pts0,
              std::vector<vgl_point_2d<double> >& new_pts);

//: Change indices in definers to refer to points generated
//  Suppose definer is used to generate a set of n=definer.size()
//  regions/pts (say pts1), by referring to some other set of m points.
//  This sets up self_definer to generate an identical set of
//  regions/pts by using the originally generated points (pts1).
//  This can only be done if there is a region centred on each
//  of the original points used in the definer.
//  The function tests for this case, and returns false if it fails.
//  In particular consider the following
//  \code
//  std::vector<vgl_point_2d<double> > pts0,pts1,pts2;
//  // Set up pts0
//  ...
//  // Generate pts1 from pts0
//  mfpf_points_from_definers(definer,pts0,pts1);
//  mfpf_renumber_to_self(definer,pts0.size())
//  // Now generate pts2 from pts1
//  mfpf_points_from_definers(self_definer,pts1,pts2);
//  // pts2 should be the same as pts1
//  \endcode
//  Note that objects pointed to by definer are changed.
//  They may be left in an invalid state if this returns false,
//  so caller should ensure a backup retained.
bool mfpf_renumber_to_self(std::vector<mfpf_region_definer*>& definer,
                           unsigned n_pts0);

#endif // mfpf_region_definer_h_
