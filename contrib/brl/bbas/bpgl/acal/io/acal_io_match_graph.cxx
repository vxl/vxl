#include <iostream>
#include "acal_io_match_graph.h"


// -----match_params-----

//: Binary save object to stream
void
vsl_b_write(vsl_b_ostream & os, const match_params& obj)
{
  constexpr short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, obj.min_n_tracks_);
  vsl_b_write(os, obj.min_n_cams_);
  vsl_b_write(os, obj.max_proj_error_);
  vsl_b_write(os, obj.max_uncal_proj_error_);
}

//: Binary load object from stream
void
vsl_b_read(vsl_b_istream & is, match_params& obj)
{
  if (!is) return;

  short io_version_no;
  vsl_b_read(is, io_version_no);
  switch (io_version_no)
  {
    case 1:
      vsl_b_read(is, obj.min_n_tracks_);
      vsl_b_read(is, obj.min_n_cams_);
      vsl_b_read(is, obj.max_proj_error_);
      vsl_b_read(is, obj.max_uncal_proj_error_);
      break;

    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, match_params&), "
                << "Unknown version number "<< io_version_no << std::endl;
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}


// -----acal_match_graph-----

//: Binary save object to stream
void
vsl_b_write(vsl_b_ostream & os, const acal_match_graph& obj)
{
  constexpr short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  // TODO
}

//: Binary load object from stream
void
vsl_b_read(vsl_b_istream & is, acal_match_graph& obj)
{
  if (!is) return;

  short io_version_no;
  vsl_b_read(is, io_version_no);
  switch (io_version_no)
  {
    case 1:
      // TODO
      break;

    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, acal_match_graph&), "
                << "Unknown version number "<< io_version_no << std::endl;
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}
