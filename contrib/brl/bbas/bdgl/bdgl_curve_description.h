#ifndef bdgl_curve_description_h_
#define bdgl_curve_description_h_

//-----------------------------------------------------------------------------
//:
// \file
// \author P.L. Bazin
// \brief interaction of vdgl_digital_curve and 2d images
//
// \verbatim
// Initial version January 24, 2003
// Modifications
// 21 August 2003 - Vishal Jain : added some more properties such as angles,points
// bounding_box.
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vgl/vgl_point_2d.h>
#include <vsol/vsol_box_2d_sptr.h>

class bdgl_curve_description
{
 public:
  vgl_point_2d<double> center_;
  double               curvature_;
  double               length_;
  double               gradient_mean_val_;
  double               gradient_mean_dir_;
  double               gradient_std_val_;
  double               gradient_std_dir_;

  vcl_vector<vgl_point_2d<double> > points_;
  vcl_vector<vgl_point_2d<double> > coarser_points_;
  vcl_vector<double> angles_;
  vcl_vector<double> grad_;
  vsol_box_2d_sptr   box_;

  //:Constructors/Destructor-------------------------
  ~bdgl_curve_description(){}

  bdgl_curve_description();
  //:Creates a description from a digital curve (edgel chain)
  bdgl_curve_description(vdgl_edgel_chain_sptr  ec)
  {
    init(ec);
    compute_bounding_box(ec);
  }
  void compute_bounding_box(vdgl_edgel_chain_sptr const& ec);
  void init(vdgl_edgel_chain_sptr const& ec);

  // display information
  void info();
};

#endif
