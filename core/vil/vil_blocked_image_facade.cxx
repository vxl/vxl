// This is core/vil/vil_blocked_image_facade.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
#include <vil/vil_property.h>
#include <vcl_cstring.h>
#include <vil/vil_image_view.h>
#include <vil/vil_crop.h>
#include <vil/vil_copy.h>
#include <vil/vil_blocked_image_facade.h>

//#define DEBUG

static const unsigned vil_size_block_i = 256, vil_size_block_j = 256;

vil_blocked_image_facade::vil_blocked_image_facade(const vil_image_resource_sptr &src, const unsigned sbi, const unsigned sbj):
  src_(src)
{
  //cases
  // I the blocking is specified so use it
  if(sbi>0&&sbj>0)
    {
      sbi_ = sbi; sbj_=sbj;
      return;
    }
  // II Use the default block size
  sbi_ = vil_size_block_i;  sbj_ = vil_size_block_j; 
  //set up the buffer
}

vil_image_view_base_sptr 
vil_blocked_image_facade::fill_block(vil_image_view_base_sptr& view) const
{
  switch (vil_pixel_format_component_format(pixel_format()))
    {
#define FILL_BLOCK_CASE(FORMAT, T) \
       case FORMAT: { \
      vil_image_view<T>* dest = new vil_image_view<T>(sbi_, sbj_, nplanes()); \
      vil_image_view_base_sptr ptr = dest; \
      vil_image_view<T>* src = reinterpret_cast<vil_image_view<T>* >(view.ptr()); \
      vil_copy_to_window<T>(*src, *dest, 0, 0); \
	  return dest; \
       } break
          FILL_BLOCK_CASE(VIL_PIXEL_FORMAT_BYTE, vxl_byte);
          FILL_BLOCK_CASE(VIL_PIXEL_FORMAT_SBYTE, vxl_sbyte);
          FILL_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32);
          FILL_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_32, vxl_int_32);
          FILL_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16);
          FILL_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_16, vxl_int_16);
          FILL_BLOCK_CASE(VIL_PIXEL_FORMAT_BOOL, bool);
          FILL_BLOCK_CASE(VIL_PIXEL_FORMAT_FLOAT, float);
          FILL_BLOCK_CASE(VIL_PIXEL_FORMAT_DOUBLE, double);
#undef FILL_BLOCK_CASE
      default:
        assert(!"Unknown vil data type.");
        return 0;
      }
	return 0;
}
//Get a view that is the size of a block. If the block is on the 
//border then the block pixels outside the image boundary are undefined
vil_image_view_base_sptr 
vil_blocked_image_facade::get_block( unsigned  block_index_i,
                                       unsigned  block_index_j ) const
{
  unsigned ni = src_->ni(), nj = src_->nj();
  unsigned i0 = block_index_i*sbi_, j0 =  block_index_j*sbj_;
  if(i0>ni-1||j0>nj-1) return 0;
  //check if the view that is supplied is smaller than a block
  unsigned icrop = ni-i0, jcrop = nj-j0;
  bool needs_fill = true;
  if(icrop>sbi_)
    {
      icrop = sbi_;
      needs_fill = false;
    }
  if(jcrop>sbj_)
    {
      jcrop = sbj_;
      needs_fill = false;
    }
  vil_image_view_base_sptr view = src_->get_view(i0, icrop, j0, jcrop);
  if(needs_fill)
    view = fill_block(view);
  return view;
}

bool vil_blocked_image_facade::put_block(unsigned  block_index_i,
                                         unsigned  block_index_j,
                                         const vil_image_view_base& view)
{
  // convert to image coordinates
  unsigned i0 = block_index_i*sbi_, j0 = block_index_j*sbj_;
  // check if block is too big for the destination
  unsigned imax = i0 + sbi_, jmax = j0 + sbj_;
  unsigned icrop = sbi_, jcrop = sbj_;
  bool needs_trim = false;
  if(imax>src_->ni())
    {
      icrop = src_->ni()-i0;
      needs_trim = true;
    }
  if(jmax>src_->nj())
    {
      jcrop = src_->nj()-j0;
      needs_trim = true;
    }

  if(needs_trim)
    switch (vil_pixel_format_component_format(pixel_format()))
      {
#define TRIM_BLOCK_CASE(FORMAT, T) \
       case FORMAT: { \
        const vil_image_view<T>& curr_view = \
          static_cast<const vil_image_view< T >& >(view); \
        vil_image_view< T > cview = \
          vil_crop(curr_view, 0, icrop, 0, jcrop); \
        return src_->put_view(cview, i0, j0); \
       } break
          TRIM_BLOCK_CASE(VIL_PIXEL_FORMAT_BYTE, vxl_byte);
          TRIM_BLOCK_CASE(VIL_PIXEL_FORMAT_SBYTE, vxl_sbyte);
          TRIM_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32);
          TRIM_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_32, vxl_int_32);
          TRIM_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16);
          TRIM_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_16, vxl_int_16);
          TRIM_BLOCK_CASE(VIL_PIXEL_FORMAT_BOOL, bool);
          TRIM_BLOCK_CASE(VIL_PIXEL_FORMAT_FLOAT, float);
          TRIM_BLOCK_CASE(VIL_PIXEL_FORMAT_DOUBLE, double);
#undef TRIM_BLOCK_CASE
      default:
        assert(!"Unknown vil data type.");
        return false;
      }
  // no trim was required
return src_->put_view(view, i0, j0);
}



bool vil_blocked_image_facade::get_property(char const * tag, void * value) const
{
  if (vcl_strcmp(vil_property_quantisation_depth, tag)==0)
    return src_->get_property(tag, value);
  
  if (vcl_strcmp(vil_property_size_block_i, tag)==0)
    {
      if (value)
        *static_cast<unsigned*>(value) = this->size_block_i();
      return true;
    }

  if (vcl_strcmp(vil_property_size_block_j, tag)==0)
    {
      if (value)
        *static_cast<unsigned*>(value) = this->size_block_j();
      return true;
    }

  return false;
}
