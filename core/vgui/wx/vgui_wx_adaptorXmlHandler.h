// This is core/vgui/wx/vgui_wx_adaptorXmlHandler.h
#ifndef vgui_wx_adaptorXmlHandler_h_
#define vgui_wx_adaptorXmlHandler_h_
//:
// \file
// \brief  An XML Resource Handler for vgui_wx_adaptor
// \author Matt Leotta
// \date   August 6, 2008
//



#include <wx/xrc/xmlres.h>
#include <vgui/impl/wx/vgui_wx_adaptor.h>


//: XML Resource Handler for generating a vgui_wx_adaptor
class WXDLLIMPEXP_XRC vgui_wx_adaptorXmlHandler : public wxXmlResourceHandler
{
  DECLARE_DYNAMIC_CLASS(vgui_wx_adaptorXmlHandler)

public:
  vgui_wx_adaptorXmlHandler();
  virtual wxObject *DoCreateResource();
  virtual bool CanHandle(wxXmlNode *node);
};


#endif  //vgui_wx_adaptorXmlHandler_h_
