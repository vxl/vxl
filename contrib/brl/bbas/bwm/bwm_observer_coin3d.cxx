#include "bwm_observer_coin3d.h"
#include <bwm/algo/bwm_algo.h>

#include <vgl/vgl_homg_plane_3d.h> 
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_line_3d_2_points.h> 
#include <vgl/vgl_closest_point.h> 
#include <vgl/algo/vgl_homg_operators_3d.h>
#include <vgl/algo/vgl_fit_plane_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_det.h>
#include <vnl/algo/vnl_matrix_inverse.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>

#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_point_3d_sptr.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/algo/vpgl_backproject.h>

#include <vgui/vgui_message.h>

#include <bgui3d/bgui3d_algo.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMatrixTransform.h>
#include <Inventor/nodes/SoIndexedFaceSet.h> 
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/events/SoMouseButtonEvent.h> 
#include <Inventor/SoPath.h>  
#include <Inventor/SoPickedPoint.h> 
#include <Inventor/misc/SoChildList.h> 
#include <Inventor/details/SoDetail.h>
#include <Inventor/details/SoFaceDetail.h>

#include <bmsh3d/algo/bmsh3d_fileio.h>

//vcl_vector<SoNode*> bwm_observer_coin3d::selected_nodes = vcl_vector<SoNode*> (0);
vcl_vector<const SoPickedPoint*> bwm_observer_coin3d::picked_points = vcl_vector<const SoPickedPoint*> (0);

bwm_observer_coin3d::bwm_observer_coin3d(vpgl_proj_camera<double> *camera, SoNode* root)
: bgui3d_examiner_tableau(root), camera_(camera), num_objs_(0), T_(0), 
obs_picked_(0), divide_mode_(false)
{ 
  root_ = (SoSeparator*) root; 
  root_sel_ = new SoSelection;
  root_sel_->ref();
  root_->addChild(root_sel_);
  root_sel_->policy = SoSelection::SHIFT;
 
}
 
bwm_observer_coin3d::~bwm_observer_coin3d() 
{
  if (camera_) 
    delete camera_;
  if (T_)
    delete T_;

}

void bwm_observer_coin3d::extrude()
{
  obs_picked_ = find_selected_obs(face_id);
  if ((obs_picked_) && (face_id > -1))
    obs_picked_->extrude(face_id);
}

void bwm_observer_coin3d::divide()
{
  obs_picked_ = find_selected_obs(face_id);
  divide_mode_ = true;
  div_pts_.resize(2);
  div_idx_ = 0;
}

void bwm_observer_coin3d::handle_update(vgui_message const& msg, 
                                  bwm_observable_sptr observable) 
{
  const vcl_string* str = static_cast<const vcl_string*> (msg.data);
 
  // tranform the mesh by the rational camera norm_trans()
  bwm_observable_sptr obs = 0;
  obs = observable;
  if (T_) {
    // tranform the mesh by the rational camera norm_trans()
    obs = observable->transform(*T_);
  }

  if (str->compare("new") == 0) {
    // 4 is color blue
    SoSeparator* obj = obs->convert_coin3d(false, 0.0, 2); // = draw_mesh (M, false, 0.0, COLOR_BLUE);
    vcl_string name = create_obj_name();
    obj->setName(SbName(name.data()));

    SoTransform *myTransform = new SoTransform;
    myTransform->translation.setValue(0, 0, 0);
    myTransform->ref();
    obj->insertChild(myTransform, 0);
    root_sel_->addChild(obj);

    objects[observable] = obj;  
  } else if (str->compare("move") == 0) {
    
    // find the translation between the new and the old position
    SoSeparator *m = objects[observable];
    SoTransform *myTransform = (SoTransform *) m->getChild(0);//new SoTransform;
    SbVec3f tr = myTransform->translation.getValue();
    double x = observable->last_translation().x();
    double y = observable->last_translation().y();
    double z = observable->last_translation().z();
    //vcl_cout << "X=" << x << " Y=" << y << " Z=" << z << vcl_endl;
    float x1, y1, z1;
    tr.getValue(x1, y1, z1);
    myTransform->translation.setValue(x1+x, y1+y, z1+z);
    this->render();
    
  } else if (str->compare("update") == 0) {
    SoSeparator *m = objects[observable];
    //bmsh3d_mesh_mc* prev_mesh = meshes[observable];
    SoSeparator* group = (SoSeparator*)this->user_scene_root();
    SbName n = m->getName();
    vcl_cout << n.getString() << vcl_endl;
    SoSeparator* node = (SoSeparator *) SoNode::getByName(m->getName());
    root_sel_->removeChild(node);
    
    SoSeparator* obj = obs->convert_coin3d(false, 0.0, 2);//draw_mesh (M, true, 0.0, COLOR_GREEN);
    obj->setName(n);
    root_sel_->addChild(obj);
    objects[observable] = obj;
  
  }
  SoWriteAction myAction;
  myAction.getOutput()->openFile("C:\\test_images\\mesh\\root.iv");
  myAction.getOutput()->setBinary(FALSE);
  myAction.apply(root_sel_);
  myAction.getOutput()->closeFile();
  //bmsh3d_save_ply2 (objects[observable], "C:\\test_images\\mesh\\out.ply2");
    
}
  
bool bwm_observer_coin3d::handle(const vgui_event& e)
{
  //vcl_cout << "bwm_observer_coin3d::handle " << e.type << " " << e.button << " " << e.modifier << vcl_endl;
  if (e.type == vgui_BUTTON_DOWN && e.button == vgui_LEFT && e.modifier == vgui_CTRL) {
    SoRayPickAction rp( this->get_viewport_region());
    rp.setPoint(SbVec2s(e.wx, e.wy));
    rp.setRadius(10);
    rp.apply(this->scene_root());
    point_picked_ = rp.getPickedPoint();
    if (point_picked_) {
      SbVec3f point = point_picked_->getPoint();
      float x,y,z;
      point.getValue(x,y,z);
      vcl_cout << "[" << x << "," << y << "," << z << "]" << vcl_endl;
      if (divide_mode_) {
        div_pts_[div_idx_++] = vgl_point_3d<double> (x,y,z);
        if (div_idx_ > 1) {
          vgl_point_3d<double> point1, point2, l1, l2, l3, l4;
          find_intersection_points(face_id, div_pts_[0], div_pts_[1], point1, l1, l2, 
            point2, l3, l4);
          obs_picked_->divide_face(face_id, l1, l2, point1, l3, l4, point2);
          divide_mode_ = false;
        }
      } else {
        point_3d_.set(x, y, z);
        SoPath* path =  point_picked_->getPath();
        SoNode* node = path->getTail();
        if (node->isOfType(SoIndexedFaceSet::getClassTypeId())) {
         vcl_cout << "Found an Object!" << vcl_endl;
          node_selected_ = path->getNodeFromTail(1);
          //point_picked_ = pp;
        }
      }
    } else 
      vcl_cout << "PICKED POINT IS O" << vcl_endl;
  }
  if (e.type == vgui_BUTTON_DOWN && e.button == vgui_LEFT && e.modifier == vgui_SHIFT) {
    start_x_ = e.wx;
    start_y_ = e.wy;
    left_button_down_ = true;
    return true;
  
  } else if (e.type == vgui_BUTTON_UP && e.button == vgui_LEFT && e.modifier == vgui_SHIFT) {
    left_button_down_ = false;
    return true;
  
  } else if (e.type == vgui_MOTION && e.button == vgui_LEFT && e.modifier == vgui_SHIFT) {
   
    // first make sure that an object is selected from scene graph
    if (node_selected_) {
      double x = e.wx;
      double y = e.wy;
      double dist = (y-start_y_)/2;
  
      vcl_map<bwm_observable_sptr, SoSeparator*>::iterator iter = objects.begin();
      while (iter != objects.end()) {
        SoSeparator* sep = iter->second;
        if (node_selected_ == sep) {
          bwm_observable_sptr obs = iter->first;
          obs->move_normal_dir(dist);
        }
        iter++;
      }
      start_y_ = y;
    }
    
    return true;
  } else if (e.type == vgui_BUTTON_DOWN && e.button == vgui_RIGHT && e.modifier == vgui_SHIFT) {
      middle_button_down_ = true;
      start_x_ = e.wx;
      start_y_ = e.wy;
      
  } else if (e.type == vgui_MOTION && e.button == vgui_RIGHT && e.modifier == vgui_SHIFT) {
      // update these only if there is motion event
      float wx = e.wx;
      float wy = e.wy;
      
      // first make sure that an object is selected from scene graph
      if (node_selected_){
        double x = e.wx;
        double y = e.wy;
        double dist = (y-start_y_)/2;
        vcl_cout << dist << vcl_endl;
        start_y_ = y;
        //vcl_cout << "dist=" << dist << "= " << y << " - " << start_y_ << vcl_endl;

        if (obs_picked_)
            obs_picked_->move_extr_face(dist);
      }
      return true;
    } 
    else if (e.type == vgui_BUTTON_UP && e.button == vgui_RIGHT && e.modifier == vgui_SHIFT){
      middle_button_down_ = false;
      return true;
    }

  return bgui3d_examiner_tableau::handle(e);
}

bwm_observable_sptr bwm_observer_coin3d::find_selected_obs(int &fid)
{ 
  vcl_map<bwm_observable_sptr, SoSeparator*>::iterator iter = objects.begin();
  while (iter != objects.end()) {
    SoSeparator* sep = iter->second;
    vcl_cout << sep->getName().getString() << vcl_endl;
    vcl_cout << node_selected_->getName().getString() << vcl_endl;
    if (node_selected_ == sep) {
      bwm_observable_sptr obs = iter->first;
      fid = obs->find_closest_face(point_3d_);
      return obs;
    }
    iter++;
  }
  return 0;
}

//: given the points l1 and l2, finds the exact intersection points on the face with face_id
bool bwm_observer_coin3d::find_intersection_points(int id, 
                                                    vgl_point_3d<double> i1, 
                                                    vgl_point_3d<double> i2,
                                                    vgl_point_3d<double> &p1, 
                                                    vgl_point_3d<double>& l1, vgl_point_3d<double>& l2, // end points of the first polygon segment
                                                    vgl_point_3d<double> &p2,
                                                    vgl_point_3d<double>& l3, vgl_point_3d<double>& l4) // end points of the second polygon segment)
{
  if (obs_picked_) {
    vsol_polygon_3d_sptr poly3d = obs_picked_->extract_face(id);

    double *x_list, *y_list, *z_list;
    bwm_algo::get_vertices_xyz(poly3d, &x_list, &y_list, &z_list);
    double point1_x, point1_y, point1_z, point2_x, point2_y, point2_z;

    int edge_index1 = vgl_closest_point_to_closed_polygon(point1_x, point1_y, point1_z, 
        x_list, y_list, z_list, poly3d->size(),
        i1.x(), i1.y(), i1.z());
      
    int edge_index2 = vgl_closest_point_to_closed_polygon(point2_x, point2_y, point2_z, 
        x_list, y_list, z_list, poly3d->size(),
        i2.x(), i2.y(), i2.z());

    if (edge_index1 == edge_index2) {
      vcl_cerr << "bwm_observer_coin3d::find_intersection_points() -- Both points are on the same edge!!!" << vcl_endl;
      return false;
    }

    l1 = vgl_point_3d<double> (x_list[edge_index1], y_list[edge_index1], z_list[edge_index1]);
    int next_index = edge_index1+1;
    if (next_index == poly3d->size()) 
        next_index = 0;
      l2 = vgl_point_3d<double> (x_list[next_index], y_list[next_index], z_list[next_index]); 
      l3 = vgl_point_3d<double> (x_list[edge_index2], y_list[edge_index2], z_list[edge_index2]);
      next_index = edge_index2+1;
      if (edge_index2+1 == poly3d->size()) 
        next_index = 0;
      l4 = vgl_point_3d<double> (x_list[next_index], y_list[next_index], z_list[next_index]); 
      p1 = vgl_point_3d<double>(point1_x, point1_y, point1_z);
      p2 = vgl_point_3d<double>(point2_x, point2_y, point2_z);
      return true;
  }
  return false;
}

vcl_string bwm_observer_coin3d::create_obj_name()
{
   vcl_string base = "obj";

   vcl_stringstream strm;
   strm << vcl_fixed << num_objs_;
   vcl_string str(strm.str());
   num_objs_++;
   return (base+str);
}

