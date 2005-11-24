//:
// \file
// \brief Exceptions thrown by mbl, and a mechanism for turning them off.
// \author Ian Scott.

#include <vcl_sstream.h>
#include <mbl/mbl_exception.h>


#if 0 // should be #ifdef VCL_VC, but it soesn;twork yet - I can;t get it to link
#pragma comment(lib, "user32")
#pragma comment (lib, "dbghelp")
#include <vxl_config.h>
#define AFX_STACK_DUMP_TARGET_CLIPBOARD 0x0002
#define CF_OEMTEXT          7
#define AFXAPI __stdcall
#define WINAPI __stdcall
typedef vxl_uint_32 DWORD;
typedef void * HANDLE;
typedef unsigned int UINT;
#if (!defined(_USER32_) && (defined(_M_IX86) || defined(_M_IA64) || defined(_M_AMD64)))
#define WINUSERAPI __declspec(dllimport)
#else
#define WINUSERAPI
#endif
#if (!defined(_KERNEL32_) && (defined(_M_IX86) || defined(_M_IA64) || defined(_M_AMD64)))
#define WINBASEAPI __declspec(dllimport)
#else
#define WINBASEAPI
#endif

void AFXAPI AfxDumpStack(DWORD);
WINUSERAPI bool WINAPI OpenClipboard(HANDLE hWndNewOwner);
WINUSERAPI bool WINAPI CloseClipboard(void);
WINUSERAPI HANDLE WINAPI GetClipboardData(UINT uFormat);
WINBASEAPI void * WINAPI GlobalLock(HANDLE hMem);
static vcl_string LotsOfInfo()
{
  vcl_string text;
  text = "Stack: ";
  ::AfxDumpStack(AFX_STACK_DUMP_TARGET_CLIPBOARD);
  if ( !::OpenClipboard(0) )
    text += "Failed to open clipboard to retrieve stack trace.\n";
  HANDLE hglb = ::GetClipboardData(CF_OEMTEXT); 
  if (hglb == NULL) 
    text += "Failed to retrieve stack trace from clipboard.\n";
  const char* str = static_cast<const char *>(::GlobalLock(hglb)); 
  if (str == NULL) 
    text += "Failed to convert stack trace from clipboard.\n";
  else
    text += str;
  ::CloseClipboard();
  return text;
}
#else
static vcl_string LotsOfInfo()
{
  return "";

}
#endif 

#if !VCL_HAS_EXCEPTIONS

mbl_exception_abort::mbl_exception_abort(const vcl_string& comment):
  msg_(comment + LotsOfInfo()) {}

#else

mbl_exception_abort::mbl_exception_abort(const vcl_string& comment):
  vcl_logic_error(comment + LotsOfInfo()) {}

#endif

