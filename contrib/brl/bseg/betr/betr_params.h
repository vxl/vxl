#ifndef betr_params_h_
#define betr_params_h_
//:
// \file
// \brief A base class for holding parameters
// \author J.L. Mundy
// \date October 15, 2016
//
#include <string>
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>
#include <bjson/bjson.h>
class betr_params: public vbl_ref_count
{
 public:
  betr_params() = default;
  ~betr_params() override = default;
  //: check values of parameters to flag illegal values
  virtual bool sanity_check(std::string& errors) const = 0;
  virtual void serialize( Json::Value& root ) const = 0;
  virtual void deserialize( Json::Value& root)=0;
    };
#include "betr_params_sptr.h"
bool write_params_json(std::ostream& ostr, betr_params_sptr const& params);
bool read_params_json(std::istream& istr, betr_params_sptr& params);
bool write_params_json(std::string& json_str, betr_params_sptr const& params);
bool read_params_json(std::string const& json_str, betr_params_sptr& params);
#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_params.
