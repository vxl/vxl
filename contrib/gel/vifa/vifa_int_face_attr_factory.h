// This is gel/vifa/vifa_int_face_attr_factory.h
#ifndef _VIFA_INT_FACE_ATTR_FACTORY_H_
#define _VIFA_INT_FACE_ATTR_FACTORY_H_

#include <vdgl/vdgl_fit_lines_params.h>
#include <vtol/vtol_intensity_face_sptr.h>
#include <vifa/vifa_group_pgram_params.h>
#include <vifa/vifa_int_face_attr.h>
#include <vifa/vifa_norm_params.h>
#include <vifa/vifa_typedefs.h>


class vifa_int_face_attr_factory
{
 public:
  vifa_int_face_attr_factory();
  virtual ~vifa_int_face_attr_factory();

  virtual vifa_int_face_attr_sptr
    obtain_int_face_attr(vtol_intensity_face_sptr  face) = 0;

  virtual vifa_int_face_attr_sptr
    obtain_int_face_attr(vtol_intensity_face_sptr  face,
                         vdgl_fit_lines_params*    fpp,
                         vifa_group_pgram_params*  gpp_s,
                         vifa_group_pgram_params*  gpp_w,
                         vifa_norm_params*      np
                        ) = 0;

  virtual void reset() = 0;
};

#endif  // _VIFA_INT_FACE_ATTR_FACTORY_H_
