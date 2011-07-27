//:
// \file
// \brief Local memory implementation of an array list (NEED TO DEFINE LIST TYPE)
//
// \todo
// -make list wrap around when full
// -make list fail elegantly when overflowing

//List defaults to unsigned char
#ifndef LIST_TYPE
 #define LIST_TYPE char
#endif

//struct that maintains the start and total length of the array buffer,
// as well as pointers to the start and end of the list (and its size)
typedef struct
{
    //pointer to head of list
    __local LIST_TYPE* mem_start;

    //buffer length
    unsigned max_length;

    //start pointer
    unsigned start;
    unsigned end;

    //current list length
    int length;

} linked_list;

// "Constructor for new linked list"
linked_list new_linked_list( __local LIST_TYPE* mem_start, unsigned max_length )
{
  linked_list newList;
  newList.mem_start = mem_start;
  newList.max_length = max_length;
  newList.start = 0;
  newList.end = 0;
  newList.length = 0;
  return newList;
}

//: returns true if list is empty
bool empty( linked_list* list ) { return list->length == 0; }

//: adds an item to the end of the linked list
void push_back( linked_list* list, LIST_TYPE a )
{
  list->mem_start[ list->end ] = a;
  list->end++;
  list->length++;
}

//: returns the front item, but it remains on the list
LIST_TYPE front( linked_list* list )
{
  return list->mem_start[ list->start ];
}

//: removes an item from the front of the linked list
LIST_TYPE pop_front( linked_list* list )
{
  LIST_TYPE toReturn = list->mem_start[ list->start ];
  list->start++;
  list->length--;
  return toReturn;
}
