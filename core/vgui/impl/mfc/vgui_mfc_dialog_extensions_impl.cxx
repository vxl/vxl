// This is core/vgui/impl/mfc/vgui_mfc_dialog_extensions_impl.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Gamze Tunali, LEMS, Brown University 
// \date   16 Nov 2007
//
// See vgui_mfc_dialog_extensions_impl.h for a description of this file.

#include "vgui_mfc_dialog_extensions_impl.h"

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cstdio.h> // for sprintf()
#include <vcl_cstring.h>

#include <vgui/internals/vgui_dialog_field.h>
#include <vgui/internals/vgui_simple_field.h>
#include <vgui/impl/mfc/vgui_mfc_adaptor.h>
#include <vgui/impl/mfc/vgui_dir_dialog.h>
#include <winuser.h>

static bool debug = false;

CString TempsNewClass2;
BEGIN_MESSAGE_MAP(vgui_mfc_dialog_extensions_impl, CWnd)
        ON_COMMAND(IDOK,OnOk)
        ON_COMMAND(IDCANCEL,OnCancel)
        ON_WM_CLOSE()
        ON_CONTROL_RANGE(BN_CLICKED,ID_BROWSE_FILES,ID_BROWSE_FILES+100, OnBrowse)
        ON_CONTROL_RANGE(BN_CLICKED,ID_BROWSE_DIRS,ID_BROWSE_DIRS+100, OnBrowseDir)
        ON_CONTROL_RANGE(BN_CLICKED,ID_CHOOSE_COLOUR,ID_CHOOSE_COLOUR+100,OnChooseColour)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------
//: Constructor
vgui_mfc_dialog_extensions_impl::vgui_mfc_dialog_extensions_impl(const char* name)
  : CWnd(),vgui_dialog_extensions_impl(name)
{
  // Set some default parameters
  count_fbsr = 0;
  count_csr = 0;
  count_dbsr = 0;
  nResult = 0;
  ok_clicked = false;
}

//------------------------------------------------------------------------------
//: Destructor
vgui_mfc_dialog_extensions_impl::~vgui_mfc_dialog_extensions_impl()
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
void* vgui_mfc_dialog_extensions_impl::choice_field_widget(const char* /*txt*/,
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
void* vgui_mfc_dialog_extensions_impl::inline_tableau_widget(const vgui_tableau_sptr tab,
                                                  unsigned width, unsigned height)
{
  vgui_mfc_dialog_inline_tab* tab_data = new vgui_mfc_dialog_inline_tab;
  tab_data->tab = tab;
  tab_data->height = height;
  tab_data->width =  width;
  TempsNewClass2 = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_OWNDC|CS_GLOBALCLASS);
  return (void*)tab_data;
}

#if 0 // is_modal is not used anywhere
//: Sets the modality of the dialog box.
//  True makes the dialog modal (i.e. the dialog 'grabs' all events), this is
//  the default.  False makes the dialog non-modal.  WARNING: It is dangerous to
//  make a dialog that changes data non-modal, only messages should be non-modal.
static bool is_modal = true;
void vgui_mfc_dialog_extensions_impl::modal(bool m)
{
  is_modal = m;
}
#endif // 0

//: Called by MFC when the user clicks the OK button.
void vgui_mfc_dialog_extensions_impl::OnOk()
{
  ASSERT(::IsWindow(m_hWnd));

  if (m_nFlags & (WF_MODALLOOP|WF_CONTINUEMODAL))
    EndModalLoop(nResult);

  ::EndDialog(m_hWnd, nResult);
  ok_clicked = true;
}

//: Called by MFC when the user clicks the cancel button.
void vgui_mfc_dialog_extensions_impl::OnCancel()
{
  ASSERT(::IsWindow(m_hWnd));

  if (m_nFlags & (WF_MODALLOOP|WF_CONTINUEMODAL))
    EndModalLoop(nResult);

  ::EndDialog(m_hWnd, nResult);
  ok_clicked = false;
}

//: Called by MFC when the user clicks the (file) browse button.
//  Fires up File browser dialog box
void vgui_mfc_dialog_extensions_impl::OnBrowse(UINT uID)
{
  int which = uID-ID_BROWSE_FILES;
   ASSERT(which>=0 && which<100);
   vcl_cerr<<"File browser loading...";
   CFileDialog file_dialog(TRUE,"*.*",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "All Files (*.*)|*.*||", this);
   file_dialog.DoModal();
   CString s(file_dialog.GetPathName());
   fbsrs[which]->SetWindowText(s);  
}

//: Called by MFC when the user clicks the (directory) browse button.
//  Fires up directory browser dialog box
void vgui_mfc_dialog_extensions_impl::OnBrowseDir(UINT uID)
{
  int which = uID-ID_BROWSE_DIRS;
  ASSERT(which>=0 && which<100);
  vcl_cerr<<"Directory browser loading...";
  vgui_dir_dialog dir_dialog(NULL,"All Files |*.*|", this);
  if (dir_dialog.DoModal() == IDOK) {
    CString s(dir_dialog.GetPath());
    s.TrimRight('\\');
    dbsrs[which]->SetWindowText(s);
  }
}

//: Called by MFC when the user clicks the colour chooser button.
//  Fires up Colour chooser dialog box
void vgui_mfc_dialog_extensions_impl::OnChooseColour(UINT uID)
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
void vgui_mfc_dialog_extensions_impl::OnClose()
{
  OnCancel();
}

//: Display the dialog in a user formatted box. 
// Line breaks are used to seperate lines of elements
bool vgui_mfc_dialog_extensions_impl::ask()
{
  // Get the pointer to the main window
  CWnd *main_window = AfxGetApp()->GetMainWnd();

  // Find out the size of the dialog box needed
  int width=32, max_length = 0,fbsr_count = 0, dbsr_count=0;
  int height = 40 + 6*8;

  for (vcl_vector<element>::iterator e_iter1 = elements.begin();
       e_iter1 != elements.end(); ++e_iter1)
  {
    element l = *e_iter1;
    vgui_dialog_field *field = l.field;

    if (l.type == bool_elem)
    {
      vgui_bool_field *field = static_cast<vgui_bool_field*>(l.field);
      int field_length = vcl_strlen(field->label.c_str());
      width += 24 + (field_length+3)* 8;
    }
    else if (l.type == inline_tabl)
    {
      vgui_mfc_dialog_inline_tab* tab_data = (vgui_mfc_dialog_inline_tab*)l.widget;
      //if (max_length < int(tab_data->width/8 + 5))
       // max_length = int((7+tab_data->width)/8) + 5;
      height += int((tab_data->height) + 20); //??
      width += (int(7+tab_data->width) + 5)*8;
    }
    else if (l.type == text_msg)
    {
      vgui_int_field *field = static_cast<vgui_int_field*>(l.field);
      //if (max_length<int(field->label.size()+field->current_value().size()))
      //  max_length = field->label.size()+field->current_value().size();
      width += 24 + field->label.size()*8;
    } 
    
    else if (l.type == line_br) {
      width += 32;
      if (max_length< width)
        max_length = width;
      width = 32;
      height += 40;
    }

    else // Remaining types are text with user input boxes:
    {
      // Add 40 extra characters to the length to leave space for
      // the user response box:
      int field_length = vcl_strlen(field->label.c_str()) + 20;
      //if (max_length<field_length)
      //  max_length = field_length;
      width += 24 + field_length*8 ;
    }

    if (l.type == file_bsr || l.type == inline_file_bsr ||
        l.type == color_csr || l.type == inline_color_csr)
    {
      fbsr_count++;
      width += 8 + (20+10)*8; // 20 extra for path entry, 20 was counted in text field
    } 
    else if (l.type == dir_bsr) {
      dbsr_count++;
      width += 8 + (20+10)*8;
    }
  }

  // Width of the dialog box is approx 8 times the number of characters:
  width = max_length;
  // If the width is too small to contain the buttons then make it bigger:
  if (width < 200)
    width = 200;
  fbsr_count++;

  // Create dialog box window
  CreateEx(WS_EX_CONTROLPARENT,
           AfxRegisterWndClass(0,::LoadCursor(NULL, IDC_ARROW),(HBRUSH)(COLOR_WINDOW)),
           _T(vgui_dialog_impl::name.c_str()),
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
    // center the buttons at the bottom
    r.left = (width -1)/2 - (ok_button_text_.size()+4)*8;
    r.right = r.left+(ok_button_text_.size()+4)*8;//r.left+10*8;
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
    r.right = r.left+(cancel_button_text_.size()+4)*8; //r.left+10*8;
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
  r.left = 0; // set later
  r.right = 999; //
  r.top = 32;
  r.bottom = 3*8+2+32;
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
      r.left += 3*8;
      r.right = r.left+field->label.size()*8;
      // Set static text first
      CStatic *text = new CStatic();
      text->Create(_T(field->label.c_str()),WS_CHILD|WS_VISIBLE|SS_LEFT,r,this);
      text->SetFont(font);
      awlist.push_back(text);
      // Now set the line editor next
      CEdit *edit = new CEdit();
      r.left = r.right+2*4;
      r.right = r.left+20*8;

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
      r.left = r.right;
    }
    else if (l.type == bool_elem)
    {
      r.left += 3*8;
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
      r.left = r.right;
    }
    else if (l.type == choice_elem)
    {
      vgui_int_field *field = static_cast<vgui_int_field*>(l.field);
      r.left += 3*8;
      r.right = r.left+field->label.size()*8;
      // Set static text first
      CStatic *text = new CStatic();
      text->Create(_T(field->label.c_str()),WS_CHILD|WS_VISIBLE|SS_LEFT,r,this);
      text->SetFont(font);
      awlist.push_back(text);

      r.left = r.right + 2*4;
      r.right = r.left+20*8; 
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

      combobox->SetCurSel(ch->index);
      combobox->UpdateWindow();
      combobox->ShowWindow(SW_SHOW);
      awlist.push_back(combobox);
      wlist.push_back(combobox);
      r.left = r.right;
    }
    else if (l.type == text_msg)
    {
      r.left += 3*8;
      r.right = r.left+field->label.size()*8;

      CStatic *text = new CStatic();
      text->Create(_T(field->label.c_str()),WS_CHILD|WS_VISIBLE|SS_LEFT,r,this);
      text->SetFont(font);
      awlist.push_back(text);
      wlist.push_back(text);
      r.left = r.right;
    }
    else if (l.type == file_bsr || l.type == inline_file_bsr)
    {
      r.left += 3*8;
      r.right = r.left+field->label.size()*8;

      CStatic *text = new CStatic();
      text->Create(_T(field->label.c_str()),WS_CHILD|WS_VISIBLE|SS_LEFT,r,this);
      text->SetFont(font);
      awlist.push_back(text);

      // Now set the line editor next
      CEdit *edit = new CEdit();
      int savey = r.top;
      r.left  = r.right+2*4;
      r.right = r.left+40*8; 

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

      r.left = r.right+2*4; 
      r.right = r.left+10*8; 
      button->Create(_T("Browse..."),WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_PUSHBUTTON,
                     r,this,ID_BROWSE_FILES+count_fbsr);
      fbsrs[count_fbsr++] = edit;

      button->SetFont(font);
      button->UpdateWindow();
      button->ShowWindow(SW_SHOW);
      awlist.push_back(edit);
      awlist.push_back(button);
      wlist.push_back(edit);
      r.left = r.right;
    }
    else if (l.type == color_csr || l.type == inline_color_csr)
    {
      r.left += 3*8;
      r.right = r.left+field->label.size()*8;

      CStatic *text = new CStatic();
      text->Create(_T(field->label.c_str()),WS_CHILD|WS_VISIBLE|SS_LEFT,r,this);
      text->SetFont(font);
      awlist.push_back(text);

      // Now set the line editor next
      CEdit *edit = new CEdit();
      r.left  = r.right+2*4;
      r.right = r.left + 10*8; //width-2*8;//r.left+(l.field->current_value().size()+2)*8;
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

      r.left = r.right+2*4;
      r.right = r.left+10*8;
      button->Create(_T("Colour..."),WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_PUSHBUTTON,
                     r,this,ID_CHOOSE_COLOUR+count_csr);
      csrs[count_csr++] = edit;
      button->SetFont(font);
      button->UpdateWindow();
      button->ShowWindow(SW_SHOW);
      awlist.push_back(edit);
      awlist.push_back(button);
      wlist.push_back(edit);
      r.left = r.right;
    }
    else if (l.type == dir_bsr)
    {
      r.left += 3*8;
      r.right = r.left+field->label.size()*8;

      CStatic *text = new CStatic();
      text->Create(_T(field->label.c_str()),WS_CHILD|WS_VISIBLE|SS_LEFT,r,this);
      text->SetFont(font);
      awlist.push_back(text);

      // Now set the line editor next
      CEdit *edit = new CEdit();
      int savey = r.top;
      r.left  = r.right+2*4;
      r.right = r.left+40*8; 

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

      r.left = r.right+2*4; 
      r.right = r.left+10*8; 
      button->Create(_T("Browse..."),WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_PUSHBUTTON,
                     r,this,ID_BROWSE_DIRS+count_dbsr);
      dbsrs[count_dbsr++] = edit;

      button->SetFont(font);
      button->UpdateWindow();
      button->ShowWindow(SW_SHOW);
      awlist.push_back(edit);
      awlist.push_back(button);
      wlist.push_back(edit);
      r.left = r.right;
    }
    else if (l.type == inline_tabl)
    {
      vgui_mfc_dialog_inline_tab* tab_data
        = (vgui_mfc_dialog_inline_tab*)l.widget;
      vgui_mfc_adaptor *widg = new vgui_mfc_adaptor();
      widg->set_tableau(tab_data->tab);
      // because this adaptor is not in the main window we need to call setup_adaptor:
      widg->setup_adaptor(this, wglGetCurrentDC(), wglGetCurrentContext());
      r.left+=3*8;
      r.right=r.left + long(tab_data->width);
      widg->CreateEx(WS_EX_CLIENTEDGE,TempsNewClass2,NULL,
                     WS_CHILD|WS_BORDER|ES_LEFT|ES_AUTOHSCROLL,r,this,IDOK);

      widg->ShowWindow(SW_SHOW);
      awlist.push_back(widg);
      wlist.push_back(widg);
      delete tab_data;
      r.left = r.right;
    } else if (l.type == line_br ) {
      // put a line break i.e. start a new line to put the elements
      r.top+=4*8;
      r.bottom+=4*8;
      r.left=0;
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
    int i=0, w=0;
    vcl_vector<CWnd *>::iterator w_iter = wlist.begin();
    for (vcl_vector<element>::iterator e_iter3 = elements.begin();
         e_iter3 != elements.end(); ++e_iter3)
    {
      element l = *e_iter3;

      // line break does not have a widget element, go on to the next element
      if (l.type == line_br)
        continue;   
      
      CWnd *input = *w_iter;

      if (l.type == int_elem ||
          l.type == long_elem ||
          l.type == float_elem ||
          l.type == double_elem ||
          l.type == string_elem ||
          l.type == file_bsr ||
          l.type == dir_bsr ||
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
      w_iter++;
    }
  }
  // Remove all the created objects from the heap
  for (vcl_vector<CWnd *>::iterator w_iter = awlist.begin();w_iter!=awlist.end();++w_iter)
    delete *w_iter;

  awlist.clear();
  wlist.clear();

  count_fbsr = 0;
  count_dbsr = 0;
  count_csr  = 0;

  delete accept;
  delete cancel;
  DestroyWindow();
  
  delete font;
  
  // Enable the parent window
  AfxGetApp()->EnableModeless(TRUE);
  AfxGetApp()->GetMainWnd()->EnableWindow(TRUE);
  main_window->SetActiveWindow();
  return ok_clicked;
}
