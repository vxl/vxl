// This is core/vgui/impl/wx/widgets/wxSliderPanelXmlHandler.cxx

#include "wxSliderPanelXmlHandler.h"

#include <vcl_iostream.h>


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
  
  vcl_vector<double> min_vals, max_vals, init_vals;
  min_vals.push_back(1.0);
  min_vals.push_back(0.5);
  max_vals.push_back(2.5);
  max_vals.push_back(1.7);
  init_vals.push_back(1.5);
  init_vals.push_back(1.0);
  panel->CreateSliders(init_vals,min_vals,max_vals);

  return panel;
}



bool wxSliderPanelXmlHandler::CanHandle(wxXmlNode *node)
{
  return IsOfClass(node, wxT("wxSliderPanel"));
}

