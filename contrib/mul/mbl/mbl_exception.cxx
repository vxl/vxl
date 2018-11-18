// This is mul/mbl/mbl_exception.cxx
#include <cerrno>
#include <iostream>
#include <cstring>
#include "mbl_exception.h"
//:
// \file
// \brief Exceptions thrown by mbl, and a mechanism for turning them off.
// \author Ian Scott.

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#if 0 // should be #ifdef _MSC_VER, but it doesn't work yet - I can't get it to link
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
static std::string LotsOfInfo()
{
  std::string text;
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

#else // 0, should be _MSC_VER

static std::string LotsOfInfo()
{
  return "";
}

#endif // 0, should be _MSC_VER

mbl_exception_abort::mbl_exception_abort(const std::string& comment):
  std::logic_error(comment + LotsOfInfo()) {}

mbl_exception_os_error::mbl_exception_os_error(int errnum, const std::string &file_name,
                                               const std::string &comment/*=""*/):
  std::runtime_error(std::string("\"") + file_name + "\" " + std::strerror(errnum) + "\n" + comment),
    err_no(errnum), error_message(std::strerror(errnum)), filename(file_name),
    additional_comment(comment) {}

void mbl_exception_throw_os_error(const std::string& filename,
                                  const std::string& additional_comment /*=""*/)
{
  switch (errno)
  {
  case ENOENT:
    mbl_exception_warning(mbl_exception_os_no_such_file_or_directory(ENOENT, filename, additional_comment));
    break;
  case EACCES:
    mbl_exception_warning(mbl_exception_os_permission_denied(EACCES, filename, additional_comment));
    break;
  case EEXIST:
    mbl_exception_warning(mbl_exception_os_file_exists(EEXIST, filename, additional_comment));
    break;
  case ENOTDIR:
    mbl_exception_warning(mbl_exception_os_not_a_directory(ENOTDIR, filename, additional_comment));
    break;
  case EISDIR:
    mbl_exception_warning(mbl_exception_os_is_a_directory(EISDIR, filename, additional_comment));
    break;
  case ENOSPC:
    mbl_exception_warning(mbl_exception_os_no_space_left_on_device(ENOSPC, filename, additional_comment));
    break;
  case EINVAL:
    mbl_exception_warning(mbl_exception_os_invalid_value(EINVAL, filename, additional_comment));
    break;
  default:
    mbl_exception_warning(mbl_exception_os_error(errno, filename, additional_comment));
    break;
  }
}
