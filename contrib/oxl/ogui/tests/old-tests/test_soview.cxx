#include <vcl_iostream.h>
#include <vgui/vgui_soview.h>

int main(int /*argc*/, char ** /*argv*/)
{
  vgui_soview2D_point point(10,10);
  vcl_cerr << "point id " << point.get_id() << vcl_endl;
  vgui_soview2D_lineseg line(50,50,100,100);
  vcl_cerr << "line id " << line.get_id() << vcl_endl;

  vgui_soview *p = vgui_soview::id_to_object(point.get_id());
  vgui_soview *l = vgui_soview::id_to_object(line.get_id());


  vcl_cerr << "point id " << p->get_id() << vcl_endl;
  p->print(vcl_cerr);
  vcl_cerr << vcl_endl;

  vcl_cerr << "line id " << l->get_id() << vcl_endl;
  l->print(vcl_cerr);
  vcl_cerr << vcl_endl;

  return 0;
}
