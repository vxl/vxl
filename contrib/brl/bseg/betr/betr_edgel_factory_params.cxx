#include "betr_edgel_factory_params.h"
bool betr_edgel_factory_params::sanity_check(std::string&  /*errors*/) const{
  return true;
}
std::ostream&  operator<<(std::ostream& s, betr_edgel_factory_params const& efp){
  betr_params_sptr params_ptr = new betr_edgel_factory_params(efp);
  bool good = write_params_json(s, params_ptr);
  if(!good)
    std::cout << "JSON serialize failed to write params" << std::endl;
  return s;
}
std::istream&  operator>>(std::istream& s, betr_edgel_factory_params& efp){
  auto* cd_params = new betr_edgel_factory_params();
  betr_params_sptr params_ptr = cd_params;
  bool good = read_params_json(s, params_ptr);
  if(good)
    efp = *cd_params;
  else
    std::cout << "Failed to parse JSON file for betr_edgel_factory_params" << std::endl;
  return s;
}
