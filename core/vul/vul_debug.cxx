// This is core/vul/vul_debug.cxx

//: \file
//  \brief Get debug related information like core dumps, and stack traces
//  \author Ian Scott

#include "vul_debug.h"
#include <vcl_iostream.h>
#include <vxl_config.h>
#include <vcl_new.h>

#ifdef _WIN32

#if VXL_HAS_DBGHELP_H

#define NOATOM
#define NOGDI
#define NOGDICAPMASKS
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NOOPENFILE
#define NORASTEROPS
#define NOSCROLL
#define NOSOUND
#define NOSYSMETRICS
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOCRYPT
#define NOMCX
#include <windows.h>
#include <DbgHelp.h>
#include <vcl_cstdio.h>
#pragma comment (lib, "dbghelp")

static void vul_debug_core_dump_in_windows_seh(const char * filename,
                                               EXCEPTION_POINTERS* pep)
{
  HANDLE hFile = CreateFile( filename, GENERIC_READ | GENERIC_WRITE, 
    0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ); 

  if( ( hFile == NULL ) || ( hFile == INVALID_HANDLE_VALUE ) )
  {
    vcl_cerr << "WARNING: vul_debug_core_dump: Unable to create core dump file: " << filename << vcl_endl;
    return;
  }

	MINIDUMP_EXCEPTION_INFORMATION mdei; 
	mdei.ThreadId           = GetCurrentThreadId(); 
	mdei.ExceptionPointers  = pep; 
	mdei.ClientPointers     = FALSE; 

  if (! MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), 
    hFile, MiniDumpWithFullMemory, (pep != 0) ? &mdei : 0, 0, 0 ))
    vcl_cerr << "WARNING: vul_debug_core_dump: Unable to dump core: " << filename << vcl_endl;

  CloseHandle( hFile ); 
}

void vul_debug_core_dump_in_windows_seh(const char * filename,
                                        void* pep)
{
  vul_debug_core_dump_in_windows_seh(filename, (EXCEPTION_POINTERS*)pep);
}

// Default builds don't set the correct compiler flags
// but we don't want a warning.
#pragma warning (disable: 4535)


void vul_debug_core_dump(const char * filename)
{
  _se_translator_function current =	_set_se_translator(0);

  __try
  {
    RaiseException(0xe0000000,0,0,0);
  }
  __except(vul_debug_core_dump_in_windows_seh(filename, GetExceptionInformation()),1)
  {}
  _set_se_translator(current);
}

//: Windows structured exception code.
unsigned vul_debug_windows_structured_exception::code() const
{
  return static_cast<EXCEPTION_POINTERS*>(ex_ptr_)->ExceptionRecord->ExceptionCode;
}
//: Related execution address.
void *vul_debug_windows_structured_exception::address() const
{
  return static_cast<EXCEPTION_POINTERS*>(ex_ptr_)->ExceptionRecord->ExceptionAddress;
}	
const char *vul_debug_windows_structured_exception::what() const throw()
{
  static char buf[100];
  vcl_sprintf(buf, "Caught Windows Structured Exception. Code %lx. Address %lx", code(), address());
  return buf;
}

static const char* se_coredump_filename = 0;

void vul_debug_set_coredump_and_throw_on_windows_se_handler(
  unsigned code, EXCEPTION_POINTERS * ex_ptr)
{
  vul_debug_core_dump_in_windows_seh(se_coredump_filename, ex_ptr);
#ifdef VCL_HAS_EXCEPTIONS
  throw vul_debug_windows_structured_exception(ex_ptr);
#else
  vcl_cerr << vul_debug_windows_structured_exception(ex_ptr).what();
  vcl_abort();
#endif
}


//: Setup the system to core dump and throw a C++ exception on detection of a Structured Exception
// \throws vul_debug_windows_structured_exception. 
void vul_debug_set_coredump_and_throw_on_windows_se(const char * filename)
{
  se_coredump_filename = filename;
	_set_se_translator(vul_debug_set_coredump_and_throw_on_windows_se_handler);
}


# else //VXL_HAS_DBGHELP_H

void vul_debug_core_dump_in_windows_seh(const char *, void*)
{
  vcl_cerr << "WARNING: vul_debug_core_dump_in_windows_seh: Unable to core dump\n";
}

void vul_debug_core_dump(const char *)
{
  vcl_cerr << "WARNING: vul_debug_core_dump: Unable to core dump\n";
}

//: Windows structured exception code.
unsigned vul_debug_windows_structured_exception::code() const
{
  return 0;
}
//: Related execution address.
void *vul_debug_windows_structured_exception::address() const
{
  return 0;
}	
const char *vul_debug_windows_structured_exception::what() const throw()
{
  return "Caught Windows Exception on machine with old or no version of DbgHelp.";
}




//: Setup the system to core dump and throw a C++ exception on detection of a Structured Exception
// \throws vul_debug_windows_structured_exception. 
void vul_debug_set_coredump_and_throw_on_windows_se(const char * )
{
  vcl_cerr << "WARNING: No DbgHelp.h on this platform - can't set SE Handler.\n";
}

# endif // VXL_HAS_DBGHELP_H


#else // _WIN32 

#include <vcl_string.h>
#ifdef VXL_UNISTD_HAS_GETPID
# include <unistd.h>
#endif
#include <vul/vul_sprintf.h>

void vul_debug_core_dump(const char * filename)
{
#ifdef VXL_UNISTD_HAS_GETPID
  vcl_string syscall = "gcore -o ";
  syscall += filename;
  syscall += vul_sprintf(" %d", getpid());
  if (system(syscall.c_str())==0) return;
  syscall = "gcore -s -c ";
  syscall += filename;
  syscall += vul_sprintf(" %d", getpid());
  if (system(syscall.c_str())==0) return;
#endif
  vcl_cerr << "WARNING: vul_debug_core_dump: Unable to core dump\n";
}
// For a more reliable way of dunping core try forking and sending a SIGSTOP to the child.
// see http://kasperd.net/~kasperd/comp.os.linux.development.faq

//: Setup the system to core dump and throw a C++ exception on detection of a Structured Exception
// \throws vul_debug_windows_structured_exception. 
void vul_debug_set_coredump_and_throw_on_windows_se(const char * filename)
{
// Do nothing on non-windows box.
}

#endif // _WIN32


static const char* out_out_memory_coredump_filename = 0;

void 
#ifdef _WIN32
  __cdecl
#endif
  vul_debug_set_coredump_and_throw_on_out_of_memory_handler()
{
  vul_debug_core_dump(out_out_memory_coredump_filename);
#ifdef VCL_HAS_EXCEPTIONS
  throw vcl_bad_alloc();
#else
  vcl_cerr << "Out of Memory.\n";
  vcl_abort();
#endif
}

//: Setup the system to core dump and throw a C++ exception on detection of out of memory.
// The system will throw vcl_bad_alloc. 
void vul_debug_set_coredump_and_throw_on_out_of_memory(const char * filename)
{
  out_out_memory_coredump_filename = filename;
  vcl_set_new_handler(vul_debug_set_coredump_and_throw_on_out_of_memory_handler);
}
