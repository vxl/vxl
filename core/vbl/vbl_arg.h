#ifndef vbl_arg_h_
#define vbl_arg_h_
#ifdef __GNUC__
#pragma interface
#endif

// .KEYWORDS    command-line arguments options switches getopt
// .LIBRARY     vbl
// .HEADER      Basics Package
// .INCLUDE     vbl/vbl_arg.h
// .FILE        vbl/vbl_arg.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 05 Feb 98
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_string.h>

//: forward declare all classes and their helper functions.
class vbl_arg_info_list;
template <class T> class vbl_arg;
template <class T> void settype     (vbl_arg<T> &);
template <class T> void print_value (ostream &, vbl_arg<T> const &);
template <class T> int  parse       (vbl_arg<T>*, char**);

//: This is the base class for the templated vbl_arg<T>s
class vbl_arg_base {
public:
  static void parse_deprecated(int& argc, char **& argv, bool warn_about_unrecognized_arguments = true);
  static void include_deprecated(vbl_arg_info_list& l);

  static void add_to_current(vbl_arg_base* a);
  static void set_help_option( char const*str);
  static void display_usage(char const* msg = 0);
  static void display_usage_and_exit(char const* msg = 0);

  friend class vbl_arg_info_list;

  char const* option() { return option_; };

  // -- Returns true if arg was set on the command line.
  bool set() const { return set_; };

public://protected:
  bool set_;
  char const* option_;
  int optlen_;
  char const* help_;
  char const *type_;
  
  vbl_arg_base(vbl_arg_info_list& l, char const* option_string, char const*helpstring);
  vbl_arg_base(char const* option_string, char const*helpstring);
  virtual ~vbl_arg_base() {}

  virtual int parse(char ** argv) = 0;
  virtual ostream& print_value(ostream&) = 0;
};

//: Parse the list of arguments....
inline void vbl_arg_parse(int& argc, char **& argv, bool warn_about_unrecognized_arguments = true)
{
  vbl_arg_base::parse_deprecated(argc, argv, warn_about_unrecognized_arguments);
}

//: Add an externally supplied list of args to the global list.
inline void vbl_arg_include(vbl_arg_info_list& l)
{
  vbl_arg_base::include_deprecated(l);
}

//: Print all args, and usage messages.
inline void vbl_arg_display_usage_and_exit(char const* msg = 0)
{
  vbl_arg_base::display_usage_and_exit(msg);
}

//: parse command-line arguments
// vbl_arg::parse simplifies the parsing of command-line arguments by combining
// the variables with the option specifications.  To get a variable, you
// simply name it along with its flag, a help string, and an optional
// default value:
// <verb>
//      vbl_arg<double> threshold("-t", "Intensity threshold", 1.25);
// </verb>
// Repeat this for any other arguments and then ask the base class to parse
// the lot:
// <verb>
//      vbl_arg_parse(argc,argv);
// </verb>
//
// Now parameters such as threshold above can be referred to and will have
// either the default value or the one supplied on the command line.
//
// The big design decision here was whether or not the args should collect
// themselves into a global pool, so that the static vbl_arg_base::parse can find
// them, or whether there should be a local argPool which is passed to each
// arg in order that it may add itself.  That would give a syntax like
// <verb>
//      argList args;
//      vbl_arg<double> threshold(args, "-t", 1.25);
//                                ^^^^^ passing args in
//      args.parse(argc, argv);
// </verb>
// The latter is "better" but the former is easier to use so I chose it.
//
// Added by Geoff: call to vbl_arg_base::set_help_option("-?") means that a 
// program call with something like aprog -? will display usage info derived
// from the argument list.  Note: default is -? but can be anything. 
//
template <class T>
class vbl_arg : public vbl_arg_base {
public: 
  //friend void settype     VCL_STL_NULL_TMPL_ARGS (vbl_arg<T> &);
  //friend void print_value VCL_STL_NULL_TMPL_ARGS (ostream &, vbl_arg<T> const &);
  //friend int  parse       VCL_STL_NULL_TMPL_ARGS (vbl_arg<T>*, char**);
  T value_;// public so we don't have to worry about templated friends.
  
  // -- Construct an vbl_arg<T> with command-line switch \arg{option_string}, and default
  // value \arg{default_value}.  Add this argument to the global list of arguments
  // that vbl_arg_base::parse() uses when it eventually gets the command line.


  // If \arg{option_string} is null, then the argument is assigned to the first plain
  // word in the command line (warning: this causes problems for T=char *, but that
  // just means that you have to have a help string if you want a default... good)
  vbl_arg(char const* option_string = 0, 
	  char const* helpstring = 0, 
	  T default_value = T())
    : vbl_arg_base(option_string,helpstring), value_(default_value) { settype(); }    
  
  // -- As above, but add the arg to the list \arg{l}, on which
  // l.parse can be called later.
  vbl_arg(vbl_arg_info_list & l,
	  char const * option_string = 0,
	  char const * helpstring = 0, 
	  T default_value = T())
    : vbl_arg_base(l, option_string, helpstring), value_(default_value) { settype(); }    
  
  //: return the arg's current value, whether the default or the one from the command line.
  T      & operator () () { return value_; }
  T const& operator () () const { return value_; }
  //operator T& () { return value_; }
  
  //: returns number of args chomped, or -1 on failure.
  int parse(char ** argv) { return ::parse(this, argv); }

  //: print 
  ostream& print_value(ostream &s) { 
    ::print_value(s, *this); 
    return s; // << flush
  }

private:
  void settype() { ::settype(*this); }
};

//: vbl_arg_info_list - argparse helper.
// vbl_arg_info_list is a helper for vbl_arg::parse.
// Users might need it if they wish to parse several command lines.
class vbl_arg_info_list {
public:
  enum autonomy {
    subset,
    all
  };
  vbl_arg_info_list(autonomy autonomy__ = subset);
  ~vbl_arg_info_list();
  
  void add(vbl_arg_base* arg);
  void parse(int& argc, char **& argv, bool warn_about_unrecognized_arguments);
  void include(vbl_arg_info_list& l);
  void verbose(bool on) { verbose_ = on; }
  void set_help_option(char const* str);

public:
  int nargs;
  vbl_arg_base** args;
  vcl_string help;

  bool verbose_;
  autonomy autonomy_;

  void display_help( char const* progname= 0);
};

#ifdef VCL_KAI
#define declare_specialization(T) \
template <> void settype(vbl_arg<T > &); \
template <> void print_value(vcl_ostream &, vbl_arg<T > const &); \
template <> int  parse(vbl_arg<T> *, char **)

declare_specialization(bool);
declare_specialization(int);
declare_specialization(unsigned);
declare_specialization(char*);
declare_specialization(char const*);
declare_specialization(double);
#include <vcl/vcl_list.h>
declare_specialization(vcl_list<int>);
#include <vcl/vcl_string.h>
declare_specialization(vcl_string);

#undef declare_specialization
#endif

#endif
