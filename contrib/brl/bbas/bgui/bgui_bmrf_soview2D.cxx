//:
// \file
#include "bgui_bmrf_soview2D.h"
#include <bmrf/bmrf_epi_seg.h>
#include <bmrf/bmrf_epi_point_sptr.h>
#include <bmrf/bmrf_epi_point.h>

#include <vcl_iostream.h>
#include <vcl_limits.h> 
#include <vcl_vector.h>
#include <vcl_algorithm.h>

#include <vgl/vgl_distance.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_soview2D.h>
#include <vil/vil_image_view.h>


// Constructor
bgui_bmrf_epi_seg_soview2D::bgui_bmrf_epi_seg_soview2D( bmrf_epi_seg_sptr const & seg,
                                                        const bmrf_epipole& epipole,
                                                        bool intensity )
 : seg_sptr_(seg), intensity_view_ (NULL)
{
  if(intensity)
    intensity_view_ = make_intensity_view(epipole);
}


//: Destructor
bgui_bmrf_epi_seg_soview2D::~bgui_bmrf_epi_seg_soview2D() 
{
  delete intensity_view_;
}


//: Render the bmrf_epi_seg on the display.
void
bgui_bmrf_epi_seg_soview2D::draw() const
{
  if(intensity_view_)
    intensity_view_->draw();

  glBegin(GL_LINE_STRIP);
  vcl_vector<bmrf_epi_point_sptr>::const_iterator p_itr;
  for(p_itr = seg_sptr_->begin(); p_itr != seg_sptr_->end(); ++p_itr)
    glVertex2f((*p_itr)->x(), (*p_itr)->y());
  glEnd();
}


//: Print details about this bmrf_epi_seg to the given stream.
vcl_ostream&
bgui_bmrf_epi_seg_soview2D::print(vcl_ostream&s) const
{
  return s << "[ a bmrf_epi_seg FIXME ]";
}


//: Returns the distance squared from this bmrf_epi_seg to the given position.
float
bgui_bmrf_epi_seg_soview2D::distance_squared(float x, float y) const
{
  if(seg_sptr_->n_pts() == 0) return vcl_numeric_limits<float>::infinity();
  if(seg_sptr_->n_pts() == 1){
    bmrf_epi_point_sptr pt = *(seg_sptr_->begin());
    float dx = x-pt->x();
    float dy = y-pt->y();
    return dx*dx + dy*dy;
  }
  
  float dd = -1.0f;
  vcl_vector<bmrf_epi_point_sptr>::const_iterator p_itr;
  for(p_itr = seg_sptr_->begin(); (p_itr+1) != seg_sptr_->end(); ++p_itr){
    float nd = vgl_distance2_to_linesegment(float((*p_itr)->x()),     float((*p_itr)->y()),
                                            float((*(p_itr+1))->x()), float((*(p_itr+1))->y()),
                                            x, y);
    if (dd<0 || nd<dd)
      dd = nd;
  }
  return dd;
}


//: Returns the centroid of this bmrf_epi_seg.
void
bgui_bmrf_epi_seg_soview2D::get_centroid(float* x, float* y) const
{
  *x = 0;
  *y = 0;
  int n=0;
  vcl_vector<bmrf_epi_point_sptr>::const_iterator p_itr;
  for(p_itr = seg_sptr_->begin(); p_itr != seg_sptr_->end(); ++p_itr, ++n){
    *x += (*p_itr)->x();
    *y += (*p_itr)->y();
  }
  float s = 1.0f / float(n);
  *x *= s;
  *y *= s;
}


//: Translate this soview2D by the given x and y distances.
void
bgui_bmrf_epi_seg_soview2D::translate(float x, float y)
{
  // WARNING - This updates x,y position of each point but DOES NOT
  //           adjust any other dependent variables such as s and alpha
  vcl_vector<bmrf_epi_point_sptr>::const_iterator p_itr;
  for(p_itr = seg_sptr_->begin(); p_itr != seg_sptr_->end(); ++p_itr){
    (*p_itr)->set( (*p_itr)->x()+x, (*p_itr)->y()+y );
  }
}


vgui_soview2D_image* 
bgui_bmrf_epi_seg_soview2D::make_intensity_view(const bmrf_epipole& epipole) const
{
  double min_a = seg_sptr_->min_alpha(), max_a = seg_sptr_->max_alpha();
  double da = (max_a - min_a)/seg_sptr_->n_pts();
  int min_u = vcl_numeric_limits<int>::max();
  int max_u = vcl_numeric_limits<int>::min();
  int min_v = min_u, max_v = max_u;
  for (double a = min_a; a<=max_a; a+=da)
  {
    double ld = seg_sptr_->left_ds(a);
    double rd = seg_sptr_->right_ds(a);
    double s = seg_sptr_->s(a);
    double u,v;
    epipole.to_img_coords(s-ld, a, u, v);
    int ui = (int)(u+0.5), vi = (int)(v+0.5);
    min_u = vcl_min(min_u, ui);  max_u = vcl_max(max_u, ui);
    min_v = vcl_min(min_v, vi);  max_v = vcl_max(max_v, vi);
    
    epipole.to_img_coords(s+rd, a, u, v);
    ui = (int)(u+0.5); vi = (int)(v+0.5);
    min_u = vcl_min(min_u, ui);  max_u = vcl_max(max_u, ui);
    min_v = vcl_min(min_v, vi);  max_v = vcl_max(max_v, vi);
  }
  
  vil_image_view< vxl_byte> img( max_u - min_u +1, 
                                 max_v - min_v +1, 
                                 4 );
  img.fill(0);
  for (double a = min_a; a<=max_a; a+=da)
  {
    double ld = seg_sptr_->left_ds(a);
    vxl_byte li = vxl_byte(seg_sptr_->left_int(a)*255.0);
    double rd = seg_sptr_->right_ds(a);
    vxl_byte ri = vxl_byte(seg_sptr_->right_int(a)*255.0);
    double s = seg_sptr_->s(a);

    for(int i=0; i<=(int)ld; ++i){
      double u,v;
      epipole.to_img_coords(s-i, a, u, v);
      int ui = (int)(u+0.5)-min_u, vi = (int)(v+0.5)-min_v;
      img(ui,vi,0) = li;
      img(ui,vi,1) = li;
      img(ui,vi,2) = li;
      img(ui,vi,3) = 255;
    }
    for(int i=0; i<=(int)rd; ++i){
      double u,v;
      epipole.to_img_coords(s+i, a, u, v);
      int ui = (int)(u+0.5)-min_u, vi = (int)(v+0.5)-min_v;
      img(ui,vi,0) = ri;
      img(ui,vi,1) = ri;
      img(ui,vi,2) = ri;
      img(ui,vi,3) = 255;
    }
    
  }
  return new vgui_soview2D_image((float)min_u, (float)min_v, img, true);
  
}
