#ifndef _TEST_IFA_FACTORY_H
#define _TEST_IFA_FACTORY_H

#include <vcl_utility.h>
#include <vcl_vector.h>
#include <vdgl/vdgl_fit_lines_params.h>
#include <vifa/vifa_int_face_attr_factory.h>
#include <vifa/vifa_group_pgram_params.h>
#include <vifa/vifa_norm_params.h>
#include <vifa/vifa_typedefs.h>

typedef vcl_pair<vtol_intensity_face_sptr, vifa_int_face_attr_sptr>  ifp;

class test_ifa_factory : public vifa_int_face_attr_factory
{
  vcl_vector<ifp>*  table_;
 public:
  test_ifa_factory();
  virtual ~test_ifa_factory();

  virtual vifa_int_face_attr_sptr
    obtain_int_face_attr(vtol_intensity_face_sptr  face);

  virtual vifa_int_face_attr_sptr
    obtain_int_face_attr(vtol_intensity_face_sptr  face,
                         vdgl_fit_lines_params*    fpp,
                         vifa_group_pgram_params*  gpp_s,
                         vifa_group_pgram_params*  gpp_w,
                         vifa_norm_params*         np
                        );
  void reset();
};

#endif  // _TEST_IFA_FACTORY_H
