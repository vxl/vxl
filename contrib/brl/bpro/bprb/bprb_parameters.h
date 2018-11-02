// This is brl/bpro/bprb/bprb_parameters.h
#ifndef bprb_parameters_h_
#define bprb_parameters_h_
//:
// \file
// \brief classes to represent parameters to bprb processes
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date July 1, 2004
//
// \verbatim
//  Modifications
//    Matt Leotta   Dec 15, 2004  Migrated from vidpro
//    Amir Tamrakar Sep 19, 2006  Added a parameter type for multiple choice options (bprb_choice_param_type)
//    J.L. Mundy    Jan 30, 2007  Migrated from breye
// \endverbatim

#include <string>
#include <iostream>
#include <typeinfo>
#include <utility>
#include <vector>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vbl/vbl_ref_count.h>
#include <bxml/bxml_document.h>
#include <bprb/bprb_parameters_sptr.h>

//: The abstract base class for a process parameter
class bprb_param
{
 public:

  //: Destructor
  virtual ~bprb_param() = default;

  //: Clone this parameter
  virtual bprb_param * clone() const = 0;

  //: Return the parameter name
  std::string name() const { return name_; }
  //: Return the parameter description
  std::string description() const { return description_; }
  //: Returns true if the valid range of parameter values is bounded
  bool has_bounds() const { return has_bounds_; }

  //: Reset the value to its default
  virtual void reset() = 0;
  //: Attempt to set the value from the temporary reference
  virtual bool set_from_temp() = 0;

  //: Return a string representation of the current value
  virtual std::string value_str() const = 0;
  //: Return a string representation of the default value
  virtual std::string default_str() const = 0;
  //: Return a string representation of the type
  virtual std::string type_str() const = 0;

  //: Return a string representation of the minimum value
  virtual std::string min_str() const = 0;
  //: Return a string representation of the maximum value
  virtual std::string max_str() const = 0;

  //: Set the current value by parsing a string
  virtual bool parse_value_str(const std::string& input) = 0;

 protected:
  //: Constructor
  bprb_param(bool has_bounds, std::string  name, std::string  desc)
   : has_bounds_(has_bounds), name_(std::move(name)), description_(std::move(desc)) {}


  //: Describes whether or not the parameter has bounds
  const bool has_bounds_;
  //: Name of the parameter
  const std::string name_;
  //: Description of the parameter
  const std::string description_;
};

//: Output stream operator for bprb_params
std::ostream& operator<<(std::ostream& os, const bprb_param& p);

//===========================================================================================

//: A Templated parameter class
template< class T >
class bprb_param_type : public bprb_param
{
 public:
  // Constructor - with bounds
  bprb_param_type<T>(const std::string& name, const std::string& desc, const T& dflt, T  min, T  max)
   : bprb_param(true, name, desc), value_(dflt), default_(dflt), temp_value_(dflt),
     min_value_(std::move(min)), max_value_(std::move(max)) { assert( min_value_ <= value_ && value_ <= max_value_ ); }

  // Constructor - without bounds
  bprb_param_type<T>(const std::string& name, const std::string& desc, const T& dflt)
   : bprb_param(false, name, desc), value_(dflt), default_(dflt), temp_value_(dflt),
     min_value_(dflt), max_value_(dflt) {}

  //: Accessor for the default value;
  T default_value() const { return default_; }
  //: Accessor for the default value;
  T min_value() const { return min_value_; }
  //: Accessor for the default value;
  T max_value() const { return max_value_; }

  //: Accessor for the current value;
  T value() const { return value_; }
  //: A reference for temporary storage of values
  T& temp_ref() { temp_value_ = value_; return temp_value_; }
  //: Attempt to set the value from the temporary reference
  bool set_from_temp() override { return set_value(temp_value_); }
  //: Set the current value to \p val
  bool set_value( const T& val );

  //: Reset the value to its default
  void reset() override { value_ = default_; }

  //: Clone the parameter
  bprb_param * clone() const override { return new bprb_param_type<T>(*this); }

  //: Return a string representation of the current value
  std::string value_str() const override { return create_string(value_); }
  //: Return a string representation of the default value
  std::string default_str() const override { return create_string(default_); }
  //: Return a string representation of the default value
  std::string type_str() const override { return typeid(std::string("dummy")) == typeid(default_) ? "string" : typeid(default_).name(); }

  //: Return a string representation of the minimum value
  std::string min_str() const override { return has_bounds_? create_string(min_value_) : ""; }
  //: Return a string representation of the maximum value
  std::string max_str() const override { return has_bounds_? create_string(max_value_) : ""; }

  //: Set the current value by parsing a string
  bool parse_value_str(const std::string& input) override { return set_value(parse_string(input)); }

 private:
  //: Create a string representation of the value
  std::string create_string(const T& val) const;

  //: Parse a string representation of the value
  T parse_string(const std::string& input) const;

  //: The current parameter value
  T value_;
  //: The default parameter value
  const T default_;
  //: A temporary value for assignments by reference
  T temp_value_;
  //: The minimum allowed parameter value
  const T min_value_;
  //: The maximum allowed parameter value
  const T max_value_;
};

//===========================================================================================

//: A parameter class for handling multiple choice parameters
class bprb_choice_param_type : public bprb_param_type<unsigned>
{
 public:
  // Constructor
  bprb_choice_param_type(const std::string& name, const std::string& desc,
                         const std::vector<std::string>& choices, const unsigned def_val)
   : bprb_param_type<unsigned>(name, desc, def_val, 0, static_cast<unsigned>(choices.size()-1)), choices_(choices) {}

  //: Clone the parameter
  bprb_param * clone() const override { return new bprb_choice_param_type(*this); }

  //: Accessor for the choice list;
  std::vector<std::string> & choices() { return choices_; }

 private:

  //: Multiple choice list
  std::vector<std::string> choices_;
};

//===========================================================================================

//: This class maintains all parameters for a process
class bprb_parameters : public vbl_ref_count
{
 public:

  //: Constructor
  bprb_parameters();
  //: Destructor
  ~bprb_parameters() override;

  //: Deep pseudo copy constructor
  bprb_parameters(const bprb_parameters_sptr& old_params);
  //: Returns true if a parameter exists with \p name
  bool valid_parameter( const std::string& name ) const;

  //: Returns true if a parameter exists with \p name and type \p T
  template<class T>
  bool valid_parameter_type( const std::string& name, const T&) const
  {
    std::map< std::string, bprb_param* >::const_iterator
      itr = name_param_map_.find( name );
    if ( itr == name_param_map_.end() ) {
      return false; // Not Found
    }
    return dynamic_cast<bprb_param_type<T> *>(itr->second) != NULL;
  }

  //: Add a new parameter with no bounds
  template<class T>
  bool add( const std::string& desc, const std::string& name, const T& default_val )
  { return add(new bprb_param_type<T>(name, desc, default_val)); }

  //: Add a new parameter with bounds
  template<class T>
  bool add( const std::string& desc, const std::string& name, const T& default_val,
            const T& min_val, const T& max_val )
  { return add(new bprb_param_type<T>(name, desc, default_val, min_val, max_val)); }

  //: Add a new parameter for multiple choice options
  template<class T>
  bool add( const std::string& desc, const std::string& name,
            const std::vector<std::string>& choices, const T& default_val )
  { return add(new bprb_choice_param_type(name, desc, choices, default_val)); }

  //: Set the value of the existing parameter named \p name
  template<class T>
  bool set_value( const std::string& name , const T& value )
  {
    bprb_param_type<T> * param = NULL;
    if ( get_param(name, param) && param ){
      return param->set_value(value);
    }
    return false;
  }

  //: Return the value of the parameter named \p name by reference
  template<class T>
  bool get_value( const std::string& name , T& value ) const
  {
    bprb_param_type<T> * param = nullptr;
    if ( get_param(name, param) && param ){
      value = param->value();
      return true;
    }
    return false;
  }

  //: Return the value of the parameter
  //  Be careful when using this method, if the parameter DOES NOT EXIST --> returns 0
  //  so MAKE SURE that a parameter with the given name EXISTS in the parameter list of the process
  template<class T>
  T value( const std::string& name )
  {
    bprb_param_type<T> * param = NULL;
    T val = 0;
    if ( get_param(name, param) && param ){
      val = param->value();
    }
    return val;
  }

  //: Return the default value of the parameter named \p name by reference
  template<class T>
  bool get_default( const std::string& name , T& deflt ) const
  {
    bprb_param_type<T> * param = NULL;
    if ( get_param(name, param) && param ){
      deflt = param->default_value();
      return true;
    }
    return false;
  }

  //: Return the bounds of the parameter named \p name by reference
  template<class T>
  bool get_bounds( const std::string& name, T & min, T & max ) const
  {
    bprb_param_type<T> * param = NULL;
    if ( get_param(name, param) && param ){
      min = param->min_value();
      max = param->max_value();
      return true;
    }
    return false;
  }

  //: reads the parameters and their values from an XML document
  bool parse_XML(std::string const& xml_fname, const std::string& root_tag="");

  //: prints the default parameter values to an XML document
  void print_def_XML(const std::string& root_tag,
                     const std::string& xml_path);

  //: prints the currently used parameter values to an XML document
  void print_current_XML(const std::string& root_tag,
                         const std::string& xml_path);

  //: Reset all parameters to their default values
  bool reset_all();
  //: Reset the parameter named \p name to its default value
  bool reset( const std::string& name );

  //: Return a vector of base class pointers to the parameters
  std::vector< bprb_param* > get_param_list() const;
  //: Return the description of the parameter named \p name
  std::string get_desc( const std::string& name ) const;
  //: Print all parameters to \p os
  void print_all(std::ostream& os) const;

 //private:
  //: Add parameter helper function
  bool add( bprb_param* param );
 private:
  template<class T>
  bool get_param( const std::string& name,
                  bprb_param_type<T> * &param) const
  {
    std::map< std::string, bprb_param* >::const_iterator
      itr = name_param_map_.find( name );
    if ( itr == name_param_map_.end() ) {
      return false; // Not Found
    }
    param = dynamic_cast<bprb_param_type<T> *>(itr->second);
    if ( !param )
      std::cerr << "WARNING: parameter \""<< name
               << "\" was found but has incorrect type\n";
    return true;
  }

  //: The map from names to parameters
  std::map< std::string , bprb_param* > name_param_map_;
  //: The vector of parameters in order of declaration
  std::vector< bprb_param* > param_list_;

  bxml_document xml_doc_;
};


//===========================================================================================


//: A simple class to represent a file (for use with parameters)
class bprb_filepath
{
 public:
  //: Constructor
  bprb_filepath(std::string  p = "", std::string  e = "*")
   : path(std::move(p)), ext(std::move(e)) {}

  std::string path;
  std::string ext;
};

//: Less than operator for bprb_filepath objects
bool operator<( const bprb_filepath& lhs, const bprb_filepath& rhs );
//: Less than or equal to operator for bprb_filepath objects
bool operator<=( const bprb_filepath& lhs, const bprb_filepath& rhs );
//: Output stream operator for bprb_filepath objects
std::ostream& operator<<( std::ostream& strm, const bprb_filepath& fp );
//: Input stream operator for bprb_filepath objects
std::istream& operator>>( std::istream& strm, bprb_filepath& fp );


#endif // bprb_parameters_h_
