// mpichler, 19950801
#ifdef PMAX
enum Part { goofyPart };        // confused about QvCone::Part and QvCylinder::Part
enum Binding { goofyBinding };  // confused about QvMaterialBinding/QvNormalBinding::Binding
#endif

#if defined(__KCC)
# define u_long go_away_bloody_u_long
# include <sys/types.h>
# undef  u_long
#endif

#if defined(__SUNPRO_CC)
# include <sys/types.h>
#endif

// RWMC: Had to define _strdup for linux since __PC__ set.  Hope this
// doesn't break other systems.
#include "QvBasic.h"

// strdup() is not standard
// // mpichler, 19950802
// #ifndef WIN32
// #if defined(PMAX) || defined(PMAX_GNU) || defined(__PC__)
// #  include <vcl_cstring.h>
// #  include <malloc.h>
// char* strdup (const char* s)  // strdup not contained in system libraries
// {
//   char* p = (char*) malloc (vcl_strlen (s) + 1);
//   if (p)
//     vcl_strcpy (p, s);
//   return p;
// }
// #endif
// #endif
#include <vcl_cstring.h> // strcpy()
#include <vcl_cstdlib.h> // malloc()
char* Qv_strdup (const char* s)
{
  char* p = (char*) vcl_malloc (vcl_strlen (s) + 1);
  if (p)
    vcl_strcpy (p, s);
  return p;
}

// changed by Gerbert Orasche 260695
#ifdef __PC__
#pragma warning(disable:4244)
#endif

// mpichler, 19951109
// definition of a suffix would be sufficient for compilers,
// but UNIX' makedepend does not check this
// e.g. #define CPPSUFFIX ".cxx"
// #include "QvXXX" CPPSUFFIX
// gorasche, 19960229
// VC++ now rejects them too, so we're back to the old, large ifdef

#define VRMLEXTENSIONS

# include "QvAsciiText.cxx"
# include "QvChildList.cxx"
# include "QvCone.cxx"
# include "QvCoordinate3.cxx"
# include "QvCube.cxx"
# include "QvCylinder.cxx"
# include "QvDB.cxx"
# include "QvDebugError.cxx"
# include "QvDict.cxx"
# include "QvDirectionalLight.cxx"
# include "QvElement.cxx"
# include "QvField.cxx"
# include "QvFieldData.cxx"
# include "QvFontStyle.cxx"
# include "QvGroup.cxx"
# include "QvIndexedFaceSet.cxx"
# include "QvIndexedLineSet.cxx"
# include "QvInfo.cxx"
# include "QvInput.cxx"
# include "QvLists.cxx"
# include "QvLOD.cxx"
# include "QvMFColor.cxx"
# include "QvMFFloat.cxx"
# include "QvMFLong.cxx"
# include "QvMFString.cxx"
# include "QvMFVec2f.cxx"
# include "QvMFVec3f.cxx"
# include "QvMaterial.cxx"
# include "QvMaterialBinding.cxx"
# include "QvMatrixTransform.cxx"
# include "QvName.cxx"
# include "QvNode.cxx"
# include "QvNormal.cxx"
# include "QvNormalBinding.cxx"
# include "QvOrthographicCamera.cxx"
# include "QvPList.cxx"
# include "QvPerspectiveCamera.cxx"
# include "QvPointLight.cxx"
# include "QvPointSet.cxx"
# include "QvReadError.cxx"
# include "QvRotation.cxx"
# include "QvSFBitMask.cxx"
# include "QvSFBool.cxx"
# include "QvSFColor.cxx"
# include "QvSFEnum.cxx"
# include "QvSFFloat.cxx"
# include "QvSFImage.cxx"
# include "QvSFLong.cxx"
# include "QvSFMatrix.cxx"
# include "QvSFRotation.cxx"
# include "QvSFString.cxx"
# include "QvSFVec2f.cxx"
# include "QvSFVec3f.cxx"
# include "QvScale.cxx"
# include "QvSeparator.cxx"
# include "QvShapeHints.cxx"
# include "QvSphere.cxx"
# include "QvSpotLight.cxx"
# include "QvState.cxx"
# include "QvString.cxx"
# include "QvSwitch.cxx"
# include "QvTexture2.cxx"
# include "QvTexture2Transform.cxx"
# include "QvTextureCoordinate2.cxx"
# include "QvTransform.cxx"
# include "QvTransformSeparator.cxx"
# include "QvTranslation.cxx"
# include "QvUnknownNode.cxx"
# include "QvWWWAnchor.cxx"
# include "QvWWWInline.cxx"

# ifdef VRMLEXTENSIONS
#  include "QvExtensions.cxx"
# endif
