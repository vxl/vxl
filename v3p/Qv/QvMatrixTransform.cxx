#include "QvMatrixTransform.h"

QV_NODE_SOURCE(QvMatrixTransform);


QvMatrixTransform::QvMatrixTransform()
{
    QV_NODE_CONSTRUCTOR(QvMatrixTransform);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(matrix);

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            matrix.value[i][j] = (i == j ? 1.0f : 0.0f);
}

QvMatrixTransform::~QvMatrixTransform()
{
}
