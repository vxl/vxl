// This is brl/bseg/strk/strk_art_model_display_process.cxx
#include "strk_art_model_display_process.h"
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_vertex.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_face_2d.h>

strk_art_model_display_process::strk_art_model_display_process()
{
  failure_ = false;
  first_frame_ = true;
}

strk_art_model_display_process::~strk_art_model_display_process()
{
}

static void construct_face_and_cog(const int n_verts,
                                   const int frame_index,
                                   vnl_matrix<double> const& cogs,
                                   vnl_matrix<double> const& X,
                                   vnl_matrix<double> const& Y,
                                   vtol_face_2d_sptr & face,
                                   vtol_vertex_2d_sptr& cog)
{
  vcl_vector<vtol_vertex_sptr> verts;
  for (int i = 0; i<n_verts; i++)
  {
    vtol_vertex_2d* v2d  = new vtol_vertex_2d(X[frame_index][i], Y[frame_index][i]);
        verts.push_back((vtol_vertex*)v2d);
  }
  face = new vtol_face_2d(verts);
  vgl_point_2d<double> p(cogs[frame_index][0], cogs[frame_index][1]);
  vtol_vertex_2d_sptr temp = new vtol_vertex_2d(p.x(), p.y());
  cog = temp;
}


bool strk_art_model_display_process::input_tracked_models(vcl_ifstream & str)
{
  int n_frames = 0;
  vtol_face_2d_sptr stem;
  vtol_face_2d_sptr long_arm;
  vtol_face_2d_sptr short_arm;
  int stem_n_verts =0, long_arm_n_verts = 0, short_arm_n_verts =0;

  vcl_string sf, nf, nv;
  str >> sf;
  if (sf!="START_FRAME:")
    return false;
  str >> start_frame_;
  str >> nf;
  if (nf!="N_FRAMES:")
    return false;
  str >> n_frames;

  vcl_string st, cg, la, sa, x, y;
  //============================ STEM ==========================
  str >> st;
  if (st!="STEM:")
    return false;
  str >> nv;
  if (nv!="N_VERTS:")
    return false;
  str >> stem_n_verts;
  str >> cg;
  if (cg!="COG:")
    return false;
  vnl_matrix<double> cog_stem(n_frames, 2);
  vnl_matrix<double> stem_X(n_frames, stem_n_verts);
  vnl_matrix<double> stem_Y(n_frames, stem_n_verts);
  str >> cog_stem;
  str >> x;
  if (x!="X:")
    return false;
  str >> stem_X;
  str >> y;
  if (y!="Y:")
    return false;
  str >> stem_Y;
  //=============== Long Arm =========================
  str >> la;
  if (la!="LONG_ARM:")
    return false;
  str >> nv;
  if (nv!="N_VERTS:")
    return false;
  str >> long_arm_n_verts;
  str >> cg;
  if (cg!="COG:")
    return false;
  vnl_matrix<double> cog_long_arm(n_frames, 2);
  vnl_matrix<double> long_arm_X(n_frames, long_arm_n_verts);
  vnl_matrix<double> long_arm_Y(n_frames, long_arm_n_verts);
  str >> cog_long_arm;
  str >> x;
  if (x!="X:")
    return false;
  str >> long_arm_X;
  str >> y;
  if (y!="Y:")
    return false;
  str >> long_arm_Y;

  //=============== Short Arm =========================
  str >> sa;
  if (sa!="SHORT_ARM:")
    return false;
  str >> nv;
  if (nv!="N_VERTS:")
    return false;
  str >> short_arm_n_verts;
  str >> cg;
  if (cg!="COG:")
    return false;
  vnl_matrix<double> cog_short_arm(n_frames, 2);
  vnl_matrix<double> short_arm_X(n_frames, short_arm_n_verts);
  vnl_matrix<double> short_arm_Y(n_frames, short_arm_n_verts);
  str >> cog_short_arm;
  str >> x;
  if (x!="X:")
    return false;
  str >> short_arm_X;
  str >> y;
  if (y!="Y:")
    return false;
  str >> short_arm_Y;
  // end of input

  //============  Construct the articulated model=======================
  // for now just make faces
  for (int i = 0; i<n_frames; i++)
  {
    vtol_vertex_2d_sptr c;
    construct_face_and_cog(stem_n_verts, i,
                           cog_stem, stem_X, stem_Y,
                           stem, c);

    construct_face_and_cog(long_arm_n_verts, i,
                           cog_long_arm, long_arm_X, long_arm_Y,
                           long_arm, c);

    construct_face_and_cog(short_arm_n_verts, i,
                           cog_short_arm, short_arm_X, short_arm_Y,
                           short_arm, c);
    vcl_vector<vtol_face_2d_sptr> mod;
    mod.push_back(stem);  mod.push_back(long_arm); mod.push_back(short_arm);
    tracked_models_.push_back(mod);
  }
  return true;
}

bool strk_art_model_display_process::execute()
{
  if (failure_)
    return false;
  if (this->get_N_input_images()!=1)
  {
    vcl_cout << "In strk_art_model_display_process::execute() -"
             << " not exactly one input image\n";
    failure_ = true;
    return false;
  }
  output_topo_objs_.clear();
  input_images_.clear();
  static int n_models = 0;
  if (first_frame_)
  {
    first_frame_ = false;

    vcl_ifstream str(track_file_.c_str());
    if (!str)
    {
      vcl_cout << "In strk_art_model_display_process::execute() -"
               << " could not open file " << track_file_ << "\n";
      failure_ = true;
      return false;
    }

    this->input_tracked_models(str);
    n_models = tracked_models_.size();
    if (!n_models)
    {
      vcl_cout << "In strk_art_model_display_process::execute() -"
               << " no models found in track file\n";
      failure_ = true;
    }
  }
  int frame_index = this->frame_index();
  int offset = frame_index-start_frame_;
  if (offset<0||offset>=n_models)
    return false;
  vcl_vector<vtol_face_2d_sptr> tracked_faces = tracked_models_[offset];
  for (vcl_vector<vtol_face_2d_sptr>::iterator fit = tracked_faces.begin();
       fit != tracked_faces.end(); fit++)
    {
        vcl_vector<vtol_edge_sptr> edges;
      (*fit)->edges(edges);
      for (vcl_vector<vtol_edge_sptr>::iterator eit = edges.begin();
           eit != edges.end(); eit++)
        {
          vtol_topology_object_sptr to = (*eit)->cast_to_edge();
          output_topo_objs_.push_back(to);
        }
    }
  return true;
}

bool strk_art_model_display_process::set_input_file(vcl_string const& file_name)
{
  track_file_ = file_name;
  if (track_file_=="")
    return false;
  vcl_ifstream str(track_file_.c_str());
  if (!str)
  {
    vcl_cout << "In strk_art_model_display_process::set_input_file() -"
             << " could not open file " << track_file_ << "\n";
    return false;
  }
  str.close();
  return true;
}
