// This is core/vgui/vgui_displaybase_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   14 Sep 99
// \brief  See vgui_displaybase_tableau.h for a description of this file.

#include "vgui_displaybase_tableau.h"
#include "vgui_macro.h"

#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vcl_utility.h> // for vcl_pair<,>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_glu.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_message.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_soview.h>

bool vgui_displaybase_tableau_selection_callback::select(unsigned)
{
  return false;
}

bool vgui_displaybase_tableau_selection_callback::deselect(unsigned)
{
  return false;
}

bool vgui_displaybase_tableau_selection_callback::deselect_all()
{
  return false;
}

vgui_displaybase_tableau::vgui_displaybase_tableau()
{
  id = vgui_soview::create_id();

  gl_mode = GL_RENDER;
  highlighted = 0;
  gl_display_list = GL_INVALID_VALUE;

  cb_ = 0;

  current_grouping = "default";
  groupings.clear();
}

vgui_displaybase_tableau::~vgui_displaybase_tableau()
{
}

void vgui_displaybase_tableau::set_selection_callback(vgui_displaybase_tableau_selection_callback* cb)
{
  cb_ = cb;
}

void vgui_displaybase_tableau::add(vgui_soview* object)
{
  vcl_vector<vgui_soview*>::iterator i = vcl_find(objects.begin(), objects.end(), object);
  if (i == objects.end())
  {
    objects.push_back(object);
    vcl_map< vcl_string , vgui_displaybase_tableau_grouping >::iterator it = groupings.find( current_grouping );
    if ( it == groupings.end() )
    {
      vgui_displaybase_tableau_grouping temp;
      temp.style = NULL;
      temp.hide = false;
      temp.color_override = false;
      temp.line_width_override = false;
      temp.point_size_override = false;
      temp.objects.push_back( object );
      groupings.insert( vcl_pair< vcl_string , vgui_displaybase_tableau_grouping >( current_grouping , temp ) );
    }
    else
      it->second.objects.push_back( object );
  }
}

void vgui_displaybase_tableau::remove(vgui_soview* object)
{
  if (object->get_id() == highlighted)
  {
    // if the point to be removed is the currently highlighted
    // one, zero out the 'highlighted' field. otherwise the
    // point will still be rendered when the pointer moves.
    highlighted = 0;
  }

  // must first unselect the object, if it's selected
  deselect( object->get_id());

  vcl_vector<vgui_soview*>::iterator i = vcl_find(objects.begin(), objects.end(), object);
  if (i != objects.end())
  {
    objects.erase(i);
  }

  for ( vcl_map< vcl_string , vgui_displaybase_tableau_grouping >::iterator it = groupings.begin() ;
        it != groupings.end(); it++ )  {
    vcl_vector<vgui_soview*>::iterator a = vcl_find(it->second.objects.begin(), it->second.objects.end(), object);
    if (a != objects.end()) {
      it->second.objects.erase(a);
      if ( it->second.objects.size() == 0 )  {
        groupings.erase( it );
      }
      break; //found obj, stop iterating
    }
  }
}

//: clear all soviews from the display.
//  The soviews must be deleted otherwise we have a big memory leak.
//  It is not clear that the design intended that the creator of the
//  soview is responsible for deleting it.  In any case, such management
//  would be impossible without reference counting.  If users are going
//  to keep soviews for personal use, then the whole soview scheme
//  should be changed to smart pointers. JLM
//
void vgui_displaybase_tableau::clear()
{
  highlighted = 0;
  deselect_all();

  // destroy the objects
  for (vcl_vector<vgui_soview*>::iterator so_iter = objects.begin();
       so_iter != objects.end(); ++so_iter)
  {
    delete *so_iter;
  }

  objects.clear();

  for ( vcl_map< vcl_string , vgui_displaybase_tableau_grouping >::iterator it = groupings.begin() ;
        it != groupings.end() ;
        it++ )
  {
    it->second.objects.clear();
  }

  groupings.clear();
}

void vgui_displaybase_tableau::draw_soviews_render()
{
  vgui_macro_report_errors;
  {
    for (vcl_map< vcl_string , vgui_displaybase_tableau_grouping >::iterator it = groupings.begin();
         it != groupings.end();
         it++ )
    {
      if ( ! it->second.hide )
      {
        for (vcl_vector<vgui_soview*>::iterator so_iter= it->second.objects.begin();
             so_iter != it->second.objects.end(); ++so_iter)
        {
          vgui_soview *so = *so_iter;
          vgui_style_sptr style = so->get_style();

          if ( ! it->second.style )
            style->apply_all();
          else
          {
            if ( it->second.color_override )
              it->second.style->apply_color();
            else
              style->apply_color();

            if ( it->second.line_width_override )
              it->second.style->apply_line_width();
            else
              style->apply_line_width();

            if ( it->second.point_size_override )
              it->second.style->apply_point_size();
            else
              style->apply_point_size();
          }

          if (is_selected(so->get_id()))
            glColor3f(1.0f, 0.0f, 0.0f);

          so->draw();
        }//  for all soviews
      }
    }
  }
  vgui_macro_report_errors;
}


void vgui_displaybase_tableau::draw_soviews_select()
{
  // push the name of this displaylist onto the name stack
  glPushName(id);

  glPushName(0); // will be replaced by the id of each object

  for ( vcl_map< vcl_string , vgui_displaybase_tableau_grouping >::iterator it = groupings.begin();
        it != groupings.end();
        it++ )
  {
    if ( ! it->second.hide )
    {
      for (vcl_vector<vgui_soview*>::iterator so_iter=it->second.objects.begin();
           so_iter != it->second.objects.end(); ++so_iter)
      {
        // only highlight if the so is selectable
        vgui_soview* so = *so_iter;
        if ( so->get_selectable())
        {
          so->load_name();
          so->draw_select();
        }
      }//  for all soviews
    }
  }

  // remove name of last object
  glPopName();

  // remove the name of the displaylist from the name stack
  glPopName();
}


bool vgui_displaybase_tableau::handle(const vgui_event& e)
{
  if (e.type == vgui_DRAW)
  {
    if (gl_mode == GL_SELECT)
      draw_soviews_select();
    else
      draw_soviews_render();

    return true;
  }
  else
    return vgui_tableau::handle(e);
}

bool vgui_displaybase_tableau::is_selected(unsigned id)
{
  vcl_vector<unsigned>::iterator result = vcl_find(selections.begin(), selections.end(), id);
  return result != selections.end();
}

vcl_vector<unsigned> const & vgui_displaybase_tableau::get_selected() const
{
  return selections;
}

vcl_vector<vgui_soview*> vgui_displaybase_tableau::get_selected_soviews() const
{
  vcl_vector<vgui_soview*> svs;
  for (unsigned i=0; i<selections.size(); ++i)
  {
    svs.push_back(vgui_soview::id_to_object(selections[i]));
  }
  return svs;
}

vcl_vector<vgui_soview*> const & vgui_displaybase_tableau::get_all() const
{
  return objects;
}

vcl_vector<unsigned> const vgui_displaybase_tableau::get_all_ids() const
{
  vcl_vector<unsigned> ids;
  for (unsigned int i=0; i< objects.size(); i++)
    ids.push_back( objects[i]->get_id());

  return ids;
}

bool vgui_displaybase_tableau::select(unsigned id)
{
  vcl_vector<unsigned>::iterator result = vcl_find(selections.begin(), selections.end(), id);
  if (result == selections.end())
  {
    // add selection to vcl_list
    selections.push_back(id);

    // notify so's observers
    vgui_soview* so = vgui_soview::id_to_object(id);

    if ( so->get_selectable())
    {
      vgui_message msg;
      //msg.text = "soview select";
      msg.user = (void const*) &vgui_soview::msg_select;
      so->notify(msg);

      if (cb_) cb_->select(id);
    }
  }

  return true;
}

bool vgui_displaybase_tableau::deselect(unsigned id)
{
  vcl_vector<unsigned>::iterator result = vcl_find(selections.begin(), selections.end(), id);
  if (result != selections.end())
  {
    // remove selection from vcl_list
    selections.erase(result);

    // notify so's observers
    vgui_soview* so = vgui_soview::id_to_object(id);
    vgui_message msg;
    //msg.text = "soview deselect";
    msg.user = (void const*) &vgui_soview::msg_deselect;
    so->notify(msg);

    if (cb_) cb_->deselect(id);
  }

  return true;
}

bool vgui_displaybase_tableau::deselect_all()
{
  // this is a bit inelegant but you have to make a copy
  // of the selections vcl_list as sending the deselect message
  // may actually change the selections

  vcl_vector<unsigned> oldselections = selections;

  for (vcl_vector<unsigned>::iterator s_iter = oldselections.begin();
       s_iter != oldselections.end(); ++s_iter )
  {
    unsigned id = *s_iter;

    // notify so's observers
    vgui_soview* so = vgui_soview::id_to_object(id);
    vgui_message msg;
    msg.user = (void const*)&vgui_soview::msg_deselect;
    so->notify(msg);

    if (cb_) cb_->deselect(id);
  }

  selections.clear();
  this->post_redraw();

  return true;
}


bool vgui_displaybase_tableau::is_highlighted(unsigned id)
{
  return id == highlighted;
}

unsigned vgui_displaybase_tableau::get_highlighted()
{
  return highlighted;
}

vgui_soview* vgui_displaybase_tableau::get_highlighted_soview()
{
    return vgui_soview::id_to_object(highlighted);
}

bool vgui_displaybase_tableau::highlight(unsigned id)
{
  highlighted = id;
  return true;
}

vgui_soview* vgui_displaybase_tableau::contains_hit(vcl_vector<unsigned> names)
{
  for (vcl_vector<vgui_soview*>::iterator i = objects.begin() ;
       i != objects.end() ; ++i)
  {
    // get id of soview
    unsigned soview_id = (*i)->get_id();
#ifdef DEBUG
    vcl_cerr << "vgui_displaybase_tableau::contains_hit soview_id=" << soview_id << '\n';
#endif
    vcl_vector<unsigned>::iterator ni = vcl_find(names.begin(), names.end(), soview_id);
    if (ni != names.end())
      return *i;
  }

  return 0;
}

void vgui_displaybase_tableau::set_current_grouping( vcl_string name )
{
  current_grouping = name;
}

vcl_string vgui_displaybase_tableau::get_current_grouping()
{
  return current_grouping;
}

vgui_displaybase_tableau_grouping* vgui_displaybase_tableau::get_grouping_ptr( vcl_string name )
{
  vcl_map< vcl_string , vgui_displaybase_tableau_grouping >::iterator it = groupings.find( name );
  if ( it != groupings.end() )
    return & it->second;
  else
    return NULL;
}

vcl_vector< vcl_string > vgui_displaybase_tableau::get_grouping_names()
{
  vcl_vector< vcl_string > to_return;

  for ( vcl_map< vcl_string , vgui_displaybase_tableau_grouping >::iterator it = groupings.begin();
        it != groupings.end();
        it++ )
  {
    to_return.push_back( it->first );
  }

  return to_return;
}

