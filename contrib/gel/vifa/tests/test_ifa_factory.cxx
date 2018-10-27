#include "test_ifa_factory.h"
#include <vtol/vtol_intensity_face.h>

test_ifa_factory::
test_ifa_factory()
{
  table_ = new std::vector<ifp>;
}

test_ifa_factory::
~test_ifa_factory()
{
  delete table_;
}

void test_ifa_factory::
reset()
{
  delete table_;
  table_ = new std::vector<ifp>;
}

vifa_int_face_attr_sptr test_ifa_factory::
obtain_int_face_attr(vtol_intensity_face_sptr  face)
{
  auto  ifpi = table_->begin();

  for (; ifpi != table_->end(); ifpi++)
  {
    // Lookup the face in the cache
    if ((*ifpi).first.ptr() == face.ptr())
    {
      // Return the cached attributes
      return (*ifpi).second;
    }
  }

  // Cache miss
  return nullptr;
}

vifa_int_face_attr_sptr test_ifa_factory::
obtain_int_face_attr(vtol_intensity_face_sptr  face,
                     vdgl_fit_lines_params*    fpp,
                     vifa_group_pgram_params*  gpp_s,
                     vifa_group_pgram_params*  gpp_w,
                     vifa_norm_params*         np)
{
  auto  ifpi = table_->begin();
  for (; ifpi != table_->end(); ifpi++)
  {
    // Lookup the face in the cache
    if ((*ifpi).first.ptr() == face.ptr())
    {
      return (*ifpi).second;
    }
  }

  // New face -- create the attributes
  vifa_int_face_attr_sptr  ifa = new vifa_int_face_attr(face,
                                                        fpp,
                                                        gpp_s,
                                                        gpp_w,
                                                        np);

  // Add a face/attribute pair to the cache
  table_->push_back(std::make_pair(face, ifa));
  return ifa;
}
