// This is core/vul/vul_debug.h
#ifndef vul_debug_h_
#define vul_debug_h_

//:
// \file
// \author Ian Scott


//: Dump a core file.
// Not yet implemented on non-windows platforms.
void vul_debug_core_dump(const char * filename);


//: Force a core dump whilst inside a Structured Exception Handler in a windows program.
// To get a core dump from a Windows structured exceptions
// \verbatim
// void main()
// {
//   __try
//   {
//     // Rest of program
//   }
//   __except(vul_debug_core_dump2(filename, GetExceptionInformation()))
//   {}
// }
// \endverbatim
void vul_debug_core_dump_in_windows_se(const char * filename,
                                       void* pep);
#include <vcl_config_compiler.h>

#ifdef VCL_HAS_EXCEPTIONS
# include <vcl_stdexcept.h>
//: A translated structured exception.
class vul_debug_windows_structured_exception : public vcl_runtime_error
{
  void * ex_ptr_;
public:
  //: Windows structured exception code.
	unsigned code() const;
  //: Related execution address.
	void *address() const;
  virtual const char *what( ) const throw( );
  vul_debug_windows_structured_exception(void * ex_ptr):
    vcl_runtime_error(""), ex_ptr_(ex_ptr) {}
  virtual ~vul_debug_windows_structured_exception() throw() {}
};
#else
class vul_debug_windows_structured_exception {};
#endif //  VCL_HAS_EXCEPTIONS

//: Setup the system to core dump and throw a C++ exception on detection of a Structured Exception
// The system will throw vul_debug_windows_structured_exception.
// You must compile your code with /EHa to get the compiler to correctly handle SEs.
void vul_debug_set_coredump_and_throw_on_windows_se(const char * filename);

//: Setup the system to core dump and throw a C++ exception on detection of out of memory.
// The system will throw vcl_bad_alloc. 
void vul_debug_set_coredump_and_throw_on_out_of_memory(const char * filename);




#endif // vul_debug_h_

