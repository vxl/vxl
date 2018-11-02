#ifndef mfpf_region_about_pt_h_
#define mfpf_region_about_pt_h_
//:
// \file
// \brief Region centred on a single point, pose defined by other pts
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <mfpf/mfpf_region_definer.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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
  //: Original index of point on which region is centred
  unsigned i0_orig_;
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
  std::string form_;

public:

  //: Dflt ctor
  mfpf_region_about_pt();

  //: Destructor
  ~mfpf_region_about_pt() override;

  //: Returns true as the region is centred on an input point
  bool is_centred_on_pt() const override;

  //: Returns index of reference point on which the region is centred
  unsigned ref_point_index() const override;

  //: Returns original index of reference point on which the region is centred
  unsigned orig_ref_point_index() const override;

  //: Replace each point index i with new_index[i]
  //  Allows for re-numbering of the points used.
  //  Returns true if successful.
  bool replace_index(const std::vector<unsigned>& new_index) override;

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
  std::string form() const { return form_; }

  //: Returns reference point for region, pts[i0()]
  vgl_point_2d<double> get_ref_point(
             const std::vector<vgl_point_2d<double> >& pts) const override;

  //: Defines a region centred on a point
  mfpf_region_form set_up(
             const std::vector<vgl_point_2d<double> >& pts) override;

  //: Defines a region centred on a point
  //  The aspect ratio of the region will be the same as that
  //  from the last call to set_up.
  mfpf_region_form get_region(
                const std::vector<vgl_point_2d<double> >& pts) const override;

  //: Initialise from a stream
  bool set_from_stream(std::istream &is) override;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Create a copy on the heap and return base class pointer
  mfpf_region_definer* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif
