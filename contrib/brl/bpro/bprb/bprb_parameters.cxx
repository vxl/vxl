// This is brl/bpro/bprb/bprb_parameters.cxx
#include <utility>
#include <iostream>
#include "bprb_parameters.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bxml/bxml_read.h>
#include <bxml/bxml_find.h>
#include <bxml/bxml_write.h>

//: Output stream operator for bprb_params
std::ostream& operator<<(std::ostream& os, const bprb_param& p)
{
  os << "parameter{\n  Description: " << p.description();
  if (p.has_bounds())
    os << "\n  Range: " << p.min_str() << " to " << p.max_str();
  os << "\n  Default: " << p.default_str()
     << "\n  Value: " << p.value_str() << "\n}\n";

  return os;
}

//==============================================================================

//: Constructor
bprb_parameters::bprb_parameters()
= default;


//: Destructor
bprb_parameters::~bprb_parameters()
{
  for (auto & it : param_list_) {
    delete it;
  }
}

//: Deep copy constructor
bprb_parameters::bprb_parameters(const bprb_parameters_sptr& old_params)
{
  for (auto & it : old_params->param_list_) {

    //deep copy this param
    bprb_param * new_param = it->clone();

    param_list_.push_back( new_param );
    name_param_map_.insert( std::pair< std::string , bprb_param* >( new_param->name() , new_param ) );
  }
}


//: Returns true if a parameter exists with \p flag
bool
bprb_parameters::valid_parameter( const std::string& name ) const
{
  auto itr = name_param_map_.find( name );
  return itr != name_param_map_.end();
}


//: reads the parameters and their values from an XML document
// It assumes the following XML format
// <ProcessName>
//   <param1 type="" desc="" value=""/>
//   <param2 type="" desc="" value=""/>
//   .
//   .
// </ProcessName>
bool bprb_parameters::parse_XML(const std::string& xml_path,
                                const std::string& root_tag)
{
  // open the XML document
  if (xml_path.size() == 0) {
    std::cout << "bprb_parameters::parse_XML -- xml file path is not set" << std::endl;
    return false;
  }

  bxml_document xml_doc_ = bxml_read(xml_path);
  if (!xml_doc_.root_element()) {
    std::cout << "bprb_parameters::parse_XML -- xml root not found" << std::endl;
    return false;
  }

  if (xml_doc_.root_element()->type() != bxml_data::ELEMENT) {
    std::cout << "bprb_parameters::parse_XML params root is not ELEMENT" << std::endl;
    return false;
  }

  bxml_data_sptr root;
  // if the root tag is not defined, used the document root, else find the element
  if (root_tag.size() == 0)
    //root = static_cast<bxml_element*> (xml_doc_.root_element().as_pointer());
    root = xml_doc_.root_element();
  else {
    bxml_element query(root_tag);
    //root = static_cast<bxml_element*> (bxml_find_by_name(xml_doc_.root_element(), query).as_pointer());
    root = bxml_find_by_name(xml_doc_.root_element(), query);
    if (!root) {
      std::cout << "bprb_parameters::parse_XML root tag: " << root_tag << " is not found" << std::endl;
      return false;
    }
  }

  // iterate over he elements and find out their types
  auto* h_elm = static_cast<bxml_element*>(root.ptr());
  for (auto i = h_elm->data_begin(); i != h_elm->data_end();  ++i) {
    bxml_data_sptr elm = *i;
    if (elm->type() == bxml_data::ELEMENT) {
      auto* param = static_cast<bxml_element*> (elm.as_pointer());
      if (param) {
        std::string value = param->attribute("value");
        std::string type = param->attribute("type");
        std::string desc = param->attribute("desc");
        bprb_param* p=nullptr;
        if (!type.compare("float")) {
          p =  new bprb_param_type<float>(param->name(), desc, 0);
        } else if ( (!type.compare("unsigned int")) || (!type.compare("unsigned")) ) {
          p =  new bprb_param_type<unsigned>(param->name(), desc, 0);
        } else if (!type.compare("int")) {
          p =  new bprb_param_type<int>(param->name(), desc, 0);
        } else if (!type.compare("string")) {
          p =  new bprb_param_type<std::string>(param->name(), desc, "");
        } else if (!type.compare("bool")) {
          p =  new bprb_param_type<bool>(param->name(), desc, false);
        } else if (!type.compare("double")) {
          p =  new bprb_param_type<double>(param->name(), desc, false);
        } else {
          std::cerr << "Parsing Error: Unknown parameter type \"" << type << "\"" << std::endl;
          continue; // maybe should abort and return false here?
        }
        p->parse_value_str(value);
        param_list_.push_back(p);
        name_param_map_[p->name()] = p;
      }
    }
  }

  return true;
}

//: prints the default parameter values to an XML document
void bprb_parameters::print_def_XML(const std::string& root_tag,
                                    const std::string& xml_path)
{
  bxml_element* root = new bxml_element(root_tag);
  root->append_text("\n");
  // iterate over each parameter, and get the default ones
  for (auto & it : param_list_) {
    std::string name = it->name();
    std::string def_value = it->default_str();
    bxml_element* param_elem = new bxml_element(name);
    param_elem->set_attribute("type", it->type_str());
    param_elem->set_attribute("desc", it->description());
    param_elem->set_attribute("value", def_value);
    root->append_data(param_elem);
    root->append_text("\n");
  }
  bxml_document doc;
  doc.set_root_element(root);
  bxml_write(xml_path, doc);
}

//: prints the currently used parameter values to an XML document
void bprb_parameters::print_current_XML(const std::string& root_tag,
                                        const std::string& xml_path)
{
  bxml_element* root = new bxml_element(root_tag);
  root->append_text("\n");
  // iterate over each parameter, and get the default ones
  for (auto & it : param_list_) {
    std::string name = it->name();
    std::string value = it->value_str();
    bxml_element* param_elem = new bxml_element(name);
    param_elem->set_attribute("type", it->type_str());
    param_elem->set_attribute("desc", it->description());
    param_elem->set_attribute("value", value);
    root->append_data(param_elem);
    root->append_text("\n");
  }
  bxml_document doc;
  doc.set_root_element(root);
  bxml_write(xml_path, doc);
}

//: Reset all parameters to their default values
bool
bprb_parameters::reset_all()
{
  for (auto & it : param_list_) {
    it->reset();
  }
  return true;
}


//: Reset the parameter named \p name to its default value
bool
bprb_parameters::reset( const std::string& name )
{
  auto it = name_param_map_.find( name );
  if ( it == name_param_map_.end() ) {
    return false;
  }

  it->second->reset();

  return true;
}


//: Return a vector of base class pointers to the parameters
std::vector< bprb_param* >
bprb_parameters::get_param_list() const
{
  return param_list_;
}


//: Return the description of the parameter named \p name
std::string
bprb_parameters::get_desc( const std::string& name ) const
{
  auto it = name_param_map_.find( name );
  if ( it == name_param_map_.end() ) {
    return "";
  }
  return it->second->description();
}


//: Print all parameters to \p os
void
bprb_parameters::print_all(std::ostream& os) const
{
  for (auto it : param_list_) {
    os << it;
  }
}


//: Add parameter helper function
bool
bprb_parameters::add( bprb_param* param )
{
  if ( !param )
    return false;
  std::string name = param->name();
  std::string desc = param->description();
  if ( name_param_map_.find( name ) != name_param_map_.end() ||
       desc == "" || name == "" ) {
    delete param;
    return false;
  }

  param_list_.push_back( param );
  name_param_map_.insert( std::pair< std::string , bprb_param* >( name , param ) );

  return true;
}

//==============================================================================

//: Less than operator for bprb_filepath objects
bool operator<( const bprb_filepath& lhs, const bprb_filepath& rhs )
{
  return lhs.path < rhs.path;
}

//: Less than or equal to operator for bprb_filepath objects
bool operator<=( const bprb_filepath& lhs, const bprb_filepath& rhs )
{
  return lhs.path <= rhs.path;
}

//: Output stream operator for bprb_filepath objects
std::ostream& operator<<( std::ostream& strm, const bprb_filepath& fp )
{
  strm << fp.path << '\n' << fp.ext << std::ends;
  return strm;
}

//: Input stream operator for bprb_filepath objects
std::istream& operator>>( std::istream& strm, bprb_filepath& fp )
{
  strm >> fp.path >> fp.ext;
  return strm;
}
