// This is core/vul/vul_debug.cxx

//: \file
//  \brief Get debug related information like core dumps, and stack traces
//  \author Ian Scott

#include "vul_debug.h"
#include <vcl_iostream.h>

#ifdef _WIN32

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



  MINIDUMP_TYPE mdt = (MINIDUMP_TYPE)(MiniDumpWithFullMemory | 
    MiniDumpWithHandleData | MiniDumpWithProcessThreadData | MiniDumpWithUnloadedModules ); 

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

void vul_debug_core_dump(const char * filename)
{

  __try
  {
    RaiseException(0xe0000000,0,0,0);
  }
  __except(vul_debug_core_dump_in_windows_seh(filename, GetExceptionInformation()),1)
  {}
}
#else

#include <vcl_string.h>
#include <vxl_config.h>
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
  vcl_cerr << "WARNING: vul_debug_core_dump: Unable to create core dump file: " << filename << vcl_endl;

}
// For a more reliable way of dunping core try forking and sending a SIGSTOP to the child.
// see http://kasperd.net/~kasperd/comp.os.linux.development.faq
#endif
