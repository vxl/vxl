// This is gel/vifa/vifa_image_histogram.cxx
#include <vil/vil_image_view.h>
#include <vifa/vifa_image_histogram.h>


vifa_image_histogram::
vifa_image_histogram(vil_image_view_base_sptr  image,
                     double                    percentage)
{
  // Compute max. # of pixel intensities
  form_ = image->pixel_format();
  byte_depth_ = vil_pixel_format_sizeof_components(form_) *
                vil_pixel_format_num_components(form_);

  // Initialize the underlying histogram, based on the image pixel format
  init();

  // Fill the histogram
  fill_histogram(image, percentage);
}

void vifa_image_histogram::
init(void)
{
  num = 1 << (byte_depth_ * 8);

  // Delete these because they have already been allocated by base constructor
  delete [] vals;
  delete [] counts;

  vals = new float [num];
  counts = new float [num];

  delta = 1.0;

  if (vals != NULL && counts != NULL)
  {
    register float*  pval = vals;
    register float*  pcount = counts;

    switch (num)
    {
      case 256:
      {
        *pval = 0;
        *pcount = 0;
        vmin = 0;
        vmax = 255;
        break;
      }

      case 65536:
      {
        *pval = 0;
        *pcount = 0;
        vmin = 0;
        vmax = 65535;
        break;
      }

      default:
        break;
    }

    for (register int i = 1; i < num; ++i)
    {
      float  val = *(pval++);
      *pval = val + 1;
      *(++pcount) = 0;
    }
  }
}

void vifa_image_histogram::
fill_histogram(vil_image_view_base_sptr  image,
               double                    /* percentage */)
{
  // Get the base histogram's array of counts
  float*  counts = this->GetCounts();

  switch (form_)
  {
    case VIL_PIXEL_FORMAT_BYTE:
    case VIL_PIXEL_FORMAT_SBYTE:
    {
      // Cast the abstract image view to a compatible concrete type
      vil_image_view<vxl_byte>*  img =
                (vil_image_view<vxl_byte>*)(image.ptr());

      // Are all the pixels in contiguous memory?
      if (img->is_contiguous())
      {
        // Yes - use fast iterator to scan pixels
        vxl_byte*    cur_pix = img->begin();
        vxl_byte*    last_pix = img->end();
        vcl_ptrdiff_t  istep = img->istep();
        while (cur_pix != last_pix)
        {
          counts[*cur_pix]++;
          cur_pix += istep;
        }
      }
      else
      {
        // No - use pixel coordinates
        unsigned int  max_j = img->nj();
        unsigned int  max_i = img->ni();

        for (unsigned int j = 0; j < max_j; j++)
          for (unsigned int i = 0; i < max_i; i++)
            counts[(*img)(i, j)]++;
      }

      break;
    }

    case VIL_PIXEL_FORMAT_UINT_16:
    case VIL_PIXEL_FORMAT_INT_16:
    {
      // Cast the abstract image view to a compatible concrete type
      vil_image_view<vxl_int_16>*  img =
                (vil_image_view<vxl_int_16>*)(image.ptr());

      // Are all the pixels in contiguous memory?
      if (img->is_contiguous())
      {
        // Yes - use fast iterator to scan pixels
        vxl_int_16*    cur_pix = img->begin();
        vxl_int_16*    last_pix = img->end();
        vcl_ptrdiff_t  istep = img->istep();
        while (cur_pix != last_pix)
        {
          counts[*cur_pix]++;
          cur_pix += istep;
        }
      }
      else
      {
        // No - use pixel coordinates
        unsigned int  max_j = img->nj();
        unsigned int  max_i = img->ni();

        for (unsigned int j = 0; j < max_j; j++)
          for (unsigned int i = 0; i < max_i; i++)
            counts[(*img)(i, j)]++;
      }

      break;
    }

    default:
      break;
  }
}
