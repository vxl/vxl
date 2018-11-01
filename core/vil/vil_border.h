// This is core/vil/vil_border.h
#ifndef vil_border_h_
#define vil_border_h_
//:
// \file
// \brief Border handling for images.
// \author Nicolas Burrus - Paris

#include <limits>
#include <cassert>
// not used? #include <vcl_compiler.h>

//: Vil border modes.
enum vil_border_mode
{
  //: Border pixels are constant
  vil_border_constant = 0,

  //: Border pixels take the value of the closest image point.
  // This ensures object continuity.
  // ex: im(im.ni()+2, -1) == im(im.ni()-1, 0)
  vil_border_geodesic = 1,

  //: Border pixels take the value of the image point which is its symmetric w.r.t. the closest image edge.
  // ex: im(im.ni()+3, 0) = im(im.ni()-4, 0)
  vil_border_reflect = 2,

  //: The image is seen as periodic, after the right (respectively bottom) edge comes the left (respectively top).
  // ex: im(im.ni()+2,0) == im(2,0)
  // ex: im(im.ni()+3, im.nj()+2) == im(3,2)
  vil_border_periodic = 3,
};

//: Border class.  Makes pixel access outside image range transparent and configurable.
// Note that a border is not assigned to a specific image, but only holds border properties.
template <class imT>
class vil_border
{
 public:
  typedef typename imT::pixel_type pixel_type;

 public:
  //: Default constructor, creates a constant border.
  vil_border ()
  : border_kind_(vil_border_constant)
  , constant_value_()
  {}

  //: Set the border kind.
  void set_kind (vil_border_mode brdr_kind) { border_kind_ = brdr_kind; }
  //: Get the current border kind.
  vil_border_mode kind() const { return border_kind_; }

  //: True if border values are constant.
  inline bool is_constant() const
  { return border_kind_ == vil_border_constant; }

  //: If border kind is vil_border_constant, returns the border value.
  inline const pixel_type& constant_value() const { return constant_value_; }

  //: Set the border value if the border kind is vil_border_constant.
  void set_constant_value (const pixel_type& val) { constant_value_ = val; }

  //: Return read-only reference to pixel at (i,j,p) on the given image.
  // If the pixel falls out of the image range, the border value is returned.
  inline const pixel_type&
  operator()(const imT& im, int i, int j, int p = 0) const
  {
    if (im.in_range(i,j,p))
      return im(i,j,p);

    return in_border(im, i, j, p);
  }

 protected:
  inline const pixel_type&
  in_border(const imT& im, int i, int j, int p = 0) const
  {
    switch (border_kind_)
    {
    case vil_border_constant:
      return constant_value_;
    case vil_border_geodesic:
      if (i < 0) i = 0; else if (i >= (int)im.ni()) i = im.ni()-1;
      if (j < 0) j = 0; else if (j >= (int)im.nj()) j = im.nj()-1;
      if (p < 0) p = 0; else if (p >= (int)im.nplanes()) p = im.nplanes()-1;
      return im(i,j,p);
    case vil_border_reflect:
      if (i < 0) i = -i-1; else if (i >= (int)im.ni()) i = 2*im.ni()-i-1;
      if (j < 0) j = -j-1; else if (j >= (int)im.nj()) j = 2*im.nj()-j-1;
      if (p < 0) p = -p-1; else if (p >= (int)im.nplanes()) p = 2*im.nplanes()-p-1;
      return im(i,j,p);
    case vil_border_periodic:
      if (i < 0) i = im.ni()-((-i)%im.ni());
      else i = i%im.ni();

      if (j < 0) j = im.nj()-((-j)%im.nj());
      else j = j%im.nj();

      if (p < 0) p = im.nplanes()-((-p)%im.nplanes());
      else p = p%im.nplanes();

      return im(i,j,p);
    default:
      assert(false);
      return constant_value_; // To avoid warnings
    };
  }

 private:
  vil_border_mode border_kind_;
  pixel_type constant_value_;
};

//: Provides a pixel accessor which is syntax-compatible with vil_image_view.
// It acts like a proxy to the underlying image,
// transparently providing border pixel values if required.
template <class imT>
class vil_border_accessor
{
 public:
  typedef typename imT::pixel_type pixel_type;

 public:
  //: Constructor.
  vil_border_accessor(const imT& img, const vil_border<imT>& brdr)
  : im(img), border(brdr)
  {}

  //: Returns a const reference on the pixel (i,j,p).
  // If the pixel falls out of the image range, a border value is returned.
  inline const pixel_type&
  operator()(int i, int j, int p = 0) const
  { return border(im, i, j, p); }

 private:
  const imT& im;
  vil_border<imT> border;
};

//: Instantiates a border accessor, provided for convenience.
template <class imT>
vil_border_accessor<imT>
vil_border_create_accessor(const imT& im, const vil_border<imT>& border)
{
  return vil_border_accessor<imT>(im, border);
}

//: Instantiate a constant border whose type is derived from imT.
template <class imT>
inline vil_border<imT>
vil_border_create_constant(const imT&, typename imT::pixel_type constant_val = 0)
{
  vil_border<imT> border;
  border.set_kind(vil_border_constant);
  border.set_constant_value(constant_val);
  return border;
}

//: Instantiate a geodesic border whose type is derived from imT.
template <class imT>
inline vil_border<imT>
vil_border_create_geodesic(const imT&)
{
  vil_border<imT> border;
  border.set_kind(vil_border_geodesic);
  return border;
}

//: Instantiate a reflect border whose type is derived from imT.
template <class imT>
inline vil_border<imT>
vil_border_create_reflect(const imT&)
{
  vil_border<imT> border;
  border.set_kind(vil_border_reflect);
  return border;
}

//: Instantiate a reflect border whose type is derived from imT.
template <class imT>
inline vil_border<imT>
vil_border_create_periodic(const imT&)
{
  vil_border<imT> border;
  border.set_kind(vil_border_periodic);
  return border;
}

#endif // vil_border_h_
