// This is core/vil1/vil1_vil.h
#ifndef vil1_vil_h_
#define vil1_vil_h_
//:
// \file
// \brief Provide vil wrappers of vil1 and vice-versa.

#include <vil1/vil1_fwd.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vcl_iostream.h>

//: Create a view of a vil1_memory_image_of<T>
// Warning, the created view does not share ownership of the underlying image. The
// vil1_memory_image_of<T> should not be destroyed while the view is in use.
// The function assumes that the addressing increments to the next column, row or plane
// are constant throughout the image.

template <class T>
vil_image_view<T> vil1_to_vil_image_view(const vil1_memory_image_of<T> &vil1_im)
{
  const T* tl = &vil1_im(0,0);

  vcl_ptrdiff_t planestep;

  if (vil1_im.planes() == 1)
    planestep = vil1_im.cols()* vil1_im.rows();
  else
    planestep = &((vil1_memory_image_of<T>(vil1_im.get_plane(1)))(0,0)) - tl;

  return vil_image_view<T>(tl,
                           vil1_im.cols(), vil1_im.rows(), vil1_im.planes(),
                           &vil1_im(1,0) - tl, &vil1_im(0,1) - tl, planestep);
}


//: Create a vil1 memory image from a vil image view.
// Warning, the created vil1 image doesn't not share ownership of the underlying image. The
// vil_image_view<T> should not be destroyed while the vil1 image is in use.
template <class T>
vil1_memory_image_of<T> vil1_from_vil_image_view(const vil_image_view<T> &vil_im)
{
  if (!vil_im.is_contiguous() || vil_im.nplanes() != 1)
  {
    vcl_cerr << "WARNING vil1_vil1_from_image_view(): "
             << "Unable to create vil1_memory_image_of<T>\n";
    return vil1_memory_image_of<T>();
  }

  return vil1_memory_image_of<T>(const_cast<T*>(vil_im.top_left_ptr()),
                                                vil_im.ni(), vil_im.nj());
}

//: Create a vil_image_resource from a vil1 vil1_image.
vil_image_resource_sptr vil1_to_vil_image_resource(const vil1_image &vil1_im);


//: This class wraps a vil1_image to provide a vil style interface
class vil1_vil_image_resource: public vil_image_resource
{
  vil1_vil_image_resource(const vil1_image &src);
  friend vil_image_resource_sptr vil1_to_vil_image_resource(const vil1_image &vil1_im);
  vil1_image src_;

 public:
  virtual vil_pixel_format pixel_format() const;

  virtual unsigned ni() const;
  virtual unsigned nj() const;
  virtual unsigned nplanes() const;

  virtual bool get_property(char const *tag, void *property_value=0) const;
  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni, unsigned j0, unsigned nj) const;

  virtual bool put_view(const vil_image_view_base &im, unsigned i0, unsigned j0);
};

#endif // vil1_vil_h_

