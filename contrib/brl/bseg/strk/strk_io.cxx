// This is brl/bseg/strk/strk_io.cxx
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matlab_print2.h>
#include <vgl/vgl_point_2d.h>
#include <vtol/vtol_vertex.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_face_2d.h>
#include <strk/strk_tracking_face_2d.h>
#include "strk_io.h"

bool strk_io::write_frame_data(const unsigned int start_frame,
                               const unsigned int n_frames,
                               vcl_ofstream& strm)
{
  if(!strm)
    return false;
  strm << "START_FRAME: " << start_frame << '\n'
       << "N_FRAMES: " << n_frames << '\n';
  return true;
}
bool strk_io::write_region_data(const unsigned int n_pix, const float diameter,
                                const float aspect_ratio,
                                vcl_ofstream& strm)
{
 if(!strm)
    return false;

 strm << "N_PIXELS: " << n_pix << '\n'
      << "DIAMETER: " << diameter << '\n'
      << "ASPECT_RATIO: " << aspect_ratio << '\n';
 return true;
}

bool strk_io::write_histogram_data(const unsigned int start_frame,
                                   const unsigned int n_pixels,
                                   const float diameter,
                                   const float aspect_ratio,
                                   const unsigned int n_int_bins,
                                   const unsigned int n_grad_dir_bins,
                                   const unsigned int n_color_bins,
                                   vcl_vector<vcl_vector<float> >const& data, 
                                   vcl_ofstream& strm)
{
  unsigned int n_frames = data.size();
  if(!n_frames)
    return false;
  //checks stream
  if(!strk_io::write_frame_data(start_frame, n_frames, strm))
    return false;
  if(!strk_io::write_region_data(n_pixels, diameter, aspect_ratio, strm))
    return false;
  strm << "N_INTENSITY_BINS: " << n_int_bins << '\n'
       << "N_GRADIENT_DIR_BINS: " << n_grad_dir_bins << '\n'
       << "N_COLOR_BINS: " << n_color_bins << '\n'
       << "HISTOGRAMS: " << '\n';
  unsigned int nbins = n_int_bins + n_grad_dir_bins + n_color_bins;
  vnl_matrix<float> temp(n_frames, nbins);
  for(unsigned int i = 0;i<n_frames; ++i)
    for(unsigned int j = 0; j<nbins; ++j)
      temp[i][j]=data[i][j];
  strm << temp << '\n';
  return true;
}

bool strk_io::
write_track_data(const unsigned int start_frame,
                 vcl_vector<vtol_face_2d_sptr> const& tracked_faces,
                 vcl_ofstream& strm)
{
  unsigned int n_frames = tracked_faces.size();
  if(!n_frames)
    return false;
  //this call checks the stream
  if(!write_frame_data(start_frame, n_frames, strm))
    return false;
  vcl_vector<vtol_vertex_sptr> verts;
  tracked_faces[0]->vertices(verts);
  unsigned int n_verts = verts.size();
  if(!n_verts)
    return false;
  strm << "N_VERTS: " << n_verts << '\n';
  vnl_matrix<double> cog(n_frames, 2);
  vnl_matrix<double> X(n_frames, n_verts);
  vnl_matrix<double> Y(n_frames, n_verts);
  for (unsigned int i = 0; i<n_frames; i++)
    {
      verts.clear();
      double cog_x = 0, cog_y = 0;
      tracked_faces[i]->vertices(verts);
      for (unsigned int j = 0; j<n_verts; j++)
        {
          vtol_vertex_2d_sptr v = verts[j]->cast_to_vertex_2d();
          if (!v)
            continue;
          cog_x += v->x(); cog_y += v->y();
          X.put(i,j,v->x());
          Y.put(i,j,v->y());
        }
      cog_x/=n_verts; cog_y/=n_verts;
      cog.put(i, 0, cog_x); cog.put(i, 1, cog_y);
    }
  strm << "COG:\n";
  vnl_matlab_print(strm, cog);
  strm << "X:\n";
  vnl_matlab_print(strm, X);
  strm << "Y:\n";
  vnl_matlab_print(strm, Y);
  return true;
}

bool strk_io::read_frame_data(vcl_ifstream& strm,
                              unsigned int& start_frame, unsigned int& n_frames)
{
  if(!strm)
    return false;
  vcl_string s;
  strm >> s;
  if (s!="START_FRAME:")
    return false;
  strm >> start_frame;
  strm >> s;
  if (s!="N_FRAMES:")
    return false;
  strm >> n_frames;
  return true;
}

bool strk_io::read_region_data(vcl_ifstream& strm,
                               unsigned int& n_pix, float& diameter,
                               float& aspect_ratio)
{
  if(!strm)
    return false;
  vcl_string s;
  strm >> s;
  if(s!="N_PIXELS:")
    return false;
  strm >> n_pix;
  strm >> s;
  if(s!="DIAMETER:")
    return false;
  strm >> diameter;
  strm >> s;
  if(s!="ASPECT_RATIO:")
    return false;
  strm >> aspect_ratio;
  return true;
}
bool strk_io::read_histogram_data(vcl_ifstream& strm, 
                                  unsigned int& start_frame,
                                  unsigned int& n_frames,
                                  unsigned int& n_pixels,
                                  float& diameter,
                                  float& aspect_ratio,
                                  unsigned int& n_int_bins,
                                  unsigned int& n_grad_dir_bins,
                                  unsigned int& n_color_bins,
                                  vnl_matrix<float>& hist_data)
{
  //checks stream
  if(!strk_io::read_frame_data(strm, start_frame, n_frames))
    return false;
  if(!strk_io::read_region_data(strm, n_pixels, diameter, aspect_ratio))
    return false;
  vcl_string s;
  strm >> s;
  if(s!="N_INTENSITY_BINS:")
    return false;
  strm >> n_int_bins;
  strm >> s;
  if(s!="N_GRADIENT_DIR_BINS:")
    return false;
  strm >> n_grad_dir_bins;
  strm >> s;
  if(s!="N_COLOR_BINS:")
    return false;
  strm >> n_color_bins;
  strm >> s;
  if(s!="HISTOGRAMS:")
    return false;
  unsigned int nbins = n_int_bins + n_grad_dir_bins + n_color_bins;
  hist_data = vnl_matrix<float>(n_frames,nbins);
  strm >> hist_data;
  return true;
}

bool strk_io::read_track_data(vcl_ifstream& strm,
                              unsigned int& start_frame,
                              unsigned int& n_frames,
                              vcl_vector<vgl_point_2d<double> >& cogs,
                              vcl_vector<vtol_face_2d_sptr>& tracked_faces)
{
  //checks stream as well
  if(!strk_io::read_frame_data(strm, start_frame, n_frames))
    return false;
  vcl_string s;
  strm >> s;
  if (s!="N_VERTS:")
    return false;
  unsigned int n_verts=0;
  strm >> n_verts;
  cogs.clear();
  tracked_faces.clear();
  vnl_matrix<double> cog(n_frames, 2);
  vnl_matrix<double> X(n_frames, n_verts);
  vnl_matrix<double> Y(n_frames, n_verts);
  vcl_string cs, x, y;
  strm>> cs;
  if (cs!="COG:")
    return false;
  strm>> cog;
  strm>> x;
  if (x!="X:")
    return false;
  strm>> X;
  strm>> y;
  if (y!="Y:")
    return false;
  strm>> Y;
  for (unsigned int i = 0; i<n_frames; i++)
  {
    vcl_vector<vtol_vertex_sptr> verts;
    for (unsigned int j = 0; j<n_verts; j++)
    {
      vtol_vertex* v2d  = new vtol_vertex_2d(X[i][j], Y[i][j]);
      verts.push_back(v2d);
    }
    vgl_point_2d<double> p(cog[i][0], cog[i][1]);
    cogs.push_back(p);
    tracked_faces.push_back(new vtol_face_2d(verts));
  }
  return true;
}
