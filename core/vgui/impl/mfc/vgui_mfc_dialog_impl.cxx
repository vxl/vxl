// This is core/vgui/impl/mfc/vgui_mfc_dialog_impl.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Marko Bacic, Oxford RRG
// \date   31 July 2000
//
// See vgui_mfc_dialog_impl.h for a description of this file.

#include "vgui_mfc_dialog_impl.h"

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cstdio.h> // for sprintf()
#include <vcl_cstring.h>

#include <vgui/internals/vgui_dialog_field.h>
#include <vgui/internals/vgui_simple_field.h>
#include <vgui/impl/mfc/vgui_mfc_adaptor.h>
#include <winuser.h>

static bool debug = false;
static bool is_modal = true;
vcl_string title;
vcl_string orig_color; // For when color chooser is cancelled.
CString TempsNewClass;
BEGIN_MESSAGE_MAP(vgui_mfc_dialog_impl, CWnd)
        ON_COMMAND(IDOK,OnOk)
        ON_COMMAND(IDCANCEL,OnCancel)
        ON_WM_CLOSE()
        ON_CONTROL_RANGE(BN_CLICKED,ID_BROWSE_FILES,ID_BROWSE_FILES+100, OnBrowse)
        ON_CONTROL_RANGE(BN_CLICKED,ID_CHOOSE_COLOUR,ID_CHOOSE_COLOUR+100,OnChooseColour)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------
//: Constructor
vgui_mfc_dialog_impl::vgui_mfc_dialog_impl(const char* name)
  : CWnd(),vgui_dialog_impl(name)
{
  title = name;
  // Set some default parameters
  count_fbsr = 0;
  count_csr = 0;
  nResult = 0;
  ok_clicked = false;
}

//------------------------------------------------------------------------------
//: Destructor
vgui_mfc_dialog_impl::~vgui_mfc_dialog_impl()
{
}

//: Structure to contain data for a choice field.
struct vgui_mfc_dialog_choice
{
  vcl_vector<vcl_string> names;
  int index;
};


//------------------------------------------------------------------------------
//: Make a choice widget
void* vgui_mfc_dialog_impl::choice_field_widget(const char* /*txt*/,
                                                const vcl_vector<vcl_string>& labels, int& val) {

  vgui_mfc_dialog_choice *ch = new vgui_mfc_dialog_choice;
  ch->names = labels;
  ch->index = val;

  return (void*)ch;
}

//: Structure to contain data for a inline tableau.
struct vgui_mfc_dialog_inline_tab
{
  vgui_tableau_sptr tab;
  unsigned height;
  unsigned width;
};

//------------------------------------------------------------------------------
//: Make a tableau widget.
void* vgui_mfc_dialog_impl::inline_tableau_widget(const vgui_tableau_sptr tab,
                                                  unsigned width, unsigned height)
{
  vgui_mfc_dialog_inline_tab* tab_data = new vgui_mfc_dialog_inline_tab;
  tab_data->tab = tab;
  tab_data->height = height;
  tab_data->width =  width;
  TempsNewClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_OWNDC|CS_GLOBALCLASS);
  return (void*)tab_data;
}

static int loop_flag = 0;

//: Called by MFC when the user clicks the OK button.
void vgui_mfc_dialog_impl::OnOk()
{
  ASSERT(::IsWindow(m_hWnd));

  if (m_nFlags & (WF_MODALLOOP|WF_CONTINUEMODAL))
    EndModalLoop(nResult);

  ::EndDialog(m_hWnd, nResult);
  ok_clicked = true;
}

//: Called by MFC when the user clicks the cancel button.
void vgui_mfc_dialog_impl::OnCancel()
{
  ASSERT(::IsWindow(m_hWnd));

  if (m_nFlags & (WF_MODALLOOP|WF_CONTINUEMODAL))
    EndModalLoop(nResult);

  ::EndDialog(m_hWnd, nResult);
}

//: Called by MFC when the user clicks the (file) browse button.
//  Fires up File browser dialog box
void vgui_mfc_dialog_impl::OnBrowse(UINT uID)
{
  int which = uID-ID_BROWSE_FILES;
  ASSERT(which>=0 && which<100);
  vcl_cerr<<"File browser loading...";
  CFileDialog file_dialog(TRUE,"*.*",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "All Files (*.*)|*.*||", this);
  file_dialog.DoModal();
  CString s(file_dialog.GetPathName());
  fbsrs[which]->SetWindowText(s);
}

//: Called by MFC when the user clicks the colour chooser button.
//  Fires up Colour chooser dialog box
void vgui_mfc_dialog_impl::OnChooseColour(UINT uID)
{
  char buffer[20];
  int which = uID-ID_CHOOSE_COLOUR;
  ASSERT(which>=0 && which<100);
  vcl_cerr<<"File browser loading...";
  CColorDialog colour_dialog(0,0, this);
  colour_dialog.DoModal();
  COLORREF colour = colour_dialog.GetColor();
  vcl_sprintf(buffer,"%4.3f",float(colour&0xff)/255.0);
  CString s(buffer);
  s+=" ";
  vcl_sprintf(buffer,"%4.3f",float((colour>>8)&0xff)/255.0);
  s+=buffer;
  s+=" ";
  vcl_sprintf(buffer,"%4.3f",float((colour>>16)&0xff)/255.0);
  s+=buffer;
  csrs[which]->SetWindowText(s);
}

//: Called by MFC when the application is about to terminate.
void vgui_mfc_dialog_impl::OnClose()
{
  OnCancel();
}

//: Display the dialog box.
bool vgui_mfc_dialog_impl::ask()
{
  // Get the pointer to the main window
  CWnd *main_window = AfxGetApp()->GetMainWnd();

  // Find out the size of the dialog box needed
  int width, max_length = 0,fbsr_count = 0;
  int height = 45 + 6*8;

  for (vcl_vector<element>::iterator e_iter1 = elements.begin();
       e_iter1 != elements.end(); ++e_iter1)
  {
    element l = *e_iter1;
    vgui_dialog_field *field = l.field;

    if (l.type == bool_elem)
    {
      vgui_bool_field *field = static_cast<vgui_bool_field*>(l.field);
      int field_length = vcl_strlen(field->label.c_str());
      if (max_length<field_length)
        max_length = field_length;
      height += 45;
    }
    else if (l.type == inline_tabl)
    {
      vgui_mfc_dialog_inline_tab* tab_data = (vgui_mfc_dialog_inline_tab*)l.widget;
      if (max_length < int(tab_data->width/8 + 5))
        max_length = int((7+tab_data->width)/8) + 5;
      height += int(tab_data->height) + 20;
    }
    else if (l.type == text_msg)
    {
      vgui_int_field *field = static_cast<vgui_int_field*>(l.field);
      if (max_length<int(field->label.size()+field->current_value().size()))
        max_length = field->label.size()+field->current_value().size();
      height += 45;
    }
    else // Remaining types are text with user input boxes:
    {
      // Add 40 extra characters to the length to leave space for
      // the user response box:
      int field_length = vcl_strlen(field->label.c_str()) + 40;
      if (max_length<field_length)
        max_length = field_length;
      height += 45;
    }

    if (l.type == file_bsr || l.type == inline_file_bsr ||
        l.type == color_csr || l.type == inline_color_csr)
    {
      fbsr_count++;
      height += 45;
    }
  }

  // Width of the dialog box is approx 8 times the number of characters:
  width = 8*max_length;
  // If the width is too small to contain the buttons then make it bigger:
  if (width < 200)
    width = 200;
  max_length = max_length - 40;
  //height = 45*(elements.size()+fbsr_count+1)+6*8;
  fbsr_count++;
  // Create dialog box window
  //width=height=600;
  CreateEx(WS_EX_CONTROLPARENT,
           AfxRegisterWndClass(0,::LoadCursor(NULL, IDC_ARROW),(HBRUSH)(COLOR_WINDOW)),
           _T(title.c_str()),
           WS_CAPTION|WS_VISIBLE|WS_SYSMENU|WS_POPUP|DS_MODALFRAME,
           100, 100, width, height, NULL, NULL, 0);
  UpdateWindow();
  ShowWindow(SW_SHOW);

  // determine default font for document
  vcl_memset(&m_logfont, 0, sizeof m_logfont);
  m_logfont.lfHeight = -8;
  lstrcpy(m_logfont.lfFaceName, _T("Microsoft Sans Serif Regular"));
  m_logfont.lfOutPrecision = OUT_TT_PRECIS;
  m_logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
  m_logfont.lfQuality = PROOF_QUALITY;
  m_logfont.lfPitchAndFamily = FF_SWISS | VARIABLE_PITCH;
  m_logfont.lfHeight = -::MulDiv(-m_logfont.lfHeight,GetDC()->GetDeviceCaps(LOGPIXELSY), 72);
  font = new CFont();
  font->CreateFontIndirect(&m_logfont);
  SetFont(font);

  // Attach basic buttons "OK" and "Cancel" at the bottom of the dialog

  // Ok button
  CButton* accept = 0;
  int right_of_ok_button;
  if (ok_button_text_.size() > 0)
  {
    CRect r;
    r.left = width-2*10*8-3*8;
    r.right = r.left+10*8;
    r.top = height-9*8;
    r.bottom = height-5*8;
    right_of_ok_button = r.right;
    accept = new CButton();
    accept->Create(_T(ok_button_text_.c_str()),
                   WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_PUSHBUTTON,
                   r,this,IDOK);
    accept->SetFocus();
    accept->SetFont(font);
  }

  // Cancel button
  CButton* cancel = 0;
  if (cancel_button_text_.size() > 0)
  {
    CRect r;
    r.left = right_of_ok_button+1*8;
    r.right = r.left+10*8;
    r.top = height-9*8;
    r.bottom = height-5*8;
    cancel = new CButton();
    cancel->Create(_T(cancel_button_text_.c_str()),
                   WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_PUSHBUTTON,
                   r,this,IDCANCEL);
    cancel->SetFont(font);
  }

  // Add elements from top to bottom
  CRect r;
  r.left = 999; // set later
  r.right = 999; //
  r.top = 0;
  r.bottom = 3*8+2;
  for (vcl_vector<element>::iterator e_iter2 = elements.begin();
       e_iter2 != elements.end(); ++e_iter2)
  {
    element l = *e_iter2;
    vgui_dialog_field *field = l.field;

    if (l.type == int_elem ||
        l.type == long_elem ||
        l.type == float_elem ||
        l.type == double_elem ||
        l.type == string_elem)
    {
      // Hard coded coordinates
      r.left = 2*4;
      r.top+=4*8;
      r.bottom+=4*8;
      r.right = r.left+field->label.size()*8;
      // Set static text first
      CStatic *text = new CStatic();
      text->Create(_T(field->label.c_str()),WS_CHILD|WS_VISIBLE|SS_LEFT,r,this);
      text->SetFont(font);
      awlist.push_back(text);
      // Now set the line editor next
      CEdit *edit = new CEdit();
      r.left = width-2*8-20*8;
      r.left = 2*4+max_length*8+2*8;
      r.right = width-2*8;//r.left+20*8;

      // CEdit::Create does not support extended window styles
      //MFC impl:Create(_T("EDIT"), NULL, dwStyle, rect, pParentWnd, nID);
      // So we use CWnd::CreateEx. Note that the class name is EDIT
      edit->CreateEx(WS_EX_CLIENTEDGE,_T("EDIT"),NULL,
                     WS_CHILD|WS_BORDER|WS_TABSTOP|ES_LEFT|ES_AUTOHSCROLL,r,this,IDOK);
      edit->SetFont(font);
      edit->SetWindowText(l.field->current_value().c_str());
      edit->UpdateWindow();
      edit->ShowWindow(SW_SHOW);
      text->UpdateWindow();
      text->ShowWindow(SW_SHOW);
      awlist.push_back(edit);
      wlist.push_back(edit);
    }
    else if (l.type == bool_elem)
    {
      r.left = 2*4;
      r.top+=5*8;
      r.bottom+=5*8;
      r.right = r.left+(field->label.size()+3)*8;
      vgui_bool_field *field = static_cast<vgui_bool_field*>(l.field);
      CButton *checkbox = new CButton();
      checkbox->Create(_T(field->label.c_str()),
                          WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_AUTOCHECKBOX, r,this,4);
      checkbox->SetFont(font);
      checkbox->SetCheck(field->var); // Make sure checkbox displays current bool value.
      checkbox->UpdateWindow();
      checkbox->ShowWindow(SW_SHOW);
      awlist.push_back(checkbox);
      wlist.push_back(checkbox);
    }
    else if (l.type == choice_elem)
    {
      vgui_int_field *field = static_cast<vgui_int_field*>(l.field);
      r.left = 2*4;
      r.top+=4*8;
      r.bottom+=4*8;
      r.right = r.left+field->label.size()*8;
      // Set static text first
      CStatic *text = new CStatic();
      text->Create(_T(field->label.c_str()),WS_CHILD|WS_VISIBLE|SS_LEFT,r,this);
      text->SetFont(font);
      awlist.push_back(text);

      r.left = width-2*8-20*8;
      r.left = 2*4+max_length*8+2*8;
      r.right = width-2*8;//r.left+20*8;
      vgui_mfc_dialog_choice *ch = (vgui_mfc_dialog_choice*)l.widget;
      r.bottom+=__min(ch->names.size(),4)*32;
      CComboBox *combobox = new CComboBox();
      combobox->CreateEx(WS_EX_CLIENTEDGE,_T("COMBOBOX"),NULL,
                         WS_CHILD|WS_BORDER|CBS_DROPDOWNLIST|WS_VSCROLL,r,this,4);
      combobox->SetFont(font);
      r.bottom-=__min(ch->names.size(),4)*32;

      int count = 0;
      for (vcl_vector<vcl_string>::iterator s_iter =  ch->names.begin();
           s_iter != ch->names.end(); ++s_iter, ++count)
        combobox->AddString(_T(s_iter->c_str()));

      combobox->SetCurSel(0);
      combobox->UpdateWindow();
      combobox->ShowWindow(SW_SHOW);
      awlist.push_back(combobox);
      wlist.push_back(combobox);
    }
    else if (l.type == text_msg)
    {
      r.left = 2*4;
      r.top+=5*8;
      r.bottom+=5*8;
      r.right = r.left+field->label.size()*8;

      CStatic *text = new CStatic();
      text->Create(_T(field->label.c_str()),WS_CHILD|WS_VISIBLE|SS_LEFT,r,this);
      text->SetFont(font);
      awlist.push_back(text);
      wlist.push_back(text);
    }
    else if (l.type == file_bsr || l.type == inline_file_bsr)
    {
      r.left = 2*4;
      r.top+=4*8;
      r.bottom+=4*8;
      r.right = r.left+field->label.size()*8;

      CStatic *text = new CStatic();
      text->Create(_T(field->label.c_str()),WS_CHILD|WS_VISIBLE|SS_LEFT,r,this);
      text->SetFont(font);
      awlist.push_back(text);

      // Now set the line editor next
      CEdit *edit = new CEdit();
      int savey = r.top;
      r.left  = r.right+3*8;
      r.left = 2*4+max_length*8+2*8;
      r.right = width-2*8;//r.left+l.field->current_value().size()*8;

      // CEdit::Create does not support extended window styles
      //MFC impl:Create(_T("EDIT"), NULL, dwStyle, rect, pParentWnd, nID);
      // So we use CWnd::CreateEx. Note that the class name is EDIT
      edit->CreateEx(WS_EX_CLIENTEDGE,_T("EDIT"),NULL,
                     WS_CHILD|WS_BORDER|ES_LEFT|ES_AUTOHSCROLL,r,this,IDOK);
      edit->SetFont(font);
      edit->SetWindowText(l.field->current_value().c_str());
      edit->UpdateWindow();
      edit->ShowWindow(SW_SHOW);
      CButton *button = new CButton();

      r.left = width-2*8-10*8;//r.right+3*8;
      r.right = width-2*8;//r.left+10*8;
      r.top+=4*8;
      r.bottom+=4*8;
      button->Create(_T("Browse..."),WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_PUSHBUTTON,
                     r,this,ID_BROWSE_FILES+count_fbsr);
      fbsrs[count_fbsr++] = edit;

      button->SetFont(font);
      button->UpdateWindow();
      button->ShowWindow(SW_SHOW);
      awlist.push_back(edit);
      awlist.push_back(button);
      wlist.push_back(edit);
    }
    else if (l.type == color_csr || l.type == inline_color_csr)
    {
      r.left = 2*4;
      r.top+=4*8;
      r.bottom+=4*8;
      r.right = r.left+field->label.size()*8;

      CStatic *text = new CStatic();
      text->Create(_T(field->label.c_str()),WS_CHILD|WS_VISIBLE|SS_LEFT,r,this);
      text->SetFont(font);
      awlist.push_back(text);

      // Now set the line editor next
      CEdit *edit = new CEdit();
      r.left  = r.right+3*8;
      r.left = 2*4+max_length*8+2*8;
      r.right = width-2*8;//r.left+(l.field->current_value().size()+2)*8;
      // CEdit::Create does not support extended window styles
      //MFC impl:Create(_T("EDIT"), NULL, dwStyle, rect, pParentWnd, nID);
      // So we use CWnd::CreateEx. Note that the class name is EDIT
      edit->CreateEx(WS_EX_CLIENTEDGE,_T("EDIT"),NULL,
                     WS_CHILD|WS_BORDER|ES_LEFT|ES_AUTOHSCROLL,r,this,IDOK);
      edit->SetFont(font);
      edit->SetWindowText(l.field->current_value().c_str());
      edit->UpdateWindow();
      edit->ShowWindow(SW_SHOW);
      CButton *button = new CButton();

      r.left = width-2*8-10*8;//r.right+3*8;
      r.right = width-2*8;//r.left+10*8;
      r.top+=4*8;
      r.bottom+=4*8;
      button->Create(_T("Colour..."),WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_PUSHBUTTON,
                     r,this,ID_CHOOSE_COLOUR+count_csr);
      csrs[count_csr++] = edit;
      button->SetFont(font);
      button->UpdateWindow();
      button->ShowWindow(SW_SHOW);
      awlist.push_back(edit);
      awlist.push_back(button);
      wlist.push_back(edit);
    }
    else if (l.type == inline_tabl)
    {
      vgui_mfc_dialog_inline_tab* tab_data
        = (vgui_mfc_dialog_inline_tab*)l.widget;
      vgui_mfc_adaptor *widg = new vgui_mfc_adaptor();
      widg->set_tableau(tab_data->tab);
      // because this adaptor is not in the main window we need to call setup_adaptor:
      widg->setup_adaptor(this, wglGetCurrentDC(), wglGetCurrentContext());
      r.left=8;
      r.right=r.left + long(tab_data->width);
      r.top += 5;
      r.bottom += r.top + long(tab_data->height);
      widg->CreateEx(WS_EX_CLIENTEDGE,TempsNewClass,NULL,
                     WS_CHILD|WS_BORDER|ES_LEFT|ES_AUTOHSCROLL,r,this,IDOK);

      widg->ShowWindow(SW_SHOW);
      awlist.push_back(widg);
      wlist.push_back(widg);
      delete tab_data;
    }
  }
  if (accept)
  {
    accept->UpdateWindow();
    accept->ShowWindow(SW_SHOW);
  }
  if (cancel)
  {
    cancel->UpdateWindow();
    cancel->ShowWindow(SW_SHOW);
  }
  AfxGetApp()->EnableModeless(FALSE);
  AfxGetApp()->GetMainWnd()->EnableWindow(FALSE);
  nResult = RunModalLoop(MLF_SHOWONIDLE);
  if (m_hWnd != NULL)
    SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|
                 SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
  if (ok_clicked)
  {
    vcl_vector<CWnd *>::iterator w_iter = wlist.begin();
    for (vcl_vector<element>::iterator e_iter3 = elements.begin();
         e_iter3 != elements.end(); ++e_iter3, ++w_iter)
    {
      element l = *e_iter3;
      CWnd *input = *w_iter;

      if (l.type == int_elem ||
          l.type == long_elem ||
          l.type == float_elem ||
          l.type == double_elem ||
          l.type == string_elem ||
          l.type == file_bsr ||
          l.type == color_csr ||
          l.type == inline_color_csr ||
          l.type == inline_file_bsr)
      {
        CString s;
        input->GetWindowText(s);
        l.field->update_value((LPCSTR)s);
      }
      else if (l.type == bool_elem)
      {
        vgui_bool_field *field = static_cast<vgui_bool_field*>(l.field);
        field->var =((CButton *)input)->GetCheck()!=0;
      }
      if (l.type == choice_elem)
      {
        vgui_int_field *field = static_cast<vgui_int_field*>(l.field);
        field->var = ((CComboBox *)input)->GetCurSel();
      }
    }
  }
  // Remove all the created objects from the heap
  for (vcl_vector<CWnd *>::iterator w_iter = awlist.begin();w_iter!=awlist.end();++w_iter)
    delete *w_iter;
  delete accept;
  delete cancel;
  delete font;
  DestroyWindow();
  // Enable the parent window
  AfxGetApp()->EnableModeless(TRUE);
  AfxGetApp()->GetMainWnd()->EnableWindow(TRUE);
  main_window->SetActiveWindow();
  return ok_clicked;
}
