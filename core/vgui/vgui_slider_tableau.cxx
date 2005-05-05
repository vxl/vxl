//:
// \file
// \author Amitha Perera
// \date   Feb 2005

#include "vgui_slider_tableau.h"

#include <vgui/vgui_gl.h>

vgui_slider_tableau::vgui_slider_tableau( slider_type type )
  : loc_( 0.0f ),
    horiz_( type == horiz ? true : false ),
    down_( false )
{
}

vgui_slider_tableau::~vgui_slider_tableau()
{
}


bool
vgui_slider_tableau::handle(const vgui_event& e)
{
  if( e.type == vgui_DRAW ) {
    draw_bar();
    return true;
  } else if( ! down_ && e.type == vgui_MOUSE_DOWN ) {
    last_x_ = e.wx;
    last_y_ = e.wy;
    last_loc_ = loc_;
    down_ = true;
    return true;
  } else if( down_ && e.type == vgui_MOTION ) {
    update_location( e.wx, e.wy );
    // call the motion callbacks
    call_callbacks( motion_callbacks_ );
    return true;
  } else if( down_ && e.type == vgui_MOUSE_UP ) {
    down_ = false;
    update_location(  e.wx, e.wy );
    // call the final & motion callbacks
    call_callbacks( motion_callbacks_ );
    call_callbacks( final_callbacks_ );
    return true;
  } else {
    return false;
  }
}


void
vgui_slider_tableau::draw_bar() const
{
  float pos = loc_*2.0f - 1.0f;

  glColor3f(1.0f,1.0f,1.0f);
  if( horiz_ ) {
    glBegin( GL_POLYGON );
    glVertex2f( pos-0.2f,  0.7f );
    glVertex2f( pos+0.2f,  0.7f );
    glVertex2f( pos+0.2f, -0.7f );
    glVertex2f( pos-0.2f, -0.7f );
    glEnd();

    glBegin( GL_LINES );
    glVertex2f( pos, -1.0f );
    glVertex2f( pos,  1.0f );
    glEnd();
  } else {
    glBegin( GL_POLYGON );
    glVertex2f(  0.7f, pos-0.2f );
    glVertex2f(  0.7f, pos+0.2f );
    glVertex2f( -0.7f, pos+0.2f );
    glVertex2f( -0.7f, pos-0.2f );
    glEnd();

    glBegin( GL_LINES );
    glVertex2f( -1.0f, pos );
    glVertex2f(  1.0f, pos );
    glEnd();
  }
}


vgui_slider_tableau::cb_handle
vgui_slider_tableau::add_motion_callback( callback cb, void* data )
{
  return motion_callbacks_.insert( motion_callbacks_.end(), callback_info( cb, data ) );
}


vgui_slider_tableau::cb_handle
vgui_slider_tableau::add_final_callback( callback cb, void* data )
{
  return final_callbacks_.insert( final_callbacks_.end(), callback_info( cb, data ) );
}


void
vgui_slider_tableau::remove_motion_callback( cb_handle cbh )
{
  motion_callbacks_.erase( cbh );
}


void
vgui_slider_tableau::remove_final_callback( cb_handle cbh )
{
  final_callbacks_.erase( cbh );
}


void
vgui_slider_tableau::set_value( float v )
{
  set_value_no_callbacks( v );
  call_callbacks( motion_callbacks_ );
  call_callbacks( final_callbacks_ );
}

void
vgui_slider_tableau::set_value_no_callbacks( float v )
{
  if( v < 0.0f ) {
    v = 0.0f;
  } else if( v > 1.0f ) {
    v = 1.0f;
  }
  loc_ = v;
  post_redraw();
}


void
vgui_slider_tableau::call_callbacks( vcl_list< callback_info > const& cbs )
{
  if( ! cbs.empty() ) {
    vcl_list< callback_info >::const_iterator it = cbs.begin();
    vcl_list< callback_info >::const_iterator end = cbs.end();
    for( ; it != end; ++it ) {
      (it->func_)( this, it->data_ );
    }
  }
}


void
vgui_slider_tableau::update_location( int newx, int newy )
{
  GLfloat vp[4];
  glGetFloatv(GL_VIEWPORT, vp);
  if( horiz_ ) {
    loc_ = last_loc_ + (newx - last_x_) / vp[2];
  } else {
    loc_ = last_loc_ + (newy - last_y_) / vp[3];
  }    
  if( loc_ < 0.0f ) {
    loc_ = 0.0f;
  } else if( loc_ > 1.0f ) {
    loc_ = 1.0f;
  }
  post_redraw();
}
