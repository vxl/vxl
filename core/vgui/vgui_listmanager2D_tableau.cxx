// This is core/vgui/vgui_listmanager2D_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   21 Oct 99
// \brief  See vgui_listmanager2D_tableau.h for a description of this file.

#include "vgui_listmanager2D_tableau.h"

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_parent_child_link.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_matrix_state.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_soview.h>
#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_displaylist2D_tableau.h>
#include <vgui/vgui_style.h>


vgui_listmanager2D_tableau::vgui_listmanager2D_tableau():
  //highlight_list(0),
  highlight_so(0)
{
}

vgui_listmanager2D_tableau::~vgui_listmanager2D_tableau()
{
}

vcl_string vgui_listmanager2D_tableau::type_name() const
{
  return "vgui_listmanager2D_tableau";
}

//: Add a child to the end of the vcl_list
void vgui_listmanager2D_tableau::add(vgui_displaylist2D_tableau_sptr const& dl)
{
  children.push_back( vgui_parent_child_link(this,dl) );
  active.push_back(true);
  visible.push_back(true);
  observers.notify();
}

//: Remove the given child from the vcl_list.
void vgui_listmanager2D_tableau::remove(vgui_displaylist2D_tableau_sptr const& t)
{
  vcl_vector<bool>::iterator ia = active.begin();
  vcl_vector<bool>::iterator iv = visible.begin();
  for (vcl_vector<vgui_parent_child_link>::iterator i=children.begin() ; i!=children.end() ; ++i, ++ia, ++iv)
    if ( *i == t )
    {
      children.erase(i);
      active.erase(ia);
      visible.erase(iv);
      observers.notify();
      return;
    }
}

void vgui_listmanager2D_tableau::set_active(int v, bool b)
{
  if (!index_ok(v)) return;
  active[v] = b;
}

bool vgui_listmanager2D_tableau::is_active(int v)
{
  if (!index_ok(v)) return false;
  return active[v];
}

void vgui_listmanager2D_tableau::set_visible(int v, bool b)
{
  if (!index_ok(v)) return;
  visible[v] = b;
}

bool vgui_listmanager2D_tableau::is_visible(int v)
{
  if (!index_ok(v)) return false;
  return visible[v];
}

bool vgui_listmanager2D_tableau::index_ok(int v)
{
  return v >= 0 && v < int(children.size());
}

bool vgui_listmanager2D_tableau::help()
{
  vcl_cerr << '\n'
           << "+- vgui_listmanager2D_tableau keys -+\n"
           << "|                                   |\n"
           << "| `1' to `9'  toggle child `n'      |\n"
           << "+-----------------------------------+\n\n";
  return false;
}

vgui_displaylist2D_tableau_sptr vgui_listmanager2D_tableau::contains_hit(vcl_vector<unsigned> const& names)
{
  for (vcl_vector<vgui_parent_child_link>::iterator i=this->children.begin() ;
       i!=this->children.end() ; ++i)
  {
    // get id of vcl_list
    vgui_displaylist2D_tableau_sptr list;
    list.vgui_tableau_sptr::operator=(i->child());
    unsigned list_id = list->get_id();

    vcl_vector<unsigned>::const_iterator ni = vcl_find(names.begin(), names.end(), list_id);
    if (ni != names.end())
      return list;
  }

  return vgui_displaylist2D_tableau_sptr();
}

void vgui_listmanager2D_tableau::get_hits(float x, float y, vcl_vector<vcl_vector<unsigned> >* hits)
{
  GLuint *ptr = vgui_utils::enter_pick_mode(x,y,100);

  int count=0;
  for (vcl_vector<vgui_parent_child_link>::iterator i=this->children.begin() ;
       i!=this->children.end() ; ++i, ++count)
  {
    vgui_displaylist2D_tableau_sptr display;
    display.vgui_tableau_sptr::operator=(i->child());

    if (this->active[count] && this->visible[count])
    {
      display->gl_mode = GL_SELECT;
      display->handle(vgui_event(vgui_DRAW));
      display->gl_mode = GL_RENDER;
    }
  }

  int num_hits = vgui_utils::leave_pick_mode();

  // get hits
  vgui_utils::process_hits(num_hits, ptr, *hits);
}

void vgui_listmanager2D_tableau::find_closest(float x, float y, vcl_vector<vcl_vector<unsigned> >* hits,
                                              vgui_soview2D** closest_so, vgui_displaylist2D_tableau_sptr* closest_display)
{
  float closest_dist = -1; // vnl_numeric_traits<float>::maxval;
  vcl_vector<unsigned> closest_hit;
  vgui_displaylist2D_tableau_sptr display;
  closest_display = 0;
  closest_so = 0;

#ifdef DEBUG
  vcl_cerr << "vgui_listmanager2D_tableau::find_closest: hits->size() = " << hits->size() << '\n';
#endif
  for (vcl_vector<vcl_vector<unsigned> >::iterator h_iter = hits->begin();
       h_iter != hits->end(); ++h_iter)
  {
    vcl_vector<unsigned> names = *h_iter;
#ifdef DEBUG
    vcl_cerr << "vgui_listmanager2D_tableau::find_closest: names.size() " << names.size() << '\n';
#endif

    // first see if this hit is in a displaylist managed by this listmanager2D
    display = contains_hit(names);
    if (display)
    {
#ifdef DEBUG
      vcl_cerr << "vgui_listmanager2D_tableau::find_closest: hit in display " << display->get_id() << '\n';
#endif
      vgui_soview2D *so = static_cast<vgui_soview2D*>(display->contains_hit(names));
      if (so)
      {
        float dist = so->distance_squared(x,y);
        if (closest_dist<0 || dist<closest_dist)
        {
          closest_dist = dist;
          *closest_display = display;
          closest_hit = *h_iter;
        }
      }
    }// end if vcl_list valid
  }// end for hits

  if (*closest_display)
  {
    // get the soview out of the closest_hit name vcl_list
    *closest_so = static_cast<vgui_soview2D*>((*closest_display)->contains_hit(closest_hit));
  }
}


//: vgui_listmanager2D_tableau events are send to the child displaylist which holds the closest soview2d.
bool vgui_listmanager2D_tableau::handle(const vgui_event& event)
{
  // save current matrix state :
  vgui_matrix_state PM;

  // "draw" events
  if (event.type==vgui_DRAW || event.type==vgui_DRAW_OVERLAY)
  {
    bool retv = true;

    int ia = 0;
    for ( vcl_vector<vgui_parent_child_link>::iterator i = children.begin(); i != children.end(); ++i, ++ia)
    {
      PM.restore();

      if (visible[ia])
        if ( !(*i)->handle(event) )
          retv=false;
    }
    return retv;
  }

  // "normal" events -- pass them to the drag_mixin, but
  // remember this one in order that it can be passed on.
  this->saved_event_ = event;
  return vgui_tableau::handle(event);
}

bool vgui_listmanager2D_tableau::motion(int x, int y)
{
  vgui_projection_inspector pi;
  float ix, iy;
  pi.window_to_image_coordinates(int(x),int(y), ix,iy);

  vcl_vector<vcl_vector<unsigned> > hits;
  get_hits(x,y,&hits);

  vgui_soview2D* closest_so;
  vgui_displaylist2D_tableau_sptr closest_display;
  find_closest(ix, iy, &hits, &closest_so, &closest_display);

#ifdef DEBUG
  if (closest_so && closest_display)
    vcl_cerr << "vgui_listmanager2D_tableau::motion: hit " << closest_so->get_id()
             << " in vcl_list " << closest_display->get_id() << vcl_endl;
#endif

  vgui_utils::begin_sw_overlay();

  if (highlight_so)
  {
    vgui_soview* so = highlight_so;
    vgui_style_sptr style = so->get_style();
    style->apply_point_size();
    style->apply_line_width();

    if (highlight_list->is_selected(so->get_id()))
      glColor3f(1.0f, 0.0f, 0.0f);
    else
    {
      style->apply_color();
    }
    so->draw();
  }


  if (closest_so)
  {
#ifdef DEBUG
    vcl_cerr << "vgui_listmanager2D_tableau::motion highlighting : " << closest_id << '\n';
#endif
    vgui_soview* so = closest_so;
    vgui_style_sptr style = so->get_style();
    style->apply_point_size();
    style->apply_line_width();
    glColor3f(0.0f,1.0f,1.0f);
    so->draw();
  }

  vgui_utils::end_sw_overlay();

  highlight_list = closest_display;
  highlight_so = closest_so;

  return true;
}


bool vgui_listmanager2D_tableau::mouse_down(int x, int y, vgui_button button, vgui_modifier modifier)
{
  if (button == vgui_MIDDLE && (modifier & vgui_SHIFT))
  {
    int count=0;
    for (vcl_vector<vgui_parent_child_link>::iterator i=this->children.begin() ;
         i!=this->children.end() ; ++i, ++count)
    {
      vgui_displaylist2D_tableau_sptr display;
      display.vgui_tableau_sptr::operator=(i->child());
      if (this->active[count] && this->visible[count])
        display->handle(this->saved_event_);
    }
    return true;
  }

  // send the event only to the displaylist that contains the closest soview
  vgui_projection_inspector pi;
  float ix, iy;
  pi.window_to_image_coordinates(int(x),int(y), ix,iy);

  vcl_vector<vcl_vector<unsigned> > hits;
  get_hits(x,y,&hits);

  vgui_soview2D* closest_so;
  vgui_displaylist2D_tableau_sptr closest_display;
  find_closest(ix, iy, &hits, &closest_so, &closest_display);

  return closest_display && closest_display->handle(this->saved_event_);
}

bool vgui_listmanager2D_tableau::key_press(int /*x*/, int /*y*/, vgui_key key, vgui_modifier)
{
#ifdef DEBUG
  vcl_cerr << "vgui_listmanager2D_tableau_handler::key_press " << key << '\n';
#endif

  if (key >= '1' && key <= '9')
  {
    char text[1];
    text[0] = key;
    int num = atoi(text);

    bool active = this->is_active(num-1);
    bool visible = this->is_visible(num-1);

    if (active)
    {
      this->set_active(num-1, false);
      this->set_visible(num-1, false);
      vgui_displaylist2D_tableau_sptr list;
      list.vgui_tableau_sptr::operator=(this->children[num-1].child());
      if (highlight_list == list)
      {
        highlight_list = vgui_displaylist2D_tableau_sptr();
        highlight_so = 0;
      }
    }
    else if (visible)
    {
      this->set_active(num-1, true);
      this->set_visible(num-1, true);
    }
    else
    {
      this->set_active(num-1, false);
      this->set_visible(num-1, true);

      vgui_displaylist2D_tableau_sptr list;
      list.vgui_tableau_sptr::operator=(this->children[num-1].child());
      if (highlight_list == list)
      {
        highlight_list = vgui_displaylist2D_tableau_sptr();
        highlight_so = 0;
      }
    }

    this->post_redraw();
    return true;
  }
  return false;
}
