// This is core/vgui/impl/win32/vgui_win32_cmdtarget.h

// To simplify the message handling code in globalWndProc(in vgui_win32.cxx),
// which is usually a long switch-case statement, We structure that code
// using message-mapping technique employed by Microsoft Foundation Class.
// To do this, a number of macros (DECLARE_MESSAGE_MAP, BEGIN_MESSAGE_MAP,
// END_MESSAGE_MAP, ON_COMMAND), structures (WIN32_MSGMAP,
// WIN32_MSGMAP_ENTRY), and the class vgui_win32_cmdtarget (whose MFC
// counterpart is CCmdTarget) are defined here. In particular
// vgui_win32_cmdtarget is the final stop of message processing.

// TODO: vgui_win32_cmdtarget is not fully developed yet and for now not
// used in the Win32 SDK implementation of VGUI.

#ifndef vgui_win32_cmdtarget_h_
#define vgui_win32_cmdtarget_h_

#include <windows.h>

struct AFX_MSGMAP_ENTRY;

struct AFX_MSGMAP
{
  const AFX_MSGMAP* pBaseMessageMap;
  const AFX_MSGMAP_ENTRY* lpEntries;
};

#define DECLARE_MESSAGE_MAP() \
private: \
  static const AFX_MSGMAP_ENTRY messageEntries_[]; \
protected: \
  static const AFX_MSGMAP messageMap; \
  virtual const AFX_MSGMAP* GetMessageMap() const;

#define BEGIN_MESSAGE_MAP(theClass, baseClass) \
  const AFX_MSGMAP* theClass::GetMessageMap() const \
  { return &theClass::messageMap; } \
  const AFX_MSGMAP theClass::messageMap = \
  { &(baseClass::messageMap), \
   (AFX_MSGMAP_ENTRY*) &(theClass::messageEntries_) }; \
  const AFX_MSGMAP_ENTRY theClass::messageEntries_[] = \
  {

#define END_MESSAGE_MAP() \
    { 0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0 } \
  };

enum AfxSig
{
   AfxSig_end = 0,     // [marks end of message map]
   AfxSig_vv,
};

#define ON_COMMAND(id, memberFxn) \
    { WM_COMMAND, 0, (WORD)id, (WORD)id, AfxSig_vv, (AFX_PMSG)memberFxn },
#define ON_WM_CREATE() \
    { WM_CREATE, 0, 0, 0, AfxSig_is, \
     (AFX_PMSG)(AFX_PMSGW)(int AFX_MSG_CALL CWnd::*)(LPCREATESTRUCT)OnCreate},

class vgui_win32_cmdtarget
{
public:
  vgui_win32_cmdtarget() {}
  ~vgui_win32_cmdtarget() {}

  // Message handling function
  virtual BOOL OnCmdMsg(UINT message, WPARAM wParam, LPARAM lParam);

  DECLARE_MESSAGE_MAP()
};

typedef void (vgui_win32_cmdtarget::*AFX_PMSG)(void);

struct AFX_MSGMAP_ENTRY  // MFC 4.0 format
{
  UINT nMessage; // windows message
  UINT nCode;    // control code or WM_NOTIFY code
  UINT nID;      // control ID (or 0 for windows messages)
  UINT nLastID;  // used for entries specifying a range of control id's
  UINT nSig;     // signature type (action) or pointer to message #
  AFX_PMSG pfn;  // routine to call (or special value)
};

#endif // vgui_win32_cmdtarget_h_
