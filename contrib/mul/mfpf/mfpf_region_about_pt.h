#ifndef mfpf_region_about_pt_h_
#define mfpf_region_about_pt_h_
//:
// \file
// \brief Region centred on a single point, pose defined by other pts
// \author Tim Cootes

#include <mfpf/mfpf_region_definer.h>
#include <vcl_iosfwd.h>

//: Region centred on a single point, pose defined by other pts
//  Region defined by three points, p0=pts[i0], p1, p2.
//  The region is centred on p0.
//  Bounding box of region is aligned with the vector (p2-p1),
//  and its width and height are defined relative to |p2-p1|
//
//  Parameters in text file:
//  \verbatim
//  { i0: 5 i1: 31 i2: 37  rel_wi: 0.5 rel_wj: 0.2 form: box }
//  \endverbatim
class mfpf_region_about_pt : public mfpf_region_definer
{
private:
  //: Index of point on which region is centred
  unsigned i0_;
  //: Index of first point defining reference frame
  unsigned i1_;
  //: Index of second point defining reference frame
  unsigned i2_;

  //: Width of ROI, relative to |p2-p1|
  double rel_wi_;

  //: Height of ROI, relative to |p2-p1|
  double rel_wj_;

  //: Width of ROI in world coords
  double wi_;

  //: Height of ROI in world coords
  double wj_;

  //: Name of form of shape ("box","ellipse")
  vcl_string form_;

public:

  //: Dflt ctor
  mfpf_region_about_pt();

  //: Destructor
  virtual ~mfpf_region_about_pt();

  //: Index of point on which region is centred
  unsigned i0() const { return i0_; }

  //: Index of first point defining reference frame
  unsigned i1() const { return i1_; }

  //: Index of second point defining reference frame
  unsigned i2() const { return i2_; }

  //: Width of ROI, relative to |p2-p1|
  double rel_wi() const { return rel_wi_; }

  //: Height of ROI, relative to |p2-p1|
  double rel_wj() const { return rel_wj_; }

  //: Width of ROI in world coords
  double wi() const { return wi_; }

  //: Height of ROI in world coords
  double wj() const { return wj_; }

  //: Name of form of shape ("box","ellipse")
  vcl_string form() const { return form_; }

  //: Defines a region centred on a point
  virtual mfpf_region_form set_up(
             const vcl_vector<vgl_point_2d<double> >& pts);

  //: Defines a region centred on a point
  //  The aspect ratio of the region will be the same as that
  //  from the last call to set_up.
  virtual mfpf_region_form get_region(
                const vcl_vector<vgl_point_2d<double> >& pts);

  //: Initialise from a stream
  virtual bool set_from_stream(vcl_istream &is);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual mfpf_region_definer* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif
