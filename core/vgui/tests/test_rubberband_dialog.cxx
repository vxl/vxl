// The test is to run this executable, and click on the main tableau.
// This should pop up a dialog.  Now click on the dialog.  Does that
// cause a second dialog to pop up?  If not, all is well.
//
// This issue is (was) that the MFC adaptor somehow passed the mouse
// click through the dialog to the tableau, even the mouse events were
// happening (appeared to happen) on the dialog.

#include <vcl_iostream.h>

#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_rubberband_tableau.h>
#include <vgui/vgui_shell_tableau.h>

struct pick_client
  : public vgui_rubberband_client
{
  void add_point( float x, float y )
  {
    vgui_dialog dlg( "Test dialog" );

    static int count = 0;

    vcl_cout << "Add point with count=" << count << vcl_endl;

    if( count == 0 )
    {
      dlg.message( "Move the dialog over the main window, and\n"
                   "click on the dialog so that the\n"
                   "mouse is over the main window too\n" );
      int i;
      dlg.field( "Integer", i );
    }
    else if( count == 1 )
    {
      dlg.message( "This is a second dialog caused by the mouse\n"
                   "click.  It shouldn't have happened, if you\n"
                   "clicked only in the dialog window" );
    }
    else
    {
      dlg.message( "This is a third or subsequent dialog caused by the mouse\n"
                   "click.  It shouldn't have happened, if you\n"
                   "clicked only in the dialog window" );
    }

    vcl_cout << "About to pop up dialog " << count << vcl_endl;
    ++count;
    dlg.ask();
    vcl_cout << "Done with dialog" << vcl_endl;
  }
};


int main( int argc, char** argv )
{
  vgui::init( argc, argv );

  pick_client client;

  vgui_rubberband_tableau_new rubbertab( &client );
  vgui_shell_tableau_new shell_tab( rubbertab );

  vgui::adapt( shell_tab, 320, 240, "Test rubberband w/ popup dialog" );

  vgui::out << "Click somewhere in the tableau";

  rubbertab->rubberband_point();

  return vgui::run();
}
