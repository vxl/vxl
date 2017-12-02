/*
 *
 *  Copyright (C) 1997-2002, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  ofstd
 *
 *  Author:  Andreas Barth
 *
 *  Purpose:
 *      Defines a template stack class with interfaces similar to the C++ Standard
 *
 */


#ifndef OFSTACK_H
#define OFSTACK_H

#include <stack>
#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "oftypes.h"

#if defined(HAVE_STL) || defined(HAVE_STL_STACK)
// It is possible to use the standard template library list class since the
// interface is nearly identical.
// Important: If you want to use the standard template library, no variable
// in a namespace with using a list shall have the name stack
#define OFStack stack
#else

#define INCLUDE_CASSERT
#define INCLUDE_CSTDDEF
#include "ofstdinc.h"

#ifndef HAVE_CLASS_TEMPLATE
#error Your C++ compiler cannot handle class templates:
#endif


/**  non-template single linked list class, used to store elements of a stack.
 *  Implicitly used by OFStack, should not be called by users.
 */
struct OFStackLinkBase
{
    /// pointer to next entry in list
    OFStackLinkBase * next;

    /// default constructor
    OFStackLinkBase()
      : next(NULL)
    {
    }

    /// destructor
    virtual ~OFStackLinkBase()
    {
    }
  private:

    /// private undefined copy constructor
    OFStackLinkBase(const OFStackLinkBase &);

    /// private undefined copy assignment operator
    OFStackLinkBase &operator=(const OFStackLinkBase &);
};

/** non-template base class for OFStack.
 *  Implicitly used by OFStack, should not be called by users.
 */
class OFStackBase
{
  public:
    /// default constructor
    OFStackBase()
      : head(NULL),
        stackSize(0)
    {
    }

    /// destructor
    virtual ~OFStackBase()
    {
      while (!base_empty())
        base_pop();
    }

    /** checks if the stack is empty
     *  @return true if stack is emtpy, false otherwise
     */
    OFBool base_empty() const { return head == NULL; }

    /** returns size of stack
     *  @return size of stack
     */
    size_t base_size() const { return stackSize; }

    /** returns element on top of stack.
     *  precondition: stack is not empty
     */
    OFStackLinkBase * base_top()
    {
    assert(head!=NULL);
    return head;
    }

    /** pushes element onto stack.
     *  @param element pointer to element
     */
    void base_push(OFStackLinkBase * element)
    {
      element->next = head;
      head = element;
      stackSize++;
    }

    /** removes top element from stack.
     *  precondition: stack not empty.
     */
    void base_pop()
    {
      assert(head!=NULL);
      OFStackLinkBase * delObj = head;
      head = head->next;
      delete delObj;
      stackSize--;
    }

  protected:

    /// pointer to top element of stack
    OFStackLinkBase * head;

    /// size of stack
    size_t stackSize;

  private:

    /// private undefined copy constructor
    OFStackBase(const OFStackBase &);

    /// private undefined copy assignment operator
    OFStackBase &operator=(const OFStackBase &);
};

/** template class used to store stack entries.
 *  Implicitly used by OFStack, should not be called by users.
 */
template <class T>
struct OFStackLink : public OFStackLinkBase
{
    /// element on stack
    T info;

    /** constructor
     *  @param i element maintain by this object
     */
    OFStackLink(const T & i) : OFStackLinkBase(), info(i)  { }

    /// destructor
    virtual ~OFStackLink()
    {
    }

  private:

    /// private undefined copy constructor
    OFStackLink(const OFStackLink<T> &);

    /// private undefined copy assignment operator
    OFStackLink<T> &operator=(const OFStackLink<T> &);
};


/** template stack class. The interface is a subset of
 *  the STL stack class.
 */
template <class T>
class OFStack : private OFStackBase
{
  public:

    /// Default constructor
    OFStack() {};

    /// copy constructor
    OFStack(const OFStack<T> & x) : OFStackBase()
    {
        copy(x);
    }

    /// Assignment operator
    OFStack<T> &operator=(const OFStack<T> &x)
    {
        copy(x);
        return *this;
    }

    /** checks if the stack is empty.
     *  @return OFTrue if stack is empty, OFFalse otherwise.
     */
    OFBool empty() const { return OFStackBase::base_empty(); }

    /** returns the number of elements on the stack
     *  @return number of elements on stack
     */
    size_t size() const { return OFStackBase::base_size(); }

    /** returns a reference to the top element on the stack.
     *  This method may not be called if the stack is empty.
     *  @return reference to top element
     */
    T & top()
    {
        return ((OFStackLink<T>*)(OFStackBase::base_top()))->info;
    }

    /** inserts a new element on top of the stack. The value of
     *  the new element is copy constructed from the given argument.
     *  @param x value to be pushed (copied) onto the stack
     */
    void push(const T & x)
    {
        OFStackBase::base_push(new OFStackLink<T>(x));
    }

    /** removes the top element from the stack.
     *  This method may not be called if the stack is empty.
     */
    void pop() { OFStackBase::base_pop(); }

  private:

    /** copy assignment of a stack.
     *  @param x stack to be copied
     *  @return dummy value, required to keep Sun CC 2.0.1 happy
     */
    int copy(const OFStack<T> & x)
    {
        stackSize = x.size();
        if (stackSize)
        {
            head = new OFStackLink<T>(((OFStackLink<T>*)(x.head))->info);
            OFStackLinkBase * newPtr = head;
            OFStackLinkBase * oldPtr = x.head->next;
            while (oldPtr)
            {
            newPtr->next =
                new OFStackLink<T>(((OFStackLink<T>*)oldPtr)->info);
            oldPtr = oldPtr->next;
            newPtr = newPtr->next;
            }
        }
        return 0;
    }
};

#endif
#endif
