#include <cassert>
#include <iostream>
#include "heap_list.h"

template<typename T, typename Comp>
bool HeapList<T,Comp>::is_leaf(const BTNode<T>* ptr) const
{
    assert(ptr != nullptr);
    return (ptr->right_child == nullptr and
            ptr->left_child  == nullptr);
}

template<typename T, typename Comp>
BTNode<T>* HeapList<T,Comp>::left_child(const BTNode<T>* ptr) const
{
    return ptr->left_child;
}

template<typename T, typename Comp>
BTNode<T>* HeapList<T,Comp>::right_child(const BTNode<T>* ptr) const
{
    return ptr->right_child;
}

template<typename T, typename Comp>
BTNode<T>* HeapList<T,Comp>::parent(const BTNode<T>* ptr) const
{
    return ptr->parent;
}

template<typename T, typename Comp>
BTNode<T>* HeapList<T,Comp>::create_node(const T& val)
{
    BTNode<T>* ptr = new BTNode<T>;
    ptr->parent = nullptr;
    ptr->right_child = nullptr;
    ptr->left_child = nullptr;
    ptr->value = val;
    return ptr;
}

template<typename T, typename Comp>
HeapList<T,Comp>::~HeapList()
{
    BTNode<T>* v = root_;
    if(v != nullptr)
    {
        while(!is_leaf(v) or v->parent != nullptr)
        {
            if(!is_leaf(v))
            {
                BTNode<T>* rt = v->right_child, *lt = v->left_child;
                if(rt != nullptr)
                    v = rt;
                else if(lt != nullptr)
                    v = lt;
            }
            else if(v->parent != nullptr)
            {
                BTNode<T>* p = v->parent;
                if(p->right_child == v)
                    p->right_child = nullptr;
                else
                    p->left_child = nullptr;
                delete v;
                v = p;
            }
        }
        delete v;
    }
}

template<typename T, typename Comp>
void HeapList<T,Comp>::push_back(BTNode<T>* node)
{
    num_++;
    if(root_ == nullptr)
    {
        root_ = node;
        root_ ->parent = nullptr;
        root_->right_child = nullptr;
        root_->left_child = nullptr;
    }
    else
    {
        BTNode<T>* ptr = tail();
        if((num_ & 1))
            ptr->right_child = node;
        else
            ptr->left_child = node;
        node->parent = ptr;
        node->right_child = nullptr;
        node->left_child = nullptr;
        ptr = node;
        while((ptr->parent != nullptr) && (Comp::prior(ptr->value, ptr->parent->value)))
        {
            this->swap(ptr, ptr->parent);
            ptr = parent(ptr);
        }
    }
}

template<typename T, typename Comp>
void HeapList<T,Comp>::push_back(const T& val)
{
    num_++;
    if(root_ == nullptr)
    {
        root_ = create_node(val);
        root_ ->parent = nullptr;
        root_->right_child = nullptr;
        root_->left_child = nullptr;
    }
    else
    {
        BTNode<T>* ptr = tail();
        BTNode<T>* node = create_node(val);
        if((num_ & 1))
            ptr->right_child = node;
        else
            ptr->left_child = node;
        node->parent = ptr;
        ptr = node;
        while((ptr->parent != nullptr) && (Comp::prior(ptr->value, ptr->parent->value)))
        {
            this->swap(ptr, ptr->parent);
            ptr = parent(ptr);
        }
    }
}

template<typename T, typename Comp>
BTNode<T>* HeapList<T, Comp>::tail()
{
    if(root_==nullptr)
        return nullptr;

    Int num = num_>>1;
    return get_node(num);
}

template<typename T, typename Comp>
BTNode<T>* HeapList<T,Comp>::last()
{
    if(root_==nullptr)
        return nullptr;

    Int num = num_;
    return get_node(num);
}

template<typename T, typename Comp>
BTNode<T>* HeapList<T,Comp>::get_node(const Int& pos)
{
    assert(pos <= num_);
    if(root_ == nullptr) 
        return nullptr;

    int max_bit = MyMath::log2(pos);
    BTNode<T>* curr = root_;
    for(int i = max_bit-1; i >= 0; --i)
    {   
        Int is_set = pos & (1<<i);
        if(is_set) 
            curr = curr->right_child;
        else
            curr = curr->left_child;
    }
    return curr;
}

template<typename T, typename Comp>
void HeapList<T,Comp>::swap(const Int& i, const Int& j)
{
    BTNode<T>* ptr1 = get_node(i);
    BTNode<T>* ptr2 = get_node(j);
    T tmp = ptr1->value;
    ptr1->value = ptr2->value;
    ptr2->value = tmp;
}

template<typename T, typename Comp>
void HeapList<T,Comp>::swap(BTNode<T>* ptr1, BTNode<T>* ptr2)
{   
    T tmp = ptr1->value;
    ptr1->value = ptr2->value;
    ptr2->value = tmp;
}

template<typename T, typename Comp>
BTNode<T>* HeapList<T,Comp>::pop()
{
    if(is_empty())
        return nullptr;

    BTNode<T>* ptr = last();
    swap(root_, ptr);

    BTNode<T>* p = ptr->parent;
    if(p != nullptr)
    {
        if(p->right_child != nullptr)
            p->right_child = nullptr;
        else
            p->left_child = nullptr;
    }
    num_--;

    if(num_ != 0)
        sift_down(root_);
    else
        root_ = nullptr;
    return ptr;
}

//TODO: implement throwing exception not just exit
template<typename T, typename Comp>
T HeapList<T,Comp>::top()
{
    if(num_ == 0)
        exit(-1);
    return root_->value;
}

template<typename T, typename Comp>
void HeapList<T,Comp>::remove_top()
{
    if(!is_empty())
    {
        BTNode<T>* ptr = last();
        swap(root_, ptr);

        BTNode<T>* p = ptr->parent;
        if(p != nullptr)
        {
            if(p->right_child != nullptr)
                p->right_child = nullptr;
            else
                p->left_child = nullptr;
        }
        num_--;

        if(num_ != 0)
            sift_down(root_);
        else
            root_ = nullptr;
        delete ptr;
    }
}

template<typename T, typename Comp>
bool HeapList<T,Comp>::is_empty() const
{
    return num_ == 0;
}

//private function
template<typename T, typename Comp>
void HeapList<T,Comp>::sift_down(BTNode<T>* ptr)
{
    while(!is_leaf(ptr))
    {
        BTNode<T>* lc = left_child(ptr);
        BTNode<T>* rc = right_child(ptr);
        if((rc != nullptr) && Comp::prior(rc->value, lc->value))
            lc = rc;
        if((Comp::prior(ptr->value, lc->value)))
            break;
        swap(ptr, lc);
        ptr = lc;
    }
}

template<typename T, typename Comp>
void HeapList<T,Comp>::heapify()
{
    for(Int i = ((num_>>1)-1); i >= 0; --i)
    {
        BTNode<T>* curr = get_node(i);
        sift_down(curr);
    }
}

template<typename T, typename Comp>
void HeapList<T,Comp>::merge(HeapList<T,Comp>& heap)
{
    BTNode<T>* v = heap.get_root();
    if(v != nullptr)
    {
        while(!is_leaf(v) or v->parent != nullptr)
        {
            if(!is_leaf(v))
            {
                BTNode<T>* rt = v->right_child, *lt = v->left_child;

                if(rt != nullptr)
                    v = rt;
                else if(lt != nullptr)
                    v = lt;
            }
            else if(v->parent != nullptr)
            {
                BTNode<T>* p = v->parent;
                if(p->right_child == v)
                    p->right_child = nullptr;
                else
                    p->left_child = nullptr;
           
                //std::cout << v->value << std::endl; 
                push_back(v);
                v = p;
            }
        }
        push_back(v);
        heap.root_ = nullptr;
        heap.num_ = 0;
    }
}

template<typename T, typename Comp>
void HeapList<T,Comp>::delete_node(BTNode<T>* ptr)
{
    delete ptr;
}

template<typename T, typename Comp>
T HeapList<T,Comp>::get_node_value(BTNode<T>* ptr)
{
    return ptr->value;
}

template<typename T, typename Comp>
void HeapList<T,Comp>::check_heap()
{
    int k = MyMath::log2(num_);
    //std::cout << k << std::endl;
    for(int i = 1; i <= k+1; ++i)
    {
        for(int j = 1<<(i-1); j <= num_ and j < (1<<i); ++j)
        {
            BTNode<T>* curr = get_node(j);
            std::cout << curr->value << " ";
        } 
        std::cout << std::endl;
    }
}

template class HeapList<Int,Min>;
