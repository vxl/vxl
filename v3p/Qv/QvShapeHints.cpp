#include <QvShapeHints.h>

QV_NODE_SOURCE(QvShapeHints);

QvShapeHints::QvShapeHints()
{
    QV_NODE_CONSTRUCTOR(QvShapeHints);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(vertexOrdering);
    QV_NODE_ADD_FIELD(shapeType);
    QV_NODE_ADD_FIELD(faceType);
    QV_NODE_ADD_FIELD(creaseAngle);
    
    vertexOrdering.value = UNKNOWN_ORDERING;
    shapeType.value = UNKNOWN_SHAPE_TYPE;
    faceType.value = CONVEX;
    creaseAngle.value = 0.5;
    
    QV_NODE_DEFINE_ENUM_VALUE(VertexOrdering,	UNKNOWN_ORDERING);
    QV_NODE_DEFINE_ENUM_VALUE(VertexOrdering,	CLOCKWISE);
    QV_NODE_DEFINE_ENUM_VALUE(VertexOrdering,	COUNTERCLOCKWISE);

    QV_NODE_DEFINE_ENUM_VALUE(ShapeType,	UNKNOWN_SHAPE_TYPE);
    QV_NODE_DEFINE_ENUM_VALUE(ShapeType,	SOLID);

    QV_NODE_DEFINE_ENUM_VALUE(FaceType,		UNKNOWN_FACE_TYPE);
    QV_NODE_DEFINE_ENUM_VALUE(FaceType,		CONVEX);

    QV_NODE_SET_SF_ENUM_TYPE(vertexOrdering,	VertexOrdering);
    QV_NODE_SET_SF_ENUM_TYPE(shapeType,		ShapeType);
    QV_NODE_SET_SF_ENUM_TYPE(faceType,		FaceType);
}

QvShapeHints::~QvShapeHints()
{
}
