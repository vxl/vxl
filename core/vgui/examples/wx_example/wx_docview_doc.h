// This is core/vgui/examples/wx_example/wx_docview_doc.h
#ifndef wx_docview_doc_h_
#define wx_docview_doc_h_
//=========================================================================
//:
// \file
// \brief  wxWidgets document class.
// \author Miguel A. Figueroa-Villanueva (miguelfv)
//
// Contains class wx_docview_doc, which holds the vgui main tableau.
//
// \verbatim
//  Modifications
//   03/19/2006 - File created. (miguelfv)
// \endverbatim
//=========================================================================

#include <wx/docview.h>

#include <vgui/vgui_tableau_sptr.h>

//-------------------------------------------------------------------------
//: The wxWidgets document class.
//-------------------------------------------------------------------------
class wx_docview_doc : public wxDocument
{
  DECLARE_DYNAMIC_CLASS(wx_docview_doc)

 public:
  wx_docview_doc();
  virtual ~wx_docview_doc() { }

  virtual bool OnCreate(const wxString& path, long flags);
  
  vgui_tableau_sptr tableau() { return tableau_; }

 private:
  vgui_tableau_sptr tableau_;
};

#endif // wx_docview_doc_h_
