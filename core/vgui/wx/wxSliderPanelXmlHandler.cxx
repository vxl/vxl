// This is core/vgui/wx/wxSliderPanelXmlHandler.cxx

#include "wxSliderPanelXmlHandler.h"

IMPLEMENT_DYNAMIC_CLASS(wxSliderPanelXmlHandler, wxXmlResourceHandler)

wxSliderPanelXmlHandler::wxSliderPanelXmlHandler()
: wxXmlResourceHandler()
{
}

wxObject *wxSliderPanelXmlHandler::DoCreateResource()
{
  long base_id = GetLong(wxT("base_id"),10100);

  wxSliderPanel* panel =
      new wxSliderPanel(m_parentAsWindow,
                        GetID(),
                        base_id,
                        GetPosition(), GetSize(),
                        GetStyle(),
                        GetName());

  // TODO: Load initial sliders from the XML File
  //std::vector<double> min_vals, max_vals, init_vals;
  //panel->CreateSliders(init_vals,min_vals,max_vals);

  return panel;
}


bool wxSliderPanelXmlHandler::CanHandle(wxXmlNode *node)
{
  return IsOfClass(node, wxT("wxSliderPanel"));
}
