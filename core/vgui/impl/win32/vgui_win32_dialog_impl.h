// This is core/vgui/impl/win32/vgui_win32_dialog_impl.h
#ifndef vgui_win32_dialog_impl_h_
#define vgui_win32_dialog_impl_h_
//:
// \file
// \brief The Win32 Application Programming Interfaces (API) implementation of vgui_dialog_impl.
// \author Lianqing Yu
// \date July 30, 2009              Initial version
//
// \todo TODO issues:
// 1. dir_bsr, line_br are not implemented.
// 2. Dialog font is not set.
// 3. get_current_tab
// 4. inline_file and inline_color

#include <vgui/internals/vgui_dialog_impl.h>
#include <vgui/impl/win32/vgui_win32_adaptor.h>
#include <vgui/vgui_command.h>

#include <windows.h>

#define IDC_STATIC      -1
#define DLG_ID_START    0x8000

typedef struct tag_inline_tab_data
{
  unsigned short     childId;
  HWND               hWnd;
  vgui_win32_adaptor *adaptor;
} inline_tab_data;

// Used as a search table to launch command.
typedef struct tag_callback_control_data
{
  unsigned short    child_id;
  vgui_command_sptr cmnd;
} callback_control_data;


class vgui_win32_dialog_impl : public vgui_dialog_impl
{
 public:
  vgui_win32_dialog_impl(const char*, HWND hWndParent = NULL);
  ~vgui_win32_dialog_impl();

  // Overloaded virtual functions that return all kinds of widget that Win32
  // supports.
  void* pushbutton_field_widget(const char*, const void*);
  void* choice_field_widget(const char*, const std::vector<std::string>&, int&);
  void* inline_tableau_widget(const vgui_tableau_sptr tab, unsigned width, unsigned height);
  void modal(bool m) { is_modal = m; }
  bool ask();
  virtual void run();

  // Called within message processing loop for controls registered with
  // callback function.
  void dialog_dispatcher(int item_id);

  // Virtual message handling functions
  virtual LRESULT DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
  virtual void OnOK();
  virtual void OnCancel();
  virtual BOOL OnBrowse(HWND hDlg, WORD wCtrlId);
  virtual BOOL OnColor(HWND hDlg, WORD wCtrlId, LPTSTR lpColor);

  // Return the number of inline tableaus in the dialog.
  int get_inline_tableau_size() { return inline_tableaus.size(); }
  // Return the inline tableau with focus.
  // TODO: fix me.
  vgui_win32_adaptor* get_current_tab() { return inline_tableaus.front().adaptor; }

 protected:
  COLORREF ColorStringToRGB(LPTSTR lpColor);

  bool ok_clicked;
  bool is_modal; // dialog is modal or modaless

  HWND hWndParent; // application (parent) window of this dialog box,
                   // used to create dialog box
  HWND hWnd; // window handle of this dialog box.

 private:
  // Find out the size of the dialog box.
  void FindDialogSize(int &width, int &height,
                      int &max_length, int &fbsr_count,
                      int cxChar, int cyChar, int width_sep, int height_sep,
                      int button_length, int edit_length, int browser_length);

  // Draw an image on an owner-draw button
  void DrawImageOnButton(HDC hDC, RECT* lprcItem, HBITMAP hBitmap, unsigned w, unsigned h, BOOL isDisabled);

  // Determine if the control with identifier "ctrl_id"
  // is a file browser button.
  bool IsFileBrowserButton(unsigned short ctrl_id);
  // Determine if the control with identifier "ctrl_id"
  // is a color chooser button.
  bool IsColorChooserButton(unsigned short ctrl_id);
  // Determine if the control with identifier "ctrl_id"
  // has a callback function with it.
  bool IsCallbackControl(unsigned short ctrl_id);

  // Find the inline adaptor with identifier "ctrl_id".
  vgui_win32_adaptor* find_adaptor(unsigned short ctrl_id);

  // Save information of all inline tableaus added in the dialog box.
  std::vector<inline_tab_data> inline_tableaus;

  // Save identifiers of file-browser/color-chooser buttons so that
  // OnBrowse/OnColor are called when these buttons are clicked.
  std::vector<unsigned short> fb_ids, cc_ids;

  // Save identifiers of all controls that connect to a callback function.
  std::vector<callback_control_data> callback_controls;
};


#endif // vgui_win32_dialog_impl_h_
