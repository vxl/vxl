// This is core/vul/vul_arg.h
#ifndef vul_arg_h_
#define vul_arg_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Command-line arguments
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   05 Feb 98
//
// \verbatim
//  Modifications
//   PDA (Manchester) 21/03/2001: Tidied up the documentation
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
// \endverbatim

#include <iosfwd>
#include <list>
#include <string>
#include <utility>
#include <vcl_compiler.h>
#include <vector>
#include <vul/vul_export.h>
#include <vxl_config.h>

//: forward declare all classes and their helper functions.
class vul_arg_info_list;
template <class T> class vul_arg;
template <class T> void settype     (vul_arg<T> &);
template <class T> void print_value (std::ostream &, vul_arg<T> const &);
template <class T> int  parse       (vul_arg<T>*, char**);

//: This is the base class for the templated vul_arg<T>s
class vul_arg_base
{
 protected:
   struct required_option_type {}; // see constructors of vul_arg

 public:
  static VUL_EXPORT required_option_type is_required;

  static void parse_deprecated(int& argc, char **& argv,
                               bool warn_about_unrecognized_arguments = true);
  static void include_deprecated(vul_arg_info_list& l);

  static void add_to_current(vul_arg_base* a);
  static void set_help_option( char const*str);
  static void set_help_description( char const*str);
  static void set_help_precis( char const*str);
  static void display_usage(char const* msg = nullptr);
  static void display_usage_and_exit(char const* msg = nullptr);

  friend class vul_arg_info_list;

  char const* option();
  char const* help();

  //: Returns true if arg was set on the command line.
  bool set() const;

  virtual std::ostream& print_value(std::ostream&) = 0;

 public:   // Avoid errors on some compilers that don't follow
           // protected: directive correctly with type_

  //: Static text describing type of option (e.g. bool or double).
  char const *type_;
 protected:
  //: After parsing, true iff value was set on command line.
  bool set_;
  //: if true, this flag must be set on command line.
  bool required_;
  //: Option flag including "-" or "--".
  std::string option_;
  //: Description of argument.
  std::string help_;

  vul_arg_base(vul_arg_info_list& l, char const* option_string,
               char const*helpstring, bool required= false);
  vul_arg_base(char const* option_string, char const*helpstring, bool required= false);
  virtual ~vul_arg_base() = default;

  virtual int parse(char ** argv) = 0;
};

//: parse command-line arguments
// vul_arg_parse simplifies the parsing of command-line arguments by combining
// the variables with the option specifications.  To get a variable, you
// simply name it along with its flag, a help string, and an optional
// default value:
// \code
//      vul_arg<double> threshold("-t", "Intensity threshold", 1.25);
// \endcode
// Repeat this for any other arguments and then ask the base class to parse
// the lot:
// \code
//      vul_arg_parse(argc,argv);
// \endcode
//
// Now parameters such as threshold above can be referred to and will have
// either the default value or the one supplied on the command line.
//
// The big design decision here was whether or not the args should collect
// themselves into a global pool, so that the static vul_arg_base::parse can
// find them, or whether there should be a local argPool which is passed to
// each arg in order that it may add itself.  That would give a syntax like
// \code
//      vul_arg_info_list args;
//      vul_arg<double> threshold(args, "-t", 1.25);
//                                ^^^^^ passing args in
//      args.parse(argc, argv, true);
// \endcode
// The latter is "better" but the former is easier to use so I chose it.
//
// Added by Geoff: call to vul_arg_base::set_help_option("-?") means that a
// program call with something like aprog -? will display usage info derived
// from the argument list.  Note: default is -? but can be anything.
//
void vul_arg_parse(int& argc, char **& argv,
                   bool warn_about_unrecognized_arguments = true);

//: Add an externally supplied list of args to the global list.
void vul_arg_include(vul_arg_info_list& l);

//: Print all args, and usage messages.
void vul_arg_display_usage_and_exit(char const* msg = nullptr);

//: parse command-line arguments
template <class T>
class vul_arg : public vul_arg_base
{
 private:
  void settype() { ::settype(*this); }
 public:
  T value_;// public so we don't have to worry about templated friends.

  //: Construct a vul_arg<T> with command-line switch and default value.
  // Command line switch \a option_string, and default value
  // \a default_value.  Add this argument to the global
  // list of arguments that vul_arg_base::parse() uses when it eventually
  // gets the command line.
  //
  // If \a option_string is null, then the argument is assigned to the
  // first plain word in the command line (warning: this causes problems for
  // T=char *, but that just means that you have to have a help string if you
  // want a default... good)
  vul_arg(char const* option_string = nullptr,
          char const* helpstring = nullptr,
          T default_value = T()
         )
    : vul_arg_base(option_string,helpstring, false),
      value_(std::move(default_value)) { settype(); }

  //: As above, but add the arg to the list \a l, on which \c parse() can be called later.
  vul_arg(vul_arg_info_list & l,
          char const * option_string = nullptr,
          char const * helpstring = nullptr,
          T default_value = T() )
    : vul_arg_base(l, option_string, helpstring, false),
      value_(std::move(default_value)) { settype(); }

  //: Dummy parameter to be passed during construction. It sets a flag as required.

  //: Construct a vul_arg<T> that user must set in command line.
  // Note that a default value does not make sense.
  // Add this argument to the global list of arguments that
  // vul_arg_base::parse() uses when it eventually gets the command line.
  //
  // As in the previous constructors, if \a option_string is null, then the argument is assigned to the
  // first plain word in the command line. However, this constructor adds a new option, allowing us to declare
  // a non-null flag, which can appears anywhere, and that is REQUIRED.
  //
  // Note that the parameters are not optional. This interface has been chosen to ensure backward compatibility.
  // \seealso is_required
  vul_arg(char const* option_string,
          char const* helpstring,
          required_option_type /*dummy*/)
    : vul_arg_base(option_string,helpstring, true),
      value_(T()) { settype(); }

  //: As above, but add the arg to the list \a l, on which \c parse() can be called later.
  vul_arg(vul_arg_info_list & l,
          char const * option_string,
          char const * helpstring,
          required_option_type /*dummy*/ )
    : vul_arg_base(l, option_string, helpstring, true),
      value_(T()) { settype(); }

  //: return the arg's current value, whether the default or the one from the command line.
  T      & operator () () { return value_; }
  T const& operator () () const { return value_; }
  //operator T& () { return value_; }

  //: returns number of args chomped, or -1 on failure.
  int parse(char ** argv) override { return ::parse(this, argv); }

  //: print
  std::ostream& print_value(std::ostream &s) override {
    ::print_value(s, *this);
    return s; // << flush
  }
};

//: a helper for vul_arg::parse.
// Users might need it if they wish to parse several command lines.
//
class vul_arg_info_list
{
 public:
  enum autonomy {
    subset,
    all
  };
  //: Construct an empty vul_arg_info_list.
  vul_arg_info_list(autonomy autonomy__ = subset)
    : help_("-?"), // default help operator!
      verbose_(false), autonomy_(autonomy__) {}

  ~vul_arg_info_list() = default;

  void add(vul_arg_base* arg);
  void parse(int& argc, char **& argv, bool warn_about_unrecognized_arguments);
  void include(vul_arg_info_list& l);
  void verbose(bool on) { verbose_ = on; }

  void set_help_option(char const* str);

  //: Set the (short) text used to describe the command
  void set_help_precis(char const* str) { command_precis_ = str; }

  //: Set the (possibly long) text used to document the command.
  // It is displayed at the end of the help page.
  void set_help_description(char const* str) { description_ = str; }

 public://protected:
  std::vector<vul_arg_base*> args_;
  std::string help_;
  std::string description_;
  std::string command_precis_;
  bool verbose_;
  autonomy autonomy_;

  void display_help( char const* progname= nullptr);

 private:
  // Disallow assigning to objects of this class:
  vul_arg_info_list(vul_arg_info_list const &) {}
  vul_arg_info_list& operator=(vul_arg_info_list const &) { return *this; }
};

#endif // vul_arg_h_
