//:
//  \file

/*
  fsm
*/
#include <vcl_iostream.h>

#include <vul/vul_arg.h>

#include <vil/vil_image.h>
#include <vil/vil_load.h>

#include <osl/osl_easy_canny.h>
#include <osl/osl_load_topology.h>

#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_section_buffer.h>

void draw(osl_edge const &e)
{
  glBegin(GL_POINTS);
  glVertex2f(e.GetV1()->GetY(), e.GetV1()->GetX());
  glVertex2f(e.GetV2()->GetY(), e.GetV2()->GetX());
  glEnd();

  glBegin(GL_LINE_STRIP);
  for (int j=0; j<e.size(); ++j)
    glVertex2f(e.GetY(j), e.GetX(j));
  if (e.GetV1() == e.GetV2())
    glVertex2f(e.GetY(0), e.GetX(0));
  glEnd();
}

//: renders a canny segmentation on top of the given image.
struct example_tab_canny : vgui_tableau
{
  int canny;
  vil_image image;
  vcl_string topofile;
  vcl_list<osl_edge*> edges;
  vgui_section_buffer *section;

  example_tab_canny(vil_image const &image_, int canny_, vcl_string const &topofile_)
    : canny(canny_)
    , image(image_)
    , topofile(topofile_)
    , section(0) // don't create the section_buffer till the GL context is up.
    { }

  ~example_tab_canny() {
    for (vcl_list<osl_edge*>::iterator e=edges.begin(); e!=edges.end(); ++e)
      (*e)->unref();
    edges.clear();

    delete section;
    section = 0;
  }

  void init();
  bool handle(vgui_event const &e) {
    if (e.type == vgui_DRAW) {
      init();

      section->draw_as_image();

      glColor3f(1,1,1);
      glPointSize(4);
      glLineWidth(1);
      for (vcl_list<osl_edge*>::const_iterator i=edges.begin(); i!=edges.end(); ++i)
        ::draw(**i);

      return true;
    }
    else
      return false;
  }
};

void example_tab_canny::init() {
  static bool once = false;
  if (once)
    return;

  section = new vgui_section_buffer(0, 0, image.width(), image.height());
  section->apply(image);

  if (topofile == "")
    osl_easy_canny(canny, image, &edges);
  else {
    vcl_list<osl_Vertex*> dummy;
    osl_load_topology(topofile.c_str(), edges, dummy);
  }
  once = true;
}


int main(int argc, char **argv)
{
  vgui::init(argc, argv);
  vul_arg<int> canny("-canny", "which canny? (0:oxford, 1:rothwell1, 2:rothwell2)", 0);
  vul_arg_parse(argc, argv);

  char const *i_file = argv[1] ? argv[1] : "az32_10.tif";
  char const *t_file = (argv[1] && argv[2]) ? argv[2] : "";
  vil_image image = vil_load(i_file);
  if (!image)
    return 1;
  vcl_cerr << i_file << ": " << image << vcl_endl;

  vgui_tableau_sptr tab = new example_tab_canny(image, canny(), t_file);
  vgui_viewer2D_new zoom(tab);
  return vgui::run(zoom, image.width(), image.height());
}
