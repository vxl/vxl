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
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vgl/vgl_point_2d.h>

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

  //:Constructors/Destructor-------------------------
  ~bdgl_curve_description(){}

  bdgl_curve_description();
  //:Creates a description from a digital curve (edgel chain)
  bdgl_curve_description(vdgl_edgel_chain_sptr const& ec){ init(ec); }
  void init(vdgl_edgel_chain_sptr const& ec);

  // display information
  void info();
};

#endif
