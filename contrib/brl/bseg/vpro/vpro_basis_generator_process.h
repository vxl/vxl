// This is brl/bseg/vpro/vpro_basis_generator_process.h
#ifndef vpro_basis_generator_process_h_
#define vpro_basis_generator_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief  Generates an image sequence corresponding to a basis
// \author J.L. Mundy
//
// \verbatim
//  Modifications
//   J.L. Mundy - October 16, 2003 - Initial version.
// \endverbatim
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vil1/vil1_memory_image_of.h>
#include <vpro/vpro_video_process.h>

class vpro_basis_generator_process : public vpro_video_process
{
 public:
  vpro_basis_generator_process(vcl_string const & video_file);
  ~vpro_basis_generator_process();
  virtual process_data_type get_output_type() { return NOTYPE; }
  //: compute edges on the input image
  virtual bool execute();
  virtual bool finish();
 private:
  //members
  vcl_string video_file_;
  vcl_vector<vil1_memory_image_of<float> > frames_;
};

#endif // vpro_basis_generator_process_h_
