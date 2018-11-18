// This is core/vgui/wx/wxVideoControlXmlHandler.h
#ifndef wxVideoControlXmlHandler_h_
#define wxVideoControlXmlHandler_h_
//:
// \file
// \brief  An XML Resource Handler for wxVideoControl
// \author Matt Leotta
// \date   April 10, 2009
//



#include <wx/xrc/xmlres.h>
#include <vgui/wx/wxVideoControl.h>


//: XML Resource Handler for generating a wxVideoControl
class WXDLLIMPEXP_XRC wxVideoControlXmlHandler : public wxXmlResourceHandler
{
  DECLARE_DYNAMIC_CLASS(wxVideoControlXmlHandler)

public:
  wxVideoControlXmlHandler();
  virtual wxObject *DoCreateResource();
  virtual bool CanHandle(wxXmlNode *node);
};


#endif  //wxVideoControlXmlHandler_h_
