#include "bwm_observer.h"
#include "bwm_observable.h"

#include <vgui/vgui_message.h>
//#include <vgui/vgui_soview2D.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_3d.h>

void bwm_observer::update (vgui_message const& msg) 
{
  vgui_message m = const_cast <vgui_message const& > (msg);
  
  const bwm_observable* o = static_cast<const bwm_observable*> (m.from);
  bwm_observable* observable = const_cast<bwm_observable*> (o);
  handle_update(msg, observable);
}



//: returns the vertex (x,y) values of a 2D polygon in seperate x and y arrays 
void bwm_observer::get_vertices_xy(vsol_polygon_2d_sptr poly2d, 
                                          float **x, float **y)
{
  int n = poly2d->size();
  *x = (float*) malloc(sizeof(float) * n);
  *y = (float*) malloc(sizeof(float) * n);
  for (int i=0; i<n; i++) {
    (*x)[i] = poly2d->vertex(i)->x();
    (*y)[i] = poly2d->vertex(i)->y();
   //vcl_cout << "X=" << poly2d->vertex(i)->x() << " Y=" << poly2d->vertex(i)->y() << vcl_endl;
  }
}

void bwm_observer::get_vertices_xyz(vsol_polygon_3d_sptr poly3d, 
                                          double **x, double **y, double **z)
{
  int n = poly3d->size();
  *x = (double*) malloc(sizeof(double) * n);
  *y = (double*) malloc(sizeof(double) * n);
  *z = (double*) malloc(sizeof(double) * n);
  for (int i=0; i<n; i++) {
    (*x)[i] = poly3d->vertex(i)->x();
    (*y)[i] = poly3d->vertex(i)->y();
    (*z)[i] = poly3d->vertex(i)->z();
    //vcl_cout << i << " " << *(poly3d->vertex(i)) << vcl_endl;
  }
}
