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
// Modifications
// PDA (Manchester) 21/03/2001: Tidied up the documentation
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
// \endverbatim

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_list.h>
#include <vcl_iostream.h>

//: forward declare all classes and their helper functions.
class vul_arg_info_list;
template <class T> class vul_arg;
template <class T> void settype     (vul_arg<T> &);
template <class T> void print_value (vcl_ostream &, vul_arg<T> const &);
template <class T> int  parse       (vul_arg<T>*, char**);

//: This is the base class for the templated vul_arg<T>s
class vul_arg_base
{
 public:
  static void parse_deprecated(int& argc, char **& argv,
                               bool warn_about_unrecognized_arguments = true);
  static void include_deprecated(vul_arg_info_list& l);

  static void add_to_current(vul_arg_base* a);
  static void set_help_option( char const*str);
  static void set_help_description( char const*str);
  static void set_help_precis( char const*str);
  static void display_usage(char const* msg = 0);
  static void display_usage_and_exit(char const* msg = 0);

  friend class vul_arg_info_list;

  char const* option();
  char const* help();

  //: Returns true if arg was set on the command line.
  bool set() const;

  virtual vcl_ostream& print_value(vcl_ostream&) = 0;

 public:   // Avoid errors on some compilers that don't follow
           // protected: directive correctly with type_

  //: Static text describing type of option (e.g. bool or double).
  char const *type_;
 protected:
  //: After parsing, true iff value was set on command line.
  bool set_;
  //: Option flag including "-" or "--".
  vcl_string option_;
  //: Description of argument.
  vcl_string help_;

  vul_arg_base(vul_arg_info_list& l, char const* option_string,
               char const*helpstring);
  vul_arg_base(char const* option_string, char const*helpstring);
  virtual ~vul_arg_base();

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
//      argList args;
//      vul_arg<double> threshold(args, "-t", 1.25);
//                                ^^^^^ passing args in
//      args.parse(argc, argv);
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
void vul_arg_display_usage_and_exit(char const* msg = 0);

//: parse command-line arguments
template <class T>
class vul_arg : public vul_arg_base
{
 public:
  T value_;// public so we don't have to worry about templated friends.

  //: Construct an vul_arg<T> with command-line switch and default value.
  // Command line switch \a option_string, and default value
  // \a default_value.  Add this argument to the global
  // list of arguments that vul_arg_base::parse() uses when it eventually
  // gets the command line.
  //
  // If \a option_string is null, then the argument is assigned to the
  // first plain word in the command line (warning: this causes problems for
  // T=char *, but that just means that you have to have a help string if you
  // want a default... good)
  vul_arg(char const* option_string = 0,
          char const* helpstring = 0,
          T default_value = T())
    : vul_arg_base(option_string,helpstring),
      value_(default_value) { settype(); }

  //: As above, but add the arg to the list \a l, on which \c parse() can be called later.
  vul_arg(vul_arg_info_list & l,
          char const * option_string = 0,
          char const * helpstring = 0,
          T default_value = T())
    : vul_arg_base(l, option_string, helpstring),
      value_(default_value) { settype(); }

  //: return the arg's current value, whether the default or the one from the command line.
  T      & operator () () { return value_; }
  T const& operator () () const { return value_; }
  //operator T& () { return value_; }

  //: returns number of args chomped, or -1 on failure.
  int parse(char ** argv) { return ::parse(this, argv); }

  //: print
  vcl_ostream& print_value(vcl_ostream &s) {
    ::print_value(s, *this);
    return s; // << flush
  }

 private:
  void settype() { ::settype(*this); }
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
  vul_arg_info_list(autonomy autonomy__ = subset);
  ~vul_arg_info_list();

  void add(vul_arg_base* arg);
  void parse(int& argc, char **& argv, bool warn_about_unrecognized_arguments);
  void include(vul_arg_info_list& l);
  void verbose(bool on);
  void set_help_option(char const* str);
  void set_help_precis(char const* str);
  void set_help_description(char const* str);

 public://protected:
  vcl_vector<vul_arg_base*> args_;
  vcl_string help_;
  vcl_string description_;
  vcl_string command_precis_;
  bool verbose_;
  autonomy autonomy_;

  void display_help( char const* progname= 0);

 private:
  vul_arg_info_list(vul_arg_info_list const &) { }
  void operator=(vul_arg_info_list const &) { }
};

#if defined(VCL_KAI) || defined(VCL_COMO) || defined(VCL_ICC)
#define declare_specialization(T) \
template<> void settype(vul_arg<T > &); \
template<> void print_value(vcl_ostream &, vul_arg<T > const &); \
template<> int  parse(vul_arg<T > *, char **)

declare_specialization(bool);
declare_specialization(int);
declare_specialization(unsigned);
declare_specialization(char*);
declare_specialization(char const*);
declare_specialization(float);
declare_specialization(double);
declare_specialization(vcl_list<int>);
declare_specialization(vcl_vector<int>);
declare_specialization(vcl_vector<unsigned>);
declare_specialization(vcl_vector<double>);
declare_specialization(vcl_string);

#undef declare_specialization
#endif // VCL_KAI

#endif // vul_arg_h_
