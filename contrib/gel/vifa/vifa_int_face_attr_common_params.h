// This is gel/vifa/vifa_int_face_attr_common_params.h
#ifndef VIFA_INT_FACE_ATTR_COMMON_PARAMS_H
#define VIFA_INT_FACE_ATTR_COMMON_PARAMS_H

//-----------------------------------------------------------------------------
//:
// \file
// \brief Parameter mixin for intensity face attribute computation.
//
// \author Anthony Hoogs, from DDB in TargetJr
//
// \date January 2001
//
// The parameter mixin for intensity face attribute computation, for
// attributes common to single faces and groups of faces.
//
// \verbatim
//  Modifications:
//   MPP Mar/Apr 2003, Ported to VXL
// \endverbatim
//-----------------------------------------------------------------------------

#include <gevd/gevd_param_mixin.h>
#include <vul/vul_timestamp.h>
#include <vbl/vbl_ref_count.h>
#include <sdet/sdet_fit_lines_params.h>
#include <vifa/vifa_coll_lines_params.h>
#include <vifa/vifa_group_pgram_params.h>
#include <vifa/vifa_norm_params.h>
#include <vifa/vifa_typedefs.h>


//: Container holding parameters for common intensity face attribute computations.
class vifa_int_face_attr_common_params : public gevd_param_mixin,
                     public vul_timestamp,
                     public vbl_ref_count
{
 public:
  //: Line fitting parameters (incl. fit_length).
  sdet_fit_lines_params_sptr    _fitter_params;

  //: Strong projected parallelism parameters.
  vifa_group_pgram_params_sptr  _gpp_s;

  //: Weak projected parallelism parameters.
  vifa_group_pgram_params_sptr  _gpp_w;

  //: Collinearization parameters.
  vifa_coll_lines_params_sptr    _cpp;

  //: Pixel normalization parameters.
  vifa_norm_params_sptr      _np;

 public:
  //: Default constructor.
  vifa_int_face_attr_common_params(sdet_fit_lines_params*    fitter_params = NULL,
                                   vifa_group_pgram_params*  gpp_s = NULL,
                                   vifa_group_pgram_params*  gpp_w = NULL,
                                   vifa_coll_lines_params*  cpp = NULL,
                                   vifa_norm_params*      np = NULL
                                  );

  //: Copy constructor.
  vifa_int_face_attr_common_params(const vifa_int_face_attr_common_params&  old_params);

  //: Destructor.
  virtual ~vifa_int_face_attr_common_params(void);

  //: Publicly available setting of parameters.
  void  set_params(const vifa_int_face_attr_common_params&  ifap);

  //: Check that parameters are within acceptable bounds.
  bool  SanityCheck();

  //: Output contents
  void  print_info(void);

 protected:
  //: Assign internal parameter blocks.
  void  init_params(sdet_fit_lines_params*    fitter_params,
                    vifa_group_pgram_params*  gpp_s,
                    vifa_group_pgram_params*  gpp_w,
                    vifa_coll_lines_params*   cpp,
                    vifa_norm_params*         np
                   );
  void  init_params(const sdet_fit_lines_params&  fitter_params,
                    const vifa_group_pgram_params&  gpp_s,
                    const vifa_group_pgram_params&  gpp_w,
                    const vifa_coll_lines_params&   cpp,
                    const vifa_norm_params&         np
                   );
};


#endif  // VIFA_INT_FACE_ATTR_COMMON_PARAMS_H
