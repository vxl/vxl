#ifndef mbl_exception_h_
#define mbl_exception_h_
//:
// \file
// \brief Exceptions thrown by mbl, and a mechanism for turning them off.
// \author Ian Scott.

#include <string>
#include <iostream>
#include <cstdlib>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <stdexcept>


//: Throw an exception indicating a real problem.
// If exceptions have been disabled, this function
// may abort.
template <class T>
void mbl_exception_error(T exception)
{
  std::cerr << "\nERROR: " << exception.what() << std::endl;
#if !defined MBL_EXCEPTIONS_DISABLE
  throw exception;
#else
  std::abort();
#endif
}

//: Throw an exception indicating a potential problem.
// If exceptions have been disabled, this function
// may return.
template <class T>
void mbl_exception_warning(T exception)
{
  std::cerr << "\nWARNING: " << exception.what() << std::endl;
#if !defined MBL_EXCEPTIONS_DISABLE
  throw exception;
#endif
}

  //: Indicates that mbl_cloneables_factory has not heard of value name.
  class mbl_exception_no_name_in_factory : public std::logic_error
  {
   public:
    std::string failed_value, valid_values;
    mbl_exception_no_name_in_factory(const std::string& failed_name, const std::string& valid_names):
      std::logic_error(std::string("No such value: ") +failed_name + "\nValid values are: ["+valid_names+"]"),
        failed_value(failed_name), valid_values(valid_names) {}
    ~mbl_exception_no_name_in_factory() throw() override = default;
  };

  //: General purpose - a replacement for std::abort.
  // The only point of catching this exception, is to
  // give you a chance to save your data. If this exception
  // is thrown, then the program correctness is in doubt.
  class mbl_exception_abort : public std::logic_error
  {
   public:
    mbl_exception_abort(const std::string& comment);
    ~mbl_exception_abort() throw() override = default;
  };


  //: Indicates a problem whilst parsing text configuration data.
  class mbl_exception_parse_error: public std::runtime_error
  {
   public:
    mbl_exception_parse_error(const std::string &msg)
      : std::runtime_error(msg) {}
    ~mbl_exception_parse_error() throw() override = default;
  };

  //: Indicates a problem whilst parsing a file.
  class mbl_exception_parse_file_error: public mbl_exception_parse_error
  {
    std::string filename_;
   public:
     mbl_exception_parse_file_error(const std::string &msg, const std::string& filename):
       mbl_exception_parse_error(filename.empty() ? msg : msg+" in "+filename), filename_(filename) {}
    const char * filename() const {return filename_.c_str();}
    ~mbl_exception_parse_file_error() throw() override = default;
  };

  //: Data from two sources or files was inconsistent.
  // This is distinct from a parse error, which can be used when the data within
  // in a single config file is inconsistent. This is most useful at algorithm run time,
  // when there are several external data files, and some external agent has broken the
  // consistency invariant by modifying one of them. The application can then report that
  // someone has messed up its data.
  class mbl_exception_inconsistent_external_data: public std::runtime_error
  {
    std::string source1_, source2_;
   public:
    mbl_exception_inconsistent_external_data(const std::string &msg,
      const std::string& source1 = "", const std::string& source2 = "")
      : std::runtime_error( source1.empty() && source2.empty()
          ? msg : msg+" between "+source1+" and "+source2),
        source1_(source1), source2_(source2) {}
    const char * source1() const {return source1_.c_str();}
    const char * source2() const {return source2_.c_str();}
    ~mbl_exception_inconsistent_external_data() throw() override = default;
  };

  //: Indicates a problem whilst parsing text configuration data.
  class mbl_exception_missing_property: public mbl_exception_parse_error
  {
   public:
   std::string missing_label;
    mbl_exception_missing_property(const std::string &missing)
      : mbl_exception_parse_error(
          std::string("Couldn't find expected property label: \""+missing+'\"')),
        missing_label(missing)
    {}
    ~mbl_exception_missing_property() throw() override = default;
  };

  //: Indicates that mbl_exception_look_for_unused_props found some unused properties.
  class mbl_exception_unused_props : public mbl_exception_parse_error
  {
   public:
    std::string function_name, unused_properties;
    mbl_exception_unused_props(const std::string &fn_name, const std::string &unused_props)
      : mbl_exception_parse_error(fn_name + ": Unused properties found:\n" + unused_props),
        function_name(fn_name), unused_properties(unused_props) {}
    ~mbl_exception_unused_props() throw() override = default;
  };

  //: Indicates a problem whilst parsing text configuration data into an mbl_read_props object.
  class mbl_exception_read_props_parse_error: public mbl_exception_parse_error
  {
   public:
    mbl_exception_read_props_parse_error(const std::string &msg)
      : mbl_exception_parse_error(std::string("mbl_read_props: ") + msg) {}
    ~mbl_exception_read_props_parse_error() throw() override = default;
  };

  //: Indicates a problem whilst parsing a block of text configuration data.
  class mbl_exception_parse_block_parse_error: public mbl_exception_parse_error
  {
   public:
    //: Description of problem
    std::string msg;
    //: Contents of string which failed to be parsed.
    std::string contents;
    mbl_exception_parse_block_parse_error(const std::string &msg,
      const std::string &contents)
    : mbl_exception_parse_error(std::string("mbl_parse_block: ") + msg +
      "Contents of block:\n" + contents), msg(msg), contents(contents) {}
    ~mbl_exception_parse_block_parse_error() throw() override = default;
  };

//:Throw mbl_exception_os_error or one of its derivatives, based on errno.
// If exceptions are disabled, this behaves like mbl_exception_warning() above.
void mbl_exception_throw_os_error(const std::string& filename,
                                  const std::string& additional_comment="");


  //: Indicates a problem reported during an OS call.
  class mbl_exception_os_error: public std::runtime_error
  {
   public:
    //: Reported errno
    int err_no;
    //: System supplied error message.
    std::string error_message;
    //: Filename or pathname or other id on which OS call failed.
    std::string filename;
    //: Optional additional comments.
    std::string additional_comment;
    mbl_exception_os_error(int errnum, const std::string &file_name,
      const std::string &comment="");
    ~mbl_exception_os_error() throw() override = default;
  };

#define MACRO( E ) \
class E : public mbl_exception_os_error{ public: \
  E (int err_no, const std::string &file_name, const std::string &comment=""): \
    mbl_exception_os_error(err_no, file_name, comment) {} }

MACRO(mbl_exception_os_no_such_file_or_directory);
MACRO(mbl_exception_os_permission_denied);
MACRO(mbl_exception_os_file_exists);
MACRO(mbl_exception_os_not_a_directory);
MACRO(mbl_exception_os_is_a_directory);
MACRO(mbl_exception_os_no_space_left_on_device);
MACRO(mbl_exception_os_invalid_value);

#undef MACRO

#endif // mbl_exception_h_
