#ifndef brct_volume_processor_h_
#define brct_volume_processor_h_

//:
// \file
// \brief A class to compute volume occupancy statistics
//
// \author J.L. Mundy
// \verbatim
//  Initial version March. 1, 2004
// \endverbatim
//
//////////////////////////////////////////////////////////////////////

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vsol/vsol_box_3d_sptr.h>
#include <bsol/bsol_point_index_3d.h>
#include <brct/brct_volume_processor_params.h>
class brct_volume_processor : public brct_volume_processor_params
{
 public:
  brct_volume_processor(brct_volume_processor_params const& sp);
  ~brct_volume_processor();

  // Accessors

  //:Mutators
  bool read_points_3d_vrml(vcl_string const&  filename);
  bool read_change_data_vrml(vcl_string const&  filename);
  //: Utility functions
  bool write_prob_volumes_vrml(vcl_string const&  filename);
  bool compute_change();
  bool write_changed_volumes_vrml(vcl_string const&  filename);

 protected:
  //members
  int ncols_;
  int nrows_;
  int nslabs_;
  vsol_box_3d_sptr box_;
  bsol_point_index_3d* index_;
  bsol_point_index_3d* change_index_;
  vcl_vector<vsol_box_3d_sptr> change_volumes_;
};

#endif // brct_volume_processor_h_
