// This is core/vgui/vgui_text_tableau.cxx
//:
// \file
// \brief  See vgui_text_tableau.h for a description of this file.
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   19 Oct 1999
//
// \verbatim
//  Modifications
//   19-OCT-1999 P.Pritchett - Initial version.
// \endverbatim

#include "vgui_text_tableau.h"
#include <vgui/vgui_text_put.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_gl.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

vgui_text_tableau::vgui_text_tableau()
  : cur_r_( 1 ), cur_g_( 0 ), cur_b_( 0 ),
    cur_sz_( 24 ),
    first_empty(0)
{
  // Default text colour is red:
  cur_r_ = 1; cur_g_ = 0; cur_b_ = 0;
}

std::string vgui_text_tableau::type_name() const { return "vgui_text_tableau"; }

unsigned vgui_text_tableau::size() const
{
  unsigned N=xs.size();
#if 0
  assert(N == ys.size()); // just to
  assert(N == ts.size()); // make sure.
#endif // 0
  return N;
}

void vgui_text_tableau::clear()
{
  if (size() > 0) {
    xs.clear();
    ys.clear();
    ts.clear();
    post_redraw();
    first_empty = 0;
  }
}

int vgui_text_tableau::add(float x, float y, char const *text)
{
  int return_val = first_empty;
  if (first_empty < size()) {
    xs[first_empty] = x;
    ys[first_empty] = y;
    ts[first_empty] = text;
    r_[first_empty] = cur_r_;
    g_[first_empty] = cur_g_;
    b_[first_empty] = cur_b_;
    sz_[first_empty] = cur_sz_;
    // Find next empty slot:
    while (first_empty < size()  &&  r_[first_empty] != -1)
      first_empty++;
  }
  else {
    xs.push_back(x);
    ys.push_back(y);
    ts.push_back(text);
    r_.push_back(cur_r_);
    g_.push_back(cur_g_);
    b_.push_back(cur_b_);
    sz_.push_back(cur_sz_);
    first_empty++;
  }
  post_redraw();
  return return_val;
}

void vgui_text_tableau::move(int hndl, float nx, float ny)
{
  assert(hndl >= 0);
  assert((unsigned int)hndl < size());
  xs[hndl] = nx;
  ys[hndl] = ny;
  post_redraw();
}

void vgui_text_tableau::set_colour(float r, float g, float b)
{
  if (0 <= r && r <= 1 && 0 <= g && g <= 1 && 0 <= b && b <= 1)
  {
    cur_r_ = r;
    cur_g_ = g;
    cur_b_ = b;
  }
}

void vgui_text_tableau::set_size( unsigned s )
{
  cur_sz_ = s;
}

float vgui_text_tableau::get_posx(int hndl) const
{
  assert(hndl >= 0);
  assert((unsigned int)hndl < size());
  return xs[hndl];
}

float vgui_text_tableau::get_posy(int hndl) const
{
  assert(hndl >= 0);
  assert((unsigned int)hndl < size());
  return ys[hndl];
}

std::string const &vgui_text_tableau::get_text(int hndl) const
{
  assert(hndl >= 0);
  assert((unsigned int)hndl < size());
  return ts[hndl];
}

void vgui_text_tableau::remove(int hndl)
{
  assert(hndl >= 0);
  assert((unsigned int)hndl < size());
  // Using r_ to indicate empty is okay because valid values for r are in [0,1].
  r_[hndl] = -1;

#if 0 // kym - don't do this because it changes handles of values remaining in list:
  xs.erase(xs.begin() + hndl);
  ys.erase(ys.begin() + hndl);
  ts.erase(ts.begin() + hndl);
#endif // 0

  post_redraw();
  if ((unsigned int)hndl < first_empty)
    first_empty = hndl;
}

void vgui_text_tableau::change(int hndl, char const *ntext)
{
  assert(hndl >= 0);
  assert((unsigned int)hndl < size());
  ts[hndl] = ntext;
  post_redraw();
}

bool vgui_text_tableau::handle(vgui_event const &e)
{
  if (e.type != vgui_DRAW)
    return false;

  // set OpenGL state suitable for rendering 2D bitmap fonts :
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glShadeModel(GL_FLAT);

  for (unsigned i=0; i<size(); ++i) {
    if (r_[i] != -1) {
      glColor3f(r_[i],g_[i],b_[i]);
      glRasterPos2f(xs[i], ys[i]);
      ::vgui_text_put(ts[i].c_str(),sz_[i]);
    }
  }
  return true;
}
