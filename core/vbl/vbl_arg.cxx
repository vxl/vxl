// Note that even though this file defines instances of a templated 
// class, it is a .cxx file and not a .txx file because it does not 
// supply a class definition for use by clients.
//
// If you need to define your own vbl_arg<T>, you should #include vbl_arg.h
// ONLY, in your source file (myarg.cxx, say), define these three global 
// functions (which can by static if you like) in myarg.cxx
// -  void settype(vbl_arg<T> &);
// -  void print_value(vbl_arg<T> const &, ostream &);
// -  int  parse(vbl_arg<T>*, char**);
// and then instantiate the class vbl_arg<T> as usual (in myarg.cxx).

#ifdef __GNUC__
#pragma implementation
#endif

#include <vcl/vcl_cassert.h>
#include <vcl/vcl_cstdio.h>
#include <vcl/vcl_cmath.h>   // floor()
#include <vcl/vcl_vector.h>
#include <vcl/vcl_list.h>
#include <vcl/vcl_iostream.h>

#include <vbl/vbl_arg.h>
#include <vbl/vbl_string.h>
#include <vbl/vbl_reg_exp.h>
#include <vbl/vbl_printf.h>
#include <vbl/vbl_sprintf.h>

//--------------------------------------------------------------------------------

// class vbl_arg_base

static vbl_arg_info_list& current_list() // instance "method"
{
  static vbl_arg_info_list *list = 0;
  if (!list)
    list = new vbl_arg_info_list;
  return *list;
}

// -- Add another vbl_arg_info_list to the current one.  This allows for the inclusion
// of different sets of arguments into the main program, from different libraries.
void vbl_arg_base::include_deprecated(vbl_arg_info_list& l)
{
  current_list().include(l);
}

//
void vbl_arg_base::add_to_current(vbl_arg_base* a)
{
  current_list().add(a);
}

// -- The main static method.
void vbl_arg_base::parse_deprecated(int& argc, char **& argv, bool warn_about_unrecognized_arguments)
{
  current_list().parse(argc, argv, warn_about_unrecognized_arguments);
}

void vbl_arg_base::set_help_option(char const* str)
{
  current_list().set_help_option( str);
}

void vbl_arg_base::display_usage(char const* msg)
{
  if (msg) cerr << "** WARNING ** " << msg << endl;
  current_list().display_help("");
}

void vbl_arg_base::display_usage_and_exit(char const* msg)
{
  if (msg) cerr << "** ERROR ** " << msg << endl;
  current_list().display_help("");
  exit(-1);
}

// vbl_arg_base constructors

vbl_arg_base::vbl_arg_base(vbl_arg_info_list& l, char const* option, char const*helpstring):
  option_(option),
  optlen_(option ? strlen(option) : 0),
  help_(helpstring)
{
  l.add(this);
}

vbl_arg_base::vbl_arg_base(char const* option, char const*helpstring):
  option_(option),
  optlen_(option ? strlen(option) : 0),
  help_(helpstring)
{
  current_list().add(this);
}

//--------------------------------------------------------------------------------

// -- Construct an empty vbl_arg_info_list.
vbl_arg_info_list::vbl_arg_info_list(autonomy autonomy__):
  nargs(0),
  args(new vbl_arg_base*[100]),
  help("-?"), // default help operator!
  verbose_(false),
  autonomy_(autonomy__)
{
}

// -- Destructor.
vbl_arg_info_list::~vbl_arg_info_list()
{
  delete [] args;
}

// -- Change the help operator (defaults to -?)
void vbl_arg_info_list::set_help_option(char const* str)
{
  // check that the operator isn't already being used
  for (int i=0; i<nargs; i++) {
    if (strcmp(args[i]->option(),str) == 0) {
      cerr << "vbl_arg_info_list: WARNING: requested help operator already assigned" 
	   << endl;
      return;
    }
  }

  help = str;
}

// -- Add an argument to the list.
void vbl_arg_info_list::add(vbl_arg_base* arg)
{
  if ( arg->option() && help == arg->option() )
    cerr << "vbl_arg_info_list: WARNING: '-" << help 
	 << "' option reserved and will be ignored" << endl;
  else
    args[nargs++] = arg;
}

// -- Append another list.  The other list is not copied, just pointed to.
void vbl_arg_info_list::include(vbl_arg_info_list& l)
{
  assert(&l != this);

  for(int i = 0; i < l.nargs; ++i)
    add(l.args[i]);
}

// -- Display help about each option in the arg list.
// note that this function does not exit at the end.
void vbl_arg_info_list::display_help( char const*progname)
{
  if (progname)
    cout << "Usage: " << progname << " ";
  else
    cout << "Usage: aprog ";

  // Print "prog [-a int] string string"
  for ( int i=0; i< nargs; i++) {
    if (args[i]->option()) {
      cout << "[";
      cout << args[i]->option();
      if (strlen(args[i]->type_)> 0)
	cout << " " << args[i]->type_;
      cout << "] ";
    } else {
      // options without switches are required.
      cout << args[i]->type_ << " ";
    }
  }

  cout << endl << endl;

  // Find longest option, type name, or default
  int maxl_option  = 8; // Length of "REQUIRED"
  int maxl_type    = 0; // Length of "Type"
  //  int maxl_default = 0;
  for (int i=0; i< nargs; i++)
    if (args[i]->help_) {
      if (args[i]->option()) {
	int l = strlen(args[i]->option());
	if (l > maxl_option)
	  maxl_option = l;
      }
      int l = strlen(args[i]->type_);
      if (l > maxl_type)
	maxl_type = l;
    }
	
  // Print long form of args
  char fmtbuf[1024];
  sprintf(fmtbuf, "%%%ds %%-%ds %%s ", maxl_option, maxl_type);
  
  // Do required args first
  vbl_printf(cerr, "REQUIRED:\n");
  for (int i=0; i< nargs; i++)
    if (args[i]->help_)
      if (args[i]->option() == 0) {
	vbl_printf(cerr, fmtbuf, "", args[i]->type_, args[i]->help_);
	cerr << " ["; args[i]->print_value(cerr); cerr << "]\n"; // default
      }
  cerr << endl;

  // Then others
  vbl_printf(cerr, "Optional:\n");
  vbl_printf(cerr, fmtbuf, "Switch", "Type", "Help [value]") << endl << endl;
  for (int i=0; i< nargs; i++)
    if (args[i]->help_)
      if (args[i]->option() != 0) {
	vbl_printf(cerr, fmtbuf, args[i]->option(), args[i]->type_, args[i]->help_);
	
	cerr << " ["; args[i]->print_value(cerr); cerr << "]\n"; // default
      }
}

// -- Parse the command line, using the current list of args.  Remove all
// recognised arguments from the command line by modifying argc and argv.
void vbl_arg_info_list::parse(int& argc, char **& argv, bool warn_about_unrecognized_arguments)
{
  vcl_vector<bool> done_once(nargs, false);

  // 0. Check that there are no duplicate switches, O(n^2) as n is tiny.
  for (int i = 0; i < nargs; ++i)
    if (args[i]->option_)
      for (int j = i+1; j < nargs; ++j)
	if (args[j]->option_)
	  if (0==strcmp(args[i]->option_, args[j]->option_))
	    cerr << "vbl_arg_info_list: WARNING: repeated switch [" 
		 << args[j]->option_ << "]\n";
  
  // 0a. Clear "set" flags on args
  for (int i = 0; i < nargs; ++i)
    args[i]->set_ = false;
  
  // 1. Collect option arguments (i.e. ones with "-"),
  // and squeeze them out of argv.
  // Make sure to do things sequentially
  
  char ** my_argv = argv + 1; // Skip program name
  while (*my_argv) {
    char* arg = *my_argv;
    bool eaten = false;
    for (int i = 0; i < nargs; ++i)
      if (args[i]->option_) {
	if ( help == arg ) { // look for the '-?' operator (i.e. HELP)
	  display_help(argv[0]);
	  exit(1);
	}

	if (0==strcmp(arg, args[i]->option_)) {
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
    cerr << "args remaining:";
    for(char ** av = argv; *av; ++av)
      cerr << " [" << *av << "]";
    cerr << endl;
  }

  // 2. Just take from the list to fill the non-option arguments
  my_argv = argv + 1;
  int num_satisfied = 0;
  for (int i = 0; i < nargs; ++i)
    if (!args[i]->option_)
      if (*my_argv) {
	done_once[i] = true;
	int advance = args[i]->parse(my_argv);
	my_argv += advance;
	++num_satisfied;
      } else {
	display_help(argv[0]);

	cerr << "\nargParse::ERROR: Required arg " << (num_satisfied+1) 
	     << " not supplied\n\n";
	exit(1);
      }
  

  // 3. Move my_argv down to first unused arg, and reset argc
  argc = 1;
  for(char ** av = my_argv; *av; ++av)
    ++argc;
  for(int i = 1; i < argc; ++i)
    argv[i] = my_argv[i-1];
  argv[argc] = 0;

  // 4. Error checking.

  // 4.2 Sometimes it's bad if all args weren't used (i.e. trailing args)
  if (autonomy_ == all) {
    cerr << "vbl_arg_info_list: Some arguments were unused: ";
    for(char ** av = argv; *av; ++av)
      cerr << " " << *av;
    cerr << endl;
    display_help(argv[0]);
  }
  
  // 4.3 It's often bad if a switch was not recognized.
  if (warn_about_unrecognized_arguments)
    for(char ** av = argv; *av; ++av)
      if (**av == '-') {
	display_help(argv[0]);
	cerr << "vbl_arg_info_list: WARNING: Unparsed switch [" << *av << "]\n";
      }
  
  // 5. Some people like a chatty program.
  if (verbose_) {
#if 0 //fsm: do not print outcome -- it looks like an error message.
    // Print outcome
    for (int i = 0; i < nargs; ++i)
      if (args[i]->option_) {
	cerr << "Switch " << args[i]->option_ << ": ";
	cerr << (!done_once[i]? "not ":"") << "done, ";
	cerr << "value [";
	args[i]->print_value(cerr);
	cerr << "]\n";
      }

    for (int i = 0; i < nargs; ++i)
      if (!args[i]->option_) {
	cerr << "Trailer: ";
	args[i]->print_value(cerr);
	cerr << endl;
      }

    cerr << "args remaining [argc = " << argc << "]:";
    for(char ** av = argv; *av; ++av)
      cerr << " " << *av;
    cerr << endl;
    cerr << "--------------\n";
#endif
  }
}






//--------------------------------------------------------------------------------

// function to parse matlab or UNIX style integer ranges.
// eg. 1:3 is matlab for 1,2,3 and 1-3 is UNIX for 1,2,3
//
//    parsed as follows:  any character other than '-' and ':' is considered a list separator
//                        simple ranges can be written as 1:3 or 1-3 (=1,2,3) or 3:1 (=3,2,1)
//                        complete ranges can be written as 1:2:5 or 1-2-5 (=1,3,5)
//                        negative numbers are handled 'transparently' (e.g. -1:-3 
//                              or -1--3 or even -1--1--3 ...:).
static int list_parse(vcl_list<int> &out, char ** argv)
{
  // sorry geoff (or whoever wrote this), your indentation style made the function
  // twice as long as it need to be and that's annoying when one is trying to read
  // it. it is a truly marvellous function, though.

  out.clear();
  vcl_string str(argv[0]);
  
#define REGEXP_INTEGER "\\-?[0123456789]+"

  vbl_reg_exp range_regexp("(" REGEXP_INTEGER ")"      // int
			   "([:-]" REGEXP_INTEGER ")?" // :int [optional]
			   "([:-]" REGEXP_INTEGER ")?" // :int [optional]
			   );
			   
  bool error= false;

  while (str.length() > 0 && range_regexp.find(str)) {
    // the start/end positions (ref from 0) of the
    //    current ',' separated token.
    long start= range_regexp.start(0);
    long end  = range_regexp.end(0);
    if (start != 0) {
      cerr << "vbl_arg<vcl_list<int> >: Bad argument [" << argv[0] << "]\n";
      return 0;
    }
    
    // this is the current token.
    vcl_string token = str.substr(start, end);
    vcl_string match1 = range_regexp.match(1);
    vcl_string match2 = range_regexp.match(2);
    vcl_string match3 = range_regexp.match(3);

    // Remove this match from the front of string.
    str.erase(0, end + 1);

    // cerr << "Range regexp matched [" << token <<  "]: parts ["<<match1<<"] ["<<match2<<"] ["<<match3<<"]\n"; cerr << "  str->[" << str << "]" << endl;
    
    bool matched2 = range_regexp.match(2).size() > 0;
    bool matched3 = range_regexp.match(3).size() > 0;

    int s = vbl_string_atoi(match1);
    int d = 1;
    int e = s;
    if (matched3) {
      // "1:2:10"
      d = vbl_string_atoi(match2.substr(1));
      e = vbl_string_atoi(match3.substr(1));
    } else if (matched2) {
      d = 1;
      e = vbl_string_atoi(match2.substr(1));
    } else {
      d = 1;
      e = s;
    }

    // cerr << "  " << s << ":" << d << ":" << e << endl;
    if (e >= s) {
      if (d < 0) {
	cerr << "WARNING: d < 0\n";
	d = -d;
      }
      for(int i = s; i <= e; i += d)
	out.push_back(i);
    } else {
      if (d > 0) {
	cerr << "WARNING: d > 0\n";
	d = -d;
      }
      for(int i = s; i >= e; i += d)
	out.push_back(i);
    }
  }
  
  return error ? 0 : 1;
}

//--------------------------------------------------------------------------------

// specializations for specific types. C-s for "//: unsigned" to find the
// implementation for vbl_arg<unsigned>

#if 1
# define VDS VCL_DEFINE_SPECIALIZATION
#else
# define VDS /* template <> */
#endif

//: bool
VDS void settype(vbl_arg<bool> &arg) { arg.type_ = "bool"; }
VDS void print_value(ostream &s, vbl_arg<bool> const &arg) 
{ s << (arg() ? "set" : "not set"); }
VDS int parse(vbl_arg<bool>* arg, char ** /*argv*/) {
  arg->value_ = true;
  return 0; // bool sucks zero args, most others take one.
}
template class vbl_arg<bool>;

//: int
VDS void settype(vbl_arg<int> &arg) { arg.type_ = "integer"; }
VDS void print_value(ostream  &s, vbl_arg<int> const &arg) 
{ s << arg(); }
VDS int parse(vbl_arg<int>* arg, char ** argv) {
  char* endptr = 0;
  double v = strtod(argv[0], &endptr);
  if (*endptr != '\0') {
    // There is junk after the number, or no number was found
    cerr << "vbl_arg_parse: WARNING: Attempt to parse " << *argv << " as int\n";
    return -1;
  }
  if (v != floor(v)) {
    cerr << "vbl_arg_parse: Expected integer: saw " << argv[0] << endl;
    return -1;
  }
  arg->value_ = int(v);
  return 1;
}
template class vbl_arg<int>;

//: unsigned
VDS void settype(vbl_arg<unsigned> &arg) { arg.type_ = "integer"; }
VDS void print_value(ostream &s, vbl_arg<unsigned> const &arg) 
{ s << arg(); }
VDS int parse(vbl_arg<unsigned>* arg, char ** argv) {
  char* endptr = 0;
  double v = strtod(argv[0], &endptr);
  if (*endptr != '\0') {
    // There is junk after the number, or no number was found
    cerr << "vbl_arg_parse: WARNING: Attempt to parse " << *argv << " as int\n";
    return -1;
  }
  if (v != floor(v)) {
    cerr << "vbl_arg_parse: Expected integer: saw " << argv[0] << endl;
    return -1;
  }
  arg->value_ = unsigned(v);
  return 1;
}
template class vbl_arg<unsigned>;

//: float
VDS void settype(vbl_arg<float> &arg) { arg.type_ = "float"; }
VDS void print_value(ostream &s, vbl_arg<float> const &arg) 
{ s << arg(); }
VDS int parse(vbl_arg<float>* arg, char ** argv) {
  char* endptr = 0;
  arg->value_ = strtod(argv[0], &endptr);
  if (*endptr == '\0')
    return 1;
  // There is junk after the number, or no number was found
  cerr << "vbl_arg_parse: WARNING: Attempt to parse " << *argv << " as float\n";
  return -1;
}
template class vbl_arg<float>;

//: double
VDS void settype(vbl_arg<double> &arg) { arg.type_ = "float"; }
VDS void print_value(ostream &s, vbl_arg<double> const &arg) 
{ s << arg(); }
VDS int parse(vbl_arg<double>* arg, char ** argv) {
  char* endptr = 0;
  arg->value_ = strtod(argv[0], &endptr);
  if (*endptr == '\0')
    return 1;
  // There is junk after the number, or no number was found
  cerr << "vbl_arg_parse: WARNING: Attempt to parse " << *argv << " as double\n";
  return -1;
}
template class vbl_arg<double>;

//: char *
VDS void settype(vbl_arg<char *> &arg) { arg.type_ = "string"; }
VDS void print_value(ostream &s, vbl_arg<char *> const &arg) 
{ s << '\'' << (arg()?arg():"(null)") << '\''; }
VDS int parse(vbl_arg<char*>* arg, char ** argv) {
  arg->value_ = argv[0]; // argv is valid till the end of the program so 
  return 1;              // it's ok to just grab the pointer.
}
template class vbl_arg<char*>;

//: char const *
VDS void settype(vbl_arg<char const *> &arg) { arg.type_ = "string"; }
VDS void print_value(ostream &s, vbl_arg<char const *> const &arg) 
{ s << '\'' << arg() << '\''; }
VDS int parse(vbl_arg<char const *>* arg, char ** argv) {
  arg->value_ = argv[0]; // argv is valid till the end of the program so 
  return 1;              // it's ok to just grab the pointer.
}
template class vbl_arg<char const*>;

//: vcl_string
VDS void settype(vbl_arg<vcl_string> &arg) { arg.type_ = "string"; }
VDS void print_value(ostream &s, vbl_arg<vcl_string> const &arg) 
{ s << '\'' << arg() << '\''; }
VDS int parse(vbl_arg<vcl_string>* arg, char ** argv) {
  if (argv[0]) {
    arg->value_ = argv[0];
    return 1;
  }
  else {
    cerr << __FILE__ ": no argument to string option" << endl;
    return 0;
  }
}
template class vbl_arg<vcl_string>;

//: vcl_list<int>
VDS void settype(vbl_arg<vcl_list<int> > &arg) { arg.type_ = "integer list"; }
VDS void print_value(ostream &s, vbl_arg<vcl_list<int> > const &arg) {
  for (vcl_list<int>::const_iterator i=arg().begin(); i!=arg().end(); ++i)
    s << ' ' << *i;
}
VDS int parse(vbl_arg<vcl_list<int> >* arg, char ** argv) {
  return list_parse(arg->value_,argv);
}
template class vbl_arg<vcl_list<int> >;

//: vcl_vector<int>
VDS void settype(vbl_arg<vcl_vector<int> > &arg) { arg.type_ = "integer list"; }
VDS void print_value(ostream &s, vbl_arg<vcl_vector<int> > const &arg) {
  for (unsigned i=0; i<arg().size(); ++i)
    s << ' ' << arg()[i];
}
VDS int parse(vbl_arg<vcl_vector<int> >* arg, char ** argv) {
  vcl_list<int> tmp;
  int retval = list_parse(tmp,argv);
  for (vcl_list<int>::iterator i=tmp.begin() ; i!=tmp.end() ; ++i)
    arg->value_.push_back( *i );
  return retval;
}
template class vbl_arg<vcl_vector<int> >;

//: vcl_vector<unsigned>
VDS void settype(vbl_arg<vcl_vector<unsigned> > &arg) { arg.type_ = "integer list"; }
VDS void print_value(ostream &s, vbl_arg<vcl_vector<unsigned> > const &arg) {
  for (unsigned i=0; i<arg().size(); ++i)
    s << ' ' << arg()[i];
}
VDS int parse(vbl_arg<vcl_vector<unsigned> >* arg, char ** argv) {
  vcl_list<int> tmp;
  int retval = list_parse(tmp,argv);
  for (vcl_list<int>::iterator i=tmp.begin() ; i!=tmp.end() ; ++i)
    arg->value_.push_back( unsigned(*i) );
  return retval;
}
template class vbl_arg<vcl_vector<unsigned> >;
