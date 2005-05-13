#ifndef mbl_exception_h_
#define mbl_exception_h_
//:
// \file
// \brief Exceptions thrown by mbl, and amechanism for turning them off.
// \author Ian Scott.

#include <vcl_string.h>
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#if VCL_HAS_EXCEPTIONS
# include <vcl_stdexcept.h>
#endif



//: Throw an exception indicating a real problem.
// If exceptions have been disabled, this function
// may abort.
template <class T>
void mbl_exception_error(T exception)
{
  vcl_cerr << "\nERROR: " << exception.what() << vcl_endl;
#if !defined MBL_EXCEPTIONS_DISABLE  && VCL_HAS_EXCEPTIONS
  throw exception;
#else
  vcl_abort();
#endif
}

//: Throw an exception indicating a potential problem.
// If exceptions have been disabled, this function
// may return.
template <class T>
void mbl_exception_warning(T exception)
{
  vcl_cerr << "\nWARNING: " << exception.what() << vcl_endl;
#if !defined MBL_EXCEPTIONS_DISABLE  && VCL_HAS_EXCEPTIONS
  throw exception;
#endif
}


class mbl_read_props_type;

//: Throw error if there are any keys in props that aren't in ignore.
// \throw mbl_exception_unused_props
void mbl_exception_look_for_unused_props(
  const vcl_string & function_name,
  const mbl_read_props_type &props,
  const mbl_read_props_type &ignore);



#if !VCL_HAS_EXCEPTIONS

  //: Indicates that mbl_cloneables_factory has not heard of value name.
  class mbl_exception_no_name_in_factory
  {
    vcl_string msg_;
   public:
    mbl_exception_no_name_in_factory(vcl_string failed_name, vcl_string valid_names)
      : msg_(vcl_string("No such value: ")+failed_name+"\nValid values are: ["+valid_names+"]") {}
    const char * what() const {return msg_.c_str();}
  };

#else

  //: Indicates that mbl_cloneables_factory has not heard of value name.
  class mbl_exception_no_name_in_factory : public vcl_logic_error
  {
   public:
    vcl_string failed_value, valid_values;
    mbl_exception_no_name_in_factory(const vcl_string& failed_name, const vcl_string& valid_names):
      vcl_logic_error(vcl_string("No such value: ") +failed_name + "\nValid values are: ["+valid_names+"]"),
        failed_value(failed_name), valid_values(valid_names) {}
//    virtual ~mbl_exception_no_name_in_factory() throw() {};
  };

#endif


#if !VCL_HAS_EXCEPTIONS

  //: Indicates that mbl_exception_look_for_unused_props found some unused properties.
  class mbl_exception_unused_props
  {
    vcl_string msg_;
   public:
    mbl_exception_unused_props(const vcl_string &function_name, const vcl_string &unused_props)
      : msg_(function_name + ": Unused properties found: \n" + unused_props) {}
    const char * what() const {return msg_.c_str();}
  };

#else

  //: Indicates that mbl_exception_look_for_unused_props found some unused properties.
  class mbl_exception_unused_props : public vcl_logic_error
  {
   public:
    vcl_string function_name, unused_properties;
    mbl_exception_unused_props(const vcl_string &fn_name, const vcl_string &unused_props)
      : vcl_logic_error(function_name + ": Unused properties found: \n" + unused_props),
        function_name(fn_name), unused_properties(unused_props) {}
//    virtual ~mbl_exception_unused_props() throw() {};
  };

#endif


#endif // mbl_exception_h_

