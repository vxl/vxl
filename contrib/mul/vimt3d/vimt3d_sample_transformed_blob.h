// This is mul/vimt3d/vimt3d_sample_transformed_blob.h
#ifndef vimt3d_sample_transformed_blob_h_
#define vimt3d_sample_transformed_blob_h_
//:
// \file
// \brief Sample transformed irregular region
// \author Tim Cootes

#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_chord.h>
#include <vimt3d/vimt3d_transform_3d.h>
#include <vil3d/vil3d_trilin_interp.h>
#include <vnl/vnl_vector.h>

//: Samples irregular region described by chords
//  Region described by chords in an integer grid frame.
//  If pi is the i-th (integer) point in this, then
//  on exit sample[i]=trilin_interp(image,i2i(pi)).
//  Assumes that sample has already been resized correctly.
//  WARNING: This uses no bound checks, so the transformed
//  points must be completely within the image.
template<class T>
inline void vimt3d_sample_transformed_blob(
                          const vil3d_image_view<T>& image,
                          const vimt3d_transform_3d& i2i,
                          const std::vector<vil3d_chord>& chords,
                          vnl_vector<double>& sample)
{
  double *s = sample.data_block();

  std::vector<vil3d_chord>::const_iterator c = chords.begin();
  vgl_vector_3d<double> dp = i2i(1,0,0)-i2i.origin();

  std::ptrdiff_t istep = image.istep();
  std::ptrdiff_t jstep = image.jstep();
  std::ptrdiff_t kstep = image.kstep();
  const T* im_data = image.origin_ptr();

  for (;c!=chords.end();++c)
  {
    // Sample points along a chord, assuming all inside image
    vgl_point_3d<double> p=i2i(c->start_x(),c->y(),c->z());
    double *s_end = s+c->length();
    for (;s!=s_end;++s,p+=dp)
    {
      *s = vil3d_trilin_interp_raw(p.x(),p.y(),p.z(),
                                   im_data,istep,jstep,kstep);
    }
  }
}

//: Samples irregular region described by chords
//  Region described by chords in an integer grid frame.
//  If pi is the i-th (integer) point in this, then
//  on exit sample[i]=trilin_interp(image,i2i(pi)).
//  Assumes that sample has already been resized correctly.
//  This uses safe interpolation, so any transformed
//  points outside the image will be evaluated as outval.
template<class T>
inline void vimt3d_sample_transformed_blob_safe(
                          const vil3d_image_view<T>& image,
                          const vimt3d_transform_3d& i2i,
                          const std::vector<vil3d_chord>& chords,
                          vnl_vector<double>& sample,
                          T outval=0)
{
  double *s = sample.data_block();

  std::vector<vil3d_chord>::const_iterator c = chords.begin();
  vgl_vector_3d<double> dp = i2i(1,0,0)-i2i.origin();

  std::ptrdiff_t istep = image.istep();
  std::ptrdiff_t jstep = image.jstep();
  std::ptrdiff_t kstep = image.kstep();
  unsigned ni=image.ni(),nj=image.nj(),nk=image.nk();
  const T* im_data = image.origin_ptr();

  for (;c!=chords.end();++c)
  {
    // Sample points along a chord, assuming all inside image
    vgl_point_3d<double> p=i2i(c->start_x(),c->y(),c->z());
    double *s_end = s+c->length();
    for (;s!=s_end;++s,p+=dp)
    {
      *s = vil3d_trilin_interp_safe(p.x(),p.y(),p.z(),
                                    im_data,ni,nj,nk,
                                    istep,jstep,kstep,outval);
    }
  }
}

#endif // vimt3d_sample_transformed_blob_h_
