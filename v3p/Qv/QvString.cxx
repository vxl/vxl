#include "QvString.h"

QvString::~QvString()
{
    if (string != staticStorage)
        delete [] string;
}

void
QvString::expand(int bySize)
{
    int newSize = strlen(string) + bySize + 1;

    if (newSize >= QV_STRING_STATIC_STORAGE_SIZE &&
        (string == staticStorage || newSize > storageSize)) {

        char *newString = new char[newSize];

        strcpy(newString, string);

        if (string != staticStorage)
            delete [] string;

        string      = newString;
        storageSize = newSize;
    }
}

u_long
QvString::hash(const char *s)
{
    u_long total, shift;

    total = shift = 0;
    while (*s) {
        total = total ^ ((*s) << shift);
        shift+=5;
        if (shift>24) shift -= 24;
        s++;
    }

    return( total );
}

void
QvString::makeEmpty(QvBool freeOld)
{
    if (string != staticStorage) {
        if (freeOld)
            delete [] string;
        string = staticStorage;
    }
    string[0] = '\0';
}

QvString &
QvString::operator =(const char *str)
{
    int size = strlen(str) + 1;

    if (str >= string &&
        str < string + (string != staticStorage ? storageSize :
                        QV_STRING_STATIC_STORAGE_SIZE)) {

        QvString tmp = str;
        *this = tmp;
        return *this;
    }

    if (size < QV_STRING_STATIC_STORAGE_SIZE) {
        if (string != staticStorage)
            makeEmpty();
    }

    else if (string == staticStorage)
        string = new char[size];

    else if (size > storageSize) {
        delete [] string;
        string = new char[size];
    }

    strcpy(string, str);
    storageSize = size;
    return *this;
}

QvString &
QvString::operator +=(const char *str)
{
    expand(strlen(str));
    strcat(string, str);
    return *this;
}

bool
operator ==(const QvString &str, const char *s)
{
    return (str.string[0] == s[0] && ! strcmp(str.string, s));
}

bool
operator !=(const QvString &str, const char *s)
{
    return (str.string[0] != s[0] || strcmp(str.string, s));
}
