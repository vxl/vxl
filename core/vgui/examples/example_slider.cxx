// This example displays a line in an OpenGL area in the window:

#include <vgui/vgui.h> 
#include <vgui/vgui_slider_tableau.h> 
#include <vgui/vgui_poly_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_soview2D.h>

// This callback will change one of the coordinates of the line. The
// appropriate coordinate is specified in the data tag associated with
// the callback, so the implmentation of this callback is simple.
void change_number_cb( vgui_slider_tableau* tab, void* data )
{
  float* number = static_cast<float*>(data);
  *number = tab->value() * 2.0f - 1.0f;
}

int main(int argc, char **argv) 
{
  vgui::init(argc, argv);

  // Create an easy2D with two lines on it.
  vgui_easy2D_tableau_new easy_tab;

  easy_tab->set_line_width( 3 );
  easy_tab->set_foreground( 1.0f, 0.0f, 0.0f );
  vgui_soview2D_lineseg* line1 = easy_tab->add_line( 0, 0, 0, 0 );

  easy_tab->set_line_width( 1 );
  easy_tab->set_foreground( 1.0f, 1.0f, 0.0f );
  vgui_soview2D_lineseg* line2 = easy_tab->add_line( 0, 0, 0, 0 );

  // Create two sliders that will be used to update one of the
  // endpoints of each line.
  vgui_slider_tableau_new slider_h( vgui_slider_tableau::horiz );
  vgui_slider_tableau_new slider_v( vgui_slider_tableau::vert );


  // Callbacks to move the thick line when the sliders stop moving
  slider_h->add_final_callback( change_number_cb, &(line1->x1) );
  slider_v->add_final_callback( change_number_cb, &(line1->y1) );

  // Callbacks to move the thin lines while the sliders are in motion
  slider_h->add_motion_callback( change_number_cb, &(line2->x1) );
  slider_v->add_motion_callback( change_number_cb, &(line2->y1) );

  // Move the sliders to somewhere. This'll call the callbacks to
  // update the line positions.
  slider_h->set_value( 0.2f );
  slider_v->set_value( 0.2f );

  // Fit the sliders and the easy2D into the window
  vgui_poly_tableau_new poly;
  poly->add( easy_tab, 0.1, 0.1, 0.9, 0.9 );
  poly->add( slider_h, 0.1, 0.0, 0.9, 0.1 );
  poly->add( slider_v, 0.0, 0.1, 0.1, 0.9 );

  vgui_shell_tableau_new shell(poly);
  return vgui::run(shell, 512, 512);
}
