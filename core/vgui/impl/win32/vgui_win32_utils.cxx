// This is core/vgui/impl/win32/vgui_win32_utils.cxx

#include <iostream>
#include <cstddef>
#include "vgui_win32_utils.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// (Create if necessary and) return singleton instance of this class.
vgui_win32_utils* vgui_win32_utils::instance()
{
  static vgui_win32_utils instance_;
  return &instance_;
}


// Convert a vgui_menu to equivalent MENUTEMPLATE structure used by
// Win32 function LoadMenuIndirect.
HMENU vgui_win32_utils::vgui_menu_to_win32(vgui_menu const &vguimenu,
                                           std::vector<vgui_command_sptr> &callbacks_, HACCEL *hAccel, bool isPopup)
{
  HMENU hMenu;
  MENUITEMTEMPLATEHEADER *pMenuHeader;
  MENUITEMTEMPLATE *pMenuItem;

  // Note that the MENUITEMTEMPLATE structures are variable in length,
  // Therefore we allocate capacity*sizeof(char) initially.
  menu_capacity = 1024;    // in unit of unsigned char
  item_count = 0;
  callbacks.clear();

  pAccel = 0;
  accel_capacity = 16;
  accel_count = 0;

  pMenu = (unsigned char *)malloc(sizeof(MENUITEMTEMPLATEHEADER) + menu_capacity);
  if ( pMenu == NULL ) {
    std::cerr << "Memory allocation error\n";
    return NULL;
  }

  pAccel = (ACCEL *)malloc(sizeof(ACCEL)*accel_capacity);
  if ( pAccel == NULL )
    std::cerr << "Memory allocation error\n";

  // Fill up the MENUITEMTEMPLATEHEADER structure.
  pMenuHeader = (MENUITEMTEMPLATEHEADER *)pMenu;
  pMenuHeader->versionNumber = 0; // This member must be zero.
  pMenuHeader->offset = 0;  // The menu item list follows immediately after the header.
  int offset = sizeof(MENUITEMTEMPLATEHEADER);

  if ( isPopup ) {
    // Add a dumb header so that TrackPopMenu shows the popup menu properly.
    pMenuItem = (MENUITEMTEMPLATE*)(pMenu + offset);
    pMenuItem->mtOption = MF_END | MF_POPUP;
    offset += sizeof(pMenuItem->mtOption);
    pMenuItem->mtID = 0;
    offset += sizeof(WCHAR);
  }

  addMenuItems(vguimenu, offset, isPopup);

  hMenu = LoadMenuIndirect(pMenu);
  if ( !hMenu )
     ShowErrorMessage(GetLastError());

  if ( !isPopup ) // create an accelerator table for non-popup menu.
    *hAccel = CreateAcceleratorTable(pAccel, accel_count);

  free(pMenu);
  free(pAccel);

  callbacks_ = callbacks;
  return hMenu;
}


int vgui_win32_utils::addMenuItems(vgui_menu const &vguimenu, int offset_in, bool is_popup)
{
  static unsigned int max_menuitem_size = 256;

  MENUITEMTEMPLATE *pMenuItem;
  WCHAR *pMenuItemText;
  std::string menuItemText;
  int stride, offset; // in unit of unsigned char

  // Loop over all menu items and convert them into MENUITEMTEMPLATE
  // Note that there are four types of menu item in vgui_menu:
  // command, submenu, toggle button, and separator.
  offset = offset_in;
  for ( unsigned int i=0; i < vguimenu.size(); ++i ) {
    pMenuItem = (MENUITEMTEMPLATE*)(pMenu + offset);

    // MENUITEMTEMPLATE does not have separator.
    if (vguimenu[i].is_separator() ||
        vguimenu[i].is_toggle_button())
      continue;

    stride = 0;
    pMenuItem->mtOption = 0;
    stride += sizeof(pMenuItem->mtOption);
    if ( i == vguimenu.size()-1 )
      pMenuItem->mtOption |= MF_END; // indiate this is the last menu item

    if (vguimenu[i].is_command()) {
      int menuItemID = is_popup ? POPUPMENU_ID_START+item_count++ : MENU_ID_START+item_count++;
      pMenuItem->mtID = menuItemID;
      stride += sizeof(pMenuItem->mtID);

      menuItemText = vguimenu[i].name;
      addAccelerator(menuItemText, vguimenu[i], menuItemID);

      // Copy the menu item text.
      std::size_t j;
      pMenuItemText = pMenuItem->mtString;
      for ( j = 0; j < menuItemText.size(); j++ )
        *(pMenuItemText+j) = (WCHAR)menuItemText.c_str()[j];
      *(pMenuItemText+j) = 0;
      stride += sizeof(WCHAR) * (wcslen(pMenuItemText)+1);

      // Add the associated callback function pointer to the callback list.
      callbacks.push_back(vguimenu[i].cmnd);
    }
    else if (vguimenu[i].is_submenu()) {
      pMenuItem->mtOption |= MF_POPUP;

      menuItemText = vguimenu[i].name;
      std::size_t j;
      // Note that the MENUITEMTEMPLATE structure for an item that opens a
      // drop-down menu or submenu does not contain the mtID member.
      pMenuItemText = (WCHAR *)&pMenuItem->mtID;
      for ( j = 0; j < menuItemText.size(); j++ )
        *(pMenuItemText+j) = menuItemText.c_str()[j];
      *(pMenuItemText+j) = 0;
      stride += sizeof(WCHAR) * (wcslen(pMenuItemText)+1);

      // Call itself recursively for submenu
      stride += addMenuItems(*vguimenu[i].menu, offset+stride, is_popup);
    }

    offset += stride;
    // Check for illegal buffer access. We'are too late at this point.
    // Increase maximum size of a menu item "max_menuitem_size" (default 256)
    // to avoid this error.
    assert(offset < menu_capacity);

    // Deal with overflow.
    if ( offset > menu_capacity-(int)max_menuitem_size ) {
      menu_capacity <<= 1; // double the capacity.
      pMenu = (unsigned char *)realloc(pMenu, sizeof(MENUITEMTEMPLATEHEADER)+menu_capacity);
      if ( pMenu == NULL ) {
        std::cerr << "Memory allocation error\n";
        return 0;
      }
    }
  }

  return offset - offset_in;
}

// Convert a vgui_menu to equivalent extended MENUTEMPLATE structure used by
// Win32 function LoadMenuIndirect.
HMENU vgui_win32_utils::vgui_menu_to_win32ex(vgui_menu const &vguimenu,
                                             std::vector<vgui_command_sptr> &callbacks_, HACCEL *hAccel, bool isPopup)
{
  HMENU hMenu;
  MENUEX_TEMPLATE_HEADER *pMenuHeader;
  MENUEX_TEMPLATE_ITEM *pMenuItem;

  // Note that the MENUEX_TEMPLATE_ITEM structures are variable in length,
  // but are aligned on DWORD boundaries. Therefore we allocate
  // capacity*sizeof(WORD) initially.
  menu_capacity = 1024;
  item_count = 0;
  callbacks.clear();

  pAccel = 0;
  accel_capacity = 16;
  accel_count = 0;

  pMenu = (unsigned char *)malloc(sizeof(MENUEX_TEMPLATE_HEADER) + menu_capacity);
  if ( pMenu == NULL ) {
    std::cerr << "Memory allocation error\n";
    return NULL;
  }

  pAccel = (ACCEL *)malloc(sizeof(ACCEL)*accel_capacity);
  if ( pAccel == NULL )
    std::cerr << "Memory allocation error\n";

  // Fill up the MENUEX_TEMPLATE_HEADER structure.
  pMenuHeader = (MENUEX_TEMPLATE_HEADER *)pMenu;
  pMenuHeader->wVersion = 1; // This member must be 1.
  pMenuHeader->wOffset = 4;  // The menu item list follows immediately after the header.
  pMenuHeader->dwHelpId = 0;
  int offset = sizeof(MENUEX_TEMPLATE_HEADER);

  if ( isPopup ) {
    // Add a dumb header so that TrackPopMenu shows the popup menu properly.
    pMenuItem = (MENUEX_TEMPLATE_ITEM*)(pMenu+offset);
    pMenuItem->dwType = MFT_STRING;
    offset += sizeof(pMenuItem->dwType);
    pMenuItem->dwState = MFS_ENABLED;
    offset += sizeof(pMenuItem->dwState);
    pMenuItem->menuId = 0;
    offset += sizeof(pMenuItem->menuId);
    pMenuItem->bResInfo = 0x01 | 0x80;
    offset += sizeof(pMenuItem->bResInfo);
    pMenuItem->szText = 0;
    offset += sizeof(pMenuItem->szText);
    pMenuItem->dwHelpId = 0;
    offset += sizeof(pMenuItem->dwHelpId);
  }

  addMenuItemsEx(vguimenu, offset, isPopup);

  hMenu = LoadMenuIndirect(pMenu);
  if ( !hMenu )
     ShowErrorMessage(GetLastError());

  if ( !isPopup ) // create an accelerator table for non-popup menu.
    *hAccel = CreateAcceleratorTable(pAccel, accel_count);

  free(pMenu);
  free(pAccel);

  callbacks_ = callbacks;
  return hMenu;
}

int vgui_win32_utils::addMenuItemsEx(vgui_menu const &vguimenu, int offset_in, bool is_popup)
{
  static unsigned int max_menuitem_size = 256;

  MENUEX_TEMPLATE_ITEM *pMenuItem;
  WCHAR *pMenuItemText;
  std::string menuItemText;
  int stride, offset;
  bool last_item;

  // Loop over all menu items and convert them into MENUTEMPLATE
  // Note that there are four types of menu item in vgui_menu:
  // command, submenu, toggle button, and separator.
  offset = offset_in;
  for ( unsigned int i=0; i < vguimenu.size(); ++i ) {
    pMenuItem = (MENUEX_TEMPLATE_ITEM*)(pMenu + offset);
    stride = 0;

    // indiate this is the last menu item
    last_item = (i == vguimenu.size()-1) ? true : false;

    pMenuItem->dwType = MFT_STRING;
    stride += sizeof(pMenuItem->dwType);
    pMenuItem->dwState = MFS_ENABLED;
    stride += sizeof(pMenuItem->dwState);
    pMenuItem->menuId = 0;
    stride += sizeof(pMenuItem->menuId);

    if (vguimenu[i].is_separator()) {
      pMenuItem->dwType = MFT_SEPARATOR;
      pMenuItem->bResInfo = last_item ? 0x80 : 0;
      stride += sizeof(pMenuItem->bResInfo);
      pMenuItem->szText = 0;
      stride += sizeof(pMenuItem->szText);
      if ( stride % 4 ) { // aligned on DWORD boundary.
        stride += 4;
        stride &= ~3;
      }
    }
    else if (vguimenu[i].is_command()) {
      int menuItemID = is_popup ? POPUPMENU_ID_START+item_count++ : MENU_ID_START+item_count++;
      pMenuItem->menuId = menuItemID;

      pMenuItem->bResInfo = last_item ? 0x80 : 0;
      stride += sizeof(pMenuItem->bResInfo);

      menuItemText = vguimenu[i].name;
      addAccelerator(menuItemText, vguimenu[i], menuItemID);

      // Copy the menu item text.
      pMenuItemText = (WCHAR *)&pMenuItem->szText;
      std::size_t j;
      for ( j = 0; j < menuItemText.size(); j++ )
        *(pMenuItemText+j) = menuItemText.c_str()[j];
      *(pMenuItemText+j) = 0;

      stride += sizeof(WCHAR) * (wcslen(pMenuItemText)+1);

      if ( stride % 4 ) { // aligned on DWORD boundary.
        stride += 4;
        stride &= ~3;
      }

      // Add the associated callback function pointer to the callback list.
      callbacks.push_back(vguimenu[i].cmnd);
    }
    else if (vguimenu[i].is_submenu()) {
      pMenuItem->bResInfo = last_item ? 0x80 | 0x01 : 0x01;
      stride += sizeof(pMenuItem->bResInfo);

      menuItemText = vguimenu[i].name;
      pMenuItemText = (WCHAR *)&pMenuItem->szText;
      std::size_t j;
      for ( j = 0; j < menuItemText.size(); j++ )
        *(pMenuItemText+j) = menuItemText.c_str()[j];
      *(pMenuItemText+j) = 0;
      stride += sizeof(WCHAR) * (wcslen(pMenuItemText)+1);

      if ( stride % 4 ) { // aligned on DWORD boundary.
        stride += 4;
        stride &= ~3;
      }

      DWORD* dwHelpId = (DWORD *)(pMenu+offset+stride);
      *dwHelpId = 0;
      stride += sizeof(pMenuItem->dwHelpId);

      // Call itself recursively for submenu
      stride += addMenuItemsEx(*vguimenu[i].menu, offset+stride, is_popup);
    }
    else if (vguimenu[i].is_toggle_button()) {
      std::cerr << "vgui_win32_utils: toggle button is not converted\n";
    }

    offset += stride;
    // Check for illegal buffer access. We'are too late at this point.
    // Increase maximum size of a menu item "max_menuitem_size" (default 256)
    // to avoid this error.
    assert(offset < menu_capacity);

    // Deal with buffer overflow.
    if ( offset > menu_capacity-(int)max_menuitem_size ) {
      menu_capacity <<= 1; // double the capacity.
      pMenu = (unsigned char *)realloc(pMenu, sizeof(MENUEX_TEMPLATE_HEADER) + menu_capacity);
      if ( pMenu == NULL ) {
        std::cerr << "Memory allocation error\n";
        return NULL;
      }
    }
  }

  return offset-offset_in;
}

inline void vgui_win32_utils::addAccelerator(std::string &menuItemText,
                                             vgui_menu_item const &vguimenu, int menuItemId)
{
  ACCEL *pa = pAccel+accel_count;
  pa->cmd = menuItemId;
  // Give a virtual key, even if there is no modifier.
  pa->fVirt = FVIRTKEY;

  if ( vguimenu.short_cut.mod != vgui_MODIFIER_NULL ||
       vguimenu.short_cut.key != vgui_KEY_NULL )
    menuItemText += "\t"; // A tab key is required.

  if ( vguimenu.short_cut.mod & vgui_CTRL ) {
    menuItemText += "Ctrl+";
    pa->fVirt |= FCONTROL;
  }
  if ( vguimenu.short_cut.mod & vgui_SHIFT ) {
    menuItemText += "Shift+";
    pa->fVirt |= FSHIFT;
  }
  if ( vguimenu.short_cut.mod & vgui_ALT ) {
    menuItemText += "Alt+";
    pa->fVirt |= FALT;
  }

  vgui_key key = vguimenu.short_cut.key;
  if ( key != vgui_KEY_NULL ) {
    if ( key >= 'A' && key <= 'Z'|| key >= 'a' && key <= 'z') {
       if ( key >= 'a' && key <= 'z')
          key = vgui_key(key + 'A' - 'a');
       menuItemText += key;
       pa->key = key;
    }
    else {
      menuItemText += vgui_key_to_string(key);
      pa->key = vgui_key_to_virt_key(key);
    }
    // Deal with the case of buffer overflow.
    if (++accel_count >= accel_capacity) {
      accel_capacity <<= 1; // double the capacity.
      pAccel = (ACCEL *)realloc(pAccel, sizeof(ACCEL)*accel_capacity);
      if ( pAccel == NULL )
        std::cerr << "Memory allocation error\n";
    }
  }
}


inline std::string vgui_win32_utils::vgui_key_to_string(vgui_key key)
{
  std::string str;

  switch ( key ) {
    // Function keys
    case vgui_F1:
      str = "F1"; break;
    case vgui_F2:
      str = "F2"; break;
    case vgui_F3:
      str = "F3"; break;
    case vgui_F4:
      str = "F4"; break;
    case vgui_F5:
      str = "F5"; break;
    case vgui_F6:
      str = "F6"; break;
    case vgui_F7:
      str = "F7"; break;
    case vgui_F8:
      str = "F8"; break;
    case vgui_F9:
      str = "F9"; break;
    case vgui_F10:
      str = "F10"; break;
    case vgui_F11:
      str = "F11"; break;
    case vgui_F12:
      str = "F12"; break;
    case vgui_CURSOR_LEFT:
      str = "Left"; break;
    case vgui_CURSOR_UP:
      str = "Up"; break;
    case vgui_CURSOR_RIGHT:
      str = "Right"; break;
    case vgui_CURSOR_DOWN:
      str = "Down"; break;
    case vgui_PAGE_UP:
      str = "PageUp"; break;
    case vgui_PAGE_DOWN:
      str = "PageDown"; break;
    case vgui_HOME:
      str = "Home"; break;
    case vgui_END:
      str = "End"; break;
    case vgui_DELETE:
      str = "Del"; break;
    case vgui_INSERT:
      str = "Ins"; break;
    default:
      str = ""; break;
  }

  return str;
}

UINT vgui_win32_utils::vgui_key_to_virt_key(vgui_key key)
{
  UINT virt_key;

  switch ( key ) {
    // Function keys
    case vgui_F1:
    case vgui_F2:
    case vgui_F3:
    case vgui_F4:
    case vgui_F5:
    case vgui_F6:
    case vgui_F7:
    case vgui_F8:
    case vgui_F9:
    case vgui_F10:
    case vgui_F11:
    case vgui_F12:
      virt_key = VK_F1 + key - vgui_F1;
      break;
    case vgui_CURSOR_LEFT:
      virt_key = VK_LEFT; break;
    case vgui_CURSOR_UP:
      virt_key = VK_UP; break;
    case vgui_CURSOR_RIGHT:
      virt_key = VK_RIGHT; break;
    case vgui_CURSOR_DOWN:
      virt_key = VK_DOWN; break;
    case vgui_PAGE_UP:
      virt_key = VK_PRIOR; break;
    case vgui_PAGE_DOWN:
      virt_key = VK_NEXT; break;
    case vgui_HOME:
      virt_key = VK_HOME; break;
    case vgui_END:
      virt_key = VK_END; break;
    case vgui_DELETE:
      virt_key = VK_DELETE; break;
    case vgui_INSERT:
      virt_key = VK_INSERT; break;
    default: // undefined
      virt_key = 0x07; break;
  }

  return virt_key;
}

void vgui_win32_utils::ShowErrorMessage(DWORD dwErrorNo)
{
   LPSTR lpBuffer;
   FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER  |
                 FORMAT_MESSAGE_IGNORE_INSERTS  |
                 FORMAT_MESSAGE_FROM_SYSTEM,
                 NULL,
                 dwErrorNo,
                 LANG_NEUTRAL,
                 (LPTSTR) & lpBuffer,
                 0 ,
                 NULL);
  std::cerr << lpBuffer;
}
