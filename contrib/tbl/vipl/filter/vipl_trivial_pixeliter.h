//:
// \file
#ifndef vipl_trivial_pixeliter_h_
#define vipl_trivial_pixeliter_h_
//:
// This class provides a trivial "iterator" class for use as the default pixel
// iterator in the new filter paradigm.  Any pixel iterator must provide a
// cast-to integer method (for the appropriate "axis").  The class has
// typedefs, to int, for the 4 "iterators" that we expect to be used.
// It has little functionality, and also serves as an example/test.

class vipl_trivial_pixeliter
{
 public:
  typedef int Titerator;
  typedef int Xiterator;
  typedef int Yiterator;
  typedef int Ziterator;
};

#endif // vipl_trivial_pixeliter_h_
