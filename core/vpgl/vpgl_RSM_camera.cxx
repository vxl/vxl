// This is core/vpgl/vpgl_RSM_camera.cxx
#include "vpgl_RSM_camera.h"

// string from vpgl_ground_domain_id
std::string
vpgl_ground_domain_id_func::to_string(vpgl_ground_domain_id id)
{
  switch (id)
  {
    case vpgl_ground_domain_id::G:
      return "G";
    case vpgl_ground_domain_id::H:
      return "H";
    case vpgl_ground_domain_id::R:
      return "R";
    default:
      throw std::invalid_argument("vpgl_ground_domain ID not recognized");
  }
}

// vpgl_ground_domain_id from string
vpgl_ground_domain_id
vpgl_ground_domain_id_func::from_string(const std::string & buf)
{
  if (buf == "G")
    return vpgl_ground_domain_id::G;
  else if (buf == "H")
    return vpgl_ground_domain_id::H;
  else if (buf == "R")
    return vpgl_ground_domain_id::R;
  else
    throw std::invalid_argument("string not recognized as vpgl_ground_domain ID");
}

// vpgl_ground_domain_id operator<<
std::ostream &
operator<<(std::ostream & os, const vpgl_ground_domain_id & id)
{
  return os << vpgl_ground_domain_id_func::to_string(id);
}
