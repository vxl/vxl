#ifndef vbl_basic_relation_type_h
#define vbl_basic_relation_type_h

//-----------------------------------------------------------------------------
//
// .LIBRARY vbl
// .HEADER vxl package
// .INCLUDE vbl/vbl_basic_relation_type.h
// .FILE vbl/vbl_basic_relation_type.cxx
//
// .SECTION Author:
//             Rupert Curwen, August 28th, 1998
//             GE Corporate Research and Development
//
// .SECTION Modifications
//     None Yet
//
//-----------------------------------------------------------------------------

//: A base class for all vbl_basic_relations; provides type-safe downcasting
//
class vbl_basic_relation_type
{
public:
  virtual ~vbl_basic_relation_type() {}

  // This virtual function returns the address of a static variable
  // which is unique for each vbl_basic_relation templated type.
  virtual void* GetType() { return NULL; }
  virtual void* This() { return (void*)this; }

  // DEXisms.
  int GetDEXID() { return dex_id; }
  void SetDEXID(int id) { dex_id = id; }

protected:
  int dex_id;

};

#endif
