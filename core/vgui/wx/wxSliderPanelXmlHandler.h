// This is core/vgui/wx/wxSliderPanelXmlHandler.h
#ifndef wxSliderPanelXmlHandler_h_
#define wxSliderPanelXmlHandler_h_
//:
// \file
// \brief  An XML Resource Handler for wxSliderPanel
// \author Matt Leotta
// \date   August 6, 2008
//



#include <wx/xrc/xmlres.h>
#include <vgui/wx/wxSliderPanel.h>


//: XML Resource Handler for generating a wxSliderPanel
class WXDLLIMPEXP_XRC wxSliderPanelXmlHandler : public wxXmlResourceHandler
{
  DECLARE_DYNAMIC_CLASS(wxSliderPanelXmlHandler)

public:
  wxSliderPanelXmlHandler();
  virtual wxObject *DoCreateResource();
  virtual bool CanHandle(wxXmlNode *node);
};


#endif  //wxSliderPanelXmlHandler_h_
