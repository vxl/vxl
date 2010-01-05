#define X_MIN (short4)(1,0,0,0);
#define X_MAX (short4)(1,0,0,1);
#define Y_MIN (short4)(0,1,0,0);
#define Y_MAX (short4)(0,1,0,1);
#define Z_MIN (short4)(0,0,1,0);
#define Z_MAX (short4)(0,0,1,1);

// Note: in the code below the term "level" always refers to the 
// 'index' of the level. That is, if there are 5 levels in an octree
// then the level of the root is 4 and level of the lowest leaves is 0.
// When referring to a count of levels the notation n_levels is used to
// distingush the term from a level index.

//-------------------------------------------------------------
// the code is three independent codes ordered as:  
// Xcode = ret.x; Ycode = ret.y; Zcode = ret.z; level = ret.w
//-------------------------------------------------------------
short4 loc_code(float4 point, short root_level)
{
  float max_val = 1 << root_level; // index of root
  ushort4 maxl = (ushort4)max_val;
  ushort4 temp = convert_ushort4_sat(max_val*point);
  ushort4 ret = min(temp, maxl);
  ret.w = 0;
  return convert_short4(ret);
}

//---------------------------------------------------------------------
// the location code for a child at a level and given the parent's code
//---------------------------------------------------------------------
short4 child_loc_code(uchar child_index, short child_level, short4 parent_code)
{
  short4 mask = (short4)(1, 2, 4, 0);
  short4 shft = (short4)(0, 1, 2, 0);
  short4 ind = (short4)(child_index);
  short4 temp = mask & ind;
  temp = temp >> shft;
  temp = temp << (short4)(child_level);
  short4 loc = temp + parent_code;
  loc.w = child_level;
  return loc;
}
//---------------------------------------------------------------------
// the bits in the child index are stored as [00...00ZYX], so the child
// index ranges from 0-7. The level index ranges from 0 to n_levels-1
//---------------------------------------------------------------------
uchar child_index(short4 code, short level)
{
  if(level == 0) return (uchar)255; //flag for error
  short child_bit = 1 << level-1;
  short4 mask = (short4)child_bit;
  short4 index = mask & code;
  uchar ret = (uchar)0;
  if(index.x) ret += 1;
  if(index.y) ret += 2;
  if(index.z) ret += 4;
  return ret;
}
//-----------------------------------------------------------------
// Traverse from the specified root_cell to the cell specified by loc_code.
// Return the array pointer to the resulting cell. If a leaf node is 
// encoutered during the traversal down the tree before the specified 
// code is reached, the leaf node index is returned.
//-----------------------------------------------------------------
int traverse(__global int4* cells, int cell_ptr, short4 cell_loc_code, 
             short4 target_loc_code, short4* found_loc_code)
{

  int found_cell_ptr = cell_ptr;
  int ret = -1;
  int level = target_loc_code.w;
  if( level < 0)
    return ret;
  int4 curr_cell = cells[cell_ptr];
  int curr_level = cell_loc_code.w;
  *found_loc_code = cell_loc_code;
  while(level<curr_level && curr_cell.y>0)
    {
      int c_ptr = curr_cell.y;
      uchar c_index = child_index(target_loc_code, curr_level);
      (*found_loc_code) = 
        child_loc_code(c_index, curr_level-1, *found_loc_code);
      c_ptr += c_index;
      curr_cell = cells[c_ptr];
      found_cell_ptr = c_ptr;
      --curr_level;
    }
  return found_cell_ptr;
}
//-----------------------------------------------------------------
// Traverse from the specified root_cell to the cell specified by loc_code.
// Return the array pointer to the resulting cell. If a leaf node is 
// encoutered during the traversal down the tree before the specified 
// code is reached, the leaf node index is returned.
//-----------------------------------------------------------------
int traverse_to_level(__global int4* cells, int cell_ptr, 
                      short4 cell_loc_code, short4 target_loc_code,
                      short target_level,
                      short4* found_loc_code)
{
  int found_cell_ptr = cell_ptr;
  int ret = -1;
  int level = target_level;
  if( level < 0)
    return ret;
  int4 curr_cell = cells[cell_ptr];
  int curr_level = cell_loc_code.w;
  *found_loc_code = cell_loc_code;
  while(level<curr_level && curr_cell.y>0)
    {
      int c_ptr = curr_cell.y;
      uchar c_index = child_index(target_loc_code, curr_level);
      (*found_loc_code) = 
        child_loc_code(c_index, curr_level-1, *found_loc_code);
      c_ptr += c_index;
      curr_cell = cells[c_ptr];
      found_cell_ptr = c_ptr;
      --curr_level;
    }
  return found_cell_ptr;
}

//-----------------------------------------------------------------
// Traverse from the current cell to find the cell whose code is 
// closest to the specified target_loc_code and lies in the sub-tree of 
// the root cell at cell_ptr with cell_loc_code. The cell array pointer 
// of the found cell is returned. Note that the specified target code may 
// not actually lie in the sub-tree. This method is necessary since small 
// errors may lead to a computed code for a point that lies outside the 
// cell of interest
//-----------------------------------------------------------------

int traverse_force(__global int4* cells, int cell_ptr, short4 cell_loc_code,
                    short4 target_loc_code, short4* found_loc_code)
{
  int found_cell_ptr = cell_ptr;
  (*found_loc_code) = cell_loc_code;
  int ret = (int)-1;
  int level = target_loc_code.w;
  if( level < 0)
    return ret;
  int curr_level = cell_loc_code.w;
  int4 curr_cell = cells[cell_ptr];//the root of the tree to search
  short4 curr_code = cell_loc_code;
  curr_code.w = curr_level;
  while(level<curr_level && curr_cell.y>0)
    {
      int c_ptr = curr_cell.y;
      short4 child_bit = (short4)(1);
      child_bit = child_bit << (short4)(curr_level-1);
      short4 code_diff = target_loc_code-curr_code;
      // TODO: find a way to compute the following as a vector op
      uchar c_index = 0;
      if(code_diff.x >= child_bit.x)
        c_index += 1;
      if(code_diff.y >= child_bit.y)
        c_index += 2;
      if(code_diff.z >= child_bit.z)
        c_index += 4;
      curr_code = child_loc_code(c_index, curr_level-1, curr_code);
      c_ptr += c_index;
      curr_cell = cells[c_ptr];
      found_cell_ptr = c_ptr;
      *found_loc_code = curr_code;
      --curr_level;
    }
  return found_cell_ptr;
}
//--------------------------------------------------------------------
// Find the common ancestor of a cell given a binary difference
// 
int common_ancestor(__global int4* cells, int cell_ptr, short4 cell_loc_code,
                    short4 target_loc_code, short4* ancestor_loc_code)
{
  short4 bin_diff = cell_loc_code ^ target_loc_code;
  short curr_level = (short)cell_loc_code.w;
  int curr_cell_ptr = cell_ptr;
  (*ancestor_loc_code) = cell_loc_code;
  int4 curr_cell = cells[curr_cell_ptr];
  short4 mask = (short4)(1 << (curr_level));
  short4 shift_one =(short4)1;//shift the mask by 1 as a vector
  short4 arg = bin_diff & mask; //masking the bits of the difference (xor)
  while(arg.x>0||arg.y>0||arg.z>0)//might be done as vector op
    {
      curr_cell_ptr = curr_cell.x;
      curr_cell = cells[curr_cell_ptr];
      //clear the code bit at each level while ascending to common ancestor
      short4 clear_bits = ~(short4)(mask);
      curr_level++;
      (*ancestor_loc_code) = (*ancestor_loc_code) & clear_bits;
      (*ancestor_loc_code).w = curr_level;
      mask = mask << shift_one;
      arg = bin_diff & mask;
    }
  return curr_cell_ptr;
}
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
  float4 min_diff = fabs(exit_point-cell_min);
  float4 max_diff = fabs(exit_point-cell_max);

  short4 faceid=(short4) -1;

  float min=1.0;

  if(min_diff.x<min)
  {
      min=min_diff.x;
    faceid=X_MIN;
  }
  if(min_diff.y<min)
  {
    min=min_diff.y;
    faceid=Y_MIN;
  }
  if(min_diff.z<min)
  {
    min=min_diff.z;
    faceid=Z_MIN;
  }
  if(max_diff.x<min)
  {
      min=max_diff.x;
    faceid=X_MAX;
  }
  if(max_diff.y<min)
  {
    min=max_diff.y;
    faceid=Y_MAX;
  }
  if(max_diff.z<min)
  {
    min=max_diff.z;
    faceid=Z_MAX;
  }


  return faceid;
  //short4 min_cmp = convert_short4(exit_point == cell_min);
  //short4 max_cmp = convert_short4(exit_point == cell_max);
  //min_cmp.w = 0;   max_cmp.w = 0; 
  ////min has priority over max in case of multiple hits
  ////X has priority over Y which has priority over Z in case of multiple hits
  //if(any(min_cmp)){
  //  if(min_cmp.x){return X_MIN;}
  //  if(min_cmp.y){return Y_MIN;}
  //  if(min_cmp.z){return Z_MIN;}
  //}else if(any(max_cmp)){
  //  if(max_cmp.x){return X_MAX;}
  //  if(max_cmp.y){return Y_MAX;}
  //  if(max_cmp.z){return Z_MAX;}
  //}
  //exit point doesn't lie on any face of the cell
  //return (short4) -1;
}
short4 cell_exit_face_but_not_entry_face(float4 exit_point, float4 cell_min, float4 cell_max,short4 entry_face)
{
    float4 min_diff = fabs(exit_point-cell_min);
    float4 max_diff = fabs(exit_point-cell_max);

    short4 faceid=(short4) -1;

    float min=1.0;

    short4 temp;
    if(min_diff.x<min)
    {
        if(!(entry_face.x==1 && entry_face.w==0 ))
        {
            min=min_diff.x;
            faceid=X_MIN;
        }
    }
    if(min_diff.y<min )
    {
        if(!(entry_face.y==1 && entry_face.w==0 ))
        {
            min=min_diff.y;
            faceid=Y_MIN;
        }
    }
    if(min_diff.z<min)
    {
        if(!(entry_face.z==1 && entry_face.w==0 ))
        {

            min=min_diff.z;
            faceid=Z_MIN;
        }
    }
    if(max_diff.x<min )
    {
        if(!(entry_face.x==1 && entry_face.w==1 ))
        {
            min=max_diff.x;
            faceid=X_MAX;
        }
    }
    if(max_diff.y<min )
    {
        if(!(entry_face.y==1 && entry_face.w==1 ))
        {
            min=max_diff.y;
            faceid=Y_MAX;
        }
    }
    if(max_diff.z<min )
    {
        if(!(entry_face.z==1 && entry_face.w==1 ))
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
//-------------------------------------------------------------------
// Given the location code determine the bounding box for the
// cell in local tree coordinates, i.e. the max bounds of the
// tree are (0,0,0)->(1,1,1)
//-------------------------------------------------------------------
int neighbor(__global int4* cells, int cell_ptr, short4 cell_loc_code,
             short4 exit_face, short n_levels, short4* neighbor_code)
{
  short cell_level = cell_loc_code.w;
  short cell_size = 1<<cell_level;
  short4 error = (short4)-1;
  int neighbor_ptr = -1;
  // if the neighbor is on the min face
  if(exit_face.w==0)
    {
      short4 zero = (short4)0;
      (*neighbor_code) = cell_loc_code - exit_face;
      (*neighbor_code).w = 0;//smallest cell level possible
      short4 test =(short4)((*neighbor_code) < zero);
      if(any(test)){
        (*neighbor_code) = error;
        return neighbor_ptr;
      }
    }else{
      short4 largest = (short4)(1<<(n_levels-1));
      short4 csize = (short4)cell_size;
      csize.w = 0;
      (*neighbor_code) = cell_loc_code + (csize*exit_face);
      (*neighbor_code).w = 0;
      short4 test =(short4)((*neighbor_code) >= largest);
      if(any(test)){
        (*neighbor_code) = error;
        return neighbor_ptr;
      }
    }
  short4 ancestor_loc_code = error;
  int ancestor_ptr =  common_ancestor(cells, cell_ptr, cell_loc_code,
                                          (*neighbor_code),
                                          &ancestor_loc_code);
  if(ancestor_ptr<0){
    (*neighbor_code) = error;
    return neighbor_ptr;
  }
  neighbor_ptr = 
    traverse_to_level(cells, ancestor_ptr, ancestor_loc_code,
                      (*neighbor_code), cell_level, neighbor_code);
  return neighbor_ptr;
}
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
  float4 tmin_s = min(tmax, tmin); 
  // maximum t values for either bounding plane
  float4 tmax_s = max(tmax, tmin); 

  if(ray_d.x ==0.0f){
    tmin_s.x = -3.4e38f; 
    tmax_s.x = 3.4e38f;
  }
      
  if(ray_d.y ==0.0f){
    tmin_s.y = -3.4e38f; 
    tmax_s.y = 3.4e38f;
  }

      
  if(ray_d.z ==0.0f){
    tmin_s.z = -3.4e38f; 
    tmax_s.z = 3.4e38f;
  }

  // find the largest tmin and the smallest tmax
  float largest_tmin = max(max(tmin_s.x, tmin_s.y), max(tmin_s.x, tmin_s.z));
  float smallest_tmax = min(min(tmax_s.x, tmax_s.y), min(tmax_s.x, tmax_s.z));
  //if(fabs(smallest_tmax-largest_tmin)<1.0e-5f)
  //  {
  //    //advance along the ray 
  //    smallest_tmax += 1.0e-5f;
  //  }
	*tnear = largest_tmin;
	*tfar = smallest_tmax;
	return 1;//smallest_tmax > largest_tmin;
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
  if(!hit){
    (*entry_pt)=(float4)(-1.0f, -1.0f, -1.0f, -1.0f);
    return 0;
  }else{
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
  if(!hit){
    (*exit_pt)=(float4)(tnear, tfar, -1.0f, -1.0f);
    return 0;
  }else{
    (*exit_pt) = ray_org + tfar*ray_dir;
    return 1;
  }
}
// end of library kernels

