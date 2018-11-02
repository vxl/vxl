#ifndef mfpf_region_about_lineseg_h_
#define mfpf_region_about_lineseg_h_

//:
// \file
// \brief Region centred on line segment between two points
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <mfpf/mfpf_region_definer.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Region centred on a line between two points.
//  Region defined by two points, p0=pts[i0], p1=pts[i1]
//  The region is centred on 0.5(p0+p1)
//  Bounding box of region is aligned with the vector (p1-p0),
//  and its width and height are defined relative to |p1-p0|
//
//  Parameters in text file:
//  \verbatim
//  { i0: 5 i1: 31 rel_wi: 0.5 rel_wj: 0.2 form: box }
//  \endverbatim
class mfpf_region_about_lineseg : public mfpf_region_definer
{
private:
  //: Index of first point defining reference frame
  unsigned i0_;
  //: Index of second point defining reference frame
  unsigned i1_;

  //: Width of ROI, relative to |p1-p0|
  double rel_wi_;

  //: Height of ROI, relative to |p1-p0|
  double rel_wj_;

  //: Width of ROI in world coords
  double wi_;

  //: Height of ROI in world coords
  double wj_;

  //: Name of form of shape ("box","ellipse")
  std::string form_;

public:

  //: Dflt ctor
  mfpf_region_about_lineseg();

  //: Destructor
  ~mfpf_region_about_lineseg() override;

  //: Returns false as the region is not centred on an input point
  bool is_centred_on_pt() const override;

  //: Returns zero
  unsigned ref_point_index() const override;

  //: Returns zero also
  unsigned orig_ref_point_index() const override;

  //: Replace each point index i with new_index[i]
  //  Allows for re-numbering of the points used.
  //  Returns true if successful.
  bool replace_index(const std::vector<unsigned>& new_index) override;

  //: Index of first point defining reference frame
  unsigned i0() const { return i0_; }

  //: Index of second point defining reference frame
  unsigned i1() const { return i1_; }

  //: Width of ROI, relative to |p1-p0|
  double rel_wi() const { return rel_wi_; }

  //: Height of ROI, relative to |p1-p0|
  double rel_wj() const { return rel_wj_; }

  //: Width of ROI in world coords
  double wi() const { return wi_; }

  //: Height of ROI in world coords
  double wj() const { return wj_; }

  //: Name of form of shape ("box","ellipse")
  std::string form() const { return form_; }

  //: Returns reference point for region, mid_point(pts[i0],pts[i1])
  vgl_point_2d<double> get_ref_point(
             const std::vector<vgl_point_2d<double> >& pts) const override;

  //: Defines a region centred on the line between pts[i0] and pts[i1]
  mfpf_region_form set_up(
             const std::vector<vgl_point_2d<double> >& pts) override;

  //: Defines a region centred on the line between pts[i0] and pts[i1]
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
