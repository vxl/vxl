#ifndef mfpf_region_definer_h_
#define mfpf_region_definer_h_
//:
// \file
// \brief Base for objects which generate regions from sets of points
// \author Tim Cootes

#include <vcl_string.h>
#include <vcl_memory.h>

#include <vsl/vsl_binary_io.h>
#include <mfpf/mfpf_region_form.h>

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

  //: Uses some subset of pts to define a region
  //  The pose for the region will be a translation + rotation,
  //  ie region.pose().u() is a unit length
  virtual mfpf_region_form set_up(
             const vcl_vector<vgl_point_2d<double> >& pts) = 0;

  //: Uses some subset of pts to define a new region
  //  The aspect ratio of the region will be the same as that
  //  from the last call to set_up. Only region.pose() will be
  //  different.  Thus the returned region.pose() can be used
  //  to define the pose for training an mfpf_point_finder,
  //  for instance. 
  virtual mfpf_region_form get_region(
                const vcl_vector<vgl_point_2d<double> >& pts) = 0;

  //: Initialise from a stream
  virtual bool set_from_stream(vcl_istream &is);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual mfpf_region_definer* clone() const = 0;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const =0;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const =0;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs) =0;

  //: Create a concrete object, from a text specification.
  static vcl_auto_ptr<mfpf_region_definer> create_from_stream(vcl_istream &is);

};

//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const mfpf_region_definer& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mfpf_region_definer& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mfpf_region_definer& b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const mfpf_region_definer& b);

//: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const mfpf_region_definer* b);

#endif // mfpf_region_definer_h_
