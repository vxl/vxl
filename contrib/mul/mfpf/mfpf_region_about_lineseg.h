#ifndef mfpf_region_about_lineseg_h_
#define mfpf_region_about_lineseg_h_

//:
// \file
// \brief Region centred on line segment between two points
// \author Tim Cootes

#include <mfpf/mfpf_region_definer.h>
#include <vcl_iosfwd.h>

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
  vcl_string form_;

public:

  //: Dflt ctor
  mfpf_region_about_lineseg();

  //: Destructor
  virtual ~mfpf_region_about_lineseg();

  //: Returns false as the region is not centred on an input point
  virtual bool is_centred_on_pt() const;

  //: Returns zero
  virtual unsigned ref_point_index() const;

  //: Returns zero also
  virtual unsigned orig_ref_point_index() const;

  //: Replace each point index i with new_index[i]
  //  Allows for re-numbering of the points used.
  //  Returns true if successful.
  virtual bool replace_index(const vcl_vector<unsigned>& new_index);

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
  vcl_string form() const { return form_; }

  //: Returns reference point for region, mid_point(pts[i0],pts[i1])
  virtual vgl_point_2d<double> get_ref_point(
             const vcl_vector<vgl_point_2d<double> >& pts) const;

  //: Defines a region centred on the line between pts[i0] and pts[i1]
  virtual mfpf_region_form set_up(
             const vcl_vector<vgl_point_2d<double> >& pts);

  //: Defines a region centred on the line between pts[i0] and pts[i1]
  //  The aspect ratio of the region will be the same as that
  //  from the last call to set_up.
  virtual mfpf_region_form get_region(
                const vcl_vector<vgl_point_2d<double> >& pts) const;

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
