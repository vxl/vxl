//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//
// .NAME vgui_displaylist3D
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 14 Sep 99
//
//-----------------------------------------------------------------------------

#include "vgui_displaylist3D.h"

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>

#include <vbl/vbl_sprintf.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_glu.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_message.h>
#include <vgui/vgui_drag_mixin.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_viewer3D.h>
#include <vgui/vgui_soview3D.h>
#include <vgui/vgui_style.h>

// the default is 'false'. don't check in 'true'.
// static bool debug = false;

vgui_displaylist3D::vgui_displaylist3D()
{
}

vgui_displaylist3D::~vgui_displaylist3D()
{
}

bool vgui_displaylist3D::handle(const vgui_event& e) {
  
  if (e.type == vgui_LEAVE)
    return true;
    
  return vgui_displaybase::handle(e);
}

void vgui_displaylist3D::get_hits(float x, float y, vcl_vector<unsigned>& my_hits) {
    
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

bool vgui_displaylist3D::mouse_down(int x, int y, vgui_button button, vgui_modifier modifier) {

  // selecting 
  if (button == vgui_LEFT) {
//  if (debug) cerr << vbl_sprintf("selecting at %f %f", x, y) << endl;

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
  else if (button == vgui_MIDDLE) {
      
    if (modifier & vgui_SHIFT) {
//    if (debug) cerr << "deselecting all" << endl;
      this->deselect_all();
      this->post_redraw();
      return false;
    }
      
//  if (debug) cerr << vbl_sprintf("deselecting at %f %f", x, y) << endl;
      
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

