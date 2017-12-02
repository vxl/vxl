// This is core/vgui/impl/win32/vgui_win32_cmdtarget.cxx

#include "vgui_win32_cmdtarget.h"

BOOL vgui_win32_cmdtarget::OnCmdMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef DEBUG
  std::cout << "vgui_win32_cmdtarget::OnCmdMsg()" << std::endl;
#endif

  // Look through message map to see if it applies to us
  const AFX_MSGMAP* pMessageMap;
  const AFX_MSGMAP_ENTRY* lpEntry;
  for ( pMessageMap = GetMessageMap(); pMessageMap != NULL;
        pMessageMap = pMessageMap->pBaseMessageMap ) {
    lpEntry = pMessageMap->lpEntries;
    while ( lpEntry->nSig != AfxSig_end ) {
      if ( lpEntry->nMessage == message ) {
        (this->*lpEntry->pfn)();
        return TRUE;
      }
      lpEntry++;
    }
  }

  return FALSE;   // not handled
}

const AFX_MSGMAP* vgui_win32_cmdtarget::GetMessageMap() const
{
    return &vgui_win32_cmdtarget::messageMap;
}

const AFX_MSGMAP vgui_win32_cmdtarget::messageMap =
{
    NULL,
    &vgui_win32_cmdtarget::messageEntries_[0]
};

const AFX_MSGMAP_ENTRY vgui_win32_cmdtarget::messageEntries_[] =
{
    { 0, 0, 0, 0, AfxSig_end, 0 }  // no entries
};
