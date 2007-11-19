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
    virtual ~mbl_exception_no_name_in_factory() throw() {}
  };

#endif


#if !VCL_HAS_EXCEPTIONS

  //: General purpose - a replacement for vcl_abort.
  // The only point of catching this exception, is to
  // give you a chance to save your data. If this exception
  // is thrown, then the program correctness is in doubt.
  class mbl_exception_abort
  {
    vcl_string msg_;
   public:
    mbl_exception_abort(const vcl_string& comment);
    const char * what() const {return msg_.c_str();}
  };

#else

  //: General purpose - a replacement for vcl_abort.
  // The only point of catching this exception, is to
  // give you a chance to save your data. If this exception
  // is thrown, then the program correctness is in doubt.
  class mbl_exception_abort : public vcl_logic_error
  {
   public:
    mbl_exception_abort(const vcl_string& comment);
    virtual ~mbl_exception_abort() throw() {}
  };

#endif



#if !VCL_HAS_EXCEPTIONS

  //: Indicates a problem whilst parsing text configuration data.
  class mbl_exception_parse_error
  {
    vcl_string msg_;
   public:
    mbl_exception_parse_error(const vcl_string &msg)
      : msg_(msg) {}
    const char * what() const {return msg_.c_str();}
  };

#else

  //: Indicates a problem whilst parsing text configuration data.
  class mbl_exception_parse_error: public vcl_runtime_error
  {
   public:
    mbl_exception_parse_error(const vcl_string &msg)
      : vcl_runtime_error(msg) {}
    virtual ~mbl_exception_parse_error() throw() {}
  };

#endif

#if !VCL_HAS_EXCEPTIONS

  //: Indicates a problem whilst parsing a file.
  class mbl_exception_parse_file_error
  {
    vcl_string msg_;
    vcl_string filename_;
   public:
     mbl_exception_parse_file_error(const vcl_string &msg, const vcl_string& filename)
      : msg_(msg+" "+filename), filename_(filename) {}
    const char * what() const {return msg_.c_str();}
    const char * filename() const {return filename_.c_str();}
  };

#else

  //: Indicates a problem whilst parsing a file.
  class mbl_exception_parse_file_error: public mbl_exception_parse_error
  {
    vcl_string filename_;
   public:
     mbl_exception_parse_file_error(const vcl_string &msg, const vcl_string& filename)
      : mbl_exception_parse_error(msg+" "+filename), filename_(filename) {}
    const char * filename() const {return filename_.c_str();}
    virtual ~mbl_exception_parse_file_error() throw() {}
  };

#endif

#if !VCL_HAS_EXCEPTIONS

  //: Indicates that an expected property label was missing.
  class mbl_exception_missing_property
  {
    vcl_string msg_;
   public:
    mbl_exception_missing_property(const vcl_string &missing)
      : missing_(vcl_string("Couldn't find expected property label: \""+missing+'\"') {}
    const char * what() const {return missing_.c_str();}
  };

#else

  //: Indicates a problem whilst parsing text configuration data.
  class mbl_exception_missing_property: public mbl_exception_parse_error
  {
   public:
   vcl_string missing_label;
    mbl_exception_missing_property(const vcl_string &missing)
      : mbl_exception_parse_error(
          vcl_string("Couldn't find expected property label: \""+missing+'\"')),
        missing_label(missing)
    {}
    virtual ~mbl_exception_missing_property() throw() {}
  };

#endif

#if !VCL_HAS_EXCEPTIONS

  //: Indicates that mbl_exception_look_for_unused_props found some unused properties.
  class mbl_exception_unused_props
  {
    vcl_string msg_;
   public:
    mbl_exception_unused_props(const vcl_string &function_name, const vcl_string &unused_props)
      : msg_(function_name + ": Unused properties found:\n" + unused_props) {}
    const char * what() const {return msg_.c_str();}
  };

#else

  //: Indicates that mbl_exception_look_for_unused_props found some unused properties.
  class mbl_exception_unused_props : public mbl_exception_parse_error
  {
   public:
    vcl_string function_name, unused_properties;
    mbl_exception_unused_props(const vcl_string &fn_name, const vcl_string &unused_props)
      : mbl_exception_parse_error(fn_name + ": Unused properties found:\n" + unused_props),
        function_name(fn_name), unused_properties(unused_props) {}
    virtual ~mbl_exception_unused_props() throw() {}
  };

#endif



#if !VCL_HAS_EXCEPTIONS

  //: Indicates a problem whilst parsing text configuration data into an mbl_read_props object.
  class mbl_exception_read_props_parse_error
  {
    vcl_string msg_;
   public:
    mbl_exception_read_props_parse_error(const vcl_string &msg)
      : msg_(vcl_string("mbl_read_props: ") + msg) {}
    const char * what() const {return msg_.c_str();}
  };

#else

  //: Indicates a problem whilst parsing text configuration data into an mbl_read_props object.
  class mbl_exception_read_props_parse_error: public mbl_exception_parse_error
  {
   public:
    mbl_exception_read_props_parse_error(const vcl_string &msg)
      : mbl_exception_parse_error(vcl_string("mbl_read_props: ") + msg) {}
    virtual ~mbl_exception_read_props_parse_error() throw() {}
  };

#endif

#if !VCL_HAS_EXCEPTIONS

  //: Indicates a problem whilst parsing a block of text configuration data.
  class mbl_exception_parse_block_parse_error
  {
    vcl_string msg_;
   public:
    mbl_exception_parse_block_parse_error(const vcl_string &msg,
      const vcl_string &contents)
    : msg_(vcl_string("mbl_parse_block: ") + msg +
      "Contents of block: \n" + contents) {}
    const char * what() const {return msg_.c_str();}
  };

#else

  //: Indicates a problem whilst parsing a block of text configuration data.
  class mbl_exception_parse_block_parse_error: public mbl_exception_parse_error
  {
  public:
    //: Description of problem
    vcl_string msg;
    //: Contents of string which failed to be parsed.
    vcl_string contents;
    mbl_exception_parse_block_parse_error(const vcl_string &msg,
      const vcl_string &contents)
    : mbl_exception_parse_error(vcl_string("mbl_parse_block: ") + msg +
      "Contents of block: \n" + contents), msg(msg), contents(contents) {}
    virtual ~mbl_exception_parse_block_parse_error() throw() {}
  };

#endif

//:Throw mbl_exception_os_error or one of its derivatives, based on errno.
// If exceptions are disabled, this behaves like mbl_exception_warning() above.
void mbl_exception_throw_os_error(const vcl_string& filename,
                                  const vcl_string& additional_comment="");


#if !VCL_HAS_EXCEPTIONS

  //: Indicates a problem reported during an OS call.
  class mbl_exception_os_error
  {
    vcl_string msg_;

  public:
    //: Reported errno
    int errno;
    //: System supplied error message.
    vcl_string error_message;
    //: Filename or pathname or other id on which OS call failed.
    vcl_string filename;
    //: Optional additional comments.
    vcl_string additional_comment;
    mbl_exception_os_error(int err_no, const vcl_string &file_name,
      const vcl_string &comment="");
    virtual ~mbl_exception_os_error() throw() {}
    const char * what() const {return msg_.c_str();}
  };

#else

  //: Indicates a problem reported during an OS call.
  class mbl_exception_os_error: public vcl_runtime_error
  {
  public:
    //: Reported errno
    int err_no;
    //: System supplied error message.
    vcl_string error_message;
    //: Filename or pathname or other id on which OS call failed.
    vcl_string filename;
    //: Optional additional comments.
    vcl_string additional_comment;
    mbl_exception_os_error(int errnum, const vcl_string &file_name,
      const vcl_string &comment="");
    virtual ~mbl_exception_os_error() throw() {}
  };

#endif


#define MACRO( E ) \
class E : public mbl_exception_os_error{ public: \
  E (int err_no, const vcl_string &file_name, const vcl_string &comment=""): \
    mbl_exception_os_error(err_no, file_name, comment) {} }; 

MACRO(mbl_exception_os_no_such_file_or_directory);
MACRO(mbl_exception_os_permission_denied);
MACRO(mbl_exception_os_file_exists);
MACRO(mbl_exception_os_not_a_directory);
MACRO(mbl_exception_os_is_a_directory);
MACRO(mbl_exception_os_no_space_left_on_device);

#undef MACRO

#endif // mbl_exception_h_

