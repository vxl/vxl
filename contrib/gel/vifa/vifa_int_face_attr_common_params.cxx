// This is gel/vifa/vifa_int_face_attr_common_params.cxx
#include <vifa/vifa_int_face_attr_common_params.h>
//:
// \file
#include <vcl_iostream.h>
#include <vcl_sstream.h>


//: Default constructor
vifa_int_face_attr_common_params::
vifa_int_face_attr_common_params(vdgl_fit_lines_params*    fitter_params,
                                 vifa_group_pgram_params*  gpp_s,
                                 vifa_group_pgram_params*  gpp_w,
                                 vifa_coll_lines_params*   cpp,
                                 vifa_norm_params*         np
                )
{
  init_params(fitter_params,
              gpp_s,
              gpp_w,
              cpp,
              np);
}

vifa_int_face_attr_common_params::
vifa_int_face_attr_common_params(const vifa_int_face_attr_common_params&  ifap)
  : gevd_param_mixin(), vul_timestamp(), vbl_ref_count()
{
  init_params(ifap.fitter_params_.ptr(),
              ifap.gpp_s_.ptr(),
              ifap.gpp_w_.ptr(),
              ifap.cpp_.ptr(),
              ifap.np_.ptr());
}

vifa_int_face_attr_common_params::
~vifa_int_face_attr_common_params()
{
  // Smart-pointer parameter blocks self-destruct upon destruction
}

//: Publicly available setting of parameters
void vifa_int_face_attr_common_params::
set_params(const vifa_int_face_attr_common_params&  ifap)
{
  init_params(ifap.fitter_params_.ptr(),
              ifap.gpp_s_.ptr(),
              ifap.gpp_w_.ptr(),
              ifap.cpp_.ptr(),
              ifap.np_.ptr());
}

//: Checks that parameters are within acceptable bounds
bool vifa_int_face_attr_common_params::SanityCheck()
{
  vcl_stringstream  msg;
  bool valid;

  if (!fitter_params_.ptr() ||
      !gpp_s_.ptr() ||
      !gpp_w_.ptr() ||
      !cpp_.ptr())
  {
    msg << "ERROR: {FitLines, GroupParallelogram, CollinearLines} params not set." << vcl_ends;
    valid = false;
  }
  else
  {
    // Result is result of contained parameters' SanityCheck()'s
    valid = fitter_params_->SanityCheck() &&
            gpp_s_->SanityCheck() &&
            gpp_w_->SanityCheck() &&
            cpp_->SanityCheck();
  }

  SetErrorMsg(msg.str().c_str());
  return valid;
}

void vifa_int_face_attr_common_params::
print_info(void)
{
  vcl_cout << "vifa_int_face_attr_common_params:\n"
           << "  line fitting params: ";
  if (fitter_params_.ptr())
    vcl_cout << *fitter_params_;
  else
    vcl_cout << "NULL\n";
  vcl_cout << "  strong group parallelogram params: ";
  if (gpp_s_.ptr())
    gpp_s_->print_info();
  else
    vcl_cout << "NULL\n";
  vcl_cout << "  weak group parallelogram params: ";
  if (gpp_w_.ptr())
    gpp_w_->print_info();
  else
    vcl_cout << "NULL\n";
  vcl_cout << "  collinear lines params: ";
  if (cpp_.ptr())
    cpp_->print_info();
  else
    vcl_cout << "NULL\n";
  vcl_cout << "  normalization params: ";
  if (np_.ptr())
    np_->print_info();
  else
    vcl_cout << "NULL\n";
}

//: Assign internal parameter blocks
void vifa_int_face_attr_common_params::
init_params(vdgl_fit_lines_params*    fitter_params,
            vifa_group_pgram_params*  gpp_s,
            vifa_group_pgram_params*  gpp_w,
            vifa_coll_lines_params*   cpp,
            vifa_norm_params*         np
           )
{
  if (fitter_params)
    fitter_params_ = new vdgl_fit_lines_params(*fitter_params);
  else
    fitter_params_ = new vdgl_fit_lines_params(/*min_fit_length=*/ 6);
  if (gpp_s)
    gpp_s_ = new vifa_group_pgram_params(*gpp_s);
  else
    gpp_s_ = new vifa_group_pgram_params(/*angle_increment=*/ 5.0);
  if (gpp_w)
    gpp_w_ = new vifa_group_pgram_params(*gpp_w);
  else
    gpp_w_ = new vifa_group_pgram_params(/*angle_increment=*/ 20.0);
  if (cpp)
    cpp_ = new vifa_coll_lines_params(*cpp);
  else
    cpp_ = new vifa_coll_lines_params();
  if (np)
    np_ = new vifa_norm_params(*np);
  else
    np_ = new vifa_norm_params();
}


//: Assign internal parameter blocks
void vifa_int_face_attr_common_params::
init_params(const vdgl_fit_lines_params&   fitter_params,
            const vifa_group_pgram_params& gpp_s,
            const vifa_group_pgram_params& gpp_w,
            const vifa_coll_lines_params&  cpp,
            const vifa_norm_params&        np
           )
{
  fitter_params_ = new vdgl_fit_lines_params(fitter_params);
  gpp_s_ = new vifa_group_pgram_params(gpp_s);
  gpp_w_ = new vifa_group_pgram_params(gpp_w);
  cpp_ = new vifa_coll_lines_params(cpp);
  np_ = new vifa_norm_params(np);
}
