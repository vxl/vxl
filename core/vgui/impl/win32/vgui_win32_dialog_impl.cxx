// This is core/vgui/impl/win32/vgui_win32_dialog_impl.cxx
#include <algorithm>
#include <cstdio>
#include <cstddef>
#include <cstring>
#include <iostream>
#include "vgui_win32_dialog_impl.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/internals/vgui_simple_field.h>
#include <vgui/internals/vgui_file_field.h>
#include <vgui/internals/vgui_button_field.h>
#include <vgui/impl/win32/vgui_win32_adaptor.h>

extern LRESULT CALLBACK globalDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

vgui_win32_dialog_impl::vgui_win32_dialog_impl(const char* name, HWND hWnd)
  : vgui_dialog_impl(name), ok_clicked(false), is_modal(true),
  hWndParent(hWnd), hWnd(NULL)
{
  inline_tableaus.clear();
  fb_ids.clear();
  cc_ids.clear();
  callback_controls.clear();
}

struct vgui_win32_dialog_pushbutton;

vgui_win32_dialog_impl::~vgui_win32_dialog_impl()
{
  for ( std::vector<inline_tab_data>::iterator
        it = inline_tableaus.begin(); it != inline_tableaus.end(); ++it )
    delete it->adaptor;

  for (std::vector<element>::iterator iter = elements.begin();
       iter != elements.end(); ++iter) {
    if ( iter->type == button_elem )
      delete (vgui_win32_dialog_pushbutton *)iter->widget;
  }
}

// Structure to contain data for a choice field.
struct vgui_win32_dialog_choice
{
  std::vector<std::string> names;
  int index;
};


// Make a choice widget
void* vgui_win32_dialog_impl::choice_field_widget(const char* /*txt*/,
                                                  const std::vector<std::string>& labels, int& val)
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


struct vgui_win32_dialog_pushbutton
{
  // TODO: the destructor is not executed in Visual Studio debugger.
  ~vgui_win32_dialog_pushbutton()
  { if ( hBitmap!=NULL ) DeleteObject(hBitmap); }

  unsigned short ctrl_id;
  std::string label;
  HANDLE     hBitmap;
  unsigned   width, height;
};

void* vgui_win32_dialog_impl::pushbutton_field_widget(const char *label, const void *icon)
{
  vgui_win32_dialog_pushbutton *pb = new vgui_win32_dialog_pushbutton;
  // Set default values
  pb->label = label ? label : ""; // cannot assign a NULL char* to a C++ string
  pb->hBitmap = NULL;
  pb->width   = 0;
  pb->height  = 0;

  if ( icon != NULL ) {
    // Load image/icon from disk file and convert to a Windows handle.
    pb->hBitmap = LoadImage(NULL, (char*)icon, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if ( pb->hBitmap != NULL ) {
      BITMAP bm;
      if ( GetObject(pb->hBitmap, sizeof(bm), &bm) ) {
        pb->width = bm.bmWidth;
        pb->height = bm.bmHeight;
      }
    }
  }

  return (void *)pb;
}

// Find out the size of the dialog box needed
void vgui_win32_dialog_impl::FindDialogSize(int &width, int &height,
                                            int &max_length, int &fbsr_count,
                                            int cxChar, int cyChar, int width_sep, int height_sep,
                                            int button_length, int edit_length, int browser_length)
{
  // Minimum dialog size
  const int cyCaption = GetSystemMetrics(SM_CYCAPTION);
  int field_length, field_height;

  height = 3*height_sep+cyCaption; // top, bottom margin plus an OK/Cancel button;
  max_length = width_sep;
  fbsr_count = 0;

  // Count the length of OK and Cancel button first.
  if (ok_button_text_.size() > 0)
    max_length += button_length+width_sep;
  if (cancel_button_text_.size() > 0)
    max_length += button_length+width_sep;

  field_length = field_height = 0;
  for (std::vector<element>::iterator e_iter = elements.begin();
       e_iter != elements.end(); ++e_iter) {
    element l = *e_iter;
    vgui_dialog_field *field = l.field;

    if ( !use_line_break ) { // when no line break is present, each control
      field_length = 0;      // takes up one line
      field_height = 0;
    }

    if ( l.type == int_elem || l.type == long_elem ||
         l.type == float_elem || l.type == double_elem ||
         l.type == string_elem || l.type == choice_elem ||
         l.type == color_csr || l.type == inline_color_csr ) {
      // a label and a edit box.
      int w = std::strlen(field->label.c_str()) + edit_length;
      int h = height_sep;
      if ( l.type == color_csr || l.type == inline_color_csr )
        h += height_sep; // plus a row for choose color button.

      field_length += w;
      if ( field_height < h ) field_height = h;

      if ( !use_line_break ) {
        if (max_length<field_length)
          max_length = field_length;
        height += field_height;
      }
    }
    else if (l.type == bool_elem )
    {
      // a label
      int w = std::strlen(field->label.c_str());
      int h = height_sep;

      field_length += w;
      if ( field_height < h ) field_height = h;

      if ( !use_line_break ) {
        if (max_length<field_length)
          max_length = field_length;
        height += field_height;
      }
    }
    else if ( l.type == text_msg )
    {
      // one or several lines of text
      char *text, *next_text;
      int w, w1, h;

      text = (char *)field->label.c_str();
      w = 0; h = 0;
      while ( text ) {
        next_text = std::strchr(text, '\n');
        if ( next_text )
          w1 = next_text-text;
        else
          w1 = std::strlen(text);
        text = next_text ? next_text+1 : NULL;

        if ( w < w1 ) w = w1;
        h += height_sep;
      }

      field_length += w;
      if ( field_height < h ) field_height = h;

      if ( !use_line_break ) {
        if (max_length<field_length)
          max_length = field_length;
        height += field_height;
      }
    }
    else if (l.type == inline_tabl)
    {
      // a tableau
      vgui_win32_dialog_inline_tab* tab_data = (vgui_win32_dialog_inline_tab*)l.widget;
      int w = tab_data->width/cxChar + 2*width_sep; // plus width_sep on each side
      int h = tab_data->height;

      field_length += w;
      if ( field_height < h ) field_height = h;

      if ( !use_line_break ) {
        if (max_length < field_length)
          max_length = field_length;
        height += field_height;
      }
    }
    else if (l.type == file_bsr || l.type == inline_file_bsr || l.type == dir_bsr)
    {
      // a label, an edit box, and a "Browse..." button
      int w = std::strlen(field->label.c_str()) + browser_length;
      int h = 2*height_sep;

      field_length += w;
      if ( field_height < h ) field_height = h;

      if ( !use_line_break ) {
        if (max_length<field_length)
          max_length = field_length;
        height += field_height;
      }
      fbsr_count++;
    }
    else if (l.type == button_elem )
    {
      // a button with an image and/or a label
      vgui_win32_dialog_pushbutton *pb = (vgui_win32_dialog_pushbutton*)l.widget;
      // the button width is the maximum of label length, image width
      // and default button width
      int w = (pb->width+cxChar-1)/cxChar;
      int slen = std::strlen(field->label.c_str());
      if ( w < slen )
        w = slen;

      int h = pb->height;
      h += height_sep * (field->label.empty() ? 0 : 1);

      field_length += w;
      if ( field_height < h ) field_height = h;

      if ( !use_line_break ) {
        if (max_length<field_length)
          max_length = field_length;
        height += field_height;
      }
    }
    else if ( l.type == line_br )
    {
      if (max_length<field_length)
        max_length = field_length;
      height += field_height;

      field_length = 0;
      field_height = 0;
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
  constexpr int width_sep = 2; // horizontal distance between two adjacent controls.
  constexpr int edit_length = 20;
  constexpr int browser_length = 40;
  constexpr int button_length = 12;
  const int button_height = 6*cyChar/4;
  const int edit_height = 5*cyChar/4;

  int width, height, max_length, fbsr_count;
  short x, y, cx, cy;
  RECT rect;

  // Find if line_break is used
  for (std::vector<element>::iterator e_iter = elements.begin();
       e_iter != elements.end(); ++e_iter) {
    if ( e_iter->type == line_br ) {
      use_line_break = true;
      break;
    }
  }

  // Find out the size of the dialog box needed
  FindDialogSize(width, height, max_length, fbsr_count, cxChar, cyChar,
                 width_sep, height_sep, button_length, edit_length, browser_length);

#ifdef _WIN64
  HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWndParent, GWLP_HINSTANCE);
#else
  HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWndParent, GWL_HINSTANCE);
#endif //_WIN64
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
  x = cxChar; y = cyChar>>1;
  unsigned short ctrl_count = 0;
  HWND hCtrlWnd;
  short dy = 0;
  for (std::vector<element>::iterator e_iter = elements.begin();
       e_iter != elements.end(); ++e_iter) {
    element l = *e_iter;
    vgui_dialog_field *field = l.field;

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

      if ( use_line_break ) {
        x += (field->label.size()+edit_length)*cxChar+2*width_sep; // keep the pentip on the line
        if ( dy < height_sep ) dy = height_sep;
      }
      else {
        x = savex;      // move the pentip to the beginning of the next line
        y += height_sep;
      }
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


      if ( use_line_break ) {
        x += cx+width_sep; // keep the pentip on the line
        if ( dy < height_sep ) dy = height_sep;
      }
      else {
        x = cxChar;      // move the pentip to the beginning of the next line
        y += height_sep;
      }
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
                              WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
                              x, y, cx, cy, hWnd, (HMENU)wDlgCtrlId, hInstance, NULL);

      // Add optional strings to the list
      for (std::vector<std::string>::iterator s_iter =  ch->names.begin();
           s_iter != ch->names.end(); ++s_iter)
        SendMessage(hCtrlWnd, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)s_iter->c_str());

      // And set the selection.
      SendMessage(hCtrlWnd, CB_SETCURSEL, (WPARAM)ch->index, 0);

      if ( use_line_break ) {
        x += (field->label.size()+edit_length)*cxChar+2*width_sep; // keep the pentip on the line
        if ( dy < height_sep ) dy = height_sep;
      }
      else {
        x = savex;      // move the pentip to the beginning of the next line
        y += height_sep;
      }
    }

    else if (l.type == text_msg)
    {
      // one or several lines of text
      char *text, *next_text;
      int field_length;

      text = (char *)field->label.c_str();
      cx = 0; cy = 0;
      while ( text ) {
        next_text = std::strchr(text, '\n');
        if ( next_text )
          field_length = next_text-text;
        else
          field_length = std::strlen(text);
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

      if ( use_line_break ) {
        x += cx+width_sep; // keep the pentip on the line
        if ( dy < cy ) dy = cy;
      }
      else {
        x = cxChar;      // move the pentip to the beginning of the next line
        y += cy;
      }
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
      int savey = y;
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

      if ( use_line_break ) {
        x += (field->label.size()+edit_length)*cxChar+2*width_sep; // keep the pentip on the line
        y = savey;
        if ( dy < 2*height_sep ) dy = 2*height_sep;
      }
      else {
        x = savex;      // move the pentip to the beginning of the next line
        y += height_sep;
      }
    }

    else if (l.type == inline_tabl)
    {
      vgui_win32_dialog_inline_tab* tab_data = (vgui_win32_dialog_inline_tab *)l.widget;

      //int savecy = cy;
      cx = short(tab_data->width); // in dialog box base unit
      cy = short(tab_data->height);
      WORD wDlgCtrlId = DLG_ID_START + ctrl_count++;
      hCtrlWnd = CreateWindow(TEXT("vgui_win32_inline_tab"), "",
                              WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, x, y, cx, cy,
                              hWnd, (HMENU)wDlgCtrlId, hInstance, NULL);

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

      if ( use_line_break ) {
        x += cx+width_sep; // keep the pentip on the line
        if ( dy < cy ) dy = cy;
      }
      else {
        x = cxChar;      // move the pentip to the beginning of the next line
        y += cy;
      }
    }

    else if (l.type == button_elem) {
      // a button with an image and/or a label
      vgui_win32_dialog_pushbutton *pb = (vgui_win32_dialog_pushbutton*)l.widget;
      // the button width is the maximum of label length, image width,
      // and default button width
      cx = field->label.size()*cxChar;
      if ( cx < (short)(pb->width) ) cx = pb->width;
      //if ( cx < button_length ) cx = button_length;
      cy = pb->height;
      cy += field->label.empty() ? 0 : button_height;
      WORD wDlgCtrlId = DLG_ID_START + ctrl_count++;
      hCtrlWnd = CreateWindow(TEXT("BUTTON"), (char*)field->label.c_str(),
                              WS_CHILD | WS_VISIBLE | (pb->hBitmap ? BS_OWNERDRAW : BS_PUSHBUTTON),
                              x, y, cx, cy, hWnd, (HMENU)wDlgCtrlId, hInstance, NULL);
      pb->ctrl_id = wDlgCtrlId;

      callback_control_data ccd;
      vgui_button_field *l = (vgui_button_field *)field;
      ccd.child_id = wDlgCtrlId;
      ccd.cmnd     = l->cmnd;
      callback_controls.push_back(ccd);

      if ( use_line_break ) {
        x += cx+width_sep; // keep the pentip on the line
        if ( dy < cy ) dy = cy;
      }
      else {
        x = cxChar;      // move the pentip to the beginning of the next line
        y += cy;
      }
    }

    else if (l.type == line_br) { // move the pentip to
       x = cxChar;                // the beginning
       y += dy;                   // of the next line
       dy = 0;
    }
  }

  // Show the dialog box.
  ShowWindow(hWnd, SW_SHOW);
  UpdateWindow(hWnd);

  // Show inline tableaus
  for ( std::size_t i = 0; i < inline_tableaus.size(); i++ )
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
      // Disable keyboard/mouse message of parent window for modal dialog
      if ( is_modal )
        EnableWindow(GetParent(hDlg), FALSE);
      return 1;

    case WM_DESTROY:
      if ( is_modal )
        EnableWindow(GetParent(hDlg), TRUE);
      SetActiveWindow(GetParent(hDlg));
      PostQuitMessage(0);
      return 1;

    case WM_DRAWITEM: {
      LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT)lParam;
      // Obtain info of control item
      HDC hDC = lpDIS->hDC;
      RECT rectItem = lpDIS->rcItem;
      UINT ctrlID = lpDIS->CtlID;
      BOOL isDisabled = lpDIS->itemState & ODS_DISABLED; // get button state
      BOOL isFocused  = lpDIS->itemState & ODS_FOCUS;
      BOOL isPressed  =    lpDIS->itemState & ODS_SELECTED;
      //BOOL bDrawFocusRect = !(lpDIS->itemState & ODS_NOFOCUSRECT);

      // Get button's info (label, bitmap)
      std::vector<element>::iterator e_iter;
      vgui_win32_dialog_pushbutton *pb;
      for ( e_iter = elements.begin(); e_iter != elements.end(); ++e_iter) {
         if ( e_iter->type == button_elem ) {
           pb = (vgui_win32_dialog_pushbutton*)e_iter->widget;
           if ( ctrlID == pb->ctrl_id )
             break;
         }
      }

      if ( e_iter == elements.end() )
        break; // exit if the owner-draw button is not found

      SetBkMode(hDC, TRANSPARENT); // TODO: experiment

      // Paint button background
#if 0
      if (Themed) {
        DWORD state = (bIsPressed)?PBS_PRESSED:PBS_NORMAL;

        if (state == PBS_NORMAL)    {
          if (bIsFocused)
            state = PBS_DEFAULTED;
          if (bMouseOverButton)
            state = PBS_HOT;
        }
        zDrawThemeBackground(hTheme, hDC, BP_PUSHBUTTON, state, &lpDIS->rcItem, NULL);
      }
      else {
#endif
        if ( isFocused ) {
          // Simulate button pressed down
          HBRUSH hbr = CreateSolidBrush(RGB(0,0,0));
          FrameRect(hDC, &rectItem, hbr);
          InflateRect(&rectItem, -1, -1);
          DeleteObject(hbr);
        }

        //COLORREF crColor = GetSysColor(COLOR_BTNFACE);
        //HBRUSH hbr = CreateSolidBrush(crColor);
        //FillRect(hDC, &rectItem, hbr);
        //DeleteObject(hbr);

        // Draw pressed button
        if ( isPressed ) {
          // Simulate button released
          HBRUSH hbr = CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
          FrameRect(hDC, &rectItem, hbr);
          DeleteObject(hbr);
        }
        else {
          DrawFrameControl(hDC, &rectItem, DFC_BUTTON, DFCS_BUTTONPUSH);
        }
#if 0
      }
#endif

      // Draw image on the button (if any)
      if ( pb->hBitmap != NULL ) {
        RECT rcImage;

        // Center image horizontally
        CopyRect(&rcImage, &rectItem);
        UINT image_width = rcImage.right - rcImage.left;
        rcImage.left = (image_width - pb->width)/2;

        DrawState(hDC, NULL, NULL, (LPARAM)(HBITMAP)pb->hBitmap, 0,
                  rcImage.left, rcImage.top,
                  rcImage.right - rcImage.left,
                  rcImage.bottom - rcImage.top,
                  (isDisabled ? DSS_DISABLED : DSS_NORMAL) | DST_BITMAP);
        //DrawImageOnButton(hDC, &rectItem, (HBITMAP)pb->hBitmap, pb->width, pb->height, isDisabled);
      }

      // Write the button label (if any)
      if ( !pb->label.empty() ) {
        RECT rcLabel;

        CopyRect(&rcLabel, &lpDIS->rcItem);

        // If button is pressed then "press" the label also
        //if ( isPressed /*&& !Themed*/) OffsetRect(&rcLabel, 1, 1);

        // Center text
        //RECT centerRect = rcLabel;
        // Determine the width and height of rcLabel but does not draw the text.
        //DrawText(hDC, pb->label.c_str(), -1, &rcLabel, DT_WORDBREAK | DT_CENTER | DT_CALCRECT);
        rcLabel.top += pb->height;
        //LONG captionRectWidth = rcLabel.right - rcLabel.left;
        //LONG captionRectHeight = rcLabel.bottom - rcLabel.top;
        //LONG centerRectWidth = centerRect.right - centerRect.left;
        //LONG centerRectHeight = centerRect.bottom - centerRect.top;
        //OffsetRect(&rcLabel, (centerRectWidth - captionRectWidth)/2, (centerRectHeight - captionRectHeight)/2);
#if 0
        if (Themed) {
          // convert title to UNICODE obviously you don't need to do this if you are a UNICODE app.
          int nTextLen = std::strlen(sTitle);
          int mlen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (char *)sTitle, nTextLen + 1, NULL, 0);
          WCHAR* output = new WCHAR[mlen];
          if (output)
          {
            MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (char *)sTitle, nTextLen + 1, output, mlen);
            zDrawThemeText(hTheme, hDC, BP_PUSHBUTTON, PBS_NORMAL,
                           output, wcslen(output),
                           DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                           0, &rcLabel);
            delete output;
          }
        }
        else {
#endif // 0
          //SetBkMode(hDC, TRANSPARENT);
          if ( isDisabled ) {
            OffsetRect(&rcLabel, 1, 1);
            SetTextColor(hDC, ::GetSysColor(COLOR_3DHILIGHT));
            DrawText(hDC, pb->label.c_str(), -1, &rcLabel, DT_WORDBREAK | DT_CENTER);
            OffsetRect(&rcLabel, -1, -1);
            SetTextColor(hDC, ::GetSysColor(COLOR_3DSHADOW));
            DrawText(hDC, pb->label.c_str(), -1, &rcLabel, DT_WORDBREAK | DT_CENTER);
          }
          else {
            SetTextColor(hDC, GetSysColor(COLOR_BTNTEXT));
            SetBkColor(hDC, GetSysColor(COLOR_BTNFACE));
            DrawText(hDC, pb->label.c_str(), -1, &rcLabel, DT_WORDBREAK | DT_CENTER);
          }

#if 0
        }
#endif
      }

      // Draw the focus rect
      //if ( bIsFocused && bDrawFocusRect) {
      //  RECT focusRect = itemRect;
      //  InflateRect(&focusRect, -3, -3);
      //  DrawFocusRect(hDC, &focusRect);
      //}

      break;
    }

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
      if ( IsCallbackControl(wCtrlId) ) {
        dialog_dispatcher(wCtrlId);
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
  for (std::vector<element>::iterator e_iter = elements.begin();
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
  std::string strColor;

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
    std::sprintf(buffer, "%3d", GetRValue(cc.rgbResult));
    strColor += buffer;
    std::sprintf(buffer, " %3d", GetGValue(cc.rgbResult));
    strColor += buffer;
    std::sprintf(buffer, " %3d", GetBValue(cc.rgbResult));
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

  if ( std::strcmp(lpColor, "red") == 0 )
    return RGB(255, 0, 0);
  if ( std::strcmp(lpColor, "green") == 0 )
    return RGB(0, 255, 0);
  if ( std::strcmp(lpColor, "blue") == 0 )
    return RGB(0, 0, 255);
  if ( std::strcmp(lpColor, "yellow") == 0 )
    return RGB(255, 255, 0);
  if ( std::strcmp(lpColor, "pink") == 0 )
    return RGB(255, 0, 255);
  if ( std::strcmp(lpColor, "cyan") == 0 )
    return RGB(0, 255, 255);
  if ( std::strcmp(lpColor, "black") == 0 )
    return RGB(0, 0, 0);
  if ( std::strcmp(lpColor, "white") == 0 )
    return RGB(255, 255, 255);

  int ret = std::sscanf(lpColor, TEXT("%d%d%d"), &r, &g, &b);
  if ( ret == 3 )
    return COLORREF(RGB(r, g, b));
  else // in case of senseless string.
    return COLORREF(RGB(0, 0, 0));
}

inline bool vgui_win32_dialog_impl::IsFileBrowserButton(unsigned short ctrl_id)
{
  std::vector<unsigned short>::iterator result;

  result = std::find(fb_ids.begin(), fb_ids.end(), ctrl_id);
  return result == fb_ids.end() ? false : true;
}


inline bool vgui_win32_dialog_impl::IsColorChooserButton(unsigned short ctrl_id)
{
  std::vector<unsigned short>::iterator result;

  result = std::find(cc_ids.begin(), cc_ids.end(), ctrl_id);
  return result == cc_ids.end() ? false : true;
}

inline bool vgui_win32_dialog_impl::IsCallbackControl(unsigned short ctrl_id)
{
  for ( std::size_t i = 0; i < callback_controls.size(); i++ )
    if ( ctrl_id == callback_controls[i].child_id )
      return true;

  return false;
}

vgui_win32_adaptor* vgui_win32_dialog_impl::find_adaptor(unsigned short ctrl_id)
{
  for ( std::size_t i = 0; i < inline_tableaus.size(); i++ )
    if ( ctrl_id == inline_tableaus[i].childId )
      return inline_tableaus[i].adaptor;

  return NULL;
}

void vgui_win32_dialog_impl::dialog_dispatcher(int ctrl_id)
{
  for ( std::size_t i = 0; i < callback_controls.size(); i++ ) {
    if ( ctrl_id == callback_controls[i].child_id ) {
      callback_controls[i].cmnd->execute();
      break;
    }
  }
}

#if 0
void PrepareImageRect(BOOL bHasTitle, RECT* rpItem, RECT* rpTitle, BOOL bIsPressed, DWORD dwWidth, DWORD dwHeight, RECT* rpImage)
{
  CopyRect(rpImage, rpItem);

  if (bHasTitle == FALSE) {
    // Center image horizontally
    LONG rpImageWidth = rpImage->right - rpImage->left;
    rpImage->left += ((rpImageWidth - (long)dwWidth)/2);
  }
  else {
    // Image must be placed just inside the focus rect
    LONG rpTitleWidth = rpTitle->right - rpTitle->left;
    rpTitle->right = rpTitleWidth - dwWidth - 30;
    rpTitle->left = 30;

    rpImage->left = rpItem->right - dwWidth - 30;
    // Center image vertically
    LONG rpImageHeight = rpImage->bottom - rpImage->top;
    rpImage->top += ((rpImageHeight - (long)dwHeight)/2);
  }

  // If button is pressed then press image also
  if (bIsPressed && !Themed)
    OffsetRect(rpImage, 1, 1);
}
#endif // 0

void vgui_win32_dialog_impl::DrawImageOnButton(HDC hDC, RECT* lprcItem,
                                               HBITMAP hBitmap, unsigned w, unsigned h, BOOL isDisabled)
{
  RECT rcImage;

  // Center image horizontally
  CopyRect(&rcImage, lprcItem);
  UINT image_width = rcImage.right - rcImage.left;
  rcImage.left = (image_width - w)/2;

  DrawState(hDC, NULL, NULL, (LPARAM)hBitmap, 0,
            rcImage.left, rcImage.top,
            rcImage.right - rcImage.left,
            rcImage.bottom - rcImage.top,
            (isDisabled ? DSS_DISABLED : DSS_NORMAL) | DST_BITMAP);

  return;
}
