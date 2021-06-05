#ifndef HEAP_LIST_H_
#define HEAP_LIST_H_

#include <cstdlib>
#include "def.h"

template<typename T>
struct BTNode
{
    struct BTNode<T>* parent;
    T value;
    struct BTNode<T> *right_child, 
                      *left_child;
};

template<typename T, typename Comp>
class HeapList
{
  private:
    Int num_;
    struct BTNode<T>* root_;

    void heapify();
    void sift_down(BTNode<T>*);

  public:
    HeapList() : num_(0), root_(nullptr) {};
    ~HeapList();

    Int size() const { return num_; }
    
    bool is_leaf(const BTNode<T>*) const;
    bool is_empty() const;
    BTNode<T>* left_child(const BTNode<T>*) const;
    BTNode<T>* right_child(const BTNode<T>*) const;
    BTNode<T>* parent(const BTNode<T>*) const;
    
    BTNode<T>* tail();
    BTNode<T>* last();
    BTNode<T>* get_node(const Int&);
    BTNode<T>* get_root() { return root_; };
    BTNode<T>* pop();
    T top();
    void remove_top();

    void swap(const Int&, const Int&);
    void swap(BTNode<T>*, BTNode<T>*);
    void push_back(BTNode<T>*);
    void push_back(const T&);
    BTNode<T>* create_node(const T&);
    void delete_node(BTNode<T>* node);
    T get_node_value(BTNode<T>* node);   
    void merge(HeapList<T,Comp>&); 

    void check_heap();
};

#endif
