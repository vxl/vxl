#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_2x3.h>
#include <vil/vil_load.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>
#include <ihog/ihog_minimizer.h>
#include <ihog/ihog_world_roi.h>
#include <ihog/ihog_transform_2d.h>
#include <ihog/ihog_transform_2d_sptr.h>
#include <cassert>


static void filenames_from_directory(std::string const& dirname,
                                     std::vector<std::string>& filenames)
{
  std::string s(dirname);
  s += "/*.*";
  for (vul_file_iterator fit = s;fit; ++fit) {
    // check to see if file is a directory.
    if (vul_file::is_directory(fit()))
      continue;
    filenames.emplace_back(fit());
  }
}

static bool write_homographies(std::string const& filename,
                               std::vector<vnl_double_3x3 > const& homographies)
{
  std::ofstream ofile(filename.c_str(),std::ios::out);
  std::cout<<"\n Writing Homographies "<<filename;

  if (!ofile)
  {
    std::cerr<<"\n error opening output file\n";
    return false;
  }
  unsigned frame = 0;
  for (auto hit = homographies.begin();
       hit != homographies.end(); ++hit, ++frame)
  {
    ofile <<"Frame No " << frame << '\n' << *hit;
  }
  ofile.close();
  return true;
}

static ihog_transform_2d
register_image(vil_image_view<float> & curr_view,
               vil_image_view<float> & last_view,
               const std::string& transform_type ="Affine")
{
  // do registration
  vul_timer time;
  ihog_transform_2d init_xform;
  unsigned ni = curr_view.ni(), nj = curr_view.nj();
  if (transform_type  ==  "Identity")
  {
    std::cout << "In ihog/exe/compute_homographies_exe:"
             << " an identity transform doesn't make sense\n";
    assert (!"Identity transform makes no sense");
  }
  else if (transform_type ==  "Translation")
  {
    vnl_vector<double> T(2);
    T[0]=0.0000001;   T[1]=0.0000001;
    init_xform.set(T, ihog_transform_2d::Translation);
  }
  else if (transform_type ==  "ZoomOnly")
  {
    init_xform.set_zoom_only(1.0, 1.0, 0.0, 0.0);
  }
  else if (transform_type ==  "RigidBody")
  {
    vnl_vector<double> R(3);
    R[0]=0.0000001;   R[1]=0.0;  R[2]=0.0;
    init_xform.set(R, ihog_transform_2d::RigidBody);
  }
#if 0
  else if (transform_type ==  "Similarity")
  {
    vnl_vector<double> S(4);
    S[0]=1.0000001;   S[1]=0.0;  S[2]=0.0; S[3]=0.0;
    init_xform.set(S, ihog_transform_2d::Similarity);
  }
#endif
  else if (transform_type ==  "Affine")
  {
    vnl_double_2x3 A; A.set_identity();
    init_xform.set_affine(A);
  }
  else if (transform_type ==  "Projective")
  {
    vnl_double_3x3 P; P.set_identity().set(2,2, 1.0000001);
    init_xform.set_projective(P);
  }
#if 0
  else if (transform_type ==  "Reflection")
  {
    vgl_point_2d<double> m1(0, 0), m2(1,0);
    init_xform.set_reflection(m1, m2);
  }
#endif
  else {
    std::cerr << "Unrecoverable error:\n"
             << " Unknown ihog transform type " << transform_type << '\n';
    assert(!"Unrecoverable error");
  }

  int border = 10;
  ihog_world_roi roi(ni- 2*border, nj- 2*border,
                     vgl_point_2d<double>(border,border),
                     vgl_vector_2d<double>(0.99,0.0),
                     vgl_vector_2d<double>(0.0,0.99));
  ihog_image<float> last_img(last_view, ihog_transform_2d());
  ihog_image<float> curr_img(curr_view, init_xform);
  ihog_minimizer minimizer(last_img, curr_img, roi);
  minimizer.minimize(init_xform);
  std::cout << "Registration in " << time.real() << " msecs\n";
  return init_xform;
}

static bool compute_homogs(std::string const& image_indir,
                           std::string const& transform_type,
                           std::string const& homg_file
                          )
{
  ihog_transform_2d total_xform;
  total_xform.set_identity();
  std::vector<vnl_double_3x3 > homographies;
  std::vector<std::string> in_filenames;
  filenames_from_directory(image_indir, in_filenames);
  unsigned n_infiles = in_filenames.size();
  unsigned infile_counter = 0;
  //read the first image
  bool no_valid_image = true;
  vil_image_resource_sptr imgr;
  while (no_valid_image)
  {
    imgr =
      vil_load_image_resource(in_filenames[infile_counter++].c_str());
    no_valid_image = !imgr||imgr->ni()==0||imgr->nj()==0;
    if (infile_counter>=n_infiles)
      return false;
  }
  std::cout << "Initialized\n";
  vil_image_view<float> float_last_view = *vil_convert_cast(float(), imgr->get_view());

  infile_counter = 0;//return to first frame
  total_xform.set_identity();
  unsigned frame = 1;
  while (infile_counter<n_infiles)
  {
    imgr =
      vil_load_image_resource(in_filenames[infile_counter++].c_str());
    if (!imgr||imgr->ni()==0||imgr->nj()==0)
      continue;
    vil_image_view<float> float_curr_view = *vil_convert_cast(float(), imgr->get_view());
    std::cout << "Registering frame " << frame++ << '\n'<< std::flush;
    ihog_transform_2d xform = register_image(float_curr_view,
                                             float_last_view,
                                             transform_type);
    total_xform = total_xform * xform.inverse();
    vnl_double_3x3 p=total_xform.get_matrix();
    homographies.push_back(p);
    float_last_view = float_curr_view;
  }
  write_homographies(homg_file,homographies);
  return true;
}

int main(int argc,char * argv[])
{
  if (argc!=4)
  {
    std::cout<<"Usage : compute_homographies_exe.exe image_in_dir homography_type(Identity|Translation|ZoomOnly|RigidBody|Similarity|Affine|Projective) homography_outfile\n";
    return -1;
  }
  else
  {
    std::string image_indir(argv[1]);
    std::string transform_type(argv[2]);
    std::string homg_file(argv[3]);
    if (!compute_homogs(image_indir, transform_type, homg_file))
    {
      std::cout << "Registration failed\n";
      return -1;
    }
    return 0;
  }
}
