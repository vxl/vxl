#include <vcl_sstream.h>

#include <vgui/vgui.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_text_tableau.h>
#include <vgui/vgui_shell_tableau.h>

class idle_tableau
  : public vgui_tableau
{
public:
  idle_tableau( vgui_text_tableau_sptr in_text_tab )
    : text_tab_( in_text_tab ),
      start_idle_( vgui_key( 'i' ) ),
      idle_count_( 0 )
    {
      handle_ = text_tab_->add( 0.0f, 50.0f, "'i' to start idle processing" );
    }

  virtual bool handle( const vgui_event& e )
    {
      if( start_idle_( e ) ) {
        post_idle_request();
        return true;
      } else {
        return vgui_tableau::handle( e );
      }
    }

  bool idle()
    {
      if( idle_count_ >= 100 ) {
        text_tab_->change( handle_, "'i' to start idle processing" );
        idle_count_ = 0;
        return false; // idle processing completed
      } else {
        vcl_ostringstream ostr;
        ostr << "idle count: " << idle_count_;
        text_tab_->change( handle_, ostr.str() );
        ++idle_count_;
        return true; // want more idle events
      }
    }

private:
  vgui_text_tableau_sptr text_tab_;
  vgui_event_condition start_idle_;
  int handle_;
  int idle_count_;
};

int
main( int argc, char* argv[] )
{
  vgui::init( argc, argv );

  vgui_text_tableau_new text_tab;
  vgui_tableau_sptr idle_tab = new idle_tableau( text_tab );
  vgui_composite_tableau_new comp_tab( text_tab, idle_tab );
  vgui_viewer2D_tableau_new view_tab( comp_tab );
  vgui_shell_tableau_new shell_tab( view_tab );

  return vgui::run( shell_tab, 100, 100 );
}
