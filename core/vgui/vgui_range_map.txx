#include <vgui/vgui_range_map.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vil/vil_pixel_traits.h>

template <class Type>
void vgui_range_map<Type>::init_map_domain(const Type min, const Type max,
                                           long double& ratio)
{
  //A few checks on the map range parameters
  assert(max<=vil_pixel_traits<Type>::maxval());
  assert(min>=vil_pixel_traits<Type>::minval());
  assert(max>=min);
  ratio = 1.0;
  if(max!=min)
    ratio = 1.0/(max - min);
}
//Initialize the internal mapping scale factors
template <class Type>
void vgui_range_map<Type>::init()
{
  if(n_components_==1)
    {
      init_map_domain(min_L_, max_L_, ratio_L_);
      return;
    }
  if(n_components_==3||n_components_==4)
    {
      init_map_domain(min_R_, max_R_, ratio_R_);
      init_map_domain(min_G_, max_G_, ratio_G_);
      init_map_domain(min_B_, max_B_, ratio_B_);
    }      
  if(n_components_==4)
    init_map_domain(min_A_, max_A_, ratio_A_);
}
//Compute the pixel byte mapping.  Used for types that aren't table mappable
template <class Type>
vxl_byte vgui_range_map<Type>::
map_pixel_byte(const Type pix, const Type min, const Type max,
               const Type gamma, const long double ratio)
{
  if(vil_pixel_traits<Type>::num_bits()==1)
    if(pix)
      return 255;
    else
      return 0;
  //the type with largest dynamic range
  long double y = pix;
  y = (y < min) ?  min: (y > max) ? max : y;
  if(invert_)
    y = max - y;
  else
    y -= min;
  // y will now be in the range 0 to (max - min)
  // we now put y in that range
  y *= ratio;
  // if gamma >0 && !=1 make the gamma correction
  if (gamma > 0 && gamma !=1) 
    y = vcl_pow((long double)y, (long double)1/gamma);
  return (vxl_byte)((y*255.0) +0.5);//round to nearest byte
}


//Compute the float mapping.  Used for types that are table mappable
template <class Type>
float vgui_range_map<Type>::
map_pixel_float(const Type pix, const Type min, const Type max,
               const Type gamma, const long double ratio)
{
  if(vil_pixel_traits<Type>::num_bits()==1)
    if(pix)
      return 1.0f;
    else
      return 0.0f;
  //the type with largest dynamic range
  long double y = pix;
  y = (y < min) ?  min: (y > max) ? max : y;
  if(invert_)
    y = max - y;
  else
    y -= min;
  // y will now be in the range 0 to (max - min)
  // we now put y in that range
  y *=ratio;
  // if gamma >0 && !=1 make the gamma correction
  if (gamma > 0 && gamma!=1) 
    y = vcl_pow((long double)y, (long double)1/gamma);
  return y;
}

template <class Type>
vxl_byte* vgui_range_map<Type>::
compute_byte_table(const Type min, const Type max, const Type gamma,
                   const long double ratio)
{
  vxl_byte* bmap =  new vxl_byte[size_];
  //there are two cases, signed and unsigned map domains
  if(!vil_pixel_traits<Type>::is_signed())
    for(unsigned int i = 0; i < size_; i++) 
    {
      Type arg = (Type)i;//eliminate warnings
      bmap[i] = map_pixel_byte(arg, min, max, gamma, ratio);
    }
  else
    {
      //The values have to be shifted by min
      int mint = vil_pixel_traits<Type>::minval();
      int maxt = vil_pixel_traits<Type>::maxval();
      for(int i = mint; i <= maxt; i++) 
        {
          Type arg = (Type)i;//eliminate warnings
          bmap[i-mint] = map_pixel_byte(arg, min, max, gamma, ratio);
        }
    }
  maps_.push_back(bmap);
	return bmap;
}
// Hardware mapping cannot support signed Types
template <class Type>
float* vgui_range_map<Type>::
compute_float_table(const Type min, const Type max, const Type gamma,
                    const long double ratio)
{
  if(vil_pixel_traits<Type>::is_signed())
    return 0;
  float* fmap =  new float[size_];
  unsigned maxt = vil_pixel_traits<Type>::maxval();
  for(int i = 0; i <= maxt; i++) 
    {
      Type arg = (Type)i;//eliminate warnings
      fmap[i] = map_pixel_float(arg, min, max, gamma, ratio);
    }
  fmaps_.push_back(fmap);
  return fmap;
}


template <class Type>
vgui_range_map<Type>::vgui_range_map(vgui_range_map_params const& rmp)
  :   vgui_range_map_params(rmp)
{
  this->init();
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
    size_ = 1 << vil_pixel_traits<Type>::num_bits();
  //The table is not mapable so it will be necessary to compute
  //the mapping on the fly
}

template <class Type>
vgui_range_map<Type>::~vgui_range_map()
{
  for(vcl_vector<vxl_byte*>::iterator mit = maps_.begin();
      mit!=maps_.end(); ++mit)
    delete [] *mit;
  for(vcl_vector<float*>::iterator fmit = fmaps_.begin();
      fmit!=fmaps_.end(); ++fmit)
    delete [] *fmit;
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
