#include <iostream>
#include "acal_io_f_utils.h"


// -----f_params-----

//: Binary save object to stream.
void
vsl_b_write(vsl_b_ostream & os, const f_params& obj)
{
  constexpr short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, obj.epi_dist_mul_);
  vsl_b_write(os, obj.max_epi_dist_);
  vsl_b_write(os, obj.F_similar_abcd_tol_);
  vsl_b_write(os, obj.F_similar_e_tol_);
  vsl_b_write(os, obj.ray_uncertainty_tol_);
  vsl_b_write(os, obj.min_num_matches_);
}

//: Binary load object from stream.
void
vsl_b_read(vsl_b_istream & is, f_params& obj)
{
  if (!is) return;

  short io_version_no;
  vsl_b_read(is, io_version_no);
  switch (io_version_no)
  {
    case 1:
      vsl_b_read(is, obj.epi_dist_mul_);
      vsl_b_read(is, obj.max_epi_dist_);
      vsl_b_read(is, obj.F_similar_abcd_tol_);
      vsl_b_read(is, obj.F_similar_e_tol_);
      vsl_b_read(is, obj.ray_uncertainty_tol_);
      vsl_b_read(is, obj.min_num_matches_);
      break;

    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, f_params&), "
                << "Unknown version number "<< io_version_no << std::endl;
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

