#include "betr_pixelwise_change_detection_params.h"
#include "betr_params.h"


//--------------------------------------------------------------------------
bool betr_pixelwise_change_detection_params::sanity_check(std::string&  /*errors*/) const{
  return true;
}


//--------------------------------------------------------------------------
std::ostream&  operator<<(
  std::ostream& s, betr_pixelwise_change_detection_params const& ecdp)
{
  betr_params_sptr params_ptr = new betr_pixelwise_change_detection_params(ecdp);
  bool good = write_params_json(s, params_ptr);
  if(!good)
    std::cout << "JSON serialize failed to write params" << std::endl;
  return s;
}


//--------------------------------------------------------------------------
std::istream&  operator>>(
  std::istream& s, betr_pixelwise_change_detection_params& ecdp)
{
  auto* cd_params = new betr_pixelwise_change_detection_params();
  betr_params_sptr params_ptr = cd_params;
  bool good = read_params_json(s, params_ptr);
  if(good)
    ecdp = *cd_params;
  else
    std::cout << "Failed to parse JSON file for betr_pixelwise_change_detection_params" << std::endl;
  return s;
}
