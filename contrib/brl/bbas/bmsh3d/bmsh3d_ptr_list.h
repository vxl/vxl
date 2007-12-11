//:
// \file
// \brief 070510  A simple implementation of link list of NULL pointers.
// \author Ming-Ching Chang

#ifndef bmsh3d_ptr_list_h_
#define bmsh3d_ptr_list_h_

#include <vcl_set.h>
#include <vcl_cassert.h>

class bmsh3d_ptr_node
{
 protected:
  void* ptr_;
  bmsh3d_ptr_node* next_;

 public:
  //====== Constructor/Destructor ======
  bmsh3d_ptr_node () {
    ptr_ = NULL;
    next_ = NULL;
  }
  bmsh3d_ptr_node (const void* ptr) {
    ptr_ = (void*) ptr;
    next_ = NULL;
  }

  //====== Data access functions ======
  void* ptr() {
    return ptr_;
  }
  const void* ptr() const {
    return ptr_;
  }
  void set_ptr (const void* ptr) {
    ptr_ = (void*) ptr;
  }

  bmsh3d_ptr_node* next() {
    return next_;
  }
  const bmsh3d_ptr_node* next() const {
    return next_;
  }
  void set_next (bmsh3d_ptr_node* next) {
    next_ = next;
  }
};

//: return size of the list.
inline unsigned int get_all_ptrs (const bmsh3d_ptr_node* head, vcl_set<void*>& ptrs)
{
  unsigned int count = 0;
  bmsh3d_ptr_node* cur = (bmsh3d_ptr_node*) head;
  for (; cur != NULL; cur = cur->next()) {
    ptrs.insert (cur->ptr());
    count++;
  }
  ///assert (count == ptrs.size());
  return count;
}

inline unsigned int count_all_ptrs (const bmsh3d_ptr_node* head)
{
  unsigned int count = 0;
  bmsh3d_ptr_node* cur = (bmsh3d_ptr_node*) head;
  for (; cur != NULL; cur = cur->next())
    count++;
  return count;
}

inline unsigned int clear_ptr_list (bmsh3d_ptr_node*& head)
{
  unsigned int count = 0;
  bmsh3d_ptr_node* cur = head;
  while (cur != NULL) {
    bmsh3d_ptr_node* tmp = cur;
    cur = cur->next();
    delete tmp;
    count++;
  }
  head = NULL;
  return count;
}

inline bool is_in_ptr_list (const bmsh3d_ptr_node* head, const void* input)
{
  bmsh3d_ptr_node* cur = (bmsh3d_ptr_node*) head;
  for (; cur != NULL; cur = cur->next()) {
    if (cur->ptr() == input)
      return true;
  }
  return false;
}

inline void _add_to_ptr_list_head (bmsh3d_ptr_node*& head, bmsh3d_ptr_node* cur)
{
  assert (cur->next() == NULL);
  cur->set_next (head);
  head = cur;
}

inline void add_ptr_to_list (bmsh3d_ptr_node*& head, const void* input)
{
  assert (is_in_ptr_list (head, input) == false);
  assert (input != NULL);
  bmsh3d_ptr_node* cur = new bmsh3d_ptr_node (input);
  _add_to_ptr_list_head (head, cur);
}

inline void add_ptr_check (bmsh3d_ptr_node*& head, const void* input)
{
  assert (is_in_ptr_list (head, input) == false);
  add_ptr_to_list (head, input);
}

inline bool check_add_ptr (bmsh3d_ptr_node*& head, const void* input)
{
  if (is_in_ptr_list (head, input))
    return false;
  add_ptr_to_list (head, input);
  return true;
}

inline bool del_ptr (bmsh3d_ptr_node*& head, const void* input)
{
  if (head == NULL)
    return false;
  if (head->ptr() == input) { //the head contains the input ptr.
    bmsh3d_ptr_node* tmp = head;
    head = head->next();
    delete tmp;
    return true;
  }

  bmsh3d_ptr_node* prev = head;
  bmsh3d_ptr_node* cur = prev->next();
  while (cur != NULL) {
    if (cur->ptr() == input) { //found, delete cur
      prev->set_next (cur->next());
      delete cur;
      return true;
    }
    prev = cur;
    cur = cur->next();
  }
  return false;
}

#endif

