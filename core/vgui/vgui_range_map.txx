#include <vgui/vgui_range_map.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vil/vil_pixel_traits.h>

template <class Type>
void vgui_range_map<Type>::init_map_domain()
{
  //A few checks on the map range parameters
  Type min = (Type)min_val_,  max = (Type)max_val_;
  assert(max<=vil_pixel_traits<Type>::maxval());
  assert(min>=vil_pixel_traits<Type>::minval());
  assert(max>=min);
  ratio_ = 1.0;
  if(max_val_!=min_val_)
    ratio_ = 255.0/(max_val_ - min_val_);
}

//Compute the pixel mapping.  Used for types that aren't table mappable
template <class Type>
vxl_byte vgui_range_map<Type>::
map_pixel(const Type pix)
{
  if(vil_pixel_traits<Type>::num_bits()==1)
    if(pix)
      return 255;
    else
      return 0;
  //the type with largest dynamic range
  long double y = pix;
  y = (y < min_val_) ?  min_val_: (y > max_val_) ? max_val_ : y;
  if(invert_)
    y = max_val_ - y;
  else
    y -= min_val_;
  // y will now be in the range 0 to (max - min)
  // we now put y in that range
  y *=ratio_;
  // if gamma >0 && !=1 make the gamma correction
  if (gamma_ > 0 && gamma_ !=1) 
    y = 255.0*vcl_pow((long double)y/255.0, (long double)1/gamma_);
  return (vxl_byte)y;
}

template <class Type>
void vgui_range_map<Type>::compute_table()
{
  //there are two cases, signed and unsigned map domains
  if(!vil_pixel_traits<Type>::is_signed())
    for(unsigned int i = 0; i < size_; i++) 
    {
      Type arg = (Type)i;//eliminate warnings
      map_[i] = map_pixel(arg);
    }
  else
    {
      //The values have to be shifted by min
      int min = vil_pixel_traits<Type>::minval();
      int max = vil_pixel_traits<Type>::maxval();
      for(int i = min; i <= max; i++) 
        {
          Type arg = (Type)i;//eliminate warnings
          map_[i-min] = map_pixel(arg);
        }
      
    }
}

template <class Type>
vgui_range_map<Type>::vgui_range_map(vgui_range_map_params const& rmp)
  :   vgui_range_map_params(rmp.min_val_, rmp.max_val_,
                        rmp.gamma_, rmp.invert_)
{
  map_ = 0;
  this->init_map_domain();
  mapable_ = true;
  table_mapable_ = true;
  if(vil_pixel_traits<Type>::num_bits()==1,
     vil_pixel_traits<Type>::num_bits()>16||
     vil_pixel_traits<Type>::real_number_field())
    table_mapable_ = false;
  if(!table_mapable_)
    size_ = 0;
  //A lookup table is used to represent the pixel mapping
  //The table will primarily be used for byte and short pixel types
  //and can handle direct lookup of the mapping
  if(table_mapable_)
    {
      //set up the table storage
      size_ = 1 << vil_pixel_traits<Type>::num_bits();
      map_ =  new vxl_byte[size_];
      this->compute_table();
    }
  //The table is not mapable so it will be necessary to compute
  //the mapping on the fly
}

template <class Type>
vgui_range_map<Type>::~vgui_range_map()
{
    if(table_mapable_)  delete [] map_;
}

template <class Type>
void vgui_range_map<Type>::
set_map_params(vgui_range_map_params_sptr const& rmp)
{
  min_val_ = rmp->min_val_;
  max_val_ = rmp->max_val_;
  gamma_ = rmp->gamma_;
  invert_ = rmp->invert_;
  this->init_map_domain();
  if(table_mapable_)
    this->compute_table();
}

//: The offset for signed, non field, types so that negative values can be 
//  used as table index entries.
template <class Type>
int vgui_range_map<Type>::offset()
{
  if(table_mapable_)
    {
      int min = (int)vil_pixel_traits<Type>::minval();
      return -min;
    }
  return 0;
}
#undef VGUI_RANGE_MAP_INSTANTIATE
#define VGUI_RANGE_MAP_INSTANTIATE(T) \
template class vgui_range_map<T>; 
