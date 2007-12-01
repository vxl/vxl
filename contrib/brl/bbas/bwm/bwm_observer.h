#ifndef bwm_observer_h_
#define bwm_observer_h_

#include "bwm_observable.h"

#include <vcl_vector.h>

#include <vgui/vgui_observer.h>

#include <vgl/vgl_homg_plane_3d.h>

#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_3d_sptr.h>

class bwm_observer : public vgui_observer
{
public:

  bwm_observer() {}

  virtual ~bwm_observer(){}
 
  virtual vcl_string type_name() const { return "bwm_observer"; }

  virtual void update(vgui_message const& msg);

  virtual void handle_update(vgui_message const& msg, 
    bwm_observable_sptr observable)=0; 

  virtual void add_new_obj(bwm_observable_sptr observable){}

  //virtual void update_all()=0;

  //: Translates the 3D objects by vector T
  //virtual void translate(vgl_vector_3d<double> T)=0;

  //: connect the the given face as an inner face to the selected face
  virtual void connect_inner_face(vsol_polygon_2d_sptr poly)=0;

  //: create the interior of an objects, as a smaller copy of the outer 
  // object
  virtual void create_interior()=0;

  //: Select a polygon before you call this method. If it is a multiface 
  // object, it deletes the object where the selected polygon belongs to
  virtual void delete_object()=0;

  //: Deletes the whole set of objects created so far
  virtual void delete_all()=0;

protected:

};

#endif
