#include <QvIndexedLineSet.h>

QV_NODE_SOURCE(QvIndexedLineSet);

QvIndexedLineSet::QvIndexedLineSet()
{
    QV_NODE_CONSTRUCTOR(QvIndexedLineSet);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(coordIndex);
    QV_NODE_ADD_FIELD(materialIndex);
    QV_NODE_ADD_FIELD(normalIndex);
    QV_NODE_ADD_FIELD(textureCoordIndex);

    coordIndex.values[0] = 0;
    materialIndex.values[0] = QV_END_LINE_INDEX;
    normalIndex.values[0] = QV_END_LINE_INDEX;
    textureCoordIndex.values[0] = QV_END_LINE_INDEX;
}

QvIndexedLineSet::~QvIndexedLineSet()
{
}
