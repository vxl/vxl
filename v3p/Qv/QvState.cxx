#include "QvState.h"
#include <vcl_iostream.h>

const char *QvState::stackNames[QvState::NumStacks] = {
    "Camera",
    "Coordinate3",
    "FontStyle",
    "Light",
    "MaterialBinding",
    "Material",
    "NormalBinding",
    "Normal",
    "ShapeHints",
    "Texture2",
    "Texture2Transformation",
    "TextureCoordinate2",
    "Transformation",
};

QvState::QvState()
{
    stacks = new QvElement * [NumStacks];

    for (int i = 0; i < NumStacks; i++)
        stacks[i] = NULL;

    depth = 0;
}

QvState::~QvState()
{
    while (depth > 0)
        pop();

    delete [] stacks;
}

void
QvState::addElement(StackIndex stackIndex, QvElement *elt)
{
    elt->depth = depth;
    elt->next = stacks[stackIndex];
    stacks[stackIndex] = elt;
}

void
QvState::push()
{
    depth++;
}

void
QvState::pop()
{
    depth--;

    for (int i = 0; i < NumStacks; i++)
        while (stacks[i] != NULL && stacks[i]->depth > depth)
#ifdef PMAX
            popElement((enum StackIndex) i);  // difficulties with cast, mpichler, 19950802
#else
            popElement((StackIndex) i);
#endif
}

void
QvState::popElement(StackIndex stackIndex)
{
    QvElement *elt = stacks[stackIndex];
    stacks[stackIndex] = elt->next;
    delete elt;
}

void
QvState::print()
{
    vcl_cout << "Traversal state:\n";

    for (int i = 0; i < NumStacks; i++)
    {
        vcl_cout << "\tStack [" << i << "] (" << stackNames[i] << "):\n";

        if (stacks[i] == NULL)
            vcl_cout << "\t\tNULL\n";
        else
            for (QvElement *elt = stacks[i]; elt != NULL; elt = elt->next)
                elt->print();
    }
}
