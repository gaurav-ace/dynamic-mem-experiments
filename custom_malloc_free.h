#include <cstdint>  // this is for uniform fixed width integers like int8_t, int16_t etc
#include <cstddef>  // this is for general used types like size_t, ptrdiff_t and offsetof() etc


static constexpr size_t HEAP_SIZE = 1024 * 1024;  // 1 Mb, 
/*
 * constexpr : 
 * -> immutable and guaranteed result at compile time.
 * -> can be used with function or constructors as well (enables computation at compile time)
 * -> expression
 *
 * const :
 * -> immutable at runtime.
 * -> result not guranteed at compile time, used only with varaibles.
 * const int a = rand();  -> known at runtime.
 * const int a  = 5; -> Read-only but not necessarily compile-time
 */                                                 
static uint8_t heap[HEAP_SIZE];


/*
 * block for metadata
 * 
 * [ BlockHeader ][ user memory ][ BlockHeader ][ user memory ] ...
 *
 */
struct BlockHeader {

    size_t size;  // usable memory size
    bool free;
    BlockHeader* next;
};

static BlockHeader* freeList = nullptr;


void init_heap() {
   freeList = reinterpret_cast<BlockHeader*>(heap);
   freeList->size = HEAP_SIZE-sizeof(BlockHeader);
   freeList->free = true;
   freeList->next = nullptr;
}

// [blockheader] | [usable memory] | [next blockheader] | [usable memory] | ...
void* my_malloc(size_t size) {
    if(!freeList)
        init_heap();
    
    BlockHeader* cur = freeList;
    while(cur) {
        if(cur->free && cur->size >= size + sizeof(BlockHeader)) {
            BlockHeader* new_block = 
                        reinterpret_cast<BlockHeader*>(cur + 1) + size;
            new_block->size = cur->size - size - sizeof(BlockHeader);
            new_block->free = true;
            new_block->next = cur->next;
    
            cur->size = size;
            cur->free = false;
            cur->next = new_block;
            return reinterpret_cast<void*>(cur+1); // or return cur+1 will also work (implict conversion.)
        }
        cur = cur->next;  
    }
    return nullptr; // memory full
}

void my_free(void* ptr) {
    if(!ptr) return;

    BlockHeader* bh = reinterpret_cast<BlockHeader*>(ptr)-1;
    bh->free = true;
}

