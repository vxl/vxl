//----*-c++-*----tells emacs to use C++ mode----------
// This is brl/bseg/strk/strk_art_info_model.cxx
#include <strk/strk_art_info_model.h>
#include <strk/strk_tracking_face_2d.h>
#include <btol/btol_face_algs.h>
#include <vsol/vsol_point_2d.h>

//:
// \file
// See strk_art_info_model.h
//
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------
// Constructors
//
strk_art_info_model::strk_art_info_model()
{
}

double strk_art_info_model::arm_radius(strk_tracking_face_2d_sptr const& face)
{
  if(!face)
    return 0;
  double fx0 = 0, fy0 = 0;
  face->centroid(fx0, fy0);
  double px0 = stem_pivot_->x(), py0 = stem_pivot_->y();
  return vcl_sqrt((fx0-px0)*(fx0-px0)+ (fy0-py0)*(fy0-py0));
}

strk_art_info_model::
strk_art_info_model(vcl_vector<vtol_face_2d_sptr> const& faces,
                      vsol_point_2d_sptr const stem_pivot,
                    vil1_memory_image_of<float> & image)
{
	for(vcl_vector<vtol_face_2d_sptr>::const_iterator fit = faces.begin();
      fit != faces.end(); fit++)
    faces_.push_back(new strk_tracking_face_2d(*fit, image));
  stem_pivot_ = stem_pivot;
  //compute the arm radii
  long_arm_radius_ = arm_radius(faces_[LONG_TIP]);
  short_arm_radius_ = arm_radius(faces_[SHORT_TIP]);
}

strk_art_info_model::
strk_art_info_model(vcl_vector<vtol_face_2d_sptr> const& faces,
                    vsol_point_2d_sptr const stem_pivot,
                    vil1_memory_image_of<float> & image,
                    vil1_memory_image_of<float>& Ix,
                    vil1_memory_image_of<float>& Iy)
{
	for(vcl_vector<vtol_face_2d_sptr>::const_iterator fit = faces.begin();
      fit != faces.end(); fit++)
    faces_.push_back(new strk_tracking_face_2d(*fit, image, Ix, Iy));
  stem_pivot_ = stem_pivot;
  //compute the arm radii
  long_arm_radius_ = arm_radius(faces_[LONG_TIP]);
  short_arm_radius_ = arm_radius(faces_[SHORT_TIP]);
}

strk_art_info_model::strk_art_info_model(strk_art_info_model_sptr const& im)
{
  if(!im)
    return;
  for(int i = 0; i<im->n_faces(); i++)
    faces_.push_back(new strk_tracking_face_2d(im->face(i)));
  stem_pivot_ = im->stem_pivot_;
  long_arm_radius_ = im->long_arm_radius_;
  short_arm_radius_ = im->short_arm_radius_;
}

strk_art_info_model::~strk_art_info_model()

{
}

bool strk_art_info_model::
pivot_tip_face(strk_tracking_face_2d_sptr const& face,
			   const double angle, const double radius)
{
  if(!face||!radius)
    return false;
  if(!angle)
    return true;
  double fx0 = 0, fy0 = 0;
  face->centroid(fx0, fy0);
  double px0 = stem_pivot_->x(), py0 = stem_pivot_->y();

  //get the current orientation of the arm
  double c = (fx0-px0)/radius, s = (fy0-py0)/radius;
  double theta = vcl_atan2(s, c);
  theta += angle;
  double new_fx0 = radius*vcl_cos(theta)+px0, new_fy0 = radius*vcl_sin(theta)+py0;
  double tx = new_fx0-fx0, ty = new_fy0-fy0;
  face->transform(tx, ty, 0, 1.0);
  return true;
}
//:transform the articulated model
bool strk_art_info_model::transform(const double stem_tx,
                                    const double stem_ty, 
                                    const double stem_angle,
                                    const double long_arm_pivot_angle,
                                    const double short_arm_pivot_angle, 
                                    const double long_tip_angle,
                                    const double short_tip_angle)
{
//   vcl_cout << " long_radius btrans "<< arm_radius(faces_[LONG_TIP]) 
//            << " short_radius btrans"<< arm_radius(faces_[SHORT_TIP]) << "\n";

  // move the pivot
  if(!stem_pivot_)
    return false;
//   stem_pivot_->set_x(stem_pivot_->x()+stem_tx);
//   stem_pivot_->set_y(stem_pivot_->y()+stem_ty);

  //transform all the faces since the stem transform is considered global
  //for now we assume that the stem_pivot is the same as the centroid
  //of the stem face (It is being constructed that way at present, although
  //the option remains to have the pivot independently defined).
 for(vcl_vector<strk_tracking_face_2d_sptr>::iterator fit = faces_.begin();
      fit != faces_.end(); fit++)
    if(!(*fit))
      return false;
    else
      (*fit)->transform(stem_tx, stem_ty, stem_angle, 1.0);

  stem_pivot_ = btol_face_algs::centroid(faces_[STEM]->face()->cast_to_face_2d()); 
//   vcl_cout << "Stem Pivot (" << stem_pivot_->x() << " " << stem_pivot_->y()
//           << ") Stem COG(" << cog->x() << " " << cog->y() << ")\n";

 //pivot the tips by the same stem angle
 if(!this->pivot_tip_face(faces_[LONG_TIP],stem_angle,
                          long_arm_radius_))
   return false;

 if(!this->pivot_tip_face(faces_[SHORT_TIP], stem_angle,
                          short_arm_radius_))
	return false;
//   vcl_cout << " long_radius_atrans "<< arm_radius(faces_[LONG_TIP]) 
//            << " short_radius_atrans "<< arm_radius(faces_[SHORT_TIP]) << "\n";

  //transform the arms around the pivot
  //this just translates the tip faces since we assume their orientation
  //only changes by pivoting around their centroid
  if(!this->pivot_tip_face(faces_[LONG_TIP],long_arm_pivot_angle, long_arm_radius_))
	return false;

  if(!this->pivot_tip_face(faces_[SHORT_TIP], short_arm_pivot_angle,
	  short_arm_radius_))
	return false;
  
  //rotate the tip faces around their centroids
  faces_[LONG_TIP]->transform(0.0, 0.0, long_tip_angle, 1.0);
  faces_[SHORT_TIP]->transform(0.0, 0.0, short_tip_angle, 1.0);

  return true;
}
vcl_vector<vtol_face_2d_sptr> strk_art_info_model::vtol_faces()
{
  vcl_vector<vtol_face_2d_sptr> vtol_faces;
  for(vcl_vector<strk_tracking_face_2d_sptr>::iterator fit = faces_.begin();
      fit != faces_.end(); fit++)
    {
      vtol_face_2d_sptr f = (*fit)->face()->cast_to_face_2d();
      vtol_faces.push_back(f);
    }
  return vtol_faces;
}

bool strk_art_info_model::
compute_mutual_information(vil1_memory_image_of<float> const& image)
{
  if(!image)
    return false;
  vil1_memory_image_of<float> null_x, null_y;//dummy args (replace with deflts)
  double model_info = 0.0;
  for(vcl_vector<strk_tracking_face_2d_sptr>::iterator fit =  faces_.begin();
      fit != faces_.end(); fit++)
    {
      if(!(*fit)->compute_mutual_information(image, null_x, null_y));
      continue;
      model_info += (*fit)->total_info();
    }
  total_model_info_ = model_info;
  return true;
}


bool strk_art_info_model::
compute_mutual_information(vil1_memory_image_of<float> const& image,
                           vil1_memory_image_of<float> const& Ix,
                           vil1_memory_image_of<float> const& Iy)
{
  if(!image||!Ix||!Iy)
    return false;
  double model_info = 0.0;
  for(vcl_vector<strk_tracking_face_2d_sptr>::iterator fit =  faces_.begin();
      fit != faces_.end(); fit++)
    {
      if(!(*fit)->compute_mutual_information(image, Ix, Iy))
        continue;
      model_info += (*fit)->total_info();
    }
  total_model_info_ = model_info;
  return true;
}
