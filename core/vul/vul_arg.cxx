// This is vxl/vul/vul_arg.cxx

//:
// \file
// Note that even though this file defines instances of a templated
// class, it is a .cxx file and not a .txx file because it does not
// supply a class definition for use by clients.
//
// If you need to define your own vul_arg<T>, you should #include vul_arg.h
// ONLY, in your source file (myarg.cxx, say), define these three global
// functions (which can by static if you like) in myarg.cxx
// -  void settype(vul_arg<T> &);
// -  void print_value(vul_arg<T> const &, vcl_ostream &);
// -  int  parse(vul_arg<T>*, char**);
// and then instantiate the class vul_arg<T> as usual (in myarg.cxx).

#ifdef __GNUC__
#pragma implementation
#endif

#include "vul_arg.h"

#include <vcl_cassert.h>
#include <vcl_cstdio.h>
#include <vcl_cstring.h>
#include <vcl_cstdlib.h> // exit()
#include <vcl_cmath.h>   // floor()
#include <vcl_vector.h>
#include <vcl_list.h>
#include <vcl_iostream.h>

#include <vul/vul_string.h>
#include <vul/vul_reg_exp.h>
#include <vul/vul_printf.h>

//------------------------------------------------------------------------------

// class vul_arg_base
vul_arg_base::~vul_arg_base()
{
}

char const* vul_arg_base::option()
{ return option_; }

//: Parse the list of arguments....
void vul_arg_parse(int& argc, char **& argv,
                          bool warn_about_unrecognized_arguments)
{
  vul_arg_base::parse_deprecated(argc, argv,
                                 warn_about_unrecognized_arguments);
}

//: Add an externally supplied list of args to the global list.
void vul_arg_include(vul_arg_info_list& l)
{
  vul_arg_base::include_deprecated(l);
}

//: Print all args, and usage messages.
void vul_arg_display_usage_and_exit(char const* msg)
{
  vul_arg_base::display_usage_and_exit(msg);
}


//: Returns true if arg was set on the command line.
bool vul_arg_base::set() const
{ return set_; }

static vul_arg_info_list& current_list() // instance "method"
{
  static vul_arg_info_list *list = 0;
  if (!list)
    list = new vul_arg_info_list;
  return *list;
}

//: Add another vul_arg_info_list to the current one.
// This allows for the inclusion of different sets of arguments into the
// main program, from different libraries.
void vul_arg_base::include_deprecated(vul_arg_info_list& l)
{
  current_list().include(l);
}

//
void vul_arg_base::add_to_current(vul_arg_base* a)
{
  current_list().add(a);
}

//: The main static method.
void vul_arg_base::parse_deprecated(int& argc, char **& argv, bool warn_about_unrecognized_arguments)
{
  current_list().parse(argc, argv, warn_about_unrecognized_arguments);
}

void vul_arg_base::set_help_option(char const* str)
{
  current_list().set_help_option( str);
}

void vul_arg_base::display_usage(char const* msg)
{
  if (msg) vcl_cerr << "** WARNING ** " << msg << vcl_endl;
  current_list().display_help("");
}

void vul_arg_base::display_usage_and_exit(char const* msg)
{
  if (msg) vcl_cerr << "** ERROR ** " << msg << vcl_endl;
  current_list().display_help("");
  vcl_exit(-1);
}

// vul_arg_base constructors

vul_arg_base::vul_arg_base(vul_arg_info_list& l, char const* option, char const*helpstring):
  option_(option),
  optlen_(option ? vcl_strlen(option) : 0),
  help_(helpstring)
{
  l.add(this);
}

vul_arg_base::vul_arg_base(char const* option, char const*helpstring):
  option_(option),
  optlen_(option ? vcl_strlen(option) : 0),
  help_(helpstring)
{
  current_list().add(this);
}

//------------------------------------------------------------------------------

//: Construct an empty vul_arg_info_list.
vul_arg_info_list::vul_arg_info_list(vul_arg_info_list::autonomy autonomy__):
  help("-?"), // default help operator!
  verbose_(false),
  autonomy_(autonomy__)
{
}

void vul_arg_info_list::verbose(bool on)
{ verbose_ = on; }


//: Destructor.
vul_arg_info_list::~vul_arg_info_list()
{
}

//: Change the help operator (defaults to -?)
void vul_arg_info_list::set_help_option(char const* str)
{
  // check that the operator isn't already being used
  for (int i=0; i<args.size(); i++) {
    if (vcl_strcmp(args[i]->option(),str) == 0) {
      vcl_cerr << "vul_arg_info_list: WARNING: requested help operator already assigned\n";
      return;
    }
  }

  help = str;
}

//: Add an argument to the list.
void vul_arg_info_list::add(vul_arg_base* argmt)
{
  if ( argmt->option() && help == argmt->option() )
    vcl_cerr << "vul_arg_info_list: WARNING: '-" << help
             << "' option reserved and will be ignored\n";
  else
    args.push_back(argmt);
}

//: Append another list.  The other list is not copied, just pointed to.
void vul_arg_info_list::include(vul_arg_info_list& l)
{
  assert(&l != this);

  for(int i = 0; i < l.args.size(); ++i)
    add(l.args[i]);
}

//: Display help about each option in the arg list.
// Note that this function does not exit at the end.
void vul_arg_info_list::display_help( char const*progname)
{
  if (progname)
    vcl_cerr << "Usage: " << progname << " ";
  else
    vcl_cerr << "Usage: aprog ";

  // Print "prog [-a int] string string"
  for ( int i=0; i< args.size(); i++) {
    if (args[i]->option()) {
      vcl_cerr << "[" << args[i]->option();
      if (vcl_strlen(args[i]->type_)> 0)
        vcl_cerr << " " << args[i]->type_;
      vcl_cerr << "] ";
    } else {
      // options without switches are required.
      vcl_cerr << args[i]->type_ << " ";
    }
  }

  vcl_cerr << vcl_endl << vcl_endl;

  // Find longest option, type name, or default
  int maxl_option  = 8; // Length of "REQUIRED"
  int maxl_type    = 0; // Length of "Type"
  //  int maxl_default = 0;
  for (int i=0; i< args.size(); i++)
    if (args[i]->help_) {
      if (args[i]->option()) {
        int l = vcl_strlen(args[i]->option());
        if (l > maxl_option)
          maxl_option = l;
      }
      int l = vcl_strlen(args[i]->type_);
      if (l > maxl_type)
        maxl_type = l;
    }

  // Print long form of args
  char fmtbuf[1024];
  vcl_sprintf(fmtbuf, "%%%ds %%-%ds %%s ", maxl_option, maxl_type);

  // Do required args first
  vul_printf(vcl_cerr, "REQUIRED:\n");
  for (int i=0; i< args.size(); i++)
    if (args[i]->help_)
      if (args[i]->option() == 0) {
        vul_printf(vcl_cerr, fmtbuf, "", args[i]->type_, args[i]->help_);
        vcl_cerr << " ["; args[i]->print_value(vcl_cerr); vcl_cerr << "]\n"; // default
      }
  vcl_cerr << vcl_endl;

  // Then others
  vul_printf(vcl_cerr, "Optional:\n");
  vul_printf(vcl_cerr, fmtbuf, "Switch", "Type", "Help [default value]") << vcl_endl << vcl_endl;
  for (int i=0; i< args.size(); i++)
    if (args[i]->help_)
      if (args[i]->option() != 0) {
        vul_printf(vcl_cerr, fmtbuf, args[i]->option(), args[i]->type_, args[i]->help_);
        vcl_cerr << " ["; args[i]->print_value(vcl_cerr); vcl_cerr << "]\n"; // default
      }
}

//: Parse the command line, using the current list of args.
//  Remove all recognised arguments from the command line by modifying argc and argv.
void vul_arg_info_list::parse(int& argc, char **& argv, bool warn_about_unrecognized_arguments)
{
  vcl_vector<bool> done_once(args.size(), false);

  // 0. Check that there are no duplicate switches, O(n^2) as n is tiny.
  for (int i = 0; i < args.size(); ++i)
    if (args[i]->option_)
      for (int j = i+1; j < args.size(); ++j)
        if (args[j]->option_)
          if (0==vcl_strcmp(args[i]->option_, args[j]->option_))
            vcl_cerr << "vul_arg_info_list: WARNING: repeated switch ["
                     << args[j]->option_ << "]\n";

  // 0a. Clear "set" flags on args
  for (int i = 0; i < args.size(); ++i)
    args[i]->set_ = false;

  // 1. Collect option arguments (i.e. ones with "-"),
  // and squeeze them out of argv.
  // Make sure to do things sequentially

  char ** my_argv = argv + 1; // Skip program name
  while (*my_argv) {
    char* argmt = *my_argv;
    bool eaten = false;
    for (int i = 0; i < args.size(); ++i)
      if (args[i]->option_) {
        if ( help == argmt ) { // look for the '-?' operator (i.e. HELP)
          display_help(argv[0]);
          vcl_exit(1);
        }

        if (0==vcl_strcmp(argmt, args[i]->option_)) {
          done_once[i] = true;
          int advance = args[i]->parse(my_argv + 1);
          args[i]->set_ = true;
          if (advance >= 0) {
            // Pull down remaining args
            for(char ** av = my_argv; *(av + advance); ++av)
              *av = *(av + advance + 1);

            eaten = true;
            break;
          }
        }
      }
    if (!eaten)
      ++my_argv;
  }

  if (verbose_) {
    vcl_cerr << "args remaining:";
    for(char ** av = argv; *av; ++av)
      vcl_cerr << " [" << *av << "]";
    vcl_cerr << vcl_endl;
  }

  // 2. Just take from the list to fill the non-option arguments
  my_argv = argv + 1;
  int num_satisfied = 0;
  for (int i = 0; i < args.size(); ++i)
    if (!args[i]->option_)
      if (*my_argv) {
        done_once[i] = true;
        int advance = args[i]->parse(my_argv);
        my_argv += advance;
        ++num_satisfied;
      } else {
        display_help(argv[0]);

        vcl_cerr << "\nargParse::ERROR: Required arg " << (num_satisfied+1)
                 << " not supplied\n\n";
        vcl_exit(1);
      }


  // 3. Move my_argv down to first unused arg, and reset argc
  argc = 1;
  for(char ** av = my_argv; *av; ++av)
    ++argc;
  for(int i = 1; i < argc; ++i)
    argv[i] = my_argv[i-1];
  argv[argc] = 0;

  // 4. Error checking.
  //
  // 4.2 Sometimes it's bad if all args weren't used (i.e. trailing args)
  if (autonomy_ == all) {
    vcl_cerr << "vul_arg_info_list: Some arguments were unused: ";
    for(char ** av = argv; *av; ++av)
      vcl_cerr << " " << *av;
    vcl_cerr << vcl_endl;
    display_help(argv[0]);
  }

  // 4.3 It's often bad if a switch was not recognized.
  if (warn_about_unrecognized_arguments)
    for(char ** av = argv; *av; ++av)
      if (**av == '-') {
        display_help(argv[0]);
        vcl_cerr << "vul_arg_info_list: WARNING: Unparsed switch [" << *av << "]\n";
      }

  // 5. Some people like a chatty program.
#if 0
  //fsm: do not print outcome -- it looks like an error message.
  if (verbose_) {
    // Print outcome
    for (int i = 0; i < args.size(); ++i)
      if (args[i]->option_) {
        vcl_cerr << "Switch " << args[i]->option_ << ": "
                 << (!done_once[i]? "not ":"") << "done, value [";
        args[i]->print_value(vcl_cerr);
        vcl_cerr << "]\n";
      }

    for (int i = 0; i < args.size(); ++i)
      if (!args[i]->option_) {
        vcl_cerr << "Trailer: ";
        args[i]->print_value(vcl_cerr);
        vcl_cerr << vcl_endl;
      }

    vcl_cerr << "args remaining [argc = " << argc << "]:";
    for(char ** av = argv; *av; ++av)
      vcl_cerr << " " << *av;
    vcl_cerr << "\n--------------\n";
  }
#endif
}


//------------------------------------------------------------------------------

//: function to parse matlab or UNIX style integer ranges.
// eg. 1:3 is matlab for 1,2,3 and 1-3 is UNIX for 1,2,3
//
// parsed as follows:
//   any character other than '-' and ':' is considered a list separator
//   simple ranges can be written as 1:3 or 1-3 (=1,2,3) or 3:1 (=3,2,1)
//   complete ranges can be written as 1:2:5 or 1-2-5 (=1,3,5)
//   negative numbers are handled 'transparently'
//   (e.g. -1:-3 or -1--3 or even -1--1--3 ...:).
static int list_parse(vcl_list<int> &out, char ** argv)
{
  out.clear();

  // Empty list specified as the last argument.
  if( !argv[0] ) return 0;

  vcl_string str(argv[0]);

#define REGEXP_INTEGER "\\-?[0123456789]+"

  vul_reg_exp range_regexp("(" REGEXP_INTEGER ")"      // int
                           "([:-]" REGEXP_INTEGER ")?" // :int [optional]
                           "([:-]" REGEXP_INTEGER ")?" // :int [optional]
                          );

  bool error= false;

  while (str.length() > 0 && range_regexp.find(str)) {
    // the start/end positions (ref from 0) of the
    //    current ',' separated token.
    long start= range_regexp.start(0);
    long endp = range_regexp.end(0);
    if (start != 0) {
      vcl_cerr << "vul_arg<vcl_list<int> >: Bad argument [" << argv[0] << "]\n";
      return 0;
    }

    // this is the current token.
//  vcl_string token = str.substr(start, endp);
//  vcl_cout << "KYM token = " << token << endl;
    vcl_string match1 = range_regexp.match(1);
//  vcl_cout << "KYM match1 = " << match1 << endl;
    vcl_string match2 = range_regexp.match(2);
//  vcl_cout << "KYM match2 = " << match2 << endl;
    vcl_string match3 = range_regexp.match(3);
//  vcl_cout << "KYM match3 = " << match3 << endl;

    // Remove this match from the front of string.
    str.erase(0, endp + 1);

#if 0
    vcl_cerr << "Range regexp matched [" << token <<  "]: parts ["
             << match1<<"] ["<<match2<<"] ["<<match3<<"]\n"
             << "  str->[" << str << "]\n";
#endif

    bool matched2 = range_regexp.match(2).size() > 0;
    bool matched3 = range_regexp.match(3).size() > 0;

    int s = vul_string_atoi(match1);
    int d = 1;
    int e = s;
    if (matched3) {
      // "1:2:10"
      d = vul_string_atoi(match2.substr(1));
      e = vul_string_atoi(match3.substr(1));
    } else if (matched2) {
      d = 1;
      e = vul_string_atoi(match2.substr(1));
    } else {
      d = 1;
      e = s;
    }

    // vcl_cerr << "  " << s << ":" << d << ":" << e << vcl_endl;
    if (e >= s) {
      if (d < 0) {
        vcl_cerr << "WARNING: d < 0\n";
        d = -d;
      }
      for(int i = s; i <= e; i += d)
        out.push_back(i);
    } else {
      if (d > 0) {
        vcl_cerr << "WARNING: d > 0\n";
        d = -d;
      }
      for(int i = s; i >= e; i += d)
        out.push_back(i);
    }
  }

  return error ? 0 : 1;
}

//------------------------------------------------------------------------------

// specializations for specific types.
// In emacs, C-s for "//: unsigned" to find the implementation for vul_arg<unsigned>
// In vi: "/^\/\/: unsigned"

#if 1
# define VDS VCL_DEFINE_SPECIALIZATION
#else
# define VDS /* template <> */
#endif

//: bool
VDS void settype(vul_arg<bool> &argmt) { argmt.type_ = "bool"; }
VDS void print_value(vcl_ostream &s, vul_arg<bool> const &argmt)
{ s << (argmt() ? "set" : "not set"); }
VDS int parse(vul_arg<bool>* argmt, char ** /*argv*/) {
  argmt->value_ = true;
  return 0; // bool sucks zero args, most others take one.
}
template class vul_arg<bool>;

//: int
VDS void settype(vul_arg<int> &argmt) { argmt.type_ = "integer"; }
VDS void print_value(vcl_ostream  &s, vul_arg<int> const &argmt)
{ s << argmt(); }
VDS int parse(vul_arg<int>* argmt, char ** argv) {
  char* endptr = 0;
  double v = strtod(argv[0], &endptr);
  if (*endptr != '\0') {
    // There is junk after the number, or no number was found
    vcl_cerr << "vul_arg_parse: WARNING: Attempt to parse " << *argv << " as int\n";
    return -1;
  }
  if (v != vcl_floor(v)) {
    vcl_cerr << "vul_arg_parse: Expected integer: saw " << argv[0] << vcl_endl;
    return -1;
  }
  argmt->value_ = int(v);
  return 1;
}
template class vul_arg<int>;

//: unsigned
VDS void settype(vul_arg<unsigned> &argmt) { argmt.type_ = "integer"; }
VDS void print_value(vcl_ostream &s, vul_arg<unsigned> const &argmt)
{ s << argmt(); }
VDS int parse(vul_arg<unsigned>* argmt, char ** argv) {
  char* endptr = 0;
  double v = strtod(argv[0], &endptr);
  if (*endptr != '\0') {
    // There is junk after the number, or no number was found
    vcl_cerr << "vul_arg_parse: WARNING: Attempt to parse " << *argv << " as int\n";
    return -1;
  }
  if (v != vcl_floor(v)) {
    vcl_cerr << "vul_arg_parse: Expected integer: saw " << argv[0] << vcl_endl;
    return -1;
  }
  argmt->value_ = unsigned(v);
  return 1;
}
template class vul_arg<unsigned>;

//: float
VDS void settype(vul_arg<float> &argmt) { argmt.type_ = "float"; }
VDS void print_value(vcl_ostream &s, vul_arg<float> const &argmt)
{ s << argmt(); }
VDS int parse(vul_arg<float>* argmt, char ** argv) {
  char* endptr = 0;
  argmt->value_ = strtod(argv[0], &endptr);
  if (*endptr == '\0')
    return 1;
  // There is junk after the number, or no number was found
  vcl_cerr << "vul_arg_parse: WARNING: Attempt to parse " << *argv << " as float\n";
  return -1;
}
template class vul_arg<float>;

//: double
VDS void settype(vul_arg<double> &argmt) { argmt.type_ = "float"; }
VDS void print_value(vcl_ostream &s, vul_arg<double> const &argmt)
{ s << argmt(); }
VDS int parse(vul_arg<double>* argmt, char ** argv) {
  char* endptr = 0;
  argmt->value_ = strtod(argv[0], &endptr);
  if (*endptr == '\0')
    return 1;
  // There is junk after the number, or no number was found
  vcl_cerr << "vul_arg_parse: WARNING: Attempt to parse " << *argv << " as double\n";
  return -1;
}
template class vul_arg<double>;

//: char *
VDS void settype(vul_arg<char *> &argmt) { argmt.type_ = "string"; }
VDS void print_value(vcl_ostream &s, vul_arg<char *> const &argmt)
{ s << '\'' << (argmt()?argmt():"(null)") << '\''; }
VDS int parse(vul_arg<char*>* argmt, char ** argv) {
  argmt->value_ = argv[0]; // argv is valid till the end of the program so
  return 1;                // it's ok to just grab the pointer.
}
template class vul_arg<char*>;

//: char const *
VDS void settype(vul_arg<char const *> &argmt) { argmt.type_ = "string"; }
VDS void print_value(vcl_ostream &s, vul_arg<char const *> const &argmt)
{ s << '\'' << argmt() << '\''; }
VDS int parse(vul_arg<char const *>* argmt, char ** argv) {
  argmt->value_ = argv[0]; // argv is valid till the end of the program so
  return 1;                // it's ok to just grab the pointer.
}
template class vul_arg<char const*>;

//: vcl_string
VDS void settype(vul_arg<vcl_string> &argmt) { argmt.type_ = "string"; }
VDS void print_value(vcl_ostream &s, vul_arg<vcl_string> const &argmt)
{ s << '\'' << argmt() << '\''; }
VDS int parse(vul_arg<vcl_string>* argmt, char ** argv) {
  if (argv[0]) {
    argmt->value_ = argv[0];
    return 1;
  }
  else {
    vcl_cerr << __FILE__ ": no argument to string option\n";
    return 0;
  }
}
template class vul_arg<vcl_string>;

//: vcl_list<int>
VDS void settype(vul_arg<vcl_list<int> > &argmt) { argmt.type_ = "integer list"; }
VDS void print_value(vcl_ostream &s, vul_arg<vcl_list<int> > const &argmt) {
  for (vcl_list<int>::const_iterator i=argmt().begin(); i!=argmt().end(); ++i)
    s << ' ' << *i;
}
VDS int parse(vul_arg<vcl_list<int> >* argmt, char ** argv) {
  return list_parse(argmt->value_,argv);
}
template class vul_arg<vcl_list<int> >;

//: vcl_vector<int>
VDS void settype(vul_arg<vcl_vector<int> > &argmt) { argmt.type_ = "integer list"; }
VDS void print_value(vcl_ostream &s, vul_arg<vcl_vector<int> > const &argmt) {
  for (unsigned i=0; i<argmt().size(); ++i)
    s << ' ' << argmt()[i];
}
VDS int parse(vul_arg<vcl_vector<int> >* argmt, char ** argv) {
  vcl_list<int> tmp;
  int retval = list_parse(tmp,argv);
  // Defaults should be cleared when the user supplies a value
  argmt->value_.clear();
  for (vcl_list<int>::iterator i=tmp.begin(); i!=tmp.end(); ++i)
    argmt->value_.push_back( *i );
  return retval;
}
template class vul_arg<vcl_vector<int> >;

//: vcl_vector<unsigned>
VDS void settype(vul_arg<vcl_vector<unsigned> > &argmt){argmt.type_="integer list";}
VDS void print_value(vcl_ostream &s, vul_arg<vcl_vector<unsigned> > const &argmt) {
  for (unsigned i=0; i<argmt().size(); ++i)
    s << ' ' << argmt()[i];
}
VDS int parse(vul_arg<vcl_vector<unsigned> >* argmt, char ** argv) {
  vcl_list<int> tmp;
  int retval = list_parse(tmp,argv);
  // Defaults should be cleared when the user supplies a value
  argmt->value_.clear();
  for (vcl_list<int>::iterator i=tmp.begin(); i!=tmp.end(); ++i)
    argmt->value_.push_back( unsigned(*i) );
  return retval;
}
template class vul_arg<vcl_vector<unsigned> >;

//: vcl_vector<double>
VDS void settype(vul_arg<vcl_vector<double> > &argmt) {argmt.type_ = "double list";}
VDS void print_value(vcl_ostream &s, vul_arg<vcl_vector<double> > const &argmt) {
  for (unsigned int i=0; i<argmt().size(); ++i)
    s << ' ' << argmt()[i];
}
VDS int parse(vul_arg<vcl_vector<double> >* argmt, char ** argv) {
  int sucked = 0;
  // Defaults should be cleared when the user supplies a value
  argmt->value_.clear();
  while (argv[0]) {
    char* endptr = 0;
    double tmp = strtod(argv[0], &endptr);
    //argmt->value_
    if (*endptr == '\0') {
      argmt->value_.push_back(tmp);
      ++ sucked;
      ++ argv;
    }
    else if (endptr == argv[0])
      break; // OK. end of list of doubles.
    else {
      // There is junk after the number, or no number was found
      vcl_cerr << "vul_arg_parse: WARNING: Attempt to parse " << *argv << " as double\n";
      return -1;
    }
  }
  return sucked;
}

template class vul_arg<vcl_vector<double> >;
