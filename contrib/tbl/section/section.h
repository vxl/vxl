#ifndef section_h_
#define section_h_
//:
// \file
// \brief light-weight templated buffer of arbitrary dimensionality
//
//   The vipl library provides the core implementation for
//   all image processing functionality.  To make this library really generic
//   and completely independent from any other part of TargetJr/vxl, and hence
//   usable by other software packages than just TargetJr, all reference to
//   TargetJr-specific functionality is avoided or parametrised.
//
//   The class `section' is created specifically for the purpose of serving
//   as the in-memory buffer to be passed around between the low-level image
//   processing functions.  Essentially, it is just a `bare bones' T* allocated
//   buffer:  the data member section::buffer is publicly accessible.
//   The most important, and indispensable, extra functionality is its
//   interpretation as an n-dimensional rectangular pixel array, where n is
//   almost always 2, but n=3 and (importantly!) n=1 will also occur often!
//   [Note that the PIKS image model is 5-dimensional: 3 space dimensions, a
//    time dimension, and a spectral dimension.  Thus, an ordinary colour
//    image is threedimensional, with (in the interleaved case) size[0] = 3.]
//
//   The chosen dimensionality has to be passed to the constructor somehow;
//   passing it as a parameter has an important drawback: the data member size
//   is to be allocated by the constructor, and deallocated by the destructor.
//   This makes it impossible to pass variables of type section as return values
//   because the destructor gets called at `unexpected' places.
//   Therefore, the dimensionality has to be a templated parameter, so the whole
//   size[] array can be made part of the section data structure.
//
//   The only remaining allocated data member is buffer; but also there,
//   explicit deallocation by the destructor can be avoided by passing the
//   constructor an allocated buffer.  The default behaviour is of course to
//   let the buffer be allocated and deallocated automatically.
//
//   Normally, a single templated class section<T,N> should suffice.
//   Having these share a common, pure virtual parent class has an important
//   advantage:  function parameters can be of this more general, "collective"
//   type.  But using this should be avoided whenever possible!
//   Actually, two levels of collective parent classes is provided:
//   section_<T> and section__.
//
// \author
//   Peter Vanroose, ESAT/KULeuven, december 1996.

#include <iostream>
#include <cstddef>
#include <cstring>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

typedef unsigned int uint;

class section__ { protected: section__()= default; public: virtual ~section__()= default; };
template <class T> class section_ : public section__ { protected: section_():section__(){} ~section_() override = default; };

template <class T, uint N> class section_iterator; // forward declaration

template <class T, uint N> class section : public section_<T>
{
#ifdef __GNUC__
  friend class section_iterator<T,N>;
#endif

 public:
  typedef section_iterator<T,N> iterator;

  // DATA MEMBERS:

 public:
  T*     buffer;       // allocated buffer can freely be accessed.
  uint   ROI_start[N]; // upper left corner of the region of interest.
  uint   ROI_end[N];   // lower right corner of the region of interest.

 private:
  uint   size[N];    // size[0] to size[N-1]. Dimension 0 is run through
                     // first; the `slowest' increasing dimension is N-1
  std::size_t offset[N+1];// offset[i] gives the pointer difference between two
                     // neighbour pixels that only differ in the i dimension.
                     // In addition, offset[N] gives the allocated size.
  bool   allocated;  // whether buffer is (to be) allocated by this class

  // ACCESS METHODS:

 public:
  static uint   Dimensionality() { return N; }
  uint   const* Size()    const { return size; }
  uint   Size(uint i)     const { assert(i<N); return size[i]; }
  uint   width()          const { return size[0]; }
  uint   height()         const { return size[1]; }
  std::size_t Offset(uint i)   const { assert(i<=N); return offset[i]; }
  std::size_t GetSize()        const { return offset[N]; }
  T      Value(const uint pos[N]) const { return buffer[Position(pos)]; }
  T      Value(uint pos0) const { assert(N==1); return buffer[pos0*offset[0]]; }
  T      Value(uint pos0,uint pos1) const { assert(N==2); return buffer[pos0*offset[0]+pos1*offset[1]]; }
  T      Value(uint pos0,uint pos1,uint pos2) const { assert(N==3); return buffer[pos0*offset[0]+pos1*offset[1]+pos2*offset[2]]; }
  void   Set(T const& val,const uint pos[N]) { buffer[Position(pos)]=val; }
  void   Set(T const& val,uint pos0) { assert(N==1); buffer[pos0*offset[0]]=val; }
  void   Set(T const& val,uint pos0,uint pos1) { assert(N==2); buffer[pos0*offset[0]+pos1*offset[1]]=val; }
  void   Set(T const& val,uint pos0,uint pos1,uint pos2) { assert(N==3); buffer[pos0*offset[0]+pos1*offset[1]+pos2*offset[2]]=val; }
  std::size_t Position(const uint pos[N])  const { std::size_t p = 0; for (uint i=0; i<N; ++i) p += pos[i]*Offset(i); return p; }
  void   Position(uint ret[N], const std::size_t pos)  const { for (uint i=0; i<N; ++i) ret[i] = Position(pos,i); }
  uint   Position(const std::size_t pos, const uint i)  const { std::size_t p = pos % Offset(i+1); p /= Offset(i); return uint(p); }
  std::size_t ROI_start_pos()  const { return Position(ROI_start); }
  std::size_t ROI_end_pos()  const { return Position(ROI_end); }

  const T* begin() const {return buffer;}
  T*       begin()       {return buffer;}

  const T* end() const {return buffer + GetSize();}
  T*       end()       {return buffer + GetSize();}

  // CONSTRUCTORS / DESTRUCTORS:

 public:
  section(const uint sz[N], T* b=0) { init(sz,b); }
  section(T* b=0) { assert(N==0); init(0,b); }
  section(uint sz0, T* b=0) { assert(N==1); uint sz[1]={sz0}; init(sz,b); }
  section(uint sz0, uint sz1, T* b=nullptr) { assert(N==2); uint sz[2]={sz0,sz1}; init(sz,b); }
  section(uint sz0, uint sz1, uint sz2, T* b=0) { assert(N==3); uint sz[3]={sz0,sz1,sz2}; init(sz,b); }
  section(section<T,N> const& s) {
    init(s.Size(),0); std::memcpy(buffer,s.buffer,offset[N]*sizeof(T));
    for (uint i=0; i<N; ++i)ROI_start[i]=s.ROI_start[i],ROI_end[i]=s.ROI_end[i];
  }

  ~section() override{ if (allocated) delete[] buffer; }

 private:
  void init(const uint sz[N], T* buf) {
    allocated = (buf == nullptr);
    offset[0] = 1;
    for (uint i=0; i<N; ++i) {
      size[i] = sz[i]; offset[i+1] = sz[i]*offset[i];
      ROI_start[i] = 0; ROI_end[i] = sz[i];
    }
    if (allocated) buffer = new T[offset[N]]; else buffer = buf;
  }

  // OPERATORS:

 public:
  inline bool operator== (section<T,N> const&) const;
  inline section<T,N>& operator= (section<T,N> const&);
  T operator() (uint x0) { assert(N==1 && x0<size[0]); return buffer[x0]; }
  T operator() (uint x0, uint x1) {
    assert(N==2 && x0<size[0] && x1<size[1]); return buffer[x0+x1*offset[1]]; }
  T operator() (uint x0, uint x1, uint x2) {
    assert(N==3 && x0<size[0] && x1<size[1] && x2<size[2]);
    return buffer[x0+x1*offset[1]+x2*offset[2]]; }

  // UTILITY FUNCTIONS:  (made members, just for the ease of instantiating)

 public:
  //: Returns a newly allocated copy of this section.
  section<T,N> Copy() const {
    T* buf = new T[GetSize()];
    std::memcpy(buf, buffer, GetSize()*sizeof(T));
    section<T,N> t(Size(),buf);
    for (uint i=0; i<N; ++i)t.ROI_start[i]=ROI_start[i],t.ROI_end[i]=ROI_end[i];
    return t; }
};

template <class T, uint N>
bool section<T,N>::operator== (section<T,N> const& s) const
{
  {for (uint i=0; i<N; ++i) if (Size(i) != s.Size(i)) return false;}
  {for (uint i=0; i<N; ++i) if (ROI_start[i] != s.ROI_start[i]) return false;}
  {for (uint i=0; i<N; ++i) if (ROI_end[i] != s.ROI_end[i]) return false;}
  if (buffer == s.buffer) return true;
  for (std::size_t i=0; i<GetSize(); ++i) if (!(buffer[i]==s.buffer[i])) return false;
  return true;
}

template <class T, uint N>
section<T,N>& section<T,N>::operator= (section<T,N> const& s)
{
  if (allocated) delete[] buffer;
  init(s.Size(),0); std::memcpy(buffer,s.buffer,offset[N]*sizeof(T));
  for (uint i=0; i<N; ++i) ROI_start[i]=s.ROI_start[i], ROI_end[i]=s.ROI_end[i];
  return *this;
}

//: Returns part of this section, namely a slice of dimension dim.
// When dim = the last dimension (the default), no new allocation is made;
// instead, the relevant part of the original buffer is used.  Beware!
// (To avoid this, use Copy() afterwards, or set the copy parameter to true.)

template <class T, uint N>
section<T,N-1> Project(section<T,N> const& s, uint slice=0, int d=-1, bool copy=false)
{
  uint dim = (d < 0) ? N-1 : d; // default: the last dimension
  assert(N > 0 && dim < N && slice < s.Size(dim));
  uint size[N-1];
  uint i=0;
  for (; i<dim; ++i) size[i] = s.Size(i);
  for (; i<N-1; ++i) size[i] = s.Size(i+1);
  std::size_t off = slice * s.Offset(dim);
  if (dim == N-1) { // this is the simplest (and preferred) situation
                    // because no copying needs to be done.
    section<T,N-1> t(size, s.buffer+off);
    if (!copy) return t; else return t.Copy();
  }
  std::size_t len = s.Offset(dim);
  T* buf = new T[s.GetSize()/s.Size(dim)]; T* nptr = buf;
  for (T* optr=s.buffer+off; optr<s.buffer+s.GetSize(); nptr+=len,optr+=s.Offset(dim+1))
    std::memcpy(nptr, optr, len*sizeof(T));
  section<T,N-1> t(size, buf);
  for (i=0; i<dim; ++i) t.ROI_start[i]=s.ROI_start[i],t.ROI_end[i]=s.ROI_end[i];
  for (; i<N-1; ++i)t.ROI_start[i]=s.ROI_start[i+1],t.ROI_end[i]=s.ROI_end[i+1];
  return t;
}

//: const iterator for a section<T,N>'s ROI, with optional additional ROI condition

template <class T, uint N> class section_iterator
{
  friend class section<T,N>;
 protected:
  section<T,N>* data;
  std::size_t pos;

 public:
  section_iterator(section<T,N>& s) : data(&s), pos(s.ROI_start_pos()) {}
  T operator*() const { return data->buffer[pos]; }
  operator std::size_t() const { return pos; }

  bool operator==(const section_iterator<T,N>& x) const { return pos == std::size_t(x); }
  bool operator==(const uint p[N]) const { return pos == data->Position(p); }
  bool operator==(const std::size_t p) const { return pos == p; }
  bool operator<(const section_iterator<T,N>& x) const { return pos < std::size_t(x); }
  bool operator<(const uint p[N]) const { return pos < data->Position(p); }
  bool operator<(const std::size_t p) const { return pos < p; }
  bool operator>(const section_iterator<T,N>& x) const { return pos > std::size_t(x); }
  bool operator>(const uint p[N]) const { return pos > data->Position(p); }
  bool operator>(const std::size_t p) const { return pos > p; }

  section_iterator<T,N>& operator++() { ++pos;
    for (uint i=0; i<N; ++i) if (data->Position(pos,i) >= data->ROI_end[i])
      pos += data->ROI_start[i] - (pos%data->Offset(i+1)) + data->Offset(i+1);
    if (pos > data->ROI_end_pos()) pos = data->GetSize();
    return *this; }
  section_iterator<T,N> operator++(int) { section_iterator<T,N> tmp = *this; ++*this; return tmp; }
  section_iterator<T,N>& operator--()  { --pos;
    for (uint i=0; i<N; ++i) if (data->Position(pos,i) < data->ROI_start[i])
      pos += data->ROI_end[i] - (pos%data->Offset(i+1)) - data->Offset(i+1);
    if (pos < data->ROI_start_pos() || pos > data->ROI_end_pos()) pos = data->GetSize();
    return *this; }
  section_iterator<T,N> operator--(int) { section_iterator<T,N> tmp = *this; --*this; return tmp; }
};

#define SECTION_INSTANTIATE_ONE(T,N) \
template class section<T,N >; \
template class section_iterator<T,N >
#define SECTION_INSTANTIATE_NO_PROJ(T) \
template class section_<T >; \
SECTION_INSTANTIATE_ONE(T,3); \
SECTION_INSTANTIATE_ONE(T,2); \
SECTION_INSTANTIATE_ONE(T,1)
#ifdef __GNUG__
#define SECTION_INSTANTIATE(T) \
SECTION_INSTANTIATE_NO_PROJ(T); \
template section<T,2> Project(section<T,3> const&, uint, int, bool); \
template section<T,1> Project(section<T,2> const&, uint, int, bool) // i.e., no Project() for dimension 1
#else
#define SECTION_INSTANTIATE(T) \
SECTION_INSTANTIATE_NO_PROJ(T)
#endif

#endif // section_h_
