// This is core/vgui/impl/win32/vgui_win32_dialog_impl.cxx

#include "vgui_win32_dialog_impl.h"

#include <vcl_algorithm.h> // for vcl_find
#include <vcl_cstdio.h> // for vcl_sprintf
#include <vcl_cstddef.h> // for vcl_size_t
#include <vcl_cstring.h> // for vcl_strlen()
#include <vcl_iostream.h>
#include <vgui/internals/vgui_simple_field.h>
#include <vgui/internals/vgui_file_field.h>
#include <vgui/impl/win32/vgui_win32_adaptor.h>

extern LRESULT CALLBACK globalDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

vgui_win32_dialog_impl::vgui_win32_dialog_impl(const char* name, HWND hWnd)
  : vgui_dialog_impl(name), ok_clicked(false), is_modal(true),
  hWndParent(hWnd), hWnd(NULL)
{
  inline_tableaus.clear();
  fb_ids.clear();
  cc_ids.clear();
}

vgui_win32_dialog_impl::~vgui_win32_dialog_impl()
{
  for ( vcl_vector<inline_tab_data>::iterator
        it = inline_tableaus.begin(); it != inline_tableaus.end(); ++it )
    delete it->adaptor;
}

// Structure to contain data for a choice field.
struct vgui_win32_dialog_choice
{
  vcl_vector<vcl_string> names;
  int index;
};

// Make a choice widget
void* vgui_win32_dialog_impl::choice_field_widget(const char* /*txt*/,
                                                  const vcl_vector<vcl_string>& labels, int& val)
{
  vgui_win32_dialog_choice *ch = new vgui_win32_dialog_choice;
  ch->names = labels;
  ch->index = val;

  return (void*)ch;
}

// Structure to contain data for a inline tableau.
struct vgui_win32_dialog_inline_tab
{
  vgui_tableau_sptr tab;
  unsigned height;
  unsigned width;
};

// Make a tableau widget.
void* vgui_win32_dialog_impl::inline_tableau_widget(const vgui_tableau_sptr tab,
                                                    unsigned width, unsigned height)
{
  vgui_win32_dialog_inline_tab* tab_data = new vgui_win32_dialog_inline_tab;
  tab_data->tab = tab;
  tab_data->height = height;
  tab_data->width =  width;
  return (void*)tab_data;
}

// Find out the size of the dialog box needed
void vgui_win32_dialog_impl::FindDialogSize(int &width, int &height,
                                            int &max_length, int &fbsr_count,
                                            int cxChar, int cyChar, int width_sep, int height_sep,
                                            int button_length, int edit_length, int browser_length)
{
  // Minimum dialog size
  int cyCaption = GetSystemMetrics(SM_CYCAPTION);

  height = 3*height_sep+cyCaption; // top, bottom margin plus an OK/Cancel button;
  max_length = width_sep;
  fbsr_count = 0;

  // Count the length of OK and Cancel button first.
  if (ok_button_text_.size() > 0)
    max_length += button_length+width_sep;
  if (cancel_button_text_.size() > 0)
    max_length += button_length+width_sep;

  for (vcl_vector<element>::iterator e_iter = elements.begin();
       e_iter != elements.end(); ++e_iter) {
    element l = *e_iter;
    vgui_dialog_field *field = l.field;

    if ( l.type == int_elem || l.type == long_elem ||
         l.type == float_elem || l.type == double_elem ||
         l.type == string_elem || l.type == choice_elem ||
         l.type == color_csr || l.type == inline_color_csr ) {
      // a label and a edit box.
      int field_length = vcl_strlen(field->label.c_str()) + edit_length;
      if (max_length<field_length)
        max_length = field_length;
      height += height_sep;
      if ( l.type == color_csr || l.type == inline_color_csr )
        height += height_sep; // plus a row for choose color button.
    }
    else if (l.type == bool_elem )
    {
      // a label
      int field_length = vcl_strlen(field->label.c_str());
      if (max_length<field_length)
        max_length = field_length;
      height += height_sep;
    }
    else if ( l.type == text_msg )
    {
      // one or several lines of text
      char *text, *next_text;
      int field_length;

      text = (char *)field->label.c_str();
      while ( text ) {
        next_text = vcl_strchr(text, '\n');
        if ( next_text )
          field_length = next_text-text;
        else
          field_length = vcl_strlen(text);
        if (max_length<field_length)
          max_length = field_length;
        height += height_sep;
        text = next_text ? next_text+1 : NULL;
      }
    }
    else if (l.type == inline_tabl)
    {
      // a tableau
      vgui_win32_dialog_inline_tab* tab_data = (vgui_win32_dialog_inline_tab*)l.widget;
      int tab_width = tab_data->width/cxChar + 2*width_sep; // plus width_sep on each side
      if (max_length < tab_width)
        max_length = tab_width;
      height += tab_data->height;
    }
    else if (l.type == file_bsr || l.type == inline_file_bsr)
    {
      // a label, a edit box, and a "Browse..." button
      int field_length = vcl_strlen(field->label.c_str()) + browser_length;
      if (max_length<field_length)
        max_length = field_length;
      height += 2*height_sep;
      fbsr_count++;
    }
    else // Remaining types
    {
      // add a row.
      height += height_sep;
    }
  }

  // Width of the dialog box is approx 8 times the number of characters:
  width = (max_length+width_sep) * cxChar;
  // restore the max actual field length
  max_length -= fbsr_count ? browser_length : edit_length;
}

bool vgui_win32_dialog_impl::ask()
{
  // Retrieve the average width and height of system font in pixels,
  // and convert to dialog box base unit.
  // It's observed that 2 pixel equals to 1 dialog box base unit.
  const int cxChar = LOWORD(GetDialogBaseUnits());
  const int cyChar = HIWORD(GetDialogBaseUnits());

  const int height_sep = 6*cyChar/4; // vertical distance between two adjacent controls.
  const int width_sep = 2; // horizontal distance between two adjacent controls.
  const int edit_length = 20;
  const int browser_length = 40;
  const int button_length = 12;
  const int button_height = 6*cyChar/4;
  const int edit_height = 5*cyChar/4;

  int width, height, max_length, fbsr_count;
  short x, y, cx, cy;
  RECT rect;

  // Find out the size of the dialog box needed
  FindDialogSize(width, height, max_length, fbsr_count, cxChar, cyChar,
                 width_sep, height_sep, button_length, edit_length, browser_length);

  HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWndParent, GWL_HINSTANCE);
  // Get the position and size of the parent window to position the
  // popup dialog box.
  GetWindowRect(hWndParent, &rect);
  hWnd = CreateWindow(TEXT("vgui_win32_dialog"), name.c_str(),
                      WS_CAPTION | WS_POPUP | WS_SYSMENU | DS_MODALFRAME,
                      rect.left+10, rect.top+10, width, height,
                      hWndParent, NULL, hInstance, NULL);
  if ( hWnd == NULL )
    MessageBox(NULL, TEXT("Fail to create dialog box window!"),
               NULL, MB_ICONERROR);

  // Attach basic buttons "OK" and "Cancel" at the bottom of the dialog
  cx = button_length*cxChar;
  cy = button_height;
  x = width - 2*(cx + width_sep*cxChar);
  y = height - 3*height_sep;

  // Ok button
  if (ok_button_text_.size() > 0) {
    CreateWindow(TEXT("BUTTON"), (char*)ok_button_text_.c_str(),
                 WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, x, y, cx, cy,
                 hWnd, (HMENU)IDOK, hInstance, NULL);
  }

  // Cancel button
  if (cancel_button_text_.size() > 0) {
    x += cx+width_sep*cxChar;
    CreateWindow(TEXT("BUTTON"), (char*)cancel_button_text_.c_str(),
                 WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, x, y, cx, cy,
                 hWnd, (HMENU)IDCANCEL, hInstance, NULL);
  }

  // Add elements from top to bottom
  x = cxChar; y = -cyChar;
  cx = 0;
  unsigned short ctrl_count = 0;
  HWND hCtrlWnd;
  for (vcl_vector<element>::iterator e_iter = elements.begin();
       e_iter != elements.end(); ++e_iter) {
    element l = *e_iter;
    vgui_dialog_field *field = l.field;

    y += height_sep;

    if (l.type == int_elem ||
        l.type == long_elem ||
        l.type == float_elem ||
        l.type == double_elem ||
        l.type == string_elem)
    {
      // Set static text first
      cx = field->label.size()*cxChar;
      cy = edit_height;
      CreateWindow(TEXT("STATIC"), (char*)field->label.c_str(),
                   WS_CHILD | WS_VISIBLE | SS_LEFT, x, y, cx, cy,
                   hWnd, (HMENU)IDC_STATIC, hInstance, NULL);

      // Now set the line editor next
      int savex = x;
      x = (max_length)*cxChar;
      cx = edit_length*cxChar;
      WORD wDlgCtrlId = DLG_ID_START + ctrl_count++;
      CreateWindow(TEXT("EDIT"), (char*)field->current_value().c_str(),
                   WS_CHILD | WS_VISIBLE | ES_LEFT|ES_AUTOHSCROLL, x, y, cx, cy,
                   hWnd, (HMENU)wDlgCtrlId, hInstance, NULL);
      x = savex;
    }

    else if (l.type == bool_elem)
    {
      cx = (field->label.size()+3)*cxChar;
      cy = button_height;
      WORD wDlgCtrlId = DLG_ID_START + ctrl_count++;
      CreateWindow(TEXT("BUTTON"), (char*)field->label.c_str(),
                   WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, x, y, cx, cy,
                   hWnd, (HMENU)wDlgCtrlId, hInstance, NULL);
      // set check status
      vgui_bool_field *bfield = static_cast<vgui_bool_field*>(field);
      CheckDlgButton(hWnd, wDlgCtrlId, bfield->var ? BST_CHECKED : BST_UNCHECKED);
    }

    else if (l.type == choice_elem)
    {
      //vgui_int_field *field = static_cast<vgui_int_field*>(l.field);
      cx = field->label.size()*cxChar;
      cy = edit_height;
      // Set static text first
      CreateWindow(TEXT("STATIC"), (char*)field->label.c_str(),
                   WS_CHILD | WS_VISIBLE | SS_LEFT, x, y, cx, cy,
                   hWnd, (HMENU)IDC_STATIC, hInstance, NULL);

      // Then the combobox
      vgui_win32_dialog_choice *ch = (vgui_win32_dialog_choice*)l.widget;

      int savex = x;
      x = (max_length)*cxChar;
      cx = edit_length*cxChar;
      cy = ch->names.size() * cyChar;
      WORD wDlgCtrlId = DLG_ID_START + ctrl_count++;
      hCtrlWnd = CreateWindow(TEXT("COMBOBOX"), "",
                              WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST, x, y, cx, cy,
                              hWnd, (HMENU)wDlgCtrlId, hInstance, NULL);

      x = savex;

      // Add optional strings to the list
      for (vcl_vector<vcl_string>::iterator s_iter =  ch->names.begin();
           s_iter != ch->names.end(); ++s_iter)
        SendMessage(hCtrlWnd, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)s_iter->c_str());

      // And set the selection.
      SendMessage(hCtrlWnd, CB_SETCURSEL, (WPARAM)ch->index, 0);
    }

    else if (l.type == text_msg)
    {
      // one or several lines of text
      char *text, *next_text;
      int field_length;

      text = (char *)field->label.c_str();
      cx = 0; cy = 0;
      while ( text ) {
        next_text = vcl_strchr(text, '\n');
        if ( next_text )
          field_length = next_text-text;
        else
          field_length = vcl_strlen(text);
        if (cx<field_length)
          cx = field_length;
        cy++;
        text = next_text ? next_text+1 : NULL;
      }
      cx *= cxChar;
      cy *= edit_height;
      CreateWindow(TEXT("STATIC"), (char*)field->label.c_str(),
                   WS_CHILD | WS_VISIBLE | SS_LEFT, x, y, cx, cy,
                   hWnd, (HMENU)IDC_STATIC, hInstance, NULL);
    }

    else if (l.type == file_bsr || l.type == inline_file_bsr ||
             l.type == color_csr || l.type == inline_color_csr )
    {
      cx = field->label.size()*cxChar;
      cy = edit_height;
      // Set static text first
      CreateWindow(TEXT("STATIC"), (char*)field->label.c_str(),
                   WS_CHILD | WS_VISIBLE | SS_LEFT, x, y, cx, cy,
                   hWnd, (HMENU)IDC_STATIC, hInstance, NULL);

      // Next set the line editor
      int savex = x;
      x = (max_length)*cxChar;
      if ( l.type == file_bsr || l.type == inline_file_bsr )
        cx = browser_length*cxChar;
      else
        cx = edit_length*cxChar;
      WORD wDlgCtrlId = DLG_ID_START + ctrl_count++;
      CreateWindow(TEXT("EDIT"), (char*)field->current_value().c_str(),
                   WS_CHILD | WS_VISIBLE | ES_LEFT|ES_AUTOHSCROLL, x, y, cx, cy,
                   hWnd, (HMENU)wDlgCtrlId, hInstance, NULL);

      // Third, add a browse button
      y += height_sep;
      cx = button_length*cxChar;
      cy = button_height;
      wDlgCtrlId = DLG_ID_START + ctrl_count++;
      if ( l.type == file_bsr || l.type == inline_file_bsr ) {
        CreateWindow(TEXT("BUTTON"), "Browse...",
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, x, y, cx, cy,
                     hWnd, (HMENU)wDlgCtrlId, hInstance, NULL);
        fb_ids.push_back(wDlgCtrlId);
      }
      else {
        CreateWindow(TEXT("BUTTON"), "Colour...",
                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, x, y, cx, cy,
                     hWnd, (HMENU)wDlgCtrlId, hInstance, NULL);
        cc_ids.push_back(wDlgCtrlId);
      }

      x = savex;
    }

    else if (l.type == inline_tabl)
    {
      vgui_win32_dialog_inline_tab* tab_data = (vgui_win32_dialog_inline_tab *)l.widget;

      int savecy = cy;
      cx = short(tab_data->width); // in dialog box base unit
      cy = short(tab_data->height);
      WORD wDlgCtrlId = DLG_ID_START + ctrl_count++;
      hCtrlWnd = CreateWindow(TEXT("vgui_win32_inline_tab"), "",
                              WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, x, y, cx, cy,
                              hWnd, (HMENU)wDlgCtrlId, hInstance, NULL);

      y += cy-height_sep;
      cy = savecy;

      // Create a vgui_win32_adaptor as the child of the control.
      vgui_win32_adaptor* adptr = new vgui_win32_adaptor(hCtrlWnd);
      adptr->set_tableau(tab_data->tab);
      adptr->set_width(tab_data->width);
      adptr->set_height(tab_data->height);

      // Save the info of this tab.
      inline_tab_data the_tab;
      the_tab.childId = wDlgCtrlId;
      the_tab.hWnd    = hCtrlWnd;
      the_tab.adaptor = adptr;
      inline_tableaus.push_back(the_tab);
    }
  }

  // Show the dialog box.
  ShowWindow(hWnd, SW_SHOW);
  UpdateWindow(hWnd);
  // Show inline tableaus
  for ( vcl_size_t i = 0; i < inline_tableaus.size(); i++ )
    inline_tableaus[i].adaptor->post_redraw();

  // Enter the message loop.
  run();

  return ok_clicked;
}

void vgui_win32_dialog_impl::run()
{
  MSG msg;

  while ( GetMessage(&msg, NULL, 0, 0) ) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

// Handle messages that are related to vgui_dialog_impl.
LRESULT vgui_win32_dialog_impl::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message) {
    case WM_CREATE:
      // Disable keyboard/mouse message of parent window
      EnableWindow(GetParent(hDlg), FALSE);
      return 1;

    case WM_DESTROY:
      EnableWindow(GetParent(hDlg), TRUE);
      SetActiveWindow(GetParent(hDlg));
      PostQuitMessage(0);
      return 1;

    case WM_COMMAND: {
      WORD wCtrlId = LOWORD(wParam);
      if ( wCtrlId == IDOK ) {
        OnOK();
        DestroyWindow(hDlg);
        return 0;
      }
      if ( wCtrlId == IDCANCEL ) {
        OnCancel();
        DestroyWindow(hDlg);
        return 0;
      }
      if ( IsFileBrowserButton(wCtrlId) ) {
        OnBrowse(hDlg, wCtrlId);
        return 0;
      }
      if ( IsColorChooserButton(wCtrlId) ) {
        char strColor[16];
        GetWindowText(GetDlgItem(hDlg, wCtrlId-1), strColor, 16);
        OnColor(hDlg, wCtrlId, strColor);
        return 0;
      }

      break;
    }
  }

  return 0;
}

// Update values of all elements when user click OK button.
void vgui_win32_dialog_impl::OnOK()
{
  HWND hWndCtrl;
  char buf[MAX_PATH];
  int control_count, ctrl_id;

  ok_clicked = true;

  // Update values of all elements.
  control_count = 0;
  for (vcl_vector<element>::iterator e_iter = elements.begin();
       e_iter != elements.end(); ++e_iter) {
    element l = *e_iter;
    vgui_dialog_field *field = l.field;
    ctrl_id = DLG_ID_START+control_count;
    hWndCtrl = GetDlgItem(hWnd, ctrl_id);

    if ( l.type == int_elem ||
         l.type == long_elem ||
         l.type == float_elem ||
         l.type == double_elem ||
         l.type == string_elem ) { // a line/multiline edit
      GetWindowText(hWndCtrl, buf, MAX_PATH);
      field->update_value(buf);
      control_count++;
    }

    else if ( l.type == bool_elem ) { // a check box
      vgui_bool_field *bfield = static_cast<vgui_bool_field*>(field);
      bfield->var = IsDlgButtonChecked(hWnd, ctrl_id) ? true : false;
      control_count++;
    }

    else if ( l.type == choice_elem ) { // a combo box
      vgui_int_field *ifield = (vgui_int_field *)field;
      ifield->var = SendMessage(hWndCtrl, CB_GETCURSEL, 0, 0);
      control_count++;
    }

    else if ( l.type == file_bsr ||
              l.type == color_csr ||
              l.type == inline_file_bsr ||
              l.type == inline_color_csr ) { // a line edit and a button
      GetWindowText(hWndCtrl, buf, MAX_PATH);
      field->update_value(buf);
      control_count += 2;
    }

    else if ( l.type == inline_tabl ) { // a tab
      control_count++;
    }
  }
}

void vgui_win32_dialog_impl::OnCancel()
{
  ok_clicked = false;
}

// Open a file open dialog box.
BOOL vgui_win32_dialog_impl::OnBrowse(HWND hDlg, WORD wCtrlId)
{
  static TCHAR szFileName[MAX_PATH], szTitleName[MAX_PATH];
  static TCHAR szFilter[] = TEXT("All Files (*.*)\0*.*\0")
                            TEXT("All Files (*.*)\0*.*\0\0");
  static OPENFILENAME ofn;

  ofn.lStructSize       = sizeof(OPENFILENAME);
  ofn.hwndOwner         = hDlg;
  ofn.hInstance         = NULL;
  ofn.lpstrFilter       = szFilter;
  ofn.lpstrCustomFilter = NULL;
  ofn.nMaxCustFilter    = 0;
  ofn.nFilterIndex      = 0;
  ofn.lpstrFile         = szFileName;
  ofn.nMaxFile          = MAX_PATH;
  ofn.lpstrFileTitle    = szTitleName;
  ofn.nMaxFileTitle     = MAX_PATH;
  ofn.lpstrInitialDir   = NULL;
  ofn.lpstrTitle        = NULL;
  ofn.Flags             = OFN_CREATEPROMPT;
  ofn.nFileOffset       = 0;
  ofn.nFileExtension    = 0;
  ofn.lpstrDefExt       = NULL;
  ofn.lCustData         = 0;
  ofn.lpfnHook          = NULL;
  ofn.lpTemplateName    = NULL;

  if ( GetOpenFileName(&ofn) )
    SetWindowText(GetDlgItem(hDlg, wCtrlId-1), szFileName);

  return TRUE;
}

BOOL vgui_win32_dialog_impl::OnColor(HWND hDlg, WORD wCtrlId, LPTSTR lpColor)
{
  char buffer[16];
  vcl_string strColor;

  static CHOOSECOLOR cc;
  static COLORREF    crCustColors[16];

  cc.lStructSize    = sizeof(CHOOSECOLOR);
  cc.hwndOwner      = hDlg;
  cc.hInstance      = NULL;
  cc.rgbResult      = ColorStringToRGB(lpColor);
  cc.lpCustColors   = crCustColors;
  cc.Flags          = CC_RGBINIT | CC_FULLOPEN;
  cc.lCustData      = 0;
  cc.lpfnHook       = NULL;
  cc.lpTemplateName = NULL;

  if ( ChooseColor(&cc) ) {
    vcl_sprintf(buffer, "%3d", GetRValue(cc.rgbResult));
    strColor += buffer;
    vcl_sprintf(buffer, " %3d", GetGValue(cc.rgbResult));
    strColor += buffer;
    vcl_sprintf(buffer, " %3d", GetBValue(cc.rgbResult));
    strColor += buffer;
    SetWindowText(GetDlgItem(hDlg, wCtrlId-1), strColor.c_str());
  }

  return TRUE;
}

// Convert a color that is defined in string format "lpColor" to equivalent
// COLORREF format
COLORREF vgui_win32_dialog_impl::ColorStringToRGB(LPTSTR lpColor)
{
  int r, g, b;

  if ( vcl_strcmp(lpColor, "red") == 0 )
    return RGB(255, 0, 0);
  if ( vcl_strcmp(lpColor, "green") == 0 )
    return RGB(0, 255, 0);
  if ( vcl_strcmp(lpColor, "blue") == 0 )
    return RGB(0, 0, 255);
  if ( vcl_strcmp(lpColor, "yellow") == 0 )
    return RGB(255, 255, 0);
  if ( vcl_strcmp(lpColor, "pink") == 0 )
    return RGB(255, 0, 255);
  if ( vcl_strcmp(lpColor, "cyan") == 0 )
    return RGB(0, 255, 255);
  if ( vcl_strcmp(lpColor, "black") == 0 )
    return RGB(0, 0, 0);
  if ( vcl_strcmp(lpColor, "white") == 0 )
    return RGB(255, 255, 255);

  int ret = vcl_sscanf(lpColor, TEXT("%d%d%d"), &r, &g, &b);
  if ( ret == 3 )
    return COLORREF(RGB(r, g, b));
  else // in case of senseless string.
    return COLORREF(RGB(0, 0, 0));
}

inline bool vgui_win32_dialog_impl::IsFileBrowserButton(unsigned short ctrl_id)
{
  vcl_vector<unsigned short>::iterator result;

  result = vcl_find(fb_ids.begin(), fb_ids.end(), ctrl_id);
  return result == fb_ids.end() ? false : true;
}

inline bool vgui_win32_dialog_impl::IsColorChooserButton(unsigned short ctrl_id)
{
  vcl_vector<unsigned short>::iterator result;

  result = vcl_find(cc_ids.begin(), cc_ids.end(), ctrl_id);
  return result == cc_ids.end() ? false : true;
}

vgui_win32_adaptor* vgui_win32_dialog_impl::find_adaptor(unsigned short ctrl_id)
{
  for ( vcl_size_t i = 0; i < inline_tableaus.size(); i++ )
    if ( ctrl_id == inline_tableaus[i].childId )
      return inline_tableaus[i].adaptor;

  return NULL;
}
