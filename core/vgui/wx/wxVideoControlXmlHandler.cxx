// This is core/vgui/wx/wxVideoControlXmlHandler.cxx

#include "wxVideoControlXmlHandler.h"

IMPLEMENT_DYNAMIC_CLASS(wxVideoControlXmlHandler, wxXmlResourceHandler)

wxVideoControlXmlHandler::wxVideoControlXmlHandler()
: wxXmlResourceHandler()
{
}

wxObject *wxVideoControlXmlHandler::DoCreateResource()
{
  wxVideoControl* panel =
      new wxVideoControl(m_parentAsWindow,
                         GetID(),
                         GetPosition(), GetSize(),
                         GetStyle(),
                         GetName());

  return panel;
}


bool wxVideoControlXmlHandler::CanHandle(wxXmlNode *node)
{
  return IsOfClass(node, wxT("wxVideoControl"));
}
