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

class vbl_arg_info_list;

template <class T> class vbl_arg;
template <class T> int parse(vbl_arg<T>*, char**);

class vbl_arg_base {
public:
  static void parse(int& argc, char **& argv, bool warn_about_unrecognized_arguments = true);
  static void include(vbl_arg_info_list& l);

  static void add_to_current(vbl_arg_base* a);
  static void set_help_option( char const*str);
  static void display_usage(char const* msg = 0);
  static void display_usage_and_exit(char const* msg = 0);

  friend class vbl_arg_info_list;

  char const* option() { return _option; };

  // -- Returns true if arg was set on the command line.
  bool set() const { return _set; };

protected:
  // Should be static, but we need to ensure it is 0 well before main.
  // static vbl_arg_info_list current_list; 

  bool _set;
  char const* _option;
  int _optlen;
  char const* _help;
  const char *_type;
  
  vbl_arg_base(vbl_arg_info_list& l, char const* option_string, char const*helpstring);
  vbl_arg_base(char const* option_string, char const*helpstring);
  virtual ~vbl_arg_base() {}

  virtual int parse(char ** argv) = 0;
  virtual ostream& print_value(ostream&) = 0;
};

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
//      vbl_arg_base::parse(argc,argv);
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
  friend int parse(vbl_arg<T>*, char**);

public: // Until templated friends of templates are recognized
  T _value;
  
public:
  
  // -- Construct an vbl_arg<T> with command-line switch \arg{option_string}, and default
  // value \arg{default_value}.  Add this argument to the global list of arguments
  // that vbl_arg_base::parse() uses when it eventually gets the command line.


  // 
  // If \arg{option_string} is null, then the argument is assigned to the first plain
  // word in the command line (warning: this causes problems for T=char *, but that
  // just means that you have to have a help string if you want a default... good)
  vbl_arg(char const* option_string = 0, char const*helpstring= 0, T default_value = T()):
    vbl_arg_base(option_string,helpstring),
    _value(default_value)
    {
      settype();
    }    
  
  // -- As above, but add the arg to the list \arg{l}, on which
  // l.parse can be called later.
  vbl_arg(vbl_arg_info_list& l, char const* option_string = 0, char const*helpstring= 0, T default_value = T()):
    vbl_arg_base(l, option_string, helpstring),
    _value(default_value)
    {
    settype();
    }    
  
  // -- Set the current value of the arg.
  void set_value(const T& t) { _value = t; }
  
  // -- Return the arg's value, either the default or the one set by the command line.
  operator T& () { return _value; }
  
  T const& operator () () const { return _value; }
  
  // -- Returns number of args chomped, or -1 if there was a disaster.
  int parse(char ** argv) { return ::parse(this, argv); }

  ostream& print_value(ostream&);

private:
  void settype();
};

//: vbl_arg_info_list - argparse helper.
// vbl_arg_info_list is a helper for vbl_arg::parse.
// Users might need it if they wish to parse several command lines.
//
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
  char*help;

  bool verbose_;
  autonomy autonomy_;

  void display_help( char const* progname= 0);
};

#endif
