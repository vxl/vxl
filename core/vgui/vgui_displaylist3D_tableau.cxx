// This is core/vgui/vgui_displaylist3D_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   14 Sep 99
// \brief  See vgui_displaylist3D_tableau.h for a description of this file.

#include "vgui_displaylist3D_tableau.h"

#include <vcl_iostream.h>
#include <vcl_vector.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_glu.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_soview.h>

bool vgui_displaylist3D_tableau::handle(const vgui_event& e)
{
  if (e.type == vgui_LEAVE)
    return true;

  return vgui_displaybase_tableau::handle(e);
}

void vgui_displaylist3D_tableau::get_hits(float x, float y, vcl_vector<unsigned>& my_hits)
{
  GLuint *ptr = vgui_utils::enter_pick_mode(x,y,10.0,10.0);

  this->gl_mode = GL_SELECT;
  this->handle(vgui_event(vgui_DRAW));
  this->gl_mode = GL_RENDER;

  int num_hits = vgui_utils::leave_pick_mode();

  // get all hits
  vcl_vector<vcl_vector<unsigned> > hits;
  vgui_utils::process_hits(num_hits, ptr, hits);

  // for each hit get the name of the soview if it is
  // being managed by this vcl_list

  for (vcl_vector<vcl_vector<unsigned> >::iterator i=hits.begin();
       i != hits.end(); ++i) {
    vcl_vector<unsigned> names = *i;

    for (vcl_vector<unsigned>::iterator n_iter = names.begin();
         n_iter != names.end(); ++n_iter) {
      unsigned name = *n_iter;

      for (vcl_vector<vgui_soview*>::iterator so_iter = this->objects.begin();
           so_iter != this->objects.end(); ++so_iter) {
        if ((*so_iter)->get_id() == name) {
          my_hits.push_back(name);
          break;
        }
      }// for  display
    }// for  names
  }// for  hits
}

bool vgui_displaylist3D_tableau::mouse_down(int x, int y, vgui_button button, vgui_modifier modifier)
{
  // selecting
  if (button == vgui_LEFT)
  {
#ifdef DEBUG
    vcl_cerr << "selecting at " << x << ' ' << y << vcl_endl;
#endif
    vcl_vector<unsigned> hits;
    get_hits(x,y,hits);

    for (vcl_vector<unsigned>::iterator hi = hits.begin();
         hi != hits.end(); ++hi) {
      this->select(*hi);
    }

    if (hits.size() > 0) {
      this->post_redraw();
    }

    return true;
  }// end selecting

  // deselecting
  else if (button == vgui_MIDDLE)
  {
    if (modifier & vgui_SHIFT) {
#ifdef DEBUG
      vcl_cerr << "deselecting all\n";
#endif
      this->deselect_all();
      this->post_redraw();
      return false;
    }

#ifdef DEBUG
    vcl_cerr << "deselecting at " << x << ' ' << y << vcl_endl;
#endif

    vcl_vector<unsigned> hits;
    get_hits(x,y,hits);

    for (vcl_vector<unsigned>::iterator hi = hits.begin();
         hi != hits.end(); ++hi) {
      this->deselect(*hi);
    }

    if (hits.size() > 0) {
      this->post_redraw();
    }

    return true;
  }// end deselecting
  return false;
}

