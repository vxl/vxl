
#ifdef __GNUC__
#pragma implementation "vbl_arg.h"
#endif

// Note that even though part of this file is templated, it is a .cxx file because the
// templated code is only used in this file.  A .txx file should contain only templated
// code

#include <assert.h>
#include <stdio.h>

#include <vcl/vcl_string.h>
#include <vcl/vcl_vector.h>
#include <vcl/vcl_list.h>

#include <vbl/vbl_arg.h>
#include <vbl/vbl_string.h>
#include <vbl/vbl_printf.h>
#include <vbl/vbl_sprintf.h>

//
static vbl_arg_info_list& current_list(void)
{
  static vbl_arg_info_list *list = 0;
  if (!list)
    list = new vbl_arg_info_list;
  return *list;
}

// -- Add another vbl_arg_info_list to the current one.  This allows for the inclusion
// of different sets of arguments into the main program, from different libraries.
void vbl_arg_base::include(vbl_arg_info_list& l)
{
  current_list().include(l);
}

//
void vbl_arg_base::add_to_current(vbl_arg_base* a)
{
  current_list().add(a);
}

// -- The main static method.
void vbl_arg_base::parse(int& argc, char **& argv, bool warn_about_unrecognized_arguments)
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

/////////////////////////////////////////////////////////////////////////////
// vbl_arg_base methods

vbl_arg_base::vbl_arg_base(vbl_arg_info_list& l, char const* option, char const*helpstring):
  _option(option),
  _optlen(option ? strlen(option) : 0),
  _help(helpstring)
{
  l.add(this);
}

vbl_arg_base::vbl_arg_base(char const* option, char const*helpstring):
  _option(option),
  _optlen(option ? strlen(option) : 0),
  _help(helpstring)
{
  current_list().add(this);
}

/////////////////////////////////////////////////////////////////////////////
// "Private" function used by Geoff's numlist parser
//   should this go somewhere else?

// function used by parse( arg<vcl_list<int> >, ...) which returns
//  the value of the first _number_ in a GenString and the length of 
//  this number (i.e. for the string "-321:3" it would return -321 and 4).
static
int parse_gen_string_for_int(vcl_string str, int &value)
{
  int numberlength= 0;

  // zero length string => error

  if( str.length() < 1)
    return 0;

  // look for first digit

  vbl_string_compile("[0123456789]");

  // if it is in position 2 or worse (i.e. allows 1 and -1 but not --1) => error
  if(( !vbl_string_find(str)) || ( vbl_string_start() > 1))
    return 0;
 
  // if it is in position 1 but position 0 is not a '-' => error
  if(( vbl_string_start() == 1) && ( str[0L]!= '-'))
    return 0;

  // at the stage we KNOW that we have a valid digit, we just have to find it's length
  //  so let's just look for the next non-number...
  long previousstart= vbl_string_start();

  while(( vbl_string_find(str)) && ( vbl_string_start()== ( previousstart+ 1)))
      previousstart= vbl_string_start();

  numberlength= previousstart+1;

  // if we found a number then 
  if( numberlength> 0)
      value= atoi(str.c_str());

  return numberlength;
}

template<class T> 
ostream& vbl_arg<T>::print_value(ostream &s)
{
// Could this ever have worked ?
/* 
  if ((strcmp( _type, "string") == 0) && (_value==0))
    return s << "";
  
  else if ( strcmp( _type, "bool")== 0) {
    if( _value==0 )
      return s << "not set";
    else
      return s << "set";
  }

  else
    return s << _value;
*/
return s;
}

/////////////////////////////////////////////////////////////////////////////
// Specialized parse methods which implement the actual char* to Type conversions.

// lists of integer ranges (matlab or UNIX styles accepted)
//    parsed as follows:  any character other than '-' and ':' is considered a list separator
//                        simple ranges can be written as 1:3 or 1-3 (=1,2,3) or 3:1 (=3,2,1)
//                        complete ranges can be written as 1:2:5 or 1-2-5 (=1,3,5)
//                        negative numbers are handled 'transparently' (e.g. -1:-3 
//                              or -1--3 or even -1--1--3 ...:).
static int list_parse(vcl_list<int> &listje, char ** argv)
{
  // sorry geoff (or whoever wrote this), your indentation style made the function
  // twice as long as it need to be and that's annoying when one is trying to read
  // it. it is a truly marvellous function, though.
  listje.clear();
  vcl_string str( argv[0]);

  vbl_string_compile("[0123456789:-]+");

  bool error= false;

  while( vbl_string_find(str)) {
    // the start/end positions (ref from 0) of the
    //    current ',' separated token.
    long start= vbl_string_start();
    long end  = vbl_string_end();
    
    // this is the current token.
    vcl_string token;
    
    token = str.substr(start, end);
    
    int value1;
    int value2;
    int value3;
    
    unsigned int l;
    
    if(( token[long(token.length()-1)]!= '-') && ( token[long(token.length()-1)]!= ':')) {
      l= parse_gen_string_for_int( token, value1);
      
      if( l) {
	token.erase( 0, (l+1>token.length())?token.length():l+1);
	
	l= parse_gen_string_for_int( token, value2);
	
	if( l) {
	  token.erase( 0, (l+1>token.length())?token.length():l+1);
	  
	  l= parse_gen_string_for_int( token, value3);
	  
	  if( l) {
	    token.erase( 0, (l+1>token.length())?token.length():l+1);
	    
	    if(token.length()> 0)
	      error= true;
	    else {
	      // completely described range 1:3:5 or 1:-2:3 or 1-2-3...
	      //  note that things like 1:-1:3 is empty.
	      
	      if((( value3- value1) * value2)>= 0) {
		for( int v= value1; ((v-value1)*value2)<= ((value3-value1)*value2); v+= value2)
		  listje.push_back( v);
		
	      }
	    }
	  }
	  else {
	    if( token.length()> 0)
	      error= true;
	    else {
	      // simple range 1-3 or 1:3 or 3:1...
	      int inc= (value2>value1)?1:-1;  // direction of range
	      
	      for( int v= value1; ((v-value1)*inc)<= ((value2-value1)*inc); v+= inc)
		listje.push_back( v);
	    }
	  }
	}
	else {
	  // single value case
	  listje.push_back( value1);
	}
      }
      else 
	error = true;
    }
  }
  
  return error ? 0 : 1;
}

//--------------------------------------------------------------------------------

// vcl_list<int>
VCL_DECLARE_SPECIALIZATION(int parse(vbl_arg<vcl_list<int> >* arg, char ** argv))
int parse(vbl_arg<vcl_list<int> >* arg, char ** argv)
{
  return list_parse(arg->_value,argv);
}

// fsm@robots.ox.ac.uk : the native SGI compiler doesn't like a class method to
// be redeclared outside its class. So the VCL_DECLARE_SPECIALIZATION must come
// before the definition of the specialization. I seem to remember that this
// chokes some other compiler, but I can't remember which. I'd like to know.
VCL_DECLARE_SPECIALIZATION(void vbl_arg<vcl_list<int> >::settype());
void vbl_arg<vcl_list<int> >::settype()
{
  _type= "integer list";
}

//--------------------------------------------------------------------------------

// vcl_vector<int> :
VCL_DECLARE_SPECIALIZATION(int parse(vbl_arg<vcl_vector<int> >* arg, char ** argv));
int parse(vbl_arg<vcl_vector<int> >* arg, char ** argv)
{
  vcl_list<int> tmp;
  int retval = list_parse(tmp,argv);
  for (vcl_list<int>::iterator i=tmp.begin() ; i!=tmp.end() ; ++i)
    arg->_value.push_back( *i );
  return retval;
}

VCL_DECLARE_SPECIALIZATION(ostream& vbl_arg<vcl_vector<int> >::print_value(ostream &s));
ostream& vbl_arg<vcl_vector<int> >::print_value(ostream &s)
{
  for (unsigned i=0;i<_value.size();i++)
    s << ' ' << _value[i];
  return s << flush;
}
  
VCL_DECLARE_SPECIALIZATION(void vbl_arg<vcl_vector<int> >::settype());
void vbl_arg<vcl_vector<int> >::settype()
{
  _type= "integer vector";
}

// vcl_vector<unsigned> :
VCL_DECLARE_SPECIALIZATION(int parse(vbl_arg<vcl_vector<unsigned> >* arg, char ** argv));
int parse(vbl_arg<vcl_vector<unsigned> >* arg, char ** argv)
{
  vcl_list<int> tmp;
  int retval = list_parse(tmp,argv);
  for (vcl_list<int>::iterator i=tmp.begin() ; i!=tmp.end() ; ++i)
    arg->_value.push_back( *i );
  return retval;
}


VCL_DECLARE_SPECIALIZATION(ostream& vbl_arg<vcl_vector<unsigned> >::print_value( ostream &s));
ostream& vbl_arg<vcl_vector<unsigned> >::print_value( ostream &s)
{
  for (unsigned i=0;i<_value.size();i++)
    s << ' ' << _value[i];
  return s << flush;
}

VCL_DECLARE_SPECIALIZATION(void vbl_arg<vcl_vector<unsigned> >::settype());
void vbl_arg<vcl_vector<unsigned> >::settype()
{
  _type= "integer vector";
}

//--------------------------------------------------------------------------------  

// bool sucks zero args, most others take one.
VCL_DECLARE_SPECIALIZATION(int parse(vbl_arg<bool>* arg, char ** /*argv*/));
int parse(vbl_arg<bool>* arg, char ** /*argv*/)
{
  arg->_value = true;
  return 0;
}

VCL_DECLARE_SPECIALIZATION(int parse(vbl_arg<double>* arg, char ** argv));
int parse(vbl_arg<double>* arg, char ** argv)
{
  char* endptr = 0;
  arg->_value = strtod(argv[0], &endptr);
  if (*endptr == '\0')
    return 1;

  // There is junk after the number, or no number was found
  cerr << "vbl_arg_parse: WARNING: Attempt to parse " << *argv << " as double\n";
  
  return -1;
}

VCL_DECLARE_SPECIALIZATION(int parse(vbl_arg<float>* arg, char ** argv));
int parse(vbl_arg<float>* arg, char ** argv)
{
  char* endptr = 0;
  arg->_value = strtod(argv[0], &endptr);
  if (*endptr == '\0')
    return 1;

  // There is junk after the number, or no number was found
  cerr << "vbl_arg_parse: WARNING: Attempt to parse " << *argv << " as double\n";

  return -1;
}

VCL_DECLARE_SPECIALIZATION(int parse(vbl_arg<int>* arg, char ** argv));
int parse(vbl_arg<int>* arg, char ** argv)
{
  char* endptr = 0;
  double v = strtod(argv[0], &endptr);

  if (*endptr != '\0') {
    // There is junk after the number, or no number was found
    cerr << "vbl_arg_parse: WARNING: Attempt to parse " << *argv << " as int\n";
    return -1;
  }
  
  if (v != int(v)) {
    cerr << "vbl_arg_parse: Expected integer: saw " << argv[0] << endl;
    return -1;
  }

  arg->_value = (int)v;
  
  return 1;
}

// Remembering that argv is alive longer than the arglist, we just grab the pointer.
VCL_DECLARE_SPECIALIZATION(int parse(vbl_arg<char*>* arg, char ** argv));
int parse(vbl_arg<char*>* arg, char ** argv)
{
  arg->_value = argv[0];
  return 1;
}

//--------------------------------------------------------------------------------

// 

VCL_DECLARE_SPECIALIZATION(int parse(vbl_arg<vcl_string>* arg, char ** argv));
int parse(vbl_arg<vcl_string>* arg, char ** argv)
{
  arg->_value = argv[0];
  return 1;
}

VCL_DECLARE_SPECIALIZATION(ostream& vbl_arg<vcl_string>::print_value( ostream &s));
ostream& vbl_arg<vcl_string>::print_value( ostream &s)
{
  return s << this->_value << flush;
}

//--------------------------------------------------------------------------------

// settype functions seem to need to be out here for win32
VCL_DECLARE_SPECIALIZATION(void vbl_arg<bool>::settype());
void vbl_arg<bool>::settype()
{
  _type= "bool";
}

VCL_DECLARE_SPECIALIZATION(void vbl_arg<int>::settype());
void vbl_arg<int>::settype()
{
  _type= "integer";
}

VCL_DECLARE_SPECIALIZATION(void vbl_arg<double>::settype());
void vbl_arg<double>::settype()
{
  _type= "float";
}

VCL_DECLARE_SPECIALIZATION(void vbl_arg<float>::settype());
void vbl_arg<float>::settype()
{
  _type= "float";
}

VCL_DECLARE_SPECIALIZATION(void vbl_arg<char *>::settype());
void vbl_arg<char *>::settype()
{
  _type= "string";
}

VCL_DECLARE_SPECIALIZATION(void vbl_arg<vcl_string>::settype());
void vbl_arg<vcl_string>::settype()
{
  _type= "string";
}

//--------------------------------------------------------------------------------
// instantiations must come after specializations.
// or rather, specializations must come before first use.

template class vbl_arg<vcl_list<int> >;
template class vbl_arg<vcl_vector<int> >;
template class vbl_arg<vcl_vector<unsigned> >;
template class vbl_arg<bool>;
template class vbl_arg<int>;
template class vbl_arg<double>;
template class vbl_arg<float>;
template class vbl_arg<char*>;
template class vbl_arg<vcl_string>;

// -- Construct an empty vbl_arg_info_list.
vbl_arg_info_list::vbl_arg_info_list(autonomy autonomy__):
  nargs(0),
  args(new vbl_arg_base*[100]),
  help(new char[3]),
  verbose_(false),
  autonomy_(autonomy__)
{
  strcpy(help,"-?");  // default help operator!
}

// -- Destructor.
vbl_arg_info_list::~vbl_arg_info_list()
{
  delete [] help;
  delete [] args;
}

// -- Change the help operator (defaults to -?)
void vbl_arg_info_list::set_help_option(char const* str)
{
  // check that the operator isn't already being used
  for (int i=0; i<nargs; i++) {
    if (strcmp(args[i]->option(),str)==0) {
      cerr << "vbl_arg_info_list: WARNING: requested help operator already assigned" 
		     << endl;
      return;
    }
  }

  if (strlen(str)!=strlen(help)) {
    delete help;
    help= new char[strlen(str)+1];
  }

  strcpy(help,str);
}

// -- Add an argument to the list.
void vbl_arg_info_list::add(vbl_arg_base* arg)
{
  if( arg->option()) {
    if (strcmp(arg->option(),help)==0)
      cerr << "vbl_arg_info_list: WARNING: '-" << help 
		     << "' option reserved and will be ignored" << endl;
    else
      args[nargs++] = arg;
  }
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
      if (strlen(args[i]->_type)> 0)
	cout << " " << args[i]->_type;
      cout << "] ";
    } else {
      // options without switches are required.
      cout << args[i]->_type << " ";
    }
  }

  cout << endl << endl;

  // Find longest option, type name, or default
  int maxl_option  = 8; // Length of "REQUIRED"
  int maxl_type    = 0; // Length of "Type"
  //  int maxl_default = 0;
  for (int i=0; i< nargs; i++)
    if (args[i]->_help) {
      if (args[i]->option()) {
	int l = strlen(args[i]->option());
	if (l > maxl_option)
	  maxl_option = l;
      }
      int l = strlen(args[i]->_type);
      if (l > maxl_type)
	maxl_type = l;
    }
	
  // Print long form of args
  char fmtbuf[1024];
  sprintf(fmtbuf, "%%%ds %%-%ds %%s ", maxl_option, maxl_type);
  
  // Do required args first
  vbl_printf(cerr, "REQUIRED:\n");
  for (int i=0; i< nargs; i++)
    if (args[i]->_help)
      if (args[i]->option() == 0) {
	vbl_printf(cerr, fmtbuf, "", args[i]->_type, args[i]->_help);
	cerr << " ["; args[i]->print_value(cerr); cerr << "]\n"; // default
      }
  cerr << endl;

  // Then others
  vbl_printf(cerr, "Optional:\n");
  vbl_printf(cerr, fmtbuf, "Switch", "Type", "Help [default]") << endl << endl;
  for (int i=0; i< nargs; i++)
    if (args[i]->_help)
      if (args[i]->option() != 0) {
	vbl_printf(cerr, fmtbuf, args[i]->option(), args[i]->_type, args[i]->_help);
	
	cerr << " ["; args[i]->print_value(cerr); cerr << "]\n"; // default
      }
}

// -- Parse the command line, using the current list of args.  Remove all
// recognised arguments from the command line by modifying argc and argv.
void vbl_arg_info_list::parse(int& argc, char **& argv, bool warn_about_unrecognized_arguments)
{
  vcl_vector<bool> done_once(nargs, false);

  // 0. Check that there are no duplicate switches, O(n^2) as n is tiny.
  {
    for (int i = 0; i < nargs; ++i)
      if (args[i]->_option)
	for (int j = i+1; j < nargs; ++j)
	  if (args[j]->_option)
	    if (0==strcmp(args[i]->_option, args[j]->_option))
	      cerr << "vbl_arg_info_list: WARNING: repeated switch [" 
			     << args[j]->_option << "]\n";
  }

  // 0a. Clear "set" flags on args
  {
    for (int i = 0; i < nargs; ++i)
      args[i]->_set = false;
  }
  
  
  // 1. Collect option arguments (i.e. ones with "-"),
  // and squeeze them out of argv.
  // Make sure to do things sequentially
  
  char ** my_argv = argv + 1; // Skip program name
  while (*my_argv) {
    char* arg = *my_argv;
    bool eaten = false;
    for (int i = 0; i < nargs; ++i)
      if (args[i]->_option) {
	if (0==strcmp(arg, help)) { // look for the '-?' operator (i.e. HELP)
	  display_help(argv[0]);
	  exit(1);
	}

	if (0==strcmp(arg, args[i]->_option)) {
	  done_once[i] = true;
	  int advance = args[i]->parse(my_argv + 1);
	  args[i]->_set = true;
	  if (advance >= 0) {
	    // Pull down remaining args
	    for(char ** av = my_argv; *av; ++av)
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
    if (!args[i]->_option)
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
    // Print outcome
    for (int i = 0; i < nargs; ++i)
      if (args[i]->_option) {
	cerr << "Switch " << args[i]->_option << ": ";
	cerr << (!done_once[i]? "not ":"") << "done, ";
	cerr << "value [";
	args[i]->print_value(cerr);
	cerr << "]\n";
      }

    for (int i = 0; i < nargs; ++i)
      if (!args[i]->_option) {
	cerr << "Trailer: ";
	args[i]->print_value(cerr);
	cerr << endl;
      }

    cerr << "args remaining [argc = " << argc << "]:";
    for(char ** av = argv; *av; ++av)
      cerr << " " << *av;
    cerr << endl;
    cerr << "--------------\n";
  }

  return;
}
