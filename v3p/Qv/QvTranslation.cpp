#include <QvTranslation.h>

QV_NODE_SOURCE(QvTranslation);

QvTranslation::QvTranslation()
{
    QV_NODE_CONSTRUCTOR(QvTranslation);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(translation);

    translation.value[0] = translation.value[1] = translation.value[2] = 0.0;
}

QvTranslation::~QvTranslation()
{
}
