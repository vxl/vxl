// This is core/vgui/vgui_soview.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   24 Mar 1999
// \brief  See vgui_soview.h for a description of this file.
//
// \date Modified 14 Nov 2006, by B. McCane. Set AWF_USE_MAP to 1 so the
// map is used by default and added a destructor that removes the id
// from the map and therefore prevents a memory leak.

#include <iostream>
#include <map>
#include "vgui_soview.h"

#include <vcl_compiler.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_observer.h>
#include <vgui/vgui_style.h>

#define VGUI_STATIC_OBJECT(T, var) \
static T& var () { \
  static T * t = 0; \
  if (t == 0) t = new T(); \
  return *t; \
}

#define AWF_USE_MAP 1

unsigned vgui_soview::current_id = 1;

#if AWF_USE_MAP
typedef std::map<unsigned, void*, std::less<unsigned> > Map_soview;
VGUI_STATIC_OBJECT(Map_soview, object_map);
#else
typedef std::vector<void* > Map_soview;
VGUI_STATIC_OBJECT(Map_soview, object_map);
#endif

//: Destructor - delete this soview.
// Modified to erase the id from the map - otherwise we get a memory leak
vgui_soview::~vgui_soview()
{
#if AWF_USE_MAP
    object_map().erase(id);
#endif
}

unsigned vgui_soview::create_id()
{
  unsigned nid = current_id;
  current_id++;
  return nid;
}

void vgui_soview::add_id()
{
  id = create_id();
#if AWF_USE_MAP
  object_map().insert(Map_soview::value_type(id, this));
#else
  object_map().resize(id * 2);
  object_map()[id] = this;
#endif
}

vgui_soview* vgui_soview::id_to_object(unsigned id)
{
#if AWF_USE_MAP
  Map_soview::iterator i = object_map().find(id);
  if (i != object_map().end()) {
    return static_cast<vgui_soview*>((*i).second);
  }
#else
  if (id < object_map().size()) {
    return static_cast<vgui_soview*>(object_map()[id]);
  }
#endif

  return nullptr;
}


std::ostream& vgui_soview::print(std::ostream& s) const
{
  return s << "id " << id;
}


void vgui_soview::draw_select() const
{
  // default is to draw as normal. Complex objects may override
  // this behaviour.
  //
  this->draw();
}


void vgui_soview::load_name() const
{
  glLoadName(id);
}


void vgui_soview::set_colour(float r, float g, float b)
{
  vgui_style_sptr newstyle = vgui_style::new_style();

  newstyle->rgba[0] = r;
  newstyle->rgba[1] = g;
  newstyle->rgba[2] = b;

  if (style) {
    newstyle->point_size = style->point_size;
    newstyle->line_width = style->line_width;
  }

  style = newstyle;
}

void vgui_soview::set_point_size(float s)
{
  vgui_style_sptr newstyle = vgui_style::new_style();

  newstyle->point_size = s;

  if (style) {
    newstyle->rgba[0] = style->rgba[0];
    newstyle->rgba[1] = style->rgba[1];
    newstyle->rgba[2] = style->rgba[2];
    newstyle->line_width = style->line_width;
  }

  style = newstyle;
}

void vgui_soview::set_line_width(float w)
{
  vgui_style_sptr newstyle = vgui_style::new_style();

  newstyle->line_width = w;

  if (style) {
    newstyle->rgba[0] = style->rgba[0];
    newstyle->rgba[1] = style->rgba[1];
    newstyle->rgba[2] = style->rgba[2];
    newstyle->point_size = style->point_size;
  }

  style = newstyle;
}


//
const void * const vgui_soview::msg_select="select";
const void * const vgui_soview::msg_deselect="deselect";
const void * const vgui_soview::msg_highlight="highlight";
const void * const vgui_soview::msg_unhighlight="unhighlight";


//--------------------------------------------------------------------------------

// Observers. Rather than storing a std::list/std::vector/whatever of observers on each
// soview, we maintain a static multimap from soviews to observers. This makes
// the soviews smaller and optimizes the common case of soviews with no
// observers.
// fsm: I have not tested this code yet -- where is it used?

// vc++ static data members have some peculiarities, so
// we use this traditional work-around instead :
typedef std::multimap<void *, void *, std::less<void *> > mmap_Pv_Pv;
static mmap_Pv_Pv &the_map()
{
  static mmap_Pv_Pv *ptr = nullptr;
  if (!ptr)
    ptr = new mmap_Pv_Pv;
  return *ptr;
}

void vgui_soview::attach(vgui_observer* o)
{
  the_map().insert(mmap_Pv_Pv::value_type(this, o));
}

void vgui_soview::detach(vgui_observer* o)
{
  mmap_Pv_Pv::iterator lo = the_map().lower_bound(this);
  mmap_Pv_Pv::iterator hi = the_map().upper_bound(this);
  for (mmap_Pv_Pv::iterator i=lo; i!=hi; ++i)
    if (
        (*i).second == o
        ) {
      the_map().erase(i);
      return;
    }

  // not found :
  std::cerr << __FILE__ " : no such observer on this soview\n";
}

void vgui_soview::get_observers(std::vector<vgui_observer*>& vobs) const
{
  mmap_Pv_Pv::const_iterator lo = the_map().lower_bound( const_cast<vgui_soview*>(this) );
  mmap_Pv_Pv::const_iterator hi = the_map().upper_bound( const_cast<vgui_soview*>(this) );
  for (mmap_Pv_Pv::const_iterator i=lo; i!=hi; ++i)
    vobs.push_back( static_cast<vgui_observer*>((*i).second) );
}

// These two method could be optimized a bit.
void vgui_soview::notify() const
{
  std::vector<vgui_observer*> vobs;
  get_observers(vobs);
  for (unsigned i=0; i<vobs.size(); ++i)
    vobs[i]->update();
}

void vgui_soview::notify(vgui_message const &msg) const
{
  std::vector<vgui_observer*> vobs;
  get_observers(vobs);
  for (unsigned i=0; i<vobs.size(); ++i)
    vobs[i]->update(msg);
}
