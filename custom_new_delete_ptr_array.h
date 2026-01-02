#include <cstddef>
#include <cstdint>
#include <new>
#include <cstring>
#include <cstdlib>
#include <cstdio>

static constexpr size_t HEAP_SIZE = 1024 * 1024; // 1 MB
static uint8_t heap[HEAP_SIZE];

static constexpr uint64_t MAGIC = 0xDEADBEEFCAFEBABE;
static constexpr size_t FLAG_FREE = 1 << 0;

struct BlockHeader {
    uint64_t magic;
    size_t size;           // usable payload size
    size_t flags;          // free flag
    BlockHeader* next;
};

static BlockHeader* free_list = nullptr;

/* ---------------- Heap Init ---------------- */

static void init_heap() {
    free_list = reinterpret_cast<BlockHeader*>(heap);
    free_list->magic = MAGIC;
    free_list->size = HEAP_SIZE - sizeof(BlockHeader);
    free_list->flags = FLAG_FREE;
    free_list->next = nullptr;
}

/* ---------------- Block Splitting ---------------- */

static void split_block(BlockHeader* block, size_t size) {
    if (block->size >= size + sizeof(BlockHeader) + 8) {
        auto* new_block = reinterpret_cast<BlockHeader*>(
            reinterpret_cast<uint8_t*>(block + 1) + size);

        new_block->magic = MAGIC;
        new_block->size = block->size - size - sizeof(BlockHeader);
        new_block->flags = FLAG_FREE;
        new_block->next = block->next;

        block->size = size;
        block->next = new_block;
    }
}

/* ---------------- malloc ---------------- */

void* my_malloc(size_t size) {
    if (!free_list)
        init_heap();

    BlockHeader* curr = free_list;

    while (curr) {
        if ((curr->flags & FLAG_FREE) && curr->size >= size) {
            split_block(curr, size);
            curr->flags &= ~FLAG_FREE;
            return reinterpret_cast<void*>(curr + 1);
        }
        curr = curr->next;
    }

    return nullptr; // OOM
}

/* ---------------- Coalescing ---------------- */

static void coalesce() {
    BlockHeader* curr = free_list;

    while (curr && curr->next) {
        uint8_t* curr_end =
            reinterpret_cast<uint8_t*>(curr + 1) + curr->size;

        if ((curr->flags & FLAG_FREE) &&
            (curr->next->flags & FLAG_FREE) &&
            reinterpret_cast<uint8_t*>(curr->next) == curr_end) {

            curr->size += sizeof(BlockHeader) + curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

/* ---------------- free ---------------- */

void my_free(void* ptr) {
    if (!ptr) return;

    auto* block =
        reinterpret_cast<BlockHeader*>(ptr) - 1;

    // Safety checks
    if (block->magic != MAGIC) {
        std::abort(); // invalid pointer
    }
    if (block->flags & FLAG_FREE) {
        std::abort(); // double free
    }

    block->flags |= FLAG_FREE;

    // Poison memory (debug)
    std::memset(ptr, 0xFD, block->size);

    coalesce();
}

/* ---------------- Global new / delete ---------------- */

void* operator new(size_t size) {
    void* p = my_malloc(size);
    if (!p) throw std::bad_alloc();
    return p;
}

void operator delete(void* ptr) noexcept {
    my_free(ptr);
}

/* ---------------- new[] / delete[] ---------------- */

void* operator new[](size_t size) {
    size_t total = size + sizeof(size_t);
    void* raw = my_malloc(total);
    if (!raw) throw std::bad_alloc();

    // Store array size in payload prefix
    *reinterpret_cast<size_t*>(raw) = size;

    return reinterpret_cast<void*>(
        reinterpret_cast<size_t*>(raw) + 1);
}

void operator delete[](void* ptr) noexcept {
    if (!ptr) return;

    size_t* raw = reinterpret_cast<size_t*>(ptr) - 1;
    my_free(raw);
}

