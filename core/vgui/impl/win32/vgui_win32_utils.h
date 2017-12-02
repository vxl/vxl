// This is core/vgui/impl/win32/vgui_win32_utils.h

#ifndef vgui_win32_utils_h
#define vgui_win32_utils_h

#include <windows.h>
#include <vgui/vgui_menu.h>

// The definitions of MENUEX_TEMPLATE_HEADER and MENUEX_TEMPLATE_ITEM
// are not present in any standard header file. They are copied here
// from MSDN documentation.
// We use MENUEX_TEMPLATE_HEADER and MENUEX_TEMPLATE_ITEM instead of
// MENUITEMTEMPLATEHEADER and MENUITEMTEMPLATE because a separator is
// not defined in the latter.
typedef struct
{
  WORD  wVersion; // typedef unsigned short WORD;
  WORD  wOffset;
  DWORD dwHelpId; // typedef unsigned long DWORD;
} MENUEX_TEMPLATE_HEADER;

typedef struct
{
  DWORD dwType;
  DWORD dwState;
  DWORD menuId;
  WORD  bResInfo;
  WCHAR szText;   // variable length;
  DWORD dwHelpId; // only included for submenu
} MENUEX_TEMPLATE_ITEM;

#define MENU_ID_START      0x8400
#define POPUPMENU_ID_START 0x8600

class vgui_win32_utils
{
 public:
  // (Create if necessary and) return singleton instance of this class.
  static vgui_win32_utils *instance();

  // Convert a vgui_menu to a Win32 menu
  HMENU vgui_menu_to_win32(vgui_menu const &vguimenu,
                           std::vector<vgui_command_sptr> &callbacks, HACCEL *hAccel,
                           bool isPopup = false);

  // Convert a vgui_menu to an extended Win32 menu
  HMENU vgui_menu_to_win32ex(vgui_menu const &vguimenu,
                             std::vector<vgui_command_sptr> &callbacks, HACCEL *hAccel,
                             bool isPopup = false);

 protected:
  vgui_win32_utils() {}
  ~vgui_win32_utils() {}

 private:
  int addMenuItems(vgui_menu const &vguimenu, int offset, bool is_popup);
  int addMenuItemsEx(vgui_menu const &vguimenu, int offset, bool is_popup);
  void addAccelerator(std::string&, vgui_menu_item const&, int);
  std::string vgui_key_to_string(vgui_key);
  UINT vgui_key_to_virt_key(vgui_key);

  // Show explainary message for GetLastError().
  void ShowErrorMessage(DWORD dwErrorNo);

  unsigned char *pMenu; // menu template buffer
  int menu_capacity; // menu template length
  std::vector<vgui_command_sptr> callbacks; // commands called by menu items
  int item_count; // count of menu items

  ACCEL *pAccel; // pointer to accelerator table, varying length;
  int accel_capacity;
  int accel_count; // count of accelerators
};

#endif // vgui_win32_utils_h
