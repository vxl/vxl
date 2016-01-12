//: Cell Utility functions.
#define X_MIN (short4)(1,0,0,0);
#define X_MAX (short4)(1,0,0,1);
#define Y_MIN (short4)(0,1,0,0);
#define Y_MAX (short4)(0,1,0,1);
#define Z_MIN (short4)(0,0,1,0);
#define Z_MAX (short4)(0,0,1,1);

/*
uint rgbaFloatToInt(float4 rgba)
{
    rgba.x = clamp(rgba.x,0.0f,1.0f);
    rgba.y = clamp(rgba.y,0.0f,1.0f);
    rgba.z = clamp(rgba.z,0.0f,1.0f);
    rgba.w = clamp(rgba.w,0.0f,1.0f);
    return ((uint)(rgba.w*255.0f)<<24) | ((uint)(rgba.z*255.0f)<<16) | ((uint)(rgba.y*255.0f)<<8) | (uint)(rgba.x*255.0f);
}

uint intensityFloatToInt(float intensity)
{
    intensity = clamp(intensity, 0.0f, 1.0f);
    return ((uint)(intensity*255.0f)<<24) | ((uint)(intensity*255.0f)<<16) | ((uint)(intensity*255.0f)<<8) | (uint)(intensity*255.0f);
}
*/

//---------------------------------------------------------------------
// The vector result for the exit face as a short vector in X, Y, Z
// The element corresponding to the exit coordinate has the value 1
// The other coordinate elements are 0. The return has the 'w' element
// set to 1 if the result is a max face otherwise if a min face it is 0.
// cell_min is the minimum point in the cell and cell_max is the
// maximum point in the cell.
//---------------------------------------------------------------------
short4 cell_exit_face(float4 exit_point, float4 cell_min, float4 cell_max)
{
  float4 min_diff =   fabs(exit_point-cell_min);
  float4 max_diff =   fabs(exit_point-cell_max);

  float min=1e5f;
  short4 faceid=(short4)(-1,-1,-1,-1);
  if (min_diff.x<min)
  {
    min=min_diff.x;
    faceid=X_MIN;
  }
  if (min_diff.y<min)
  {
    min=min_diff.y;
    faceid=Y_MIN;
  }
  if (min_diff.z<min)
  {
    min=min_diff.z;
    faceid=Z_MIN;
  }
  if (max_diff.x<min)
  {
    min=max_diff.x;
    faceid=X_MAX;
  }
  if (max_diff.y<min)
  {
    min=max_diff.y;
    faceid=Y_MAX;
  }
  if (max_diff.z<min)
  {
    min=max_diff.z;
    faceid=Z_MAX;
  }

  return faceid;
}

short4 cell_exit_face_safe(float4 exit_point,float4 ray_d, float4 cell_min, float4 cell_max)
{
  float4 min_diff =   fabs(exit_point-cell_min);
  float4 max_diff =   fabs(exit_point-cell_max);

  float min=1e5f;
  short4 faceid=(short4)(-1,-1,-1,-1);
  if (min_diff.x<min && ray_d.x<0)
  {
    min=min_diff.x;
    faceid=X_MIN;
  }
  if (min_diff.y<min && ray_d.y<0)
  {
    min=min_diff.y;
    faceid=Y_MIN;
  }
  if (min_diff.z<min && ray_d.z<0)
  {
    min=min_diff.z;
    faceid=Z_MIN;
  }
  if (max_diff.x<min && ray_d.x>0)
  {
    min=max_diff.x;
    faceid=X_MAX;
  }
  if (max_diff.y<min&& ray_d.y>0)
  {
    min=max_diff.y;
    faceid=Y_MAX;
  }
  if (max_diff.z<min&& ray_d.z>0)
  {
    min=max_diff.z;
    faceid=Z_MAX;
  }

  return faceid;
}

short4 cell_exit_face_but_not_entry_face(float4 exit_point, float4 cell_min, float4 cell_max,short4 entry_face)
{
  float4 min_diff =  fabs(exit_point-cell_min);
  float4 max_diff =  fabs(exit_point-cell_max);

  short4 faceid=(short4) -1;

  float min=1.0f;

  // short4 temp; not used at the moment
  if (min_diff.x<min)
  {
    if (!(entry_face.x==1 && entry_face.w==0 ))
    {
      min=min_diff.x;
      faceid=X_MIN;
    }
  }
  if (min_diff.y<min )
  {
    if (!(entry_face.y==1 && entry_face.w==0 ))
    {
      min=min_diff.y;
      faceid=Y_MIN;
    }
  }
  if (min_diff.z<min)
  {
    if (!(entry_face.z==1 && entry_face.w==0 ))
    {
      min=min_diff.z;
      faceid=Z_MIN;
    }
  }
  if (max_diff.x<min )
  {
    if (!(entry_face.x==1 && entry_face.w==1 ))
    {
      min=max_diff.x;
      faceid=X_MAX;
    }
  }
  if (max_diff.y<min )
  {
    if (!(entry_face.y==1 && entry_face.w==1 ))
    {
      min=max_diff.y;
      faceid=Y_MAX;
    }
  }
  if (max_diff.z<min )
  {
    if (!(entry_face.z==1 && entry_face.w==1 ))
    {
      min=max_diff.z;
      faceid=Z_MAX;
    }
  }

  return faceid;
}

//-------------------------------------------------------------------
// Given the location code determine the bounding box for the
// cell in local tree coordinates, i.e. the max bounds of the
// tree are (0,0,0)->(1,1,1)
//-------------------------------------------------------------------
void cell_bounding_box(short4 loc_code, int n_levels,
                       float4* cell_min, float4* cell_max)
{
  float tree_size = (float)(1<<(n_levels-1));
  float cell_size = ((float)(1<<loc_code.w))/tree_size;
  float4 rtsize = (float4)(1.0f/tree_size);
  (*cell_min) = convert_float4(loc_code);
  (*cell_min) *= rtsize;
  float4 csize = (float4)cell_size;
  (*cell_max) = (*cell_min) + csize;
  (*cell_min).w = 0.0f;   (*cell_max).w = 0.0f;
}


#if 1
//--------------------------------------------------------------------------
// Given the ray origin, ray_o and its direction, ray_d and the cell min
// and max points, find the ray parameters, tnear and tfar that correspond
// to the entry and exit points of the cell-ray intersection. If the ray
// does not intersect the cell, 0 is returned.
//-------------------------------------------------------------------------
int intersect_cell(float4 ray_o, float4 ray_d, float4 cell_min, float4 cell_max, float *tnear, float *tfar)
{
  // compute intersection of ray with all six cell planes
  float4 invR = (float4)(1.0f,1.0f,1.0f,1.0f) / ray_d;

  float4 tmin = invR * (cell_min - ray_o);
  float4 tmax = invR * (cell_max - ray_o);

  // re-order intersections to find smallest and largest on each axis
  // minimum t values for either bounding plane
  float4 tmin_s =   min(tmax, tmin);
  // maximum t values for either bounding plane
  float4 tmax_s =   max(tmax, tmin);

  if (ray_d.x ==0.0f) {
    tmin_s.x = -3.4e38f;
    tmax_s.x = 3.4e38f;
  }
  if (ray_d.y ==0.0f) {
    tmin_s.y = -3.4e38f;
    tmax_s.y = 3.4e38f;
  }
  if (ray_d.z ==0.0f) {
    tmin_s.z = -3.4e38f;
    tmax_s.z = 3.4e38f;
  }

  // find the largest tmin and the smallest tmax
  float largest_tmin =   max(  max(tmin_s.x, tmin_s.y),   max(tmin_s.x, tmin_s.z));
  float smallest_tmax =   min(  min(tmax_s.x, tmax_s.y),   min(tmax_s.x, tmax_s.z));
  *tnear = largest_tmin;
  *tfar = smallest_tmax;
  return smallest_tmax > largest_tmin;
}
#endif

#if 0
//------------------------------------------------------------------------------
// New Intersect Cell takes in ray origin, ray direction, cell min and cell
// dimension (doesn't not assume cubic volume).  Doesn't use cell_max
// Lots of redundant code - everything here depends on RAY_D
//------------------------------------------------------------------------------
#if 0
int intersect_cell(float4 ray_o, float4 ray_d, float4 cell_min, float4 cell_dims, float *tnear, float *tfar)
{
  // compute intersection of ray with all six cell planes
  float4 tmin = (1.0/ray_d) * (cell_min - ray_o);
  float4 tmax = (1.0/ray_d) * (cell_min + cell_dims - ray_o);

  // re-order intersections to find smallest and largest on each axis
  // minimum t values for either bounding plane
  float4 tmin_s =   min(tmax, tmin);
  // maximum t values for either bounding plane
  float4 tmax_s =   max(tmax, tmin);

  if (ray_d.x == 0.0f) {
    tmin_s.x = -3.4e38f;
    tmax_s.x = 3.4e38f;
  }
  if (ray_d.y == 0.0f) {
    tmin_s.y = -3.4e38f;
    tmax_s.y = 3.4e38f;
  }
  if (ray_d.z == 0.0f) {
    tmin_s.z = -3.4e38f;
    tmax_s.z = 3.4e38f;
  }

  // find the largest tmin and the smallest tmax
  float largest_tmin  =   max(  max(tmin_s.x, tmin_s.y),   max(tmin_s.x, tmin_s.z));
  float smallest_tmax =   min(  min(tmax_s.x, tmax_s.y),   min(tmax_s.x, tmax_s.z));
  *tnear = largest_tmin;
  *tfar = smallest_tmax;
  return smallest_tmax > largest_tmin;
}
#endif
//------------------------------------------------------------------------------
// Optimized intersect cell takes in ray origin, ray direction, cell min and cell
// dimension (doesn't not assume cubic volume).  Doesn't use cell_max
// Lots of redundant code - everything here depends on RAY_D
//------------------------------------------------------------------------------
int intersect_cell_opt(float4 ray_o, float4 ray_d, float4 ray_d_inv, float4 cell_min, float4 cell_dims, float *tnear, float *tfar)
{

  // compute intersection of ray with all six cell planes
  float4 tmin = ray_d_inv * (cell_min - ray_o);
  float4 tmax = tmin + ray_d_inv*cell_dims;
  //float4 tmax = ray_d_inv * (cell_min + cell_dims - ray_o);

  // re-order intersections to find smallest and largest on each axis
  // minimum t values for either bounding plane
  float4 tmin_s =   min(tmax, tmin);
  // maximum t values for either bounding plane
  float4 tmax_s =   max(tmax, tmin);

  // find the largest tmin and the smallest tmax
  float largest_tmin  =   max(  max(tmin_s.x, tmin_s.y),   max(tmin_s.x, tmin_s.z));
  float smallest_tmax =   min(  min(tmax_s.x, tmax_s.y),   min(tmax_s.x, tmax_s.z));
  *tnear = largest_tmin;
  *tfar = smallest_tmax;
  return smallest_tmax > largest_tmin;
}
#endif


/*
//------------------------------------------------------------------------------
// Intersect scene takes in 3 floats for ray o, ray d, ray d inv
//------------------------------------------------------------------------------
void intersect_scene(float ray_ox, float ray_oy, float ray_oz,
                     float ray_dx, float ray_dy, float ray_dz,
                     //float min_fx, float min_fy, float min_fz,
                     //float max_fx, float max_fy, float max_fz,
                     float4 min_face, float4 max_face,
                     float *tnear, float *tfar)
{

  // compute intersection of ray with all six cell planes
  float4 tmin = ray_d_inv * (cell_min - ray_o);
  float4 tmax = tmin + ray_d_inv*cell_dims;
  //float4 tmax = ray_d_inv * (cell_min + cell_dims - ray_o);

  // re-order intersections to find smallest and largest on each axis
  // minimum t values for either bounding plane
  float4 tmin_s =   min(tmax, tmin);
  // maximum t values for either bounding plane
  float4 tmax_s =   max(tmax, tmin);

  // find the largest tmin and the smallest tmax
  float largest_tmin  =   max(  max(tmin_s.x, tmin_s.y),   max(tmin_s.x, tmin_s.z));
  float smallest_tmax =   min(  min(tmax_s.x, tmax_s.y),   min(tmax_s.x, tmax_s.z));
  *tnear = largest_tmin;
  *tfar = smallest_tmax;
  return smallest_tmax > largest_tmin;
}
*/


//-----------------------------------------------------------------------------
// find exit tvalue given an array of exit faces
//-----------------------------------------------------------------------------
float calc_t_exit(float4 ray_o, float4 ray_d_inv, float4 exit_planes)
{
  float4 tvec = (exit_planes - ray_o) * ray_d_inv;
  return min(min(tvec.x, tvec.y), tvec.z);
}

//--------------------------------------------------------------------------
// Find the ray entry point to a box that encloses the entire octree
// Returns 0 if there is no intersection.
//-------------------------------------------------------------------------
int cell_entry_point(float4 ray_org, float4 ray_dir,
                     float4 cell_min, float4 cell_max, float4* entry_pt)
{
  float tnear = 0, tfar =0;
  int hit = intersect_cell(ray_org, ray_dir, cell_min, cell_max,
                           &tnear, &tfar);
  if (!hit) {
    (*entry_pt)=(float4)(-1.0f, -1.0f, -1.0f, -1.0f);
    return 0;
  }
  else {
    (*entry_pt) = ray_org + tnear*ray_dir;
    return 1;
  }
}

//--------------------------------------------------------------------------
// Find the ray exit point from a cell that is specified by its loc_code
// Returns 0 if there is no intersection with the cell
//-------------------------------------------------------------------------

int cell_exit_point(float4 ray_org, float4 ray_dir,
                    float4 cell_min, float4 cell_max, float4* exit_pt)
{
  float tnear = 0, tfar =0;
  int hit = intersect_cell(ray_org, ray_dir, cell_min, cell_max,
                           &tnear, &tfar);
  if (!hit) {
    (*exit_pt)=(float4)(tnear, tfar, -1.0f, -1.0f);
    return 0;
  }
  else {
    (*exit_pt) = ray_org + tfar*ray_dir;
    return 1;
  }
}

int cell_contains_exit_pt(int n_levels, short4 loc_code, float4 exit_pt)
{
  exit_pt.w = 0.0f; ///should be no side effects since arg is copied
  float4 cell_min, cell_max;
  cell_bounding_box(loc_code, n_levels, &cell_min, &cell_max);
  int4 test =isless(exit_pt , cell_min);
  if (any(test)) return 0;
  test =isgreater(exit_pt , cell_max);
  if (any(test)) return 0;
  return 1;
}

