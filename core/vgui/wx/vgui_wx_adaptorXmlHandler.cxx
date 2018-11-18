// This is core/vgui/wx/vgui_wx_adaptorXmlHandler.cxx

#include "vgui_wx_adaptorXmlHandler.h"


IMPLEMENT_DYNAMIC_CLASS(vgui_wx_adaptorXmlHandler,  wxXmlResourceHandler)

vgui_wx_adaptorXmlHandler::vgui_wx_adaptorXmlHandler()
: wxXmlResourceHandler()
{
}

wxObject *vgui_wx_adaptorXmlHandler::DoCreateResource()
{
  vgui_wx_adaptor* adaptor =
      new vgui_wx_adaptor(m_parentAsWindow,
                          GetID(),
                          GetPosition(), GetSize(),
                          GetStyle(),
                          GetName() );

  return adaptor;
}



bool vgui_wx_adaptorXmlHandler::CanHandle(wxXmlNode *node)
{
  return IsOfClass(node, wxT("vgui_wx_adaptor"));
}
