#include <QvReadError.h>
#include <QvSFBool.h>

QV_SFIELD_SOURCE(QvSFBool);

QvBool
QvSFBool::readValue(QvInput *in)
{
    // accept 0 or 1
    if (in->read(value)) {
	if (value != 0 && value != 1) {
	    QvReadError::post(in, "Illegal value for QvSFBool: %d "
			      "(must be 0 or 1)", value);
	    return FALSE;
	}
	return TRUE;
    }

    // read TRUE/FALSE keyword
    QvName n;
    if (! in->read(n, TRUE))
	return FALSE;
    
    if (n == "TRUE") {
	value = TRUE;
	return TRUE;
    }

    if (n == "FALSE") {
	value = FALSE;
	return TRUE;
    }

    QvReadError::post(in, "Unknown value (\"%s\") for QvSFBool ",
		      "(must be TRUE or FALSE)", n.getString());
    return FALSE;
}
