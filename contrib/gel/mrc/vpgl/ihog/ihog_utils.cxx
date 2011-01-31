#include "ihog_utils.h"
//
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>

void ihog_utils::image_bounds(unsigned source_ni, unsigned source_nj,
                              ihog_transform_2d const& t, unsigned& dest_ni,
                              unsigned& dest_nj, ihog_transform_2d& mod_trans)
{
  //max bounds of input image
  double ni_d = source_ni-1, nj_d = source_nj-1;
  //compute inverse to determine bounds on destination image
  ihog_transform_2d tinv = t.inverse();
  //initial bounds of destination image
  vgl_point_2d<double> ul=tinv(0,0),
    ur = tinv(ni_d,0), lr = tinv(ni_d, nj_d), ll=tinv(0, nj_d);
  vgl_box_2d<double> bb;
  bb.add(ul);   bb.add(ur);   bb.add(lr);   bb.add(ll);
  double minu = bb.min_x(), minv = bb.min_y();
  double maxu = bb.max_x(), maxv = bb.max_y();

  // define shift transform so that destination image is
  // mapped for all positive image pixel coordinates
  ihog_transform_2d t_shift;

  t_shift.set_affine(vgl_point_2d<double>(minu, minv),
                     vgl_vector_2d<double>(1,0),
                     vgl_vector_2d<double>(0,1));
  //compose the two transformations
  mod_trans = t*t_shift;//note shift then map
  ihog_transform_2d dinv = mod_trans.inverse();
  //revised bounds of destination image
  ul=dinv(0,0);  ur = dinv(ni_d,0); lr = dinv(ni_d, nj_d); ll = dinv(0, nj_d);
  bb.empty();//clear the box
  bb.add(ul); bb.add(ur); bb.add(lr); bb.add(ll);
  dest_ni = static_cast<int>(maxu)+1;
  dest_nj = static_cast<int>(maxv)+1;
}

ihog_image<float> ihog_utils::destination_mask(unsigned source_ni,
                                               unsigned source_nj,
                                               ihog_transform_2d const& t)
{
  unsigned ni, nj;
  ihog_transform_2d m, m_inv;
  ihog_utils::image_bounds(source_ni, source_nj, t, ni, nj, m);
  m_inv = m.inverse();
  double ni_d = source_ni-1, nj_d = source_nj-1;
  vgl_point_2d<double> ul=m_inv(0,0),
    ur = m_inv(ni_d,0), lr = m_inv(ni_d, nj_d), ll=m_inv(0, nj_d);
  vil_image_view<float> mask(ni, nj, 1);
  mask.fill(0.0f);
  //create scan converter
  vgl_polygon<double> poly(1);

  poly.push_back(ul);poly.push_back(ur);poly.push_back(lr);poly.push_back(ll);
  vgl_polygon_scan_iterator<double> psc(poly, false);
  //set active mask pixels
  for (psc.reset(); psc.next();) {
    int v = psc.scany();
    for (int u = psc.startx(); u<=psc.endx(); ++u)
      mask(u,v) = 1.0f;
  }
  ihog_image<float>imask(mask, m_inv);
 return imask;
}
