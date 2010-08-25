// Loc Code Library Functions:
// Note: in the code below the term "level" always refers to the
// 'index' of the level. That is, if there are 5 levels in an octree
// then the level of the root is 4 and level of the lowest leaves is 0.
// When referring to a count of levels the notation n_levels is used to
// distinguish the term from a level index.

//-------------------------------------------------------------
// the code is three independent codes ordered as:
// Xcode = ret.x; Ycode = ret.y; Zcode = ret.z; level = ret.w
//-------------------------------------------------------------
short4 loc_code(float4 point, short root_level)
{
  float max_val = 1 << root_level; // index of root
  ushort4 maxl = (ushort4)max_val;
  ushort4 temp = convert_ushort4_sat(max_val*point);
  ushort4 ret =  min(temp, maxl);
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
  if (level == 0) return (uchar)255; // flag for error
  short child_bit = 1 << (level-1);
  short4 mask = (short4)child_bit;
  short4 index = mask & code;
  uchar ret = (uchar)0;
  if (index.x) ret += 1;
  if (index.y) ret += 2;
  if (index.z) ret += 4;
  return ret;
}
