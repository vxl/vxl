#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_style.h>
#include <vcl_iostream.h>

int
main()
{
  vgui_easy2D_tableau_new tab1;
  tab1->set_foreground( 1.0f, 1.0f, 1.0f );

  vgui_easy2D_tableau_new tab2;
  tab2->set_foreground( 1.0f, 1.0f, 1.0f );

  vgui_soview* line1 = tab1->add_line( 0.0f, 0.0f, 1.0f, 1.0f );
  vgui_soview* line2 = tab2->add_line( 0.0f, 0.0f, 1.0f, 1.0f );

  vcl_cout << "Try to retrieve style 1" << vcl_endl;
  if( line1->get_style()->rgba[0] != 1.0f ) {
    vcl_cout << "ERROR: Red != 1.0" << vcl_endl;
    return 1;
  } else {
    vcl_cout << "Style appears to be correct for line 1" << vcl_endl;
  }
  vcl_cout << "Try to retrieve style 2" << vcl_endl;
  if( line2->get_style()->rgba[0] != 1.0f ) {
    vcl_cout << "ERROR: Red != 1.0" << vcl_endl;
    return 1;
  } else {
    vcl_cout << "Style appears to be correct for line 2" << vcl_endl;
  }

  vcl_cout << "Clear tableau 1" << vcl_endl;
  tab1->clear();

  // this should create a new style object
  tab1->set_foreground( 0.0f, 0.0f, 0.0f );
  vgui_soview* line3 = tab1->add_line( 0.0f, 0.0f, 1.0f, 1.0f );

  vcl_cout << "Try to retrieve style 2 again" << vcl_endl;
  if( line2->get_style()->rgba[0] != 1.0f ) {
    vcl_cout << "ERROR: Red != 1.0 (is "<<line2->get_style()->rgba[0]<<")" << vcl_endl;
    return 1;
  } else {
    vcl_cout << "Style appears to be correct for line 2" << vcl_endl;
  }

  vcl_cout << "Try to retrieve style 3" << vcl_endl;
  if( line3->get_style()->rgba[0] != 0.0f ) {
    vcl_cout << "ERROR: Red != 0.0 (is "<<line3->get_style()->rgba[0]<<")" << vcl_endl;
    return 1;
  } else {
    vcl_cout << "Style appears to be correct for line 3" << vcl_endl;
  }

  return 0;
}
