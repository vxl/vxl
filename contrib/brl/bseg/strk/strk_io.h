// This is brl/bseg/strk/strk_io.h
#ifndef strk_io_h_
#define strk_io_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a I/O formatting class for track data - static methods only
//
// \author
//  J.L. Mundy - November 26, 2004
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_fstream.h>
#include <vnl/vnl_matrix.h>
#include <vgl/vgl_point_2d.h>
class strk_io 
{
 public:
  //Constructors/destructor
  ~strk_io();
  //I/O Methods
  //: write the info tracker feature histogram track data
  static bool write_histogram_data(const unsigned int start_frame,
                                   const unsigned int n_pixels,
                                   const float diameter,
                                   const float aspect_ratio,
                                   const unsigned int n_int_bins,
                                   const unsigned int n_grad_dir_bins,
                                   const unsigned int n_color_bins,
                                   vcl_vector<vcl_vector<float> >const& data, 
                                   vcl_ofstream& strm);

  //: write the info tracker track data
  static bool
    write_track_data(const unsigned int start_frame,
                     vcl_vector<vtol_face_2d_sptr> const& tracked_faces,
                     vcl_ofstream& strm);


  //: read the info tracker feature histogram track data
  static bool read_histogram_data(vcl_ifstream& strm, 
                                  unsigned int& start_frame,
                                  unsigned int& n_frames,
                                  unsigned int& n_pixels,
                                  float& diameter,
                                  float& aspect_ratio,
                                  unsigned int& n_int_bins,
                                  unsigned int& n_grad_dir_bins,
                                  unsigned int& n_color_bins,
                                  vnl_matrix<float>& hist_data);
  //: write the info tracker track data
  static bool read_track_data(vcl_ifstream& strm,
                              unsigned int& start_frame,
                              unsigned int& n_frames,
                              vcl_vector<vgl_point_2d<double> >& cogs,
                              vcl_vector<vtol_face_2d_sptr>& tracked_faces);
 protected:
  //internal utility methods
  static bool write_frame_data(const unsigned int start_frame,
                               const unsigned int n_frames,
                               vcl_ofstream& strm);

  static bool write_region_data(const unsigned int n_pix,
                                const float diameter,
                                const float aspect_ratio,
                                vcl_ofstream& strm);

  static  bool read_frame_data(vcl_ifstream& strm,
                               unsigned int& start_frame,
                               unsigned int& n_frames);

  static bool read_region_data(vcl_ifstream& strm,
                               unsigned int& n_pix, float& diameter,
                               float& aspect_ratio);


 private:
  strk_io(); //static methods only
};

#endif // strk_io_h_
