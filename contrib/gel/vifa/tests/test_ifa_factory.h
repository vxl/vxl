#ifndef _TEST_IFA_FACTORY_H
#define _TEST_IFA_FACTORY_H

#include <iostream>
#include <utility>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vdgl/vdgl_fit_lines_params.h>
#include <vifa/vifa_int_face_attr_factory.h>
#include <vifa/vifa_group_pgram_params.h>
#include <vifa/vifa_norm_params.h>
#include <vifa/vifa_typedefs.h>

typedef std::pair<vtol_intensity_face_sptr, vifa_int_face_attr_sptr>  ifp;

class test_ifa_factory : public vifa_int_face_attr_factory
{
  std::vector<ifp>*  table_;
 public:
  test_ifa_factory();
  ~test_ifa_factory() override;

  vifa_int_face_attr_sptr
    obtain_int_face_attr(vtol_intensity_face_sptr  face) override;

  vifa_int_face_attr_sptr
    obtain_int_face_attr(vtol_intensity_face_sptr  face,
                         vdgl_fit_lines_params*    fpp,
                         vifa_group_pgram_params*  gpp_s,
                         vifa_group_pgram_params*  gpp_w,
                         vifa_norm_params*         np
                        ) override;
  void reset() override;
};

#endif  // _TEST_IFA_FACTORY_H
