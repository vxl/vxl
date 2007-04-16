// This is core/vil/vil_blocked_image_resource.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "vil_blocked_image_resource.h"

#include <vcl_cassert.h>
#include <vil/vil_property.h>
#include <vil/vil_image_view.h>
#include <vil/vil_crop.h>
#include <vil/vil_copy.h>


vil_blocked_image_resource::vil_blocked_image_resource()
{}

vil_blocked_image_resource::~vil_blocked_image_resource()
{}

unsigned vil_blocked_image_resource::n_block_i() const
{
  unsigned sbi = size_block_i();
  if (sbi==0)
    return 0;
  return (ni()+sbi -1)/sbi;
}

unsigned vil_blocked_image_resource::n_block_j() const
{
  unsigned sbj = size_block_j();
  if (sbj==0)
    return 0;
  return (nj()+sbj -1)/sbj;
}

bool vil_blocked_image_resource::
get_blocks(unsigned start_block_i, unsigned end_block_i,
           unsigned  start_block_j, unsigned end_block_j,
           vcl_vector< vcl_vector< vil_image_view_base_sptr > >& blocks ) const
{
  for (unsigned bi = start_block_i; bi<=end_block_i; ++bi)
  {
    vcl_vector< vil_image_view_base_sptr > jblocks;

    for (unsigned bj = start_block_j; bj<=end_block_j; ++bj)
    {
      vil_image_view_base_sptr view =
        this->get_block(bi, bj);
      if (view) jblocks.push_back(view);
      else
        return false;
    }
    blocks.push_back(jblocks);
  }
  return true;
}

bool vil_blocked_image_resource::put_blocks( unsigned start_block_i,
                                             unsigned end_block_i,
                                             unsigned  start_block_j,
                                             unsigned end_block_j,
                                             vcl_vector< vcl_vector< vil_image_view_base_sptr > > const& blocks )
{
  for (unsigned bi = start_block_i; bi<=end_block_i; ++bi)
    for (unsigned bj = start_block_j; bj<=end_block_j; ++bj)
      if (!this->put_block(bi, bj, *blocks[bi][bj]))
        return false;
  return true;
}

vil_image_view_base_sptr vil_blocked_image_resource::
glue_blocks_together(const vcl_vector< vcl_vector< vil_image_view_base_sptr > >& blocks) const
{
  vil_image_view_base_sptr result;
  if (blocks.size()==0)
    return result;

  //first calculate the overall size of the output image (all blocks glued together)

  unsigned output_width  = 0;
  unsigned output_height = 0;
  unsigned i;
  for (i = 0 ; i < blocks.size() ; i++) {
    output_width += blocks[i][0]->ni();
  }
  for (i = 0 ; i < blocks[0].size() ; i++) {
    output_height += blocks[0][i]->nj();
  }

  //now paste all the image blocks into their proper location in outImage
  unsigned curr_i = 0;
  unsigned curr_j = 0;
  vil_pixel_format fmt = vil_pixel_format_component_format(this->pixel_format());
  switch (fmt)
  {
#define GLUE_BLOCK_CASE(FORMAT, T) \
   case FORMAT: { \
    vil_image_view<T>* output_image = \
      new vil_image_view<T>(output_width, output_height, 1, nplanes()); \
    for (unsigned bi = 0 ; bi < blocks.size() ; bi++) \
    { \
      for (unsigned bj = 0 ; bj < blocks[bi].size() ; bj++)\
      { \
        vil_copy_to_window(static_cast<vil_image_view<T>&>(*blocks[bi][bj]), *output_image, curr_i, curr_j); \
        curr_j += blocks[bi][bj]->nj(); \
      } \
      curr_j = 0; \
      curr_i += blocks[bi][0]->ni();\
    } \
    result = output_image; \
    return result; \
   }
    GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_BYTE, vxl_byte);
    GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_SBYTE, vxl_sbyte);
#if VXL_HAS_INT_64
    GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_64, vxl_uint_64);
    GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_64, vxl_int_64);
#endif
    GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32);
    GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_32, vxl_int_32);
    GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16);
    GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_16, vxl_int_16);
    GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_BOOL, bool);
    GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_FLOAT, float);
    GLUE_BLOCK_CASE(VIL_PIXEL_FORMAT_DOUBLE, double);
#undef GLUE_BLOCK_CASE

   default:
    assert(!"Unknown vil data type in tiff file format");
    break;
  }
  return result;
}

// Get the offset from the start of the block row for pixel position i
bool vil_blocked_image_resource::block_i_offset(unsigned block_i, unsigned i,
                                                unsigned& i_offset) const
{
  i_offset = 0;
  unsigned tw = size_block_i();
  if (tw==0)
    return false;
  unsigned bstrt = tw*block_i;
  if (i<bstrt)
    return false;
  i_offset = i-bstrt;
  return true;
}

// Get the offset from the start of the block column for pixel position j
bool vil_blocked_image_resource::block_j_offset(unsigned block_j, unsigned j,
                                                unsigned& j_offset) const
{
  j_offset = 0;
  unsigned tl = size_block_j();
  if (tl==0)
    return false;
  unsigned bstrt = tl*block_j;
  if (j<bstrt)
    return false;
  j_offset = j-bstrt;
  return true;
}

//The blocks may overlap the edges of the requested view
//Therefore we need to trim them in order to have the
//correct pieces to glue together to form the view.
bool vil_blocked_image_resource::trim_border_blocks(unsigned i0, unsigned ni,
                                                    unsigned j0, unsigned nj,
                                                    unsigned start_block_i,
                                                    unsigned start_block_j,
                                                    vcl_vector< vcl_vector< vil_image_view_base_sptr > >& blocks) const
{
  //loop thorugh all the boxes and trim the boxes around the border
  //if necessary.
  for (unsigned bi = 0 ; bi < blocks.size() ; bi++) {
    for (unsigned bj = 0 ; bj < blocks[bi].size() ; bj++) {
      if (!blocks[bi][bj]) continue;
      unsigned last_col_index = blocks.size()-1;
      unsigned last_row_index = blocks[bi].size()-1;
      //booleans that tell me whether this box is some sort of border box
      bool first_block_in_row = bi == 0;
      bool first_block_in_col = bj == 0;
      bool last_block_in_row = bi  == last_col_index;
      bool last_block_in_col = bj == last_row_index;

      //nothing to do if this isn't a border box
      if (!first_block_in_row && !first_block_in_col &&
          !last_block_in_row && !last_block_in_col) continue;

      unsigned bi0=0, bin=size_block_i()-1;
      unsigned bj0=0, bjn=size_block_j()-1;
      if (first_block_in_row)
        if (!block_i_offset(start_block_i+bi, i0, bi0))
          return false;
      if (last_block_in_row)
        if (!block_i_offset(start_block_i+bi, i0+ni-1, bin))
          return false;
      if (first_block_in_col)
        if (!block_j_offset(start_block_j+bj, j0, bj0))
          return false;
      if (last_block_in_col)
        if (!block_j_offset(start_block_j+bj, j0+nj-1, bjn))
          return false;

      switch (vil_pixel_format_component_format(pixel_format()))
      {
#define TRIM_BORDER_BLOCK_CASE(FORMAT, T) \
       case FORMAT: { \
        vil_image_view< T > currBlock = static_cast<vil_image_view< T >&>(*blocks[bi][bj]);\
        vil_image_view< T >* croppedBlock = new vil_image_view< T >();\
        *croppedBlock = vil_crop(currBlock, bi0, bin-bi0+1, bj0, bjn-bj0+1);\
        blocks[bi][bj] = croppedBlock;\
       } break
        TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_BYTE, vxl_byte);
        TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_SBYTE, vxl_sbyte);
#if VXL_HAS_INT_64
        TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_64, vxl_uint_64);
        TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_64, vxl_int_64);
#endif
        TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32);
        TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_32, vxl_int_32);
        TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16);
        TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_INT_16, vxl_int_16);
        TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_BOOL, bool);
        TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_FLOAT, float);
        TRIM_BORDER_BLOCK_CASE(VIL_PIXEL_FORMAT_DOUBLE, double);
#undef TRIM_BORDER_BLOCK_CASE

       default:
        assert(!"Unknown vil data type.");
        return false;
      }
    }
  }
  return true;
}

//Get blocks including those that might be in the cache
vil_image_view_base_sptr vil_blocked_image_resource::
get_copy_view(unsigned i0, unsigned n_i, unsigned j0, unsigned n_j) const
{
  vil_image_view_base_sptr view = 0;

  unsigned tw = size_block_i(), tl = size_block_j();
  if (tw==0||tl==0)
    return view;

  //block index ranges
  unsigned  bi_start = i0/tw, bi_end = (i0+n_i-1)/tw;
  unsigned  bj_start = j0/tl, bj_end = (j0+n_j-1)/tl;
  //last block index
  unsigned lbi = n_block_i()-1, lbj = n_block_j()-1;

  if (bi_start>lbi||bi_end>lbi||bj_start>lbj||bj_end>lbj)
    return view;

  //Get set of blocks covering the view
  vcl_vector<vcl_vector< vil_image_view_base_sptr > > blocks;

  this->get_blocks(bi_start, bi_end, bj_start, bj_end, blocks);
  if (!blocks.size())
    return view;
  //Trim them if necessary to fit the view
  if (!trim_border_blocks(i0, n_i, j0, n_j, bi_start, bj_start, blocks))
    return view;
  //Assemble them to fill the requested view
  view = this->glue_blocks_together(blocks);
#ifdef DEBUG
  unsigned nblocks = (bi_end-bi_start+1)*(bj_end-bj_start+1);
  if (nblocks>1)
    vcl_cout << "Get copy view of " << nblocks << " blocks in "
             << t.real() << "msecs\n";
#endif
  return view;
}

vil_blocked_image_resource_sptr blocked_image_resource(const vil_image_resource_sptr& ir)
{
  if (!ir)
    return 0;
  unsigned sbi=0, sbj=0;
  if (ir->get_property(vil_property_size_block_i, &sbi)&&
      ir->get_property(vil_property_size_block_j, &sbj))
    return (vil_blocked_image_resource*)ir.ptr();
  else
    return 0;
}
