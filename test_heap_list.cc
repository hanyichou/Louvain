#include <iostream>
#include <cstdlib>
#include <cmath>
#include "heap_list.h"
#include "def.h"

static void show_heap(HeapList<Int, Min>& heap)
{
    Int size = heap.size();
    for(Int i = 0; i < size; ++i)
    {
        BTNode<Int>* ptr = heap.pop();
        std::cout << ptr->value << std::endl;
        //heap.remove_top();
        delete ptr;
        //heap.check_heap();
        //std::cout << std::endl;
    }
}

int main(int argc, char** argv)
{
    HeapList<Int, Min> heap1;
    Int num = atoi(argv[1]);

    for(Int i = 0; i < num; ++i)
    {
        int val = rand() % (num<<2)+1;
        heap1.push_back(val);
        std::cout << heap1.size() << " " << val << std::endl;
    }
    //std::cout << std::endl;
    heap1.check_heap();
    std::cout << std::endl;
    //show_heap(heap1);
    std::cout << std::endl;

    HeapList<Int, Min> heap2;
    num = atoi(argv[2]);

    for(Int i = 0; i < num; ++i)
    {
        int val = rand() % (num<<2)+1;
        heap2.push_back(val);
        std::cout << heap2.size() << " " << val << std::endl;
    }
    std::cout << std::endl;
    heap2.check_heap();
    std::cout << std::endl;
    //show_heap(heap2);

    heap2.merge(heap1);
    show_heap(heap2);

    //heap.check_heap();    
    return 0;
}
