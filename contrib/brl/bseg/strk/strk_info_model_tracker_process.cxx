//----*-c++-*----tells emacs to use C++ mode----------
// This is brl/strk/strk_info_model_tracker_process.cxx
#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vnl/vnl_matlab_print2.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_vertex.h>
#include <vtol/Vtol_edge.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_face_2d.h>
#include <vil1/vil1_memory_image_of.h>
#include <strk/strk_tracking_face_2d.h>
#include <strk/strk_art_info_model_sptr.h>
#include <strk/strk_art_info_model.h>
#include <strk/strk_info_model_tracker.h>
#include <strk/strk_info_model_tracker_process.h>

strk_info_model_tracker_process::strk_info_model_tracker_process(strk_info_model_tracker_params & tp)
 : model_tracker_(tp) 
{
  failure_ = false;
  first_frame_ = true;
  start_frame_=0;
  write_tracked_models_ = false;
}

strk_info_model_tracker_process::~strk_info_model_tracker_process()
{
  //  model_tracker_.clear();
}

bool strk_info_model_tracker_process::execute()
{
  if(failure_)
    {
      vcl_cout << "In strk_info_model_tracker_process::execute() - process failed\n";
      return false;
    }
  if (this->get_N_input_images()!=1)
    {
      vcl_cout << "In strk_info_model_tracker_process::execute() - not exactly one"
               << " input image \n";
      failure_ = true;
      return false;
    }
  output_topo_objs_.clear();
  //assume the input images are grey scale (should really check)
  vil1_memory_image_of<unsigned char> img(vpro_video_process::get_input_image(0));
  input_images_.clear();
  if(first_frame_)
    {
      model_tracker_.set_image_0(img);
      if(!get_N_input_topo_objs())
        {
          vcl_cout << "In strk_info_model_tracker_process::execute() - no input"
                   << "correlation face  \n";
          failure_ = true;
          return false;
        }
      if(input_topo_objs_.size()!=3)
        {
          vcl_cout << "In strk_info_model_tracker_process::execute() - input"
                   << "doesn't have the right number of faces \n";
          failure_ = true;
          return false;
        }
      //set the intial model 
      vcl_vector<vtol_face_2d_sptr> model_faces;
      for(vcl_vector<vtol_topology_object_sptr>::iterator toit = 
            input_topo_objs_.begin(); toit != input_topo_objs_.end(); toit++)
        {
          vtol_topology_object_sptr to = (*toit);
          vtol_face_sptr f = to->cast_to_face();
          vtol_face_2d_sptr f2d = f->cast_to_face_2d();
          if(!f2d)
            { vcl_cout << "In strk_info_model_tracker_process::execute() - input"
                       << "is not a vtol_face_2d  \n";
            failure_ = true;
            return false;
            }
          model_faces.push_back(f2d);
        }
      start_frame_ = this->frame_index();
      tracked_models_.clear();
      model_tracker_.set_initial_model(model_faces);
      model_tracker_.init();
      //output the model for display
      for(vcl_vector<vtol_face_2d_sptr>::iterator fit = model_faces.begin();
            fit != model_faces.end(); fit++)
        {
          vtol_face_2d_sptr f2d = (*fit);
		  vcl_vector<vtol_edge_sptr> edges_2d;
          f2d->edges(edges_2d);
          for(vcl_vector<vtol_edge_sptr>::iterator eit = edges_2d.begin();
          eit != edges_2d.end(); eit++)
            {
              vtol_topology_object_sptr to = (*eit)->cast_to_edge();
              output_topo_objs_.push_back(to);
            }
        }
      first_frame_ = false;
      return true;
    }
  //the regular loop
  model_tracker_.set_image_i(img);
  model_tracker_.track();
  output_topo_objs_.clear();
  //display the result
  strk_art_info_model_sptr mod  = model_tracker_.get_best_sample();
  tracked_models_.push_back(mod);
  vcl_vector<vtol_face_2d_sptr> tracked_faces = mod->vtol_faces();
  for(vcl_vector<vtol_face_2d_sptr>::iterator fit = tracked_faces.begin();
      fit != tracked_faces.end(); fit++)
    {
      vcl_vector<vtol_edge_sptr> edges;
      (*fit)->edges(edges);
      for(vcl_vector<vtol_edge_sptr>::iterator eit = edges.begin();
          eit != edges.end(); eit++)
        {
          vtol_topology_object_sptr to = (*eit)->cast_to_edge();
          output_topo_objs_.push_back(to);
        }
    }
  return true;
}
//get the matrix elements for the current frame of the cog and face verts
static void cache_face_and_cog(vtol_face_2d_sptr const& face,
                               const int frame_index,
                               vnl_matrix<double>& cog,
                               vnl_matrix<double>& X,
                               vnl_matrix<double>& Y)

  
{
  int n_verts = 0;
  if(!face)
    return;
  vcl_vector<vtol_vertex_sptr> verts;
  face->vertices(verts);
  n_verts = verts.size();
  if(!n_verts)
    return;
  double cog_x =0, cog_y=0;
  for(int i = 0; i<n_verts; i++)
    {
      vtol_vertex_2d_sptr v = verts[i]->cast_to_vertex_2d();
      if(!v)
        continue;
      cog_x += v->x(); cog_y += v->y();
      X.put(frame_index, i, v->x());
      Y.put(frame_index, i, v->y());
    }
  cog_x/=n_verts; cog_y/=n_verts; 
  cog.put(frame_index, 0, cog_x); cog.put(frame_index, 1, cog_y);
}  
// write out the tracked model for later display
bool strk_info_model_tracker_process::finish()
{
  if(write_tracked_models_)
    {
      int n_frames = tracked_models_.size();
      if(!n_frames)
        return false;
      vcl_vector<vtol_vertex_sptr> verts;
      strk_art_info_model_sptr mod = tracked_models_[0];
      vtol_face_2d_sptr stem = mod->face(0)->face()->cast_to_face_2d();
      vtol_face_2d_sptr long_arm = mod->face(1)->face()->cast_to_face_2d();
      vtol_face_2d_sptr short_arm = mod->face(2)->face()->cast_to_face_2d();
      stem->vertices(verts);
      int stem_n_verts = verts.size();
      verts.clear();
      long_arm->vertices(verts);
      int long_arm_n_verts = verts.size();
      verts.clear();
      short_arm->vertices(verts);
      int short_arm_n_verts = verts.size();
      verts.clear();
      //The matrices for cogs and vertices
      //will be printed using matlab printing format
      vnl_matrix<double> cog_stem(n_frames, 2);
      vnl_matrix<double> cog_long_arm(n_frames, 2);
      vnl_matrix<double> cog_short_arm(n_frames, 2);
      vnl_matrix<double> stem_X(n_frames, stem_n_verts);
      vnl_matrix<double> stem_Y(n_frames, stem_n_verts);
      vnl_matrix<double> long_arm_X(n_frames, long_arm_n_verts);
      vnl_matrix<double> long_arm_Y(n_frames, long_arm_n_verts);
      vnl_matrix<double> short_arm_X(n_frames, short_arm_n_verts);
      vnl_matrix<double> short_arm_Y(n_frames, short_arm_n_verts);
      vcl_ofstream track_stream(track_file_.c_str());
      if(!track_stream.is_open())
        {
          vcl_cout << "In strk_info_tracker_process::finish() -"
                   << " could not open file " << track_file_ << "\n";
          return false;
        }
      track_stream << "START_FRAME: " << start_frame_ << "\n";
      track_stream << "N_FRAMES: " << n_frames << "\n";
      for(int i = 0; i<n_frames; i++)
        {
          mod = tracked_models_[i];

          stem = mod->face(0)->face()->cast_to_face_2d();
          cache_face_and_cog(stem, i, cog_stem, stem_X, stem_Y);           

          long_arm = mod->face(1)->face()->cast_to_face_2d();
          cache_face_and_cog(long_arm,i, cog_long_arm, long_arm_X, long_arm_Y);

          short_arm = mod->face(2)->face()->cast_to_face_2d();
          cache_face_and_cog(short_arm, i, cog_short_arm,
                             short_arm_X, short_arm_Y);
        }

      track_stream << "STEM: \n";
      track_stream << "N_VERTS: " << stem_n_verts << "\n";      
      track_stream << "COG: \n";
      vnl_matlab_print(track_stream, cog_stem);
      track_stream << "X: \n";
      vnl_matlab_print(track_stream, stem_X);
      track_stream << "Y: \n";
      vnl_matlab_print(track_stream, stem_Y);

      track_stream << "LONG_ARM: \n";
      track_stream << "N_VERTS: " << long_arm_n_verts << "\n";      
      track_stream << "COG: \n";
      vnl_matlab_print(track_stream, cog_long_arm);
      track_stream << "X: \n";
      vnl_matlab_print(track_stream, long_arm_X);
      track_stream << "Y: \n";
      vnl_matlab_print(track_stream, long_arm_Y);

      track_stream << "SHORT_ARM: \n";
      track_stream << "N_VERTS: " << short_arm_n_verts << "\n";      
      track_stream << "COG: \n";
      vnl_matlab_print(track_stream, cog_short_arm);
      track_stream << "X: \n";
      vnl_matlab_print(track_stream, short_arm_X);
      track_stream << "Y: \n";
      vnl_matlab_print(track_stream, short_arm_Y);

      track_stream.close();
    }
  return true;
}
          
bool strk_info_model_tracker_process::set_output_file(vcl_string const& file_name)
{
  write_tracked_models_ = true;
  track_file_ = file_name;
  vcl_ofstream track_stream(track_file_.c_str());
  if(!track_stream.is_open())
    {
      vcl_cout << "In strk_info_tracker_process::set_output_file() -"
               << " could not open file " << track_file_ << "\n";
      return false;
    }
  track_stream.close();
  return true;
}

          
