// This is core/vil2/vil2_vil1.h
#ifndef vil2_vil1_h_
#define vil2_vil1_h_
//:
// \file
// \brief Provide vil2 wrappers of vil1 and vice-versa.

#include <vil/vil_fwd.h>
#include <vil/vil_image.h>
#include <vil2/vil2_image_resource.h>
#include <vil2/vil2_image_view.h>

//: Create a view of a vil_memory_image_of<T>
// Warning, the created view doesn't not share ownership of the underlying image. The
// vil_memory_image_of<T> should not be destroyed while the view is in use.
// The function assumes that the addressing increments to the next column, row or plane
// are constant throughout the image.

template <class T>
vil2_image_view<T> vil2_vil1_to_image_view(const vil_memory_image_of<T> &vil1_im)
{
  const T* tl = &vil1_im(0,0);

  vcl_ptrdiff_t planestep;

  if (vil1_im.planes() == 1)
    planestep = vil1_im.cols()* vil1_im.rows();
  else
    planestep = &((vil_memory_image_of<T>((vil1_im.get_plane(1)))).operator()(0,0)) - tl;

  return vil2_image_view<T>(tl,
                            vil1_im.cols(), vil1_im.rows(), vil1_im.planes(),
                            &vil1_im(1,0) - tl, &vil1_im(0,1) - tl, planestep);
}


//: Create a vil1 memory image from a vil2 image view.
// Warning, the created vil1 image doesn't not share ownership of the underlying image. The
// vil2_image_view<T> should not be destroyed while the vil1 image is in use.
template <class T>
vil_memory_image_of<T> vil2_vil1_from_image_view(const vil2_image_view<T> &vil2_im)
{
  if (!vil2_im.is_contiguous() || vil2_im.nplanes() != 1)
  {
    vcl_cerr << "WARNING vil2_vil1_from_image_view(): "
             << "Unable to create vil_memory_image_of<T>\n";
    return vil_memory_image_of<T>();
  }

  return vil_memory_image_of<T>(const_cast<T*>(vil2_im.top_left_ptr()),
                                               vil2_im.ni(), vil2_im.nj());
}

//: Create a vil2_image_resource from a vil1 vil_image.
vil2_image_resource_sptr vil2_vil1_to_image_resource(const vil_image &vil1_im);


//: This class wraps a vil1 vil_image to provide a vil2 style interface
class vil2_vil1_image_resource: public vil2_image_resource
{
  vil2_vil1_image_resource(const vil_image &src);
  friend vil2_image_resource_sptr vil2_vil1_to_image_resource(const vil_image &vil1_im);
  vil_image src_;

public:
  virtual vil2_pixel_format pixel_format() const;

  virtual unsigned ni() const;
  virtual unsigned nj() const;
  virtual unsigned nplanes() const;

  virtual bool get_property(char const *tag, void *property_value=0) const;
  virtual vil2_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni, unsigned j0, unsigned nj) const;

  virtual bool put_view(const vil2_image_view_base &im, unsigned i0, unsigned j0);
};

#endif // vil2_vil1_h_
