// mpichler, 19950801
#ifdef PMAX
enum Part { goofyPart };        // confused about QvCone::Part and QvCylinder::Part
enum Binding { goofyBinding };  // confused about QvMaterialBinding/QvNormalBinding::Binding
#endif

// RWMC: Had to define _strdup for linux since __PC__ set.  Hope this
// doesn't break other systems.
#include <vcl/vcl_cmath.h>
#include "QvBasic.h"

// mpichler, 19950802
#ifndef WIN32
#if defined(PMAX) || defined(PMAX_GNU) || defined(__PC__)
#  include <vcl/vcl_cstring.h>
#  include <malloc.h>
char* strdup (const char* s)  // strdup not contained in system libraries
{
  char* p = (char*) malloc (strlen (s) + 1);
  if (p)
    strcpy (p, s);
  return p;
}
#endif
#endif

// changed by Gerbert Orasche 260695
#ifdef __PC__
#pragma warning(disable:4244)
#endif

// mpichler, 19951109
// definition of a suffix would be sufficient for compilers,
// but UNIX' makedepend does not check this
// e.g. #define CPPSUFFIX ".C"
// #include "QvXXX" CPPSUFFIX
// gorasche, 19960229
// VC++ now rejects them too, so we're back to the old, large ifdef

#define VRMLEXTENSIONS

# include "QvAsciiText.cpp"
# include "QvChildList.cpp"
# include "QvCone.cpp"
# include "QvCoordinate3.cpp"
# include "QvCube.cpp"
# include "QvCylinder.cpp"
# include "QvDB.cpp"
# include "QvDebugError.cpp"
# include "QvDict.cpp"
# include "QvDirectionalLight.cpp"
# include "QvElement.cpp"
# include "QvField.cpp"
# include "QvFieldData.cpp"
# include "QvFontStyle.cpp"
# include "QvGroup.cpp"
# include "QvIndexedFaceSet.cpp"
# include "QvIndexedLineSet.cpp"
# include "QvInfo.cpp"
# include "QvInput.cpp"
# include "QvLists.cpp"
# include "QvLOD.cpp"
# include "QvMFColor.cpp"
# include "QvMFFloat.cpp"
# include "QvMFLong.cpp"
# include "QvMFString.cpp"
# include "QvMFVec2f.cpp"
# include "QvMFVec3f.cpp"
# include "QvMaterial.cpp"
# include "QvMaterialBinding.cpp"
# include "QvMatrixTransform.cpp"
# include "QvName.cpp"
# include "QvNode.cpp"
# include "QvNormal.cpp"
# include "QvNormalBinding.cpp"
# include "QvOrthographicCamera.cpp"
# include "QvPList.cpp"
# include "QvPerspectiveCamera.cpp"
# include "QvPointLight.cpp"
# include "QvPointSet.cpp"
# include "QvReadError.cpp"
# include "QvRotation.cpp"
# include "QvSFBitMask.cpp"
# include "QvSFBool.cpp"
# include "QvSFColor.cpp"
# include "QvSFEnum.cpp"
# include "QvSFFloat.cpp"
# include "QvSFImage.cpp"
# include "QvSFLong.cpp"
# include "QvSFMatrix.cpp"
# include "QvSFRotation.cpp"
# include "QvSFString.cpp"
# include "QvSFVec2f.cpp"
# include "QvSFVec3f.cpp"
# include "QvScale.cpp"
# include "QvSeparator.cpp"
# include "QvShapeHints.cpp"
# include "QvSphere.cpp"
# include "QvSpotLight.cpp"
# include "QvState.cpp"
# include "QvString.cpp"
# include "QvSwitch.cpp"
# include "QvTexture2.cpp"
# include "QvTexture2Transform.cpp"
# include "QvTextureCoordinate2.cpp"
# include "QvTransform.cpp"
# include "QvTransformSeparator.cpp"
# include "QvTranslation.cpp"
# include "QvUnknownNode.cpp"
# include "QvWWWAnchor.cpp"
# include "QvWWWInline.cpp"

# ifdef VRMLEXTENSIONS
#  include "QvExtensions.cpp"
# endif
