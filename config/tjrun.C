//:
// \file
// \verbatim
// Modifications
//  200697 AWF Removed the force-load for static links.  It never worked, and
//             the makefile does it now.
// \endverbatim

#include <cool/String.h>
#include <Basics/DLoadLib.h>

typedef int (* DL_FUNCTION)(int, char **);

// -- "C" linkage to make it easy to set breakpoints here.
extern "C" 
int cmain(int argc, char ** argv, DL_FUNCTION f)
{
  return (*f)(argc-1, argv+1);
}  

main(int argc, char ** argv)
{
  if (argc < 2) {
    vcl_cerr << "usage: tjrun <shared-library> <program-args>\n"; 
    vcl_cerr << " <shared-library> must contain a function called main\n";
    exit(1);
  }
 
  char *libname = argv[1];
  dlhandle_t library = tjDLoadLib::load_library(libname);
  if(!library) {
    vcl_cerr << "tjrun: error loading library " << libname << vcl_endl;
    vcl_cerr << tjDLoadLib::load_error_msg() << vcl_endl;
    return 1;
  }
      
  CoolString function_name = "main";
  DL_FUNCTION func = (DL_FUNCTION)tjDLoadLib::get_symbol_address(library, function_name);
  if (!func) {
    vcl_cerr << "tjrun: error getting symbol ``" << function_name << "''  from library " << vcl_endl;
    vcl_cerr << tjDLoadLib::load_error_msg() << vcl_endl;
    return 2;
  }

  return cmain(argc, argv, func);
}
