#include <QvString.h>
#include <ctype.h>

#define CHUNK_SIZE	4000

struct QvNameChunk {
    char		mem[CHUNK_SIZE];
    char		*curByte;
    int			bytesLeft;
    struct QvNameChunk	*next;
};

int		QvNameEntry::nameTableSize;
QvNameEntry **	QvNameEntry::nameTable;
struct QvNameChunk *QvNameEntry::chunk;	    	

void
QvNameEntry::initClass()
{
    int i;

    
    nameTableSize = 1999;
    nameTable	  = new QvNameEntry *[nameTableSize];

    for (i = 0; i < nameTableSize; i++)
	nameTable[i] = NULL;

    chunk = NULL;
}

const QvNameEntry *
QvNameEntry::insert(const char *s)
{
    u_long		h = QvString::hash(s);
    u_long		i;
    QvNameEntry		*entry;
    QvNameEntry		*head;

    if (nameTableSize == 0)
	initClass();

    i = h % nameTableSize;
    entry = head = nameTable[i];

    while (entry != NULL) {
	if (entry->hashValue == h && entry->isEqual(s))
	    break;
	entry = entry->next;
    }

    if (entry == NULL) {

	int len = strlen(s) + 1;

	if (len >= CHUNK_SIZE)
	    s = strdup(s);

	else {

	    if (chunk == NULL || chunk->bytesLeft < len) {
		struct QvNameChunk	*newChunk = new QvNameChunk;

		newChunk->curByte   = newChunk->mem;
		newChunk->bytesLeft = CHUNK_SIZE;
		newChunk->next      = chunk;

		chunk = newChunk;
	    }

	    strcpy(chunk->curByte, s);
	    s = chunk->curByte;

	    chunk->curByte   += len;
	    chunk->bytesLeft -= len;
	}

	entry = new QvNameEntry(s, h, head);
	nameTable[i] = entry;
    }

    return entry;
}

QvName::QvName()
{
    entry = QvNameEntry::insert("");
}

QvBool
QvName::isIdentStartChar(char c)
{
    if (isdigit(c)) return FALSE;

    return isIdentChar(c);
}

QvBool
QvName::isIdentChar(char c)
{
    if (isalnum(c) || c == '_') return TRUE;

    return FALSE;
}

QvBool
QvName::isNodeNameStartChar(char c)
{
    if (isdigit(c)) return FALSE;

    return isIdentChar(c);
}

static const char
badCharacters[] = "+\'\"\\{}";

QvBool
QvName::isNodeNameChar(char c)
{
    if (isalnum(c)) return TRUE;

    if ((strchr(badCharacters, c) != NULL) ||
	isspace(c) || iscntrl(c)) return FALSE;

    return TRUE;
}
