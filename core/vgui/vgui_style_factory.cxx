// This is ./oxl/vgui/vgui_style_factory.cxx

//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   18 Oct 99
// \brief  See vgui_style_factory.h for a description of this file.
//
// \verbatim
//  Modifications:
//    18-OCT-1999 P.Pritchett - Initial version.
// \endverbatim

#ifdef __GNUC__
#pragma implementation
#endif


#include "vgui_style_factory.h"

#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <vcl_utility.h>

#include <vgui/vgui_style.h>


static bool debug = false;

bool vgui_style_factory::use_factory = false;


vgui_style_factory* vgui_style_factory::instance_ = 0;


vgui_style_factory* vgui_style_factory::instance() {
  if (!instance_)
    instance_ = new vgui_style_factory;

  return instance_;
}

vgui_style_factory::vgui_style_factory()
{
}

vgui_style_factory::~vgui_style_factory()
{
  for (vcl_vector<vgui_style*>::iterator s_iter = styles.begin();
       s_iter != styles.end(); ++s_iter) {
    delete *s_iter;
  }

  styles.clear();
}

vgui_style* vgui_style_factory::get_style(float r, float g, float b,
                                          float point_size, float line_width) {
  return instance()->get_style_impl(r,g,b,point_size,line_width);
}

vgui_style* vgui_style_factory::get_style_impl(float r, float g, float b,
                                               float point_size, float line_width) {

  vgui_style s;
  s.rgba[0] = r;
  s.rgba[1] = g;
  s.rgba[2] = b;
  s.point_size = point_size;
  s.line_width = line_width;

  vgui_style_equal seq(&s);
  vcl_vector<vgui_style*>::iterator s_iter = vcl_find_if(styles.begin(), styles.end(), seq);

  if (s_iter != styles.end()) {
    if (debug) vcl_cerr << "found style.";
    return *s_iter;
  }

  vgui_style *snew = new vgui_style(s);
  styles.push_back(snew);
  if (debug) vcl_cerr << "creating new style" << vcl_endl;
  if (debug) vcl_cerr << "number of styles : " << styles.size() << vcl_endl;
  return snew;

}

void vgui_style_factory::get_styles(vcl_vector<vgui_style*>& styles_copy) {
  instance()->get_styles_impl(styles_copy);
}

void vgui_style_factory::get_styles_impl(vcl_vector<vgui_style*>& styles_copy) {

  if (debug) vcl_cerr << "number of styles : " << styles.size() << vcl_endl;

  for (vcl_vector<vgui_style*>::iterator s_iter = styles.begin();
       s_iter != styles.end(); ++s_iter)
    styles_copy.push_back(*s_iter);
}


void vgui_style_factory::get_soviews(vgui_style* style, vcl_vector<vgui_soview*>& soviews) {
  instance()->get_soviews_impl(style, soviews);
}

void vgui_style_factory::get_soviews_impl(vgui_style* style, vcl_vector<vgui_soview*>& soviews) {

  vcl_pair<MultiMap_styles::iterator, MultiMap_styles::iterator> matches =
    styles_map.equal_range(style);

  for (MultiMap_styles::iterator r_iter = matches.first; r_iter != matches.second; ++r_iter) {
    soviews.push_back((*r_iter).second);
  }

}




bool vgui_style_factory::so_equal::operator()(MultiMap_styles::value_type obj) {
  if (obj.second == s1)
    return true;
  return false;
}

void vgui_style_factory::change_style(vgui_soview* so, vgui_style* st_new, vgui_style* st_old) {
  instance()->change_style_impl(so,st_new,st_old);
}


void vgui_style_factory::change_style_impl(vgui_soview* so, vgui_style* st_new, vgui_style* st_old) {
  if (!st_old) {
    // add tuple of <st_new, so> to multimap
    styles_map.insert(MultiMap_styles::value_type(st_new, so));
  }
  else {
    // first find range having key == st_old
    vcl_pair<MultiMap_styles::iterator, MultiMap_styles::iterator> matches =
      styles_map.equal_range(st_old);

    if (debug) vcl_cerr << "found range" << vcl_endl;

    // find element with value == so
    so_equal seq(so);
    MultiMap_styles::iterator f_iter = vcl_find_if(matches.first, matches.second, seq);

    if (debug) vcl_cerr << "found element" << vcl_endl;

    styles_map.erase(f_iter);
    styles_map.insert(MultiMap_styles::value_type(st_new, so));
  }

}
