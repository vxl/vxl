#include <QvDebugError.h>
#include <QvReadError.h>
#include <QvSFEnum.h>

// Can't use macro, since we define a constructor.

QvSFEnum::QvSFEnum()
{
    enumValues = NULL;
    enumNames  = NULL;
}

QvSFEnum::~QvSFEnum()
{
}

QvBool
QvSFEnum::findEnumValue(const QvName &name, int &val) const
{
    int	i;

    // Look through names table for one that matches
    for (i = 0; i < numEnums; i++) {
	if (name == enumNames[i]) {
	    val = enumValues[i];
	    return TRUE;
	}
    }

    return FALSE;
}

QvBool
QvSFEnum::readValue(QvInput *in)
{
    QvName n;

#ifdef DEBUG
    if (enumValues == NULL) {
	QvDebugError::post("QvSFEnum::readValue",
			   "Enum values were never initialized");
	QvReadError::post(in, "Couldn't read QvSFEnum value");
	return FALSE;
    }
#endif /* DEBUG */

    // Read mnemonic value as a character string identifier
    if (! in->read(n, TRUE))
	return FALSE;

    if (findEnumValue(n, value))
	return TRUE;

    // Not found? Too bad
    QvReadError::post(in, "Unknown QvSFEnum enumeration value \"%s\"",
		      n.getString());
    return FALSE;
}
