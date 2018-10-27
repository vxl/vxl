#include <sstream>
#include "betr_params.h"
#include <bjson/bjson.h>
bool write_params_json(std::string& json_str, betr_params_sptr const& params){
  if(!params)
    return false;
  std::string json_string;
  Json::Value serialize_root;
  params->serialize(serialize_root);
  Json::StyledWriter writer;
  json_str = writer.write(serialize_root);
  return true;
}
bool read_params_json(std::string const& json_str, betr_params_sptr& params){
  Json::Value deserialize_root;
  Json::Reader reader;
  bool good = reader.parse(json_str, deserialize_root);
  if(!good)
    return false;
  params->deserialize(deserialize_root);
  return true;
}
bool write_params_json(std::ostream& ostr, betr_params_sptr const& params){
  if(!ostr)
          return false;
  std::string json_string;
  bool good = write_params_json(json_string, params);
  if(!good)
    return false;
  ostr << json_string;
  return true;
}
bool read_params_json(std::istream& istr, betr_params_sptr& params){
  if(!istr || !params)
    return false;

  std::string json_string;
  std::stringstream sstr;
  std::string temp;
  while(istr){
    istr >> temp;
    sstr << temp;
  }
  return read_params_json(sstr.str(), params);
}
