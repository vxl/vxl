// This is core/vul/vul_arg.cxx
//:
// \file
// Note that even though this file defines instances of a templated
// class, it is a .cxx file and not a .hxx file because it does not
// supply a class definition for use by clients.
//
// If you need to define your own vul_arg<T>, you should #include vul_arg.h
// ONLY, in your source file (myarg.cxx, say), define these three global
// functions (which can by static if you like) in myarg.cxx
// \code
//   void settype(vul_arg<T> &);
//   void print_value(vul_arg<T> const &, std::ostream &);
//   int  parse(vul_arg<T>*, char**);
// \endcode
// and then instantiate the class vul_arg<T> as usual (in myarg.cxx).

#include <algorithm>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <list>
#include "vul_arg.h"

#include <cassert>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

#include "vul/vul_sprintf.h"
#include "vul/vul_string.h"
#include "vul/vul_reg_exp.h"
#include "vul/vul_printf.h"

//------------------------------------------------------------------------------

// definition: used in constructor of vul_arg for required flags
vul_arg_base::required_option_type vul_arg_base::is_required;


char const *
vul_arg_base::option()
{
  return option_.c_str();
}

char const *
vul_arg_base::help()
{
  return help_.c_str();
}

//: Parse the list of arguments....
void
vul_arg_parse(int & argc, char **& argv, bool warn_about_unrecognized_arguments)
{
  vul_arg_base::parse_deprecated(argc, argv, warn_about_unrecognized_arguments);
}

//: Add an externally supplied list of args to the global list.
void
vul_arg_include(vul_arg_info_list & l)
{
  vul_arg_base::include_deprecated(l);
}

//: Print all args, and usage messages.
void
vul_arg_display_usage_and_exit(char const * msg)
{
  vul_arg_base::display_usage_and_exit(msg);
}


//: Returns true if arg was set on the command line.
bool
vul_arg_base::set() const
{
  return set_;
}

static vul_arg_info_list &
current_list() // instance "method"
{
  static vul_arg_info_list list;
  return list;
}

//: Add another vul_arg_info_list to the current one.
// This allows for the inclusion of different sets of arguments into the
// main program, from different libraries.
void
vul_arg_base::include_deprecated(vul_arg_info_list & l)
{
  current_list().include(l);
}

//
void
vul_arg_base::add_to_current(vul_arg_base * a)
{
  current_list().add(a);
}

//: The main static method.
void
vul_arg_base::parse_deprecated(int & argc, char **& argv, bool warn_about_unrecognized_arguments)
{
  current_list().parse(argc, argv, warn_about_unrecognized_arguments);
}

void
vul_arg_base::set_help_option(char const * str)
{
  current_list().set_help_option(str);
}

void
vul_arg_base::set_help_precis(char const * str)
{
  current_list().set_help_precis(str);
}

void
vul_arg_base::set_help_description(char const * str)
{
  current_list().set_help_description(str);
}

void
vul_arg_base::display_usage(char const * msg)
{
  if (msg)
    std::cerr << "** WARNING ** " << msg << std::endl;
  current_list().display_help("");
}

void
vul_arg_base::display_usage_and_exit(char const * msg)
{
  if (msg)
    std::cerr << "** ERROR ** " << msg << std::endl;
  current_list().display_help("");
  std::exit(-1);
}

// vul_arg_base constructors

vul_arg_base::vul_arg_base(vul_arg_info_list & l, char const * option_string, char const * helpstring, bool required)
  : set_(false)
  , required_(required)
  , option_(option_string ? option_string : "\0")
  , help_(helpstring ? helpstring : "\0")
{
  l.add(this);
}

vul_arg_base::vul_arg_base(char const * option_string, char const * helpstring, bool required)
  : set_(false)
  , required_(required)
  , option_(option_string ? option_string : "\0")
  , help_(helpstring ? helpstring : "\0")
{
  current_list().add(this);
}

//------------------------------------------------------------------------------

//: Change the help operator (defaults to -?)
void
vul_arg_info_list::set_help_option(char const * str)
{
  // check that the operator isn't already being used
  for (auto & arg : args_)
  {
    if (std::strcmp(arg->option(), str) == 0)
    {
      std::cerr << "vul_arg_info_list: WARNING: requested help operator already assigned\n";
      return;
    }
  }

  help_ = str;
}


//: Add an argument to the list.
void
vul_arg_info_list::add(vul_arg_base * argmt)
{
  if (argmt->option() && help_ == argmt->option())
    std::cerr << "vul_arg_info_list: WARNING: '-" << help_ << "' option reserved and will be ignored\n";
  else
    args_.push_back(argmt);
}

//: Append another list.  The other list is not copied, just pointed to.
void
vul_arg_info_list::include(vul_arg_info_list & l)
{
  assert(&l != this);

  for (auto & arg : l.args_)
    add(arg);
}

//: Display help about each option in the arg list.
// Note that this function does not exit at the end.
void
vul_arg_info_list::display_help(char const * progname)
{
  if (progname)
    std::cerr << "Usage: " << progname << ' ';
  else
    std::cerr << "Usage: <prog_name> ";

  // Print "prog [-a int] string string"

  for (auto & arg : args_)
  {
    if (!arg->option_.empty())
    {
      if (!arg->required_)
        std::cerr << '[';
      std::cerr << arg->option();
      if (std::strlen(arg->type_) > 0)
        std::cerr << ' ' << arg->type_;
      if (!arg->required_)
        std::cerr << ']';
      std::cerr << ' ';
    }
    else
    {
      // options without switches are required.
      std::cerr << arg->type_ << ' ';
    }
  }

  std::cerr << std::endl << command_precis_ << std::endl;

  // Find longest option, type name, or default
  std::size_t maxl_option = std::max(std::size_t(8), help_.size()); // Length of "REQUIRED" or help option
  std::size_t maxl_type = 4;                                        // Length of "Type", minimum "bool"
  //  int maxl_default = 0;
  for (auto & arg : args_)
    if (!arg->help_.empty())
    {
      if (!arg->option_.empty())
      {
        std::size_t l = std::strlen(arg->option());
        if (l > maxl_option)
          maxl_option = l;
      }
      std::size_t l = std::strlen(arg->type_);
      if (l > maxl_type)
        maxl_type = l;
    }

  // Print long form of args
  std::string fmtbuf = vul_sprintf("%%%ds %%-%ds %%s ", maxl_option, maxl_type);

  // Do required args first
  vul_printf(std::cerr, "REQUIRED:\n");
  for (auto & arg : args_) // First required without option string
    if (!arg->help_.empty())
      if (arg->option_.empty() && !(arg->required_))
      {
        vul_printf(std::cerr, fmtbuf.c_str(), "", arg->type_, arg->help_.c_str());
        std::cerr << " [";
        arg->print_value(std::cerr);
        std::cerr << "]\n"; // default
      }
  for (auto & arg : args_) // Then required with option string
    if (!arg->help_.empty())
      if (arg->required_ && !arg->option_.empty())
      {
        vul_printf(std::cerr, fmtbuf.c_str(), arg->option(), arg->type_, arg->help_.c_str());
        std::cerr << '\n'; // ["; args_[i]->print_value(std::cerr); std::cerr << "]\n"; // default
      }

  std::cerr << std::endl;

  // Then others
  vul_printf(std::cerr, "Optional:\n");
  vul_printf(std::cerr, fmtbuf.c_str(), "Switch", "Type", "Help [default value]") << std::endl << std::endl;
  for (auto & arg : args_)
    if (!arg->help_.empty())
      if (!arg->option_.empty() && !(arg->required_))
      {
        vul_printf(std::cerr, fmtbuf.c_str(), arg->option(), arg->type_, arg->help_.c_str());
        std::cerr << " [";
        arg->print_value(std::cerr);
        std::cerr << "]\n"; // default
      }
  vul_printf(std::cerr, fmtbuf.c_str(), help_.c_str(), "bool", "Print this message\n");

  if (!description_.empty())
    std::cerr << '\n' << description_;
}

//: Parse the command line, using the current list of args.
//  Remove all recognised arguments from the command line by modifying argc and argv.
void
vul_arg_info_list::parse(int & argc, char **& argv, bool warn_about_unrecognized_arguments)
{
  std::vector<bool> done_once(args_.size(), false);

  // 0. Check that there are no duplicate switches, O(n^2) as n is tiny.
  for (unsigned int i = 0; i < args_.size(); ++i)
    if (!args_[i]->option_.empty())
      for (unsigned int j = i + 1; j < args_.size(); ++j)
        if (args_[i]->option_ == args_[j]->option_)
          std::cerr << "vul_arg_info_list: WARNING: repeated switch [" << args_[j]->option_ << "]\n";

  // 0a. Clear "set" flags on args
  for (auto & arg : args_)
    arg->set_ = false;

  // Generate shorter command name
  char * cmdname = argv[0] + std::strlen(argv[0]);
  while (cmdname > argv[0] && *cmdname != '/' && *cmdname != '\\')
    --cmdname;
  if (*cmdname == '\\' || *cmdname == '/')
    cmdname++;


  // 1. Collect option arguments (i.e. ones with "-"),
  // and squeeze them out of argv.
  // Make sure to do things sequentially

  char ** my_argv = argv + 1; // Skip program name
  while (*my_argv)
  {
    char * argmt = *my_argv;
    bool eaten = false;
    for (unsigned int i = 0; i < args_.size(); ++i)
    {
      if (!args_[i]->option_.empty())
      {
        if (help_ == argmt)
        { // look for the '-?' operator (i.e. HELP)
          display_help(cmdname);
          std::exit(1);
        }

        if (args_[i]->option_ == argmt)
        {
          done_once[i] = true;
          int advance = args_[i]->parse(my_argv + 1);
          args_[i]->set_ = true;
          if (advance >= 0)
          {
            // Pull down remaining args
            for (char ** av = my_argv; *(av + advance); ++av)
              *av = *(av + advance + 1);

            eaten = true;
            break;
          }
        }
      }
    }
    if (!eaten)
      ++my_argv;
  }

  if (verbose_)
  {
    std::cerr << "args remaining:";
    for (char ** av = argv; *av; ++av)
      std::cerr << " [" << *av << ']';
    std::cerr << std::endl;
  }


  // 2. Just take from the list to fill the non-option arguments
  my_argv = argv + 1;
  int num_satisfied = 0;
  for (unsigned int i = 0; i < args_.size(); ++i)
    if (args_[i]->option_.empty())
    {
      if (*my_argv)
      {
        done_once[i] = true;
        int advance = args_[i]->parse(my_argv);
        args_[i]->set_ = true;
        my_argv += advance;
        ++num_satisfied;
      }
      else
      {
        display_help(cmdname);

        std::cerr << "\nargParse::ERROR: Required arg " << (num_satisfied + 1) << " not supplied\n\n";
        std::exit(1);
      }
    }


  // 3. Move my_argv down to first unused arg, and reset argc
  argc = 1;
  for (char ** av = my_argv; *av; ++av)
    ++argc;
  for (int i = 1; i < argc; ++i)
    argv[i] = my_argv[i - 1];
  argv[argc] = nullptr;

  // 4. Error checking.
  //
  // 4.2 Sometimes it's bad if all args weren't used (i.e. trailing args)
  if (autonomy_ == all)
  {
    std::cerr << "vul_arg_info_list: Some arguments were unused: ";
    for (char ** av = argv; *av; ++av)
      std::cerr << ' ' << *av;
    std::cerr << std::endl;
    display_help(cmdname);
  }

  // 4.3 It's often bad if a switch was not recognized.
  if (warn_about_unrecognized_arguments)
    for (char ** av = argv; *av; ++av)
      if (**av == '-')
      {
        display_help(cmdname);
        std::cerr << "vul_arg_info_list: WARNING: Unparsed switch [" << *av << "]\n";
      }

  // 4.3 This is required arguments (including option) have been set
  for (auto & arg : args_)
    if (arg->required_ && !(arg->set_))
    {
      display_help(cmdname);

      std::cerr << "\nargParse::ERROR: Required arg " << arg->option_ << " not supplied\n\n";
      std::exit(1);
    }


    // 5. Some people like a chatty program.
#ifdef DEBUG // fsm: do not print outcome - it looks like an error message.
  if (verbose_)
  {
    // Print outcome
    for (unsigned int i = 0; i < args_.size(); ++i)
      if (args[i]->option_)
      {
        std::cerr << "Switch " << args_[i]->option_ << ": " << (!done_once[i] ? "not " : "") << "done, value [";
        args[i]->print_value(std::cerr);
        std::cerr << "]\n";
      }

    for (unsigned int i = 0; i < args.size(); ++i)
      if (!args[i]->option_)
      {
        std::cerr << "Trailer: ";
        args_[i]->print_value(std::cerr);
        std::cerr << std::endl;
      }

    std::cerr << "args remaining [argc = " << argc << "]:";
    for (char ** av = argv; *av; ++av)
      std::cerr << ' ' << *av;
    std::cerr << "\n--------------\n";
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
//
// Returns 1 on success and 0 on failure.
//
static int
list_parse(std::list<int> & out, char ** argv)
{
  out.clear();

  // Empty list specified as the last argument.
  if (!argv[0])
    return 0; // failure

  std::string str(argv[0]);

#define REGEXP_INTEGER "\\-?[0123456789]+"

  vul_reg_exp range_regexp("(" REGEXP_INTEGER ")"      // int
                           "([:-]" REGEXP_INTEGER ")?" // :int [optional]
                           "([:-]" REGEXP_INTEGER ")?" // :int [optional]
  );

  while (str.length() > 0 && range_regexp.find(str))
  {
    // the start/end positions (ref from 0) of the
    //    current ',' separated token.
    std::ptrdiff_t start = range_regexp.start(0);
    std::ptrdiff_t endp = range_regexp.end(0);
    if (start != 0)
    {
      std::cerr << "vul_arg<std::list<int> >: Bad argument [" << argv[0] << "]\n";
      return 0; // failure
    }

#ifdef DEBUG
    // this is the current token.
    std::string token = str.substr(start, endp);
    std::cerr << "token = " << token << '\n';
#endif
    std::string match1 = range_regexp.match(1);
#ifdef DEBUG
    std::cerr << "match1 = " << match1 << '\n';
#endif
    std::string match2 = range_regexp.match(2);
#ifdef DEBUG
    std::cerr << "match2 = " << match2 << '\n';
#endif
    std::string match3 = range_regexp.match(3);
#ifdef DEBUG
    std::cerr << "match3 = " << match3 << '\n';
#endif

    // Remove this match from the front of string.
    str.erase(0, endp);
    if (!str.empty() && str[0] == ',')
      str.erase(0, 1);

#if 0
    std::cerr << "Range regexp matched [" << token <<  "]: parts ["
             << match1<<"] ["<<match2<<"] ["<<match3<<"]\n"
             << "  str->[" << str << "]\n";
#endif

    bool matched2 = !range_regexp.match(2).empty();
    bool matched3 = !range_regexp.match(3).empty();

    int s = vul_string_atoi(match1);
    int d = 1;
    int e = s;
    if (matched3)
    {
      // "1:2:10"
      d = vul_string_atoi(match2.substr(1));
      e = vul_string_atoi(match3.substr(1));
    }
    else if (matched2)
      e = vul_string_atoi(match2.substr(1));

#ifdef DEBUG
    std::cerr << "  " << s << ':' << d << ':' << e << '\n';
#endif
    if (e >= s)
    {
      if (d < 0)
      {
        std::cerr << "WARNING: d < 0\n";
        d = -d;
      }
      for (int i = s; i <= e; i += d)
        out.push_back(i);
    }
    else
    {
      if (d > 0)
      {
        std::cerr << "WARNING: d > 0\n";
        d = -d;
      }
      for (int i = s; i >= e; i += d)
        out.push_back(i);
    }
  }

  if (str.empty())
    return 1; // success

  std::cerr << "vul_arg<std::list<int> >: Bad argument fragment  [" << str << "]\n";
  return 0;
}

//------------------------------------------------------------------------------

// specializations for specific types.
// In emacs, C-s for "//: unsigned" to find the implementation for vul_arg<unsigned>
// In vi: "/^\/\/: unsigned"

#if 1
#  define VDS template <>
#else
#  define VDS /* template <> */
#endif

//: bool
VDS void
settype(vul_arg<bool> & argmt)
{
  argmt.type_ = "bool";
}

VDS void
print_value(std::ostream & s, vul_arg<bool> const & argmt)
{
  s << (argmt() ? "set" : "not set");
}

VDS int
parse(vul_arg<bool> * argmt, char ** /*argv*/)
{
  argmt->value_ = true;
  return 0; // bool sucks zero args, most others take one.
}

template class vul_arg<bool>;

//: int
VDS void
settype(vul_arg<int> & argmt)
{
  argmt.type_ = "integer";
}

VDS void
print_value(std::ostream & s, vul_arg<int> const & argmt)
{
  s << argmt();
}

VDS int
parse(vul_arg<int> * argmt, char ** argv)
{
  if (!argv || !argv[0])
  {
    // no input
    std::cerr << "vul_arg_parse: Expected integer, none is provided.\n";
    return -1;
  }

  char * endptr = nullptr;
  double v = std::strtod(argv[0], &endptr);
  if (*endptr != '\0')
  {
    // There is junk after the number, or no number was found
    std::cerr << "vul_arg_parse: WARNING: Attempt to parse \"" << *argv << "\" as int\n";
    return -1;
  }
  if (v != std::floor(v))
  {
    std::cerr << "vul_arg_parse: Expected integer: saw " << argv[0] << std::endl;
    return -1;
  }
  argmt->value_ = int(v);
  return 1;
}

template class vul_arg<int>;

//: int64
#if VXL_HAS_INT_64
VDS void
settype(vul_arg<vxl_int_64> & argmt)
{
  argmt.type_ = "integer64";
}

VDS void
print_value(std::ostream & s, vul_arg<vxl_int_64> const & argmt)
{
  s << argmt();
}

VDS int
parse(vul_arg<vxl_int_64> * argmt, char ** argv)
{
  if (!argv || !argv[0])
  {
    // no input
    std::cerr << "vul_arg_parse: Expected integer, none is provided.\n";
    return -1;
  }

  // Ensure only digits are present allowing for the special case of an l or L suffix
  auto len = (unsigned long)std::strlen(argv[0]);
  for (unsigned long i = 0; i < len; ++i)
  {
    char tmp = argv[0][i];
    if (tmp < '0' || tmp > '9' ||                       // Make sure the number only contains valid digits
        ((tmp == 'l' || tmp == 'L') && i + 1 != len) || // Or the trailing l or L suffix
        (tmp == '-' && i != 0L && len <= 2L))           // Or a leading minus sign
    {
      std::cerr << "vul_arg_parse: WARNING: Attempt to parse \"" << *argv << "\" as int64\n";
      return -1;
    }
  }

  std::stringstream ss;
  ss << argv[0];
  ss >> argmt->value_;

  return 1;
}

template class vul_arg<vxl_int_64>;
#endif

//: unsigned
VDS void
settype(vul_arg<unsigned> & argmt)
{
  argmt.type_ = "integer";
}

VDS void
print_value(std::ostream & s, vul_arg<unsigned> const & argmt)
{
  s << argmt();
}

VDS int
parse(vul_arg<unsigned> * argmt, char ** argv)
{
  if (!argv || !argv[0])
  {
    // no input
    std::cerr << "vul_arg_parse: Expected integer, none is provided.\n";
    return -1;
  }

  char * endptr = nullptr;
  double v = std::strtod(argv[0], &endptr);
  if (*endptr != '\0')
  {
    // There is junk after the number, or no number was found
    std::cerr << "vul_arg_parse: WARNING: Attempt to parse " << *argv << " as int\n";
    return -1;
  }
  if (v != std::floor(v))
  {
    std::cerr << "vul_arg_parse: Expected integer: saw " << argv[0] << std::endl;
    return -1;
  }
  argmt->value_ = unsigned(v);
  return 1;
}

template class vul_arg<unsigned>;

//: float
VDS void
settype(vul_arg<float> & argmt)
{
  argmt.type_ = "float";
}

VDS void
print_value(std::ostream & s, vul_arg<float> const & argmt)
{
  s << argmt();
}

VDS int
parse(vul_arg<float> * argmt, char ** argv)
{
  if (!argv || !argv[0])
  {
    // no input
    std::cerr << "vul_arg_parse: Expected floating number, none is provided.\n";
    return -1;
  }

  char * endptr = nullptr;
  argmt->value_ = (float)std::strtod(argv[0], &endptr);
  if (*endptr == '\0')
    return 1;
  // There is junk after the number, or no number was found
  std::cerr << "vul_arg_parse: WARNING: Attempt to parse " << *argv << " as float\n";
  return -1;
}

template class vul_arg<float>;

//: double
VDS void
settype(vul_arg<double> & argmt)
{
  argmt.type_ = "float";
}

VDS void
print_value(std::ostream & s, vul_arg<double> const & argmt)
{
  s << argmt();
}

VDS int
parse(vul_arg<double> * argmt, char ** argv)
{
  if (!argv || !argv[0])
  {
    // no input
    std::cerr << "vul_arg_parse: Expected floating number, none is provided.\n";
    return -1;
  }

  char * endptr = nullptr;
  argmt->value_ = std::strtod(argv[0], &endptr);
  if (*endptr == '\0')
    return 1;
  // There is junk after the number, or no number was found
  std::cerr << "vul_arg_parse: WARNING: Attempt to parse " << *argv << " as double\n";
  return -1;
}

template class vul_arg<double>;

//: char *
VDS void
settype(vul_arg<char *> & argmt)
{
  argmt.type_ = "string";
}

VDS void
print_value(std::ostream & s, vul_arg<char *> const & argmt)
{
  s << '\'' << (argmt() ? argmt() : "(null)") << '\'';
}

VDS int
parse(vul_arg<char *> * argmt, char ** argv)
{
  // Reached the end?
  if (!argv || !argv[0])
  {
    // no input
    std::cerr << "vul_arg_parse: Expected string, none is provided.\n";
    return -1;
  }

  argmt->value_ = argv[0]; // argv is valid till the end of the program so
  return 1;                // it's ok to just grab the pointer.
}

template class vul_arg<char *>;

//: char const *
VDS void
settype(vul_arg<char const *> & argmt)
{
  argmt.type_ = "string";
}

VDS void
print_value(std::ostream & s, vul_arg<char const *> const & argmt)
{
  s << '\'' << (argmt() ? argmt() : "(null)") << '\'';
}

VDS int
parse(vul_arg<char const *> * argmt, char ** argv)
{
  if (!argv || !argv[0])
  {
    // no input
    std::cerr << "vul_arg_parse: Expected string, none is provided.\n";
    return -1;
  }

  argmt->value_ = argv[0]; // argv is valid till the end of the program so
  return 1;                // it's ok to just grab the pointer.
}

template class vul_arg<char const *>;

//: std::string
VDS void
settype(vul_arg<std::string> & argmt)
{
  argmt.type_ = "string";
}

VDS void
print_value(std::ostream & s, vul_arg<std::string> const & argmt)
{
  s << '\'' << argmt() << '\'';
}

VDS int
parse(vul_arg<std::string> * argmt, char ** argv)
{
  if (!argv || !argv[0])
  {
    // no input
    std::cerr << "vul_arg_parse: Expected string, none is provided.\n";
    return -1;
  }

  if (argv[0])
  {
    argmt->value_ = argv[0];
    return 1;
  }
  else
  {
    std::cerr << __FILE__ ": no argument to string option\n";
    return 0;
  }
}

template class vul_arg<std::string>;

//: std::list<int>
VDS void
settype(vul_arg<std::list<int>> & argmt)
{
  argmt.type_ = "integer list";
}

VDS void
print_value(std::ostream & s, vul_arg<std::list<int>> const & argmt)
{
  for (const auto i : argmt())
    s << ' ' << i;
}

VDS int
parse(vul_arg<std::list<int>> * argmt, char ** argv)
{
  return list_parse(argmt->value_, argv);
}

template class vul_arg<std::list<int>>;

//: std::vector<int>
VDS void
settype(vul_arg<std::vector<int>> & argmt)
{
  argmt.type_ = "integer list";
}

VDS void
print_value(std::ostream & s, vul_arg<std::vector<int>> const & argmt)
{
  for (int i : argmt())
    s << ' ' << i;
}

VDS int
parse(vul_arg<std::vector<int>> * argmt, char ** argv)
{
  std::list<int> tmp;
  int retval = list_parse(tmp, argv);
  // Defaults should be cleared when the user supplies a value
  argmt->value_.clear();
  for (const auto & i : tmp)
    argmt->value_.push_back(i);
  return retval;
}

template class vul_arg<std::vector<int>>;

//: std::vector<unsigned>
VDS void
settype(vul_arg<std::vector<unsigned>> & argmt)
{
  argmt.type_ = "integer list";
}

VDS void
print_value(std::ostream & s, vul_arg<std::vector<unsigned>> const & argmt)
{
  for (unsigned int i : argmt())
    s << ' ' << i;
}

VDS int
parse(vul_arg<std::vector<unsigned>> * argmt, char ** argv)
{
  std::list<int> tmp;
  int retval = list_parse(tmp, argv);
  // Defaults should be cleared when the user supplies a value
  argmt->value_.clear();
  for (const auto & i : tmp)
    argmt->value_.push_back(unsigned(i));
  return retval;
}

template class vul_arg<std::vector<unsigned>>;

//: std::vector<double>
VDS void
settype(vul_arg<std::vector<double>> & argmt)
{
  argmt.type_ = "double list";
}

VDS void
print_value(std::ostream & s, vul_arg<std::vector<double>> const & argmt)
{
  for (double i : argmt())
    s << ' ' << i;
}

VDS int
parse(vul_arg<std::vector<double>> * argmt, char ** argv)
{
  if (!argv || !argv[0])
  {
    // no input
    std::cerr << "vul_arg_parse: Expected a vector of floating number, none is provided.\n";
    return -1;
  }

  // if true don't treat space separator as leading to another double.
  bool found_at_least_one_comma = false;

  int sucked = 0;
  // Defaults should be cleared when the user supplies a value
  argmt->value_.clear();
  char * current = argv[0];
  while (current)
  {
    char * endptr = nullptr;
    double tmp = std::strtod(current, &endptr);
    // argmt->value_
    if (*endptr == '\0')
    {
      argmt->value_.push_back(tmp);
      ++sucked;
      ++argv;
      if (found_at_least_one_comma)
        return sucked;
      current = argv[0];
    }
    else if (*endptr == ',')
    {
      found_at_least_one_comma = true;
      argmt->value_.push_back(tmp);
      current = endptr + 1;
    }
    else if (endptr == current)
      break; // OK. end of list of doubles.
    else
    {
      // There is junk after the number, or no number was found
      std::cerr << "vul_arg_parse: WARNING: Attempt to parse " << current << " as double\n";
      return -1;
    }
  }
  return sucked;
}

template class vul_arg<std::vector<double>>;
