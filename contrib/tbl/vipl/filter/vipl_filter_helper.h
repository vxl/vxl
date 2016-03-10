#ifndef vipl_filter_helper_h_
#define vipl_filter_helper_h_

typedef unsigned int VIPL_FILTER_STATE;
// the following is defined in vipl_filter.h
//extern const VIPL_FILTER_STATE Not_Ready;
//extern const VIPL_FILTER_STATE Ready;
//extern const VIPL_FILTER_STATE Unchanged;
//extern const VIPL_FILTER_STATE Filter_Owned;
#define READY(x) (x & Ready)
#define NOT_READY(x) (!(x & Ready))
#define UNCHANGED(x) (x & Unchanged)
#define CHANGED(x) (!(x & Unchanged))
#define USER_OWNED(x) (!(x & Filter_Owned))
#define FILTER_OWNED(x) (x & Filter_Owned)
#define NOT(x) (!x)

// In order to support filtering where the destination section has different
// starting coordinates, the following macros need to be updated
// MASK_CONV, HORIZ_CONV and VERTI_CONV are the actual macros that implement
// the filtering operations - they, as well as the macros that call these will
// need to be modified to support non-uniform section filtering.
// these macros are defined HERE because two separate subclasses branches of
// this ABS filter class use them unmodified.
// These macros do not use any pointer arithmetic, so they are nowhere near as
// efficient as filters can be. A particular bottleneck is all the 2-D
// operator() calls that the convolutions make.  Chances are the filterable
// objects use a multiply-add to implement operator(). (ouch!)

#define FILL(X1, Y1, X2, Y2, DST, FV) do {\
  for (int y = (Y1); y < (Y2); ++y) {\
    for (int x = (X1); x < (X2); ++x) {\
      SET_PIXEL((DST),x, y, (FV));\
    }\
  }\
} while (false)

#define MASK_CONV(X1, Y1, X2, Y2, DELTA_X, DELTA_Y, SRC, DST, KERN) do {\
  KernType val;\
  std::clock_t ct = clock(),ct1;\
  for (int y = (Y1); y < (Y2); ++y)\
    for (int x = (X1) ; x < (X2); ++x) {\
      val = 0.0;\
      int je = (KERN).y_end(); int ie = (KERN).x_end();\
      for (int j = (KERN).y_start(), jj=y+j; j < je; ++j) {\
        for (int i = (KERN).x_start(), ii=x+i; i < ie; ++i)\
        val += ((KERN) (i, j) * FGET_PIXEL((SRC), ii++, jj));\
        ++jj;\
        FSET_PIXEL((DST), x, y, (CONVERT_TO_OUT(val)));\
      }\
    }\
  ct1 = clock();\
} while (false)

#define MASK_CONV_BOARDER(X1, Y1, X2, Y2, DELTA_X, DELTA_Y, SRC, DST, KERN) \
  do {\
  KernType val;\
  std::clock_t ct = clock(),ct1;\
  for (int y = (Y1); y < (Y2); ++y)\
    for (int x = (X1) ; x < (X2); ++x) {\
      val = 0.0;\
      int je = (KERN).y_end(); int ie = (KERN).x_end();\
      for (int j = (KERN).y_start(), jj=y+j; j < je; ++j) {\
        for (int i = (KERN).x_start(), ii=x+i; i < ie; ++i)\
        val += ((KERN) (i, j) * GET_PIXEL((SRC), ii++, jj));\
        ++jj;\
        SET_PIXEL((DST), x, y, (CONVERT_TO_OUT(val)));\
      }\
    }\
  ct1 = clock();\
} while (false)

#define NORMAL_INTERMEDIATE
#ifdef NORMAL_INTERMEDIATE

// extern int over, under;

#define HORIZ_CONV(X1, Y1, X2, Y2, DELTA_X, DELTA_Y, SRC, DST, KERN) do {\
  std::clock_t ct = clock();\
  KernType val;\
  for (int y = ((Y1) ); y < ((Y2) ); ++y) {\
    for (int x = ((X1) ); x < ((X2) ); ++x) {\
      val = 0.0;\
      for (int i = (KERN).kernel_start(0),j=i+x; i < (KERN).kernel_end(0); ++i) \
        val += ((KERN)(i) * FGET_PIXEL((SRC),j++, y));\
      FSET_PIXEL((DST),x, y, (CONVERT_TO_OUT(val)));\
    }\
  }\
  std::clock_t ct2= clock();\
} while (false)

#define HORIZ_CONV_BOARDER(X1, Y1, X2, Y2, DELTA_X, DELTA_Y, SRC, DST, KERN) do {\
  std::clock_t ct = clock();\
  KernType val;\
  for (int y = ((Y1) ); y < ((Y2) ); ++y) {\
    for (int x = ((X1) ); x < ((X2) ); ++x) {\
      val = 0.0;\
      for (int i = (KERN).kernel_start(0),j=i+x; i < (KERN).kernel_end(0); ++i) \
        val += ((KERN)(i) * GET_PIXEL((SRC),j++, y));\
      SET_PIXEL((DST),x, y, (CONVERT_TO_OUT(val)));\
    }\
  }\
  std::clock_t ct2= clock();\
} while (false)

#define VERTI_CONV(X1, Y1, X2, Y2, DELTA_X, DELTA_Y, SRC, DST, KERN) do {\
  KernType val;\
  std::clock_t ct = clock();\
  for (int x = ((X1) ); x < ((X2) ); ++x) {\
    for (int y = ((Y1) ); y < ((Y2) ); ++y) {\
      val = 0;\
      for (int i = (KERN).kernel_start(0), j =y+i; i < (KERN).kernel_end(0); ++i)\
        val += ((KERN)(i) * FGET_PIXEL((SRC),x, j++));\
      FSET_PIXEL((DST),x, y, (CONVERT_TO_OUT(val)));\
    }\
  }\
  std::clock_t ct2= clock();\
} while (false)

#define VERTI_CONV_BOARDER(X1, Y1, X2, Y2, DELTA_X, DELTA_Y, SRC, DST, KERN) do {\
  KernType val;\
  std::clock_t ct = clock();\
  for (int x = ((X1) ); x < ((X2) ); ++x) {\
    for (int y = ((Y1) ); y < ((Y2) ); ++y) {\
      val = 0;\
      for (int i = (KERN).kernel_start(0), j =y+i; i < (KERN).kernel_end(0); ++i)\
        val += ((KERN)(i) * GET_PIXEL((SRC),x, j++));\
      SET_PIXEL((DST),x, y, (CONVERT_TO_OUT(val)));\
    }\
  }\
  std::clock_t ct2= clock();\
} while (false)

#else //else we flip intermediate

#define HORIZ_CONV(X1, Y1, X2, Y2, DELTA_X, DELTA_Y, SRC, DST, KERN) do {\
  KernType val;\
  std::clock_t ct = clock();\
  for (int y = ((Y1) + DELTA_Y); y < ((Y2) - (DELTA_Y)); ++y) {\
    for (int x = ((X1) + (DELTA_X)); x < ((X2) - (DELTA_X)); ++x) {\
      val = 0.0;\
      for (int i = (KERN).kernel_start(0); i < (KERN).kernel_end(0); ++i) \
        val += ((KERN) (i) * FGET_PIXEL((SRC), x+i,y));\
      FSET_PIXEL((DST), y,x, (CONVERT_TO_OUT(val)));\
    }\
  }\
  std::clock_t ct2= clock();\
} while (false)

#define HORIZ_CONV_BOARDER(X1, Y1, X2, Y2, DELTA_X, DELTA_Y, SRC, DST, KERN) \ do {\
  KernType val;\
  std::clock_t ct = clock();\
  for (int y = ((Y1) + DELTA_Y); y < ((Y2) - (DELTA_Y)); ++y) {\
    for (int x = ((X1) + (DELTA_X)); x < ((X2) - (DELTA_X)); ++x) {\
      val = 0.0;\
      for (int i = (KERN).kernel_start(0); i < (KERN).kernel_end(0); ++i) \
        val += ((KERN) (i) * GET_PIXEL((SRC), x+i,y));\
      SET_PIXEL((DST), y,x, (CONVERT_TO_OUT(val)));\
    }\
  }\
  std::clock_t ct2= clock();\
} while (false)

#define VERTI_CONV(X1, Y1, X2, Y2, DELTA_X, DELTA_Y, SRC, DST, KERN)
   do {\
  KernType val;\
  std::clock_t ct = clock();\
  for (int x = ((X1) + (DELTA_X)); x < ((X2) - (DELTA_X)); ++x) {\
    for (int y = ((Y1) + (DELTA_Y)); y < ((Y2) - (DELTA_Y)); ++y) {\
      val = 0;\
      for (int i = (KERN).kernel_start(0); i < (KERN).kernel_end(0); ++i)\
        val += ((KERN) (i) * FGET_PIXEL((SRC),y+i,x));\
      FSET_PIXEL((DST),x, y, (CONVERT_TO_OUT(val)));\
    }\
  }\
  std::clock_t ct2= clock();\
} while (false)

#define VERTI_CONV_BOARDER(X1, Y1, X2, Y2, DELTA_X, DELTA_Y, SRC, DST, KERN)\ do {\
  KernType val;\
  std::clock_t ct = clock();\
  for (int x = ((X1) + (DELTA_X)); x < ((X2) - (DELTA_X)); ++x) {\
    for (int y = ((Y1) + (DELTA_Y)); y < ((Y2) - (DELTA_Y)); ++y) {\
      val = 0;\
      for (int i = (KERN).kernel_start(0); i < (KERN).kernel_end(0); ++i)\
        val += ((KERN) (i) * GET_PIXEL((SRC),y+i,x));\
      SET_PIXEL((DST),x, y, (CONVERT_TO_OUT(val)));\
    }\
  }\
  std::clock_t ct2= clock();\
} while (false)

#endif

// convolve the top strip from left edge of section to right edge of section
// for Top,Bot,Left,Right, the initial coordinates are for filling the borders
// Ok here is the logic

#define IMB_LEFT(D,C) ((D).curr_sec_start(X_Axis()) == (C).image_start(X_Axis()))
#define IMB_RIGHT(D,C) ((D).curr_sec_end(X_Axis()) == (C).image_end(X_Axis()))
#define IMB_TOP(D,C) ((D).curr_sec_end(Y_Axis()) == (C).image_end(Y_Axis()))
#define IMB_BOTTOM(D,C) ((D).curr_sec_start(Y_Axis()) == (C).image_start(Y_Axis()))

// the macro CONVOL_PREAMBLE is for use in an another macro to define useful values
// section start and end coordinates - short

#define CONVOL_PREAMBLE(CONTAINER, DESCRIPTOR) \
  const vipl_section_container<DataTypeOut> &C=(CONTAINER);\
  const vipl_section_descriptor<DataTypeOut> &D=(DESCRIPTOR);\
  int border = 0;\
  const int ibs = image_border_size();\
  const int d0 = ibs;\
  const int bdr = ibs;\
  const DataTypeOut fv = def_fill_value();\
  int xcs[4], ycs[4], xce[4], yce[4];\
  int do_fill[4]; \
  const int XS = D.curr_sec_start(X_Axis()), YS = D.curr_sec_start(Y_Axis());\
  const int XE = D.curr_sec_end(X_Axis()), YE = D.curr_sec_end(Y_Axis());\
  do_fill[0] = do_fill[1] = do_fill[2] = do_fill[3] = false

#define CALC_BORDER(B, D, C) (B) = 0; \
  if (IMB_LEFT((D), (C))) (B) |= 1;\
  if (IMB_TOP((D), (C))) (B) |= 2;\
  if (IMB_RIGHT((D), (C))) (B) |= 4;\
  if (IMB_BOTTOM((D), (C))) (B) |= 8;

// the next 4 are for quick "border" comparisons.

#define IMB_L (1)
#define IMB_T (2)
#define IMB_R (4)
#define IMB_B (8)

// for use in both preop and postop

#define CHECK_START_AND_END_FILL(CONTAINER) \
    if ((CONTAINER).image_start(Y_Axis()) > ycs[tmpi]){ ycs[tmpi] = (CONTAINER).image_start(Y_Axis()); }\
    if ((CONTAINER).image_start(X_Axis()) > xcs[tmpi]){ xcs[tmpi] = (CONTAINER).image_start(X_Axis()); }\
    if ((CONTAINER).image_start(Y_Axis()) > yce[tmpi]){ yce[tmpi] = (CONTAINER).image_start(Y_Axis()); }\
    if ((CONTAINER).image_start(X_Axis()) > xce[tmpi]){ xce[tmpi] = (CONTAINER).image_start(X_Axis()); }\
    if ((CONTAINER).image_end(Y_Axis()) < ycs[tmpi]){ ycs[tmpi] = (CONTAINER).image_end(Y_Axis()); }\
    if ((CONTAINER).image_end(X_Axis()) < xcs[tmpi]){ xcs[tmpi] = (CONTAINER).image_end(X_Axis()); }\
    if ((CONTAINER).image_end(Y_Axis()) < yce[tmpi]){ yce[tmpi] = (CONTAINER).image_end(Y_Axis()); }\
    if ((CONTAINER).image_end(X_Axis()) < xce[tmpi]){ xce[tmpi] = (CONTAINER).image_end(X_Axis()); }
#define CHECK_START_AND_END_OPERATION(CONTAINER) \
    if ((CONTAINER).image_start(Y_Axis()) + ibs > ycs[tmpi]){ ycs[tmpi] = (CONTAINER).image_start(Y_Axis()) +ibs; }\
    if ((CONTAINER).image_start(X_Axis()) +ibs> xcs[tmpi]){ xcs[tmpi] = (CONTAINER).image_start(X_Axis())+ibs; }\
    if ((CONTAINER).image_start(Y_Axis()) +ibs > yce[tmpi]){ yce[tmpi] = (CONTAINER).image_start(Y_Axis()) +ibs; }\
    if ((CONTAINER).image_start(X_Axis()) +ibs > xce[tmpi]){ xce[tmpi] = (CONTAINER).image_start(X_Axis()) +ibs; }\
    if ((CONTAINER).image_end(Y_Axis()) -ibs < ycs[tmpi]){ ycs[tmpi] = (CONTAINER).image_end(Y_Axis()) -ibs; }\
    if ((CONTAINER).image_end(X_Axis())-ibs < xcs[tmpi]){ xcs[tmpi] = (CONTAINER).image_end(X_Axis()) -ibs; }\
    if ((CONTAINER).image_end(Y_Axis())-ibs < yce[tmpi]){ yce[tmpi] = (CONTAINER).image_end(Y_Axis()) -ibs; }\
    if ((CONTAINER).image_end(X_Axis())-ibs < xce[tmpi]){ xce[tmpi] = (CONTAINER).image_end(X_Axis()) -ibs; }

// there is a striking similarity between the macros DO_PREOP and
// DO_POSTOP. However, in order to reduce it to one macro, it will need to
// take a LOT of arguments, and even more confusing to the eye. therefore I
// will keep the two macros separate

#define DO_PREOP(OPERATION, DESCRIPTOR, CONTAINER, SRC, DST, KERN) do {\
  CONVOL_PREAMBLE(CONTAINER, DESCRIPTOR);\
  CALC_BORDER(border, DESCRIPTOR, CONTAINER);\
  if (border & (IMB_L|IMB_B)) {\
    do_fill[0] = true;\
    xcs[0] = XS; ycs[0] = YS;\
    xce[0] = XS + ibs; yce[0] = YS + ibs;\
  } else {\
    xcs[0] = XS; ycs[0] = YS;\
    xce[0] = XS + bdr; yce[0] = YS + bdr;\
  }\
  if (border & (IMB_B)) {\
    do_fill[1] = true; \
    xcs[1] = XS + ibs; ycs[1] = YS;\
    xce[1] = XE - ibs; yce[1] = YS + ibs;\
  } else {\
    xcs[1] = XS + bdr; ycs[1] = YS;\
    xce[1] = XE - bdr; yce[1] = YS + bdr;\
  } \
  if (border & (IMB_B|IMB_R)) {\
    do_fill[2] = true;\
    xcs[2] = XE - ibs; ycs[2] = YS;\
    xce[2] = XE; yce[2] = YS + ibs;\
  } else {\
    xcs[2] = XE - bdr; ycs[2] = YS;\
    xce[2] = XE; yce[2] = YS + bdr;\
  }\
  if (border & IMB_L) {\
    do_fill[3] = true;\
    xcs[3] = XS; ycs[3] = YS + ibs;\
    xce[3] = XS + ibs; yce[3] = YE - ibs;\
  } else {\
    xcs[3] = XS; ycs[3] = YS + bdr;\
    xce[3] = XS + bdr; yce[3] = YE - bdr;\
  }\
  for (int tmpi = 0; tmpi < 4; ++tmpi) {\
    if (do_fill[tmpi]) {\
      CHECK_START_AND_END_FILL(CONTAINER) \
      handle_image_border(tmpi+1, xcs[tmpi], ycs[tmpi], xce[tmpi], yce[tmpi]);}\
    else {\
      CHECK_START_AND_END_OPERATION(CONTAINER) \
      OPERATION(xcs[tmpi], ycs[tmpi], xce[tmpi], yce[tmpi], 0, 0, SRC, DST, KERN);}\
  }\
} while (false)

// the macro DO_POSTOP shares many structural similarities with DO_PREOP, but
// to avoid 3 dozen macro arguments as well as for the sake of a slightly more
// readable macro form, I decide to keep them separate

#define DO_POSTOP(OPERATION, DESCRIPTOR, CONTAINER, SRC, DST, KERN) do {\
  CONVOL_PREAMBLE(CONTAINER, DESCRIPTOR);\
  CALC_BORDER(border, DESCRIPTOR, CONTAINER);\
  if (border & IMB_R) {\
    do_fill[0] = true;\
    xcs[0] = XE - ibs; ycs[0] = YS + ibs;\
    xce[0] = XE; yce[0] = YE - ibs;\
  } else {\
    xcs[0] = XE - bdr; ycs[0] = YS + bdr;\
    xce[0] = XE; yce[0] = YE - bdr;\
  }\
  if (border & (IMB_L|IMB_T)) {\
    do_fill[1] = true; \
    xcs[1] = XS; ycs[1] = YE - ibs;\
    xce[1] = XS + ibs; yce[1] = YE;\
  } else {\
    xcs[1] = XS; ycs[1] = YE - bdr;\
    xce[1] = XS + bdr; yce[1] = YE;\
  } \
  if (border & IMB_T) {\
    do_fill[2] = true;\
    xcs[2] = XS + ibs; ycs[2] = YE - ibs;\
    xce[2] = XE - ibs; yce[2] = YE;\
  } else {\
    xcs[2] = XS + bdr; ycs[2] = YE - bdr;\
    xce[2] = XE - bdr; yce[2] = YE;\
  }\
  if (border & (IMB_T|IMB_R)) {\
    do_fill[3] = true;\
    xcs[3] = XE - ibs; ycs[3] = YE - ibs;\
    xce[3] = XE; yce[3] = YE;\
  } else {\
    xcs[3] = XE - bdr; ycs[3] = YE - bdr;\
    xce[3] = XE; yce[3] = YE;\
  }\
  for (int tmpi = 0; tmpi < 4; ++tmpi) {\
    if (do_fill[tmpi]) {\
      CHECK_START_AND_END_FILL(CONTAINER) \
      handle_image_border(tmpi+6, xcs[tmpi], ycs[tmpi], xce[tmpi], yce[tmpi]);}\
    else {\
      CHECK_START_AND_END_OPERATION(CONTAINER) \
      OPERATION(xcs[tmpi], ycs[tmpi], xce[tmpi], yce[tmpi], 0, 0, SRC, DST, KERN);}\
  }\
} while (false)

#endif
