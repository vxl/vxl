# ./v3p/Qv/CMakeLists.txt

SET( Qv_sources
  vecutil.cxx vecutil.h
  QvLib.cxx
  wrltraverse.cxx
  wrlbuild.cxx
  QvVisitor.cxx QvVisitor.h
  QvVrmlFile.cxx QvVrmlFile.h

  QvBasic.h
  QvFields.h
  QvMFFace.h
  QvNodes.h
  QvSubField.h
  QvSubNode.h
  Qv_pi.h
  color.h
  mtl.h
  vectors.h

  QvAsciiText.h
# QvAsciiText.cxx
  QvChildList.h
# QvChildList.cxx
  QvCone.h
# QvCone.cxx
  QvCoordinate3.h
# QvCoordinate3.cxx
  QvCube.h
# QvCube.cxx
  QvCylinder.h
# QvCylinder.cxx
  QvDB.h
# QvDB.cxx
  QvDebugError.h
# QvDebugError.cxx
  QvDict.h
# QvDict.cxx
  QvDirectionalLight.h
# QvDirectionalLight.cxx
  QvElement.h
# QvElement.cxx
  QvExtensions.h
# QvExtensions.cxx
  QvField.h
# QvField.cxx
  QvFieldData.h
# QvFieldData.cxx
  QvFontStyle.h
# QvFontStyle.cxx
  QvGroup.h
# QvGroup.cxx
  QvIndexedFaceSet.h
# QvIndexedFaceSet.cxx
  QvIndexedLineSet.h
# QvIndexedLineSet.cxx
  QvInfo.h
# QvInfo.cxx
  QvInput.h
# QvInput.cxx
  QvLOD.h
# QvLOD.cxx
  QvLists.h
# QvLists.cxx
  QvMFColor.h
# QvMFColor.cxx
  QvMFFloat.h
# QvMFFloat.cxx
  QvMFLong.h
# QvMFLong.cxx
  QvMFString.h
# QvMFString.cxx
  QvMFVec2f.h
# QvMFVec2f.cxx
  QvMFVec3f.h
# QvMFVec3f.cxx
  QvMaterial.h
# QvMaterial.cxx
  QvMaterialBinding.h
# QvMaterialBinding.cxx
  QvMatrixTransform.h
# QvMatrixTransform.cxx
# QvName.cxx
  QvNode.h
# QvNode.cxx
  QvNormal.h
# QvNormal.cxx
  QvNormalBinding.h
# QvNormalBinding.cxx
  QvOrthographicCamera.h
# QvOrthographicCamera.cxx
  QvPList.h
# QvPList.cxx
  QvPerspectiveCamera.h
# QvPerspectiveCamera.cxx
  QvPointLight.h
# QvPointLight.cxx
  QvPointSet.h
# QvPointSet.cxx
  QvReadError.h
# QvReadError.cxx
  QvRotation.h
# QvRotation.cxx
  QvSFBitMask.h
# QvSFBitMask.cxx
  QvSFBool.h
# QvSFBool.cxx
  QvSFColor.h
# QvSFColor.cxx
  QvSFEnum.h
# QvSFEnum.cxx
  QvSFFloat.h
# QvSFFloat.cxx
  QvSFImage.h
# QvSFImage.cxx
  QvSFLong.h
# QvSFLong.cxx
  QvSFMatrix.h
# QvSFMatrix.cxx
  QvSFRotation.h
# QvSFRotation.cxx
  QvSFString.h
# QvSFString.cxx
  QvSFVec2f.h
# QvSFVec2f.cxx
  QvSFVec3f.h
# QvSFVec3f.cxx
  QvScale.h
# QvScale.cxx
  QvSeparator.h
# QvSeparator.cxx
  QvShapeHints.h
# QvShapeHints.cxx
  QvSphere.h
# QvSphere.cxx
  QvSpotLight.h
# QvSpotLight.cxx
  QvState.h
# QvState.cxx
  QvString.h
# QvString.cxx
  QvSwitch.h
# QvSwitch.cxx
  QvTexture2.h
# QvTexture2.cxx
  QvTexture2Transform.h
# QvTexture2Transform.cxx
  QvTextureCoordinate2.h
# QvTextureCoordinate2.cxx
  QvTransform.h
# QvTransform.cxx
  QvTransformSeparator.h
# QvTransformSeparator.cxx
  QvTranslation.h
# QvTranslation.cxx
  QvUnknownNode.h
# QvUnknownNode.cxx
  QvWWWAnchor.h
# QvWWWAnchor.cxx
  QvWWWInline.h
# QvWWWInline.cxx
)

AUX_SOURCE_DIRECTORY( Templates Qv_sources )

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(QvLib.cxx PROPERTIES COMPILE_FLAGS -O1)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_LIBRARY( Qv ${Qv_sources} )

TARGET_LINK_LIBRARIES( Qv vcl )

IF(BUILD_TESTING)
  ADD_EXECUTABLE(test_compute_vrml_centroid test-compute-vrml-centroid.cxx)
  TARGET_LINK_LIBRARIES( test_compute_vrml_centroid Qv )
ENDIF(BUILD_TESTING)
