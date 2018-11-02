// This is core/vul/vul_debug.cxx
#include <iostream>
#include <new>
#include <cstdlib>
#include <cstdio>
#include <string>
#include "vul_debug.h"
//: \file
//  \brief Get debug related information like core dumps, and stack traces
//  \author Ian Scott

#include <vxl_config.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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
#pragma comment (lib, "dbghelp")

static bool vul_debug_core_dump_in_windows_seh(const char * filename,
                                               EXCEPTION_POINTERS* pep)
{
  static char buffer[2048];
  static int count = 0;
  std::snprintf(buffer, sizeof(buffer), filename, count++);
  buffer[sizeof(buffer)-1]=0; // Just in case it is too long

  HANDLE hFile = CreateFile( buffer, GENERIC_READ | GENERIC_WRITE,
                             0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

  if ( ( hFile == NULL ) || ( hFile == INVALID_HANDLE_VALUE ) )
  {
    std::cerr << "WARNING: vul_debug_core_dump: Unable to create core dump file: " << filename << std::endl;
    return false;
  }

  MINIDUMP_EXCEPTION_INFORMATION mdei;
  mdei.ThreadId           = GetCurrentThreadId();
  mdei.ExceptionPointers  = pep;
  mdei.ClientPointers     = FALSE;

  if (! MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(),
    hFile, MiniDumpWithFullMemory, (pep != 0) ? &mdei : 0, 0, 0 ))
    std::cerr << "WARNING: vul_debug_core_dump: Unable to dump core: " << filename << std::endl;

  CloseHandle( hFile );
  return true;
}

bool vul_debug_core_dump_in_windows_seh(const char * filename,
                                        void* pep)
{
  return vul_debug_core_dump_in_windows_seh(filename, (EXCEPTION_POINTERS*)pep);
}

// Default builds don't set the correct compiler flags
// but we don't want a warning.
#pragma warning (disable: 4535)


bool vul_debug_core_dump(const char * filename)
{
  _se_translator_function current = _set_se_translator(0);

  __try
  {
    RaiseException(0xe0000000,0,0,0);
  }
  __except(vul_debug_core_dump_in_windows_seh(filename, GetExceptionInformation()),1)
  {}
  _set_se_translator(current);
  return true;
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
  std::sprintf(buf, "Caught Windows Structured Exception. Code %lx. Address %lx", code(), address());
  return buf;
}

static const char* se_coredump_filename = 0;

void vul_debug_set_coredump_and_throw_on_windows_se_handler(
  unsigned code, EXCEPTION_POINTERS * ex_ptr)
{
  vul_debug_core_dump_in_windows_seh(se_coredump_filename, ex_ptr);
  throw vul_debug_windows_structured_exception(ex_ptr);
}


//: Setup the system to core dump and throw a C++ exception on detection of a Structured Exception
// \throws vul_debug_windows_structured_exception.
void vul_debug_set_coredump_and_throw_on_windows_se(const char * filename)
{
  se_coredump_filename = filename;
  _set_se_translator(vul_debug_set_coredump_and_throw_on_windows_se_handler);
}


# else //VXL_HAS_DBGHELP_H

bool vul_debug_core_dump_in_windows_seh(const char *, void*)
{
  std::cerr << "WARNING: vul_debug_core_dump_in_windows_seh: Unable to core dump\n";
  return false;
}

bool vul_debug_core_dump(const char *)
{
  std::cerr << "WARNING: vul_debug_core_dump: Unable to core dump\n";
  return false;
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
  std::cerr << "WARNING: No DbgHelp.h on this platform - can't set SE Handler.\n";
}

# endif // VXL_HAS_DBGHELP_H


#else // _WIN32

#ifdef VXL_UNISTD_HAS_GETPID
# include <unistd.h>
#endif
#include <vul/vul_sprintf.h>

bool vul_debug_core_dump(const char * filename)
{
  static int count = 0;
#ifdef VXL_UNISTD_HAS_GETPID
  std::string syscall = "gcore -o ";
  syscall += vul_sprintf(filename, count++);
  syscall += vul_sprintf(" %d", getpid());
  if (system(syscall.c_str())==0) return true;
  syscall = "gcore -s -c ";
  syscall += filename;
  syscall += vul_sprintf(" %d", getpid());
  if (system(syscall.c_str())==0) return true;
#endif
  std::cerr << "WARNING: vul_debug_core_dump: Unable to core dump\n";
  return false;
}
// For a more reliable way of dumping core try forking and sending a SIGSTOP to the child.
// see http://kasperd.net/~kasperd/comp.os.linux.development.faq

//: Setup the system to core dump and throw a C++ exception on detection of a Structured Exception
// \throws vul_debug_windows_structured_exception.
void vul_debug_set_coredump_and_throw_on_windows_se(const char * /*filename*/)
{
// Do nothing on non-windows box.
}

#endif // _WIN32


static const char* out_of_memory_coredump_filename = nullptr;

void
#ifdef _WIN32
  __cdecl
#endif
  vul_debug_set_coredump_and_throw_on_out_of_memory_handler()
{
  vul_debug_core_dump(out_of_memory_coredump_filename);
  throw std::bad_alloc();
}

//: Setup the system to core dump and throw a C++ exception on detection of out of memory.
// The system will throw std::bad_alloc.
void vul_debug_set_coredump_and_throw_on_out_of_memory(const char * filename)
{
  out_of_memory_coredump_filename = filename;
  std::set_new_handler(vul_debug_set_coredump_and_throw_on_out_of_memory_handler);
}
