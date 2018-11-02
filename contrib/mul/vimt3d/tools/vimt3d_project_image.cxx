//:
// \file
// \brief Tool to load in a 3D image and produce 2D projection(s)
// \author Tim Cootes
// Sums pixels along one direction (eg k) and linearly stretches to a byte image.

// INCOMPLETE
// Need to resample image to cubic voxels before projection to keep aspect ratios.

#include <iostream>
#include <vul/vul_arg.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vimt3d/vimt3d_load.h>
#include <vil3d/vil3d_slice.h>
#include <vil3d/vil3d_resample_trilinear.h>
#include <vil/vil_save.h>
#include <vil3d/vil3d_save.h>
#include <vil3d/vil3d_math.h>
#include <vnl/vnl_math.h>
#include <vxl_config.h> // For vxl_byte
#include <vimt3d/vimt3d_add_all_loaders.h>
#include <vimt3d/vimt3d_save.h>

void print_usage()
{
  std::cout<<"vimt3d_project_image: Tool to load in a 3D image and produce 2D projection(s)."<<std::endl;
  std::cout<<"Sums pixels along one direction (eg k) and linearly stretches to a byte image."<<std::endl;

  vul_arg_display_usage_and_exit();
}

// Generate a projection along the i axis by summing all the i-planes
template<class T>
void i_axis_projection(const vil3d_image_view<T>& im3d,
                       vil_image_view<vxl_byte>& byte_image)
{
  unsigned ni=im3d.ni();
  unsigned nj=im3d.nj();
  unsigned nk=im3d.nk();

  vil_image_view<float> sum_image(nj,nk);
  sum_image.fill(0.0f);
  for (unsigned i=0;i<ni;++i)
    vil_math_add_image_fraction(sum_image,1.0,vil3d_slice_jk(im3d,i),1.0);

  vil_convert_stretch_range(sum_image,byte_image);
}

// Generate a projection along the i axis, finding maximum across all the i-planes
template<class T>
void i_axis_max_projection(const vil3d_image_view<T>& im3d,
                       vil_image_view<vxl_byte>& byte_image)
{
  unsigned ni=im3d.ni();

  vil_image_view<float> max_image;
  max_image.deep_copy(vil3d_slice_jk(im3d,0));
  for (unsigned i=1;i<ni;++i)
    vil_math_image_max(max_image,vil3d_slice_jk(im3d,i),max_image);

  vil_convert_stretch_range(max_image,byte_image);
}



// Generate a projection along the j axis by summing all the j-planes
template<class T>
void j_axis_projection(const vil3d_image_view<T>& im3d,
                       vil_image_view<vxl_byte>& byte_image)
{
  unsigned ni=im3d.ni();
  unsigned nj=im3d.nj();
  unsigned nk=im3d.nk();

  vil_image_view<float> sum_image(ni,nk);
  sum_image.fill(0.0f);
  for (unsigned j=0;j<nj;++j)
    vil_math_add_image_fraction(sum_image,1.0,vil3d_slice_ik(im3d,j),1.0);

  vil_convert_stretch_range(sum_image,byte_image);
}

// Generate a projection along the k axis by summing all the k-planes
template<class T>
void k_axis_projection(const vil3d_image_view<T>& im3d,
                       vil_image_view<vxl_byte>& byte_image)
{
  unsigned ni=im3d.ni();
  unsigned nj=im3d.nj();
  unsigned nk=im3d.nk();

  vil_image_view<float> sum_image(ni,nj);
  sum_image.fill(0.0f);
  for (unsigned k=0;k<nk;++k)
    vil_math_add_image_fraction(sum_image,1.0,vil3d_slice_ij(im3d,k),1.0);

  vil_convert_stretch_range(sum_image,byte_image);
}



int main(int argc, char** argv)
{
  vul_arg<std::string> image_path("-i","3D image filename");
  vul_arg<std::string> output_path("-o","Base path for output","projection");
  vul_arg<double> bx("-bx","Proportional border along x",0.1);
  vul_arg<double> by("-by","Proportional border along y",0.1);
  vul_arg<double> bz("-bz","Proportional border along z",0.1);
  vul_arg<double> voxel_width("-vw","Voxel width for resampling (or 0 for smallest in input)",0.0);
  vul_arg<unsigned > n_frames("-nf","Number of frames",36);
  vul_arg<std::string> axis_str("-a","Axis of rotation (x/y/z)","y");
  vul_arg<std::string> projection_dir("-pd","Projection direction (x/y/z)","z");
  vul_arg<float> value_threshold("-t","Threshold below which image ignored.",-999.0f);

  vul_arg_parse(argc,argv);

  if (image_path()=="")
  {
    print_usage();
    return 0;
  }

  vimt3d_add_all_loaders();  // To allow loading vimt3d_images (.v3i)

  // Load in image as float
  vimt3d_image_3d_of<float> image3d;
  bool use_mm=true;
  vimt3d_load(image_path(),image3d,use_mm);

  if (image3d.image().size()==0)
  {
    std::cout<<"Failed to load image from "<<image_path()<<std::endl;
    return 1;
  }

  std::cout<<"Image: "<<image3d<<std::endl;

  // Compute Voxel sizes
  vimt3d_transform_3d i2w=image3d.world2im().inverse();
  double dx = (i2w(1,0,0)-i2w(0,0,0)).length();
  double dy = (i2w(0,1,0)-i2w(0,0,0)).length();
  double dz = (i2w(0,0,1)-i2w(0,0,0)).length();
  std::cout<<"Original voxels: "<<dx<<","<<dy<<","<<dz<<std::endl;

  // Use smallest voxel width for cubic resampling
  double d = std::min(dx,std::min(dy,dz));
  if (voxel_width()!=0.0) d=voxel_width();
  std::cout<<"Resampling cubic voxels of width: "<<d<<std::endl;

  // Image extent
  double wx = dx*image3d.image().ni();
  double wy = dy*image3d.image().nj();
  double wz = dz*image3d.image().nk();

  // Size of image with cubic voxels covering same region
  auto ni=unsigned((1-2*bx())*wx/d+0.5);
  auto nj=unsigned((1-2*by())*wy/d+0.5);
  auto nk=unsigned((1-2*bz())*wz/d+0.5);
  std::cout<<"Resampled image: "<<ni<<"x"<<nj<<"x"<<nk<<std::endl;

  float min_v,max_v;
  vil3d_math_value_range(image3d.image(),min_v,max_v);
  std::cout<<"Pixel range: ["<<min_v<<","<<max_v<<"]"<<std::endl;

  if (min_v<value_threshold())
  {
    for (unsigned k=0;k<image3d.image().nk();++k)
      for (unsigned j=0;j<image3d.image().nj();++j)
        for (unsigned i=0;i<image3d.image().ni();++i)
          if (image3d.image()(i,j,k)<value_threshold())
            image3d.image()(i,j,k)=value_threshold();
  }

  // Unit transformation from resampled image co-ords to image3d co-ords
  vimt3d_transform_3d t1;
  t1.set_zoom_only(d/dx,d/dy,d/dz,
                   bx()*wx+1e-6,by()*wy+1e-6,bz()*wz+1e-6);

  vimt3d_transform_3d tc;  // Translate to centre
  tc.set_translation(0.5*ni,0.5*nj,0.5*nk);

  for (unsigned i=0;i<n_frames();++i)
  {
    double A=i*2.0*3.1415/n_frames();  // Radians

    vimt3d_transform_3d rot;  // Apply rotation

    if (axis_str()=="x")
      rot.set_rigid_body(A,0.0,0.0,  0.0,0.0,0.0);  // Rotation about x axis
    else if (axis_str()=="y")
      rot.set_rigid_body(0.0,A,0.0,  0.0,0.0,0.0);  // Rotation about y axis
    else
      rot.set_rigid_body(0.0,0.0,A,  0.0,0.0,0.0);  // Rotation about z axis


    vimt3d_transform_3d rot_about_centre = tc*rot*tc.inverse();
    vimt3d_transform_3d t = t1*rot_about_centre;

    // Unit vectors along each direction
    vgl_point_3d<double> p0 = t(0,0,0);
    vgl_vector_3d<double> du = t(1,0,0)-p0;
    vgl_vector_3d<double> dv = t(0,1,0)-p0;
    vgl_vector_3d<double> dw = t(0,0,1)-p0;

    vil3d_image_view<float> resampled_image(ni,nj,nk);
    vil3d_resample_trilinear_edge_extend(image3d.image(),resampled_image,
                            p0.x(),p0.y(),p0.z(), du.x(),du.y(),du.z(),
                            dv.x(),dv.y(),dv.z(), dw.x(),dw.y(),dw.z(),
                            ni,nj,nk);

    // Project along k
    // Note - ignores voxel size
    vil_image_view<vxl_byte> image2d;

    if (projection_dir()=="x")
//      i_axis_max_projection(resampled_image,image2d);
      i_axis_projection(resampled_image,image2d);
    else
    if (projection_dir()=="y")
      j_axis_projection(resampled_image,image2d);
    else
      k_axis_projection(resampled_image,image2d);

    std::stringstream ss;
    ss<<output_path()<<"_";
    if (i<10) ss<<"0";
    ss<<i<<".png";
    std::string out_path=ss.str();
    if (vil_save(image2d,out_path.c_str()))
      std::cout<<"Saved projection to "<<out_path<<std::endl;
    else
      std::cout<<"Failed to save to "<<out_path<<std::endl;
  }

  return 0;
}
