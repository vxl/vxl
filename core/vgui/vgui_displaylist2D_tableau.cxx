// This is core/vgui/vgui_displaylist2D_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   14 Sep 99
// \brief  See vgui_displaylist2D_tableau.h for a description of this file.

#include "vgui_displaylist2D_tableau.h"

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_glu.h>

#include <vgui/vgui.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_style.h>

bool vgui_displaylist2D_tableau::handle(const vgui_event& e)
{
  // if mouse leaves the context then unhighlight
  // the highlit object
  if (e.type == vgui_LEAVE) {
    this->highlight(0);
    post_overlay_redraw();
  }

  if (e.type == vgui_OVERLAY_DRAW)
  {
    unsigned highlighted = this->get_highlighted();
    if (highlighted)
    {
      vgui_soview* so = vgui_soview::id_to_object(highlighted);
      vgui_style_sptr style = so->get_style();
      if( style ) {
        style->apply_point_size();
        style->apply_line_width();
      }
      glColor3f(0.0,0.0,1.0);
      so->draw();
    }
  }

  return vgui_displaybase_tableau::handle(e);
}

//: Return indices of my elements which are near (x,y)
void vgui_displaylist2D_tableau::get_hits(float x, float y,
                                          vcl_vector<unsigned>& my_hits)
{
  // select objects within 20 pixels of the mouse
  GLuint *ptr = vgui_utils::enter_pick_mode(x,y,20);

  this->gl_mode = GL_SELECT;
  this->handle(vgui_event(vgui_DRAW));
  this->gl_mode = GL_RENDER;

  int num_hits = vgui_utils::leave_pick_mode();

  // get all hits
  vcl_vector<vcl_vector<unsigned> > hits;
  vgui_utils::process_hits(num_hits, ptr, hits);

  // for each hit get the name of the soview if it is
  // being managed by this vcl_list
  //
  // Each hit from a display list has two entries. The first is the id
  // of the display list, and the second is the id of the soview. See
  // vgui_displaybase_tableau::draw_soviews_select(). Thus, an object
  // belongs to this display list iff the first hit number is this
  // list's id.

  for (vcl_vector<vcl_vector<unsigned> >::iterator i=hits.begin();
       i != hits.end(); ++i)
  {
    vcl_vector<unsigned> const& names = *i;

    if ( ! names.empty() && names[0] == this->get_id() )
    {
      // this assertion is based on the code in
      // vgui_displaybase_tableau::draw_soviews_select(). If this is
      // not true, then please review the assumptions about the
      // selection process before updating the assertion.
      //
      assert( names.size() == 2 );
      my_hits.push_back( names[1] );
    }
  }
}

unsigned vgui_displaylist2D_tableau::find_closest(float x, float y,
                                                  vcl_vector<unsigned> const& hits)
{
  unsigned closest = 0;
  float closest_dist = -1; // vnl_numeric_traits<float>::maxval;

  for (vcl_vector<unsigned>::const_iterator h_iter = hits.begin();
       h_iter != hits.end(); ++h_iter)
  {
    // In principle, VXL shouldn't use dynamic_cast since it depends
    // on RTTI, and so far (July 2003), we are not allowed to depend
    // on RTTI in vxl/core code. However, dynamic_cast is *much*
    // safer than static_cast, so I'll sneak this in. Change it to
    // static_cast if your compiler breaks.
    //
    vgui_soview2D* so = dynamic_cast<vgui_soview2D*>(vgui_soview::id_to_object(*h_iter));
    assert( so ); // NULL => something is wrong.

    float dist = so->distance_squared(x,y);
#ifdef DEBUG
    vcl_cerr << "vgui_displaylist2D_tableau::find_closest distance to " << (void*)so << " is " << dist << '\n';
#endif
    if (closest_dist<0 || dist<closest_dist) {
      closest_dist = dist;
      closest = *h_iter;
    }
  }// end for

  return closest;
}

bool vgui_displaylist2D_tableau::motion(int x, int y)
{
  vgui_projection_inspector pi;
  float ix, iy;
  pi.window_to_image_coordinates(x,y, ix,iy);

  vcl_vector<unsigned> hits;
  get_hits(x,y,hits);
  unsigned closest_id = find_closest(ix,iy,hits);

  if ( closest_id != this->get_highlighted() ) {
    this->highlight( closest_id );
    post_overlay_redraw();
  }

  return false;
}

bool vgui_displaylist2D_tableau::mouse_down(int x, int y, vgui_button button, vgui_modifier modifier)
{
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(x,y, ix,iy);

  // selecting
  if (button == vgui_LEFT && modifier == 0)
  {
#ifdef DEBUG
    vcl_cerr << "vgui_displaylist2D_tableau::mouse_down: selecting at " << x << ',' << y << '\n';
#endif
    vcl_vector<unsigned> hits;
    get_hits(x,y,hits);
    unsigned closest_id = find_closest(ix,iy,hits);
    if (closest_id) {
      this->select(closest_id);
      this->post_redraw();
      return true;
    }

    return false;
  }// end selecting

  // deselecting
  else if (button == vgui_MIDDLE)
  {
    if (modifier & vgui_SHIFT)
    {
#ifdef DEBUG
      vcl_cerr << "vgui_displaylist2D_tableau::mouse_down: deselecting all\n";
#endif
      this->deselect_all();
      this->post_redraw();
      return false;
    }

#ifdef DEBUG
    vcl_cerr << "vgui_displaylist2D_tableau::mouse_down: deselecting at " << x << ' ' << y << '\n';
#endif
    vcl_vector<unsigned> hits;
    get_hits(x,y,hits);
    unsigned closest_id = find_closest(ix,iy,hits);
    if (closest_id) {
      this->deselect(closest_id);
      this->post_redraw();
      return true;
    }
    return false;
  }// end deselecting
  return false;
}
