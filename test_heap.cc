#include <iostream>
#include <cstdlib>
#include <cstring>
#include "heap.h"
#include "def.h"

template<typename T, typename Comp>
static void show_heap(Heap<T,Comp>* heap)
{
    using namespace std;
    cout << "HEAP INFORMATION\n";
    size_t num = heap->size();
    cout << "HEAP SIZE: " << num << endl;
    for(size_t i = 0; i < num; ++i)
    {
        T val = heap->top(); 
        heap->pop_back();
        cout << i << " " << val << endl;
    }
}

int main(int argc, char** argv)
{
    using namespace std;
    Heap<Int,Min> heap;
    size_t num = (size_t)atoi(argv[1]);
    for(size_t i = 0; i < num; ++i)
    {
       Int data = (Int)(rand() % (num<<4));
       heap.push_back(data); 
    }
    
    show_heap(&heap); 
    
    return 0;
}
