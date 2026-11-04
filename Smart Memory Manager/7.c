
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

typedef enum{
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT
} Strategy;

typedef struct{
    size_t start;
    size_t size;
    unsigned int id;
    bool allocated;
} Block;

typedef struct{
    unsigned char *memory;
    Block *blocks;
    size_t total_size;
    size_t count;
    size_t capacity;
    unsigned int next_id;
    Strategy strategy;
} MemoryManager;

bool reserve_blocks(MemoryManager *manager, size_t required){
    if (required <= manager->capacity)
        return true;

    size_t capacity = manager->capacity ? manager->capacity : 4;

    while (capacity < required)
    {
        if (capacity > SIZE_MAX / 2)
            return false;
        capacity *= 2;
    }

    if (capacity > SIZE_MAX / sizeof(Block))
        return false;

    Block *blocks = realloc(manager->blocks, capacity * sizeof(Block));

    if (!blocks)
        return false;

    manager->blocks = blocks;
    manager->capacity = capacity;
    return true;
}

bool insert_block(MemoryManager *manager, size_t index, Block block){
    if (index > manager->count || manager->count == SIZE_MAX || !reserve_blocks(manager, manager->count + 1))
        return false;

    memmove(&manager->blocks[index + 1], &manager->blocks[index], (manager->count - index) * sizeof(Block));

    manager->blocks[index] = block;
    manager->count++;
    return true;
}

void remove_block(MemoryManager *manager, size_t index){
    memmove(&manager->blocks[index], &manager->blocks[index + 1], (manager->count - index - 1) * sizeof(Block));

    manager->count--;
}

bool initialize_manager(MemoryManager *manager, size_t size){
    *manager = (MemoryManager){0};
    if (size == 0)
        return false;
    manager->memory = malloc(size);

    if (!manager->memory)
        return false;

    manager->total_size = size;
    manager->next_id = 1;
    manager->strategy = FIRST_FIT;

    if (!reserve_blocks(manager, 1))
    {
        free(manager->memory);
        manager->memory = NULL;
        return false;
    }

    manager->blocks[0] = (Block){0, size, 0, false};
    manager->count = 1;
    return true;
}

size_t first_fit(MemoryManager *manager, size_t size){
    for (size_t i = 0; i < manager->count; i++)
    {
        Block block = manager->blocks[i];

        if (!block.allocated && block.size >= size)
            return i;
    }

    return SIZE_MAX;
}

size_t best_fit(MemoryManager *manager, size_t size){
    size_t best = SIZE_MAX;

    for (size_t i = 0; i < manager->count; i++)
    {
        Block block = manager->blocks[i];

        if (block.allocated || block.size < size)
            continue;

        if (best == SIZE_MAX || block.size < manager->blocks[best].size)
            best = i;
    }

    return best;
}

size_t worst_fit(MemoryManager *manager, size_t size){
    size_t worst = SIZE_MAX;

    for (size_t i = 0; i < manager->count; i++)
    {
        Block block = manager->blocks[i];

        if (block.allocated || block.size < size)
            continue;

        if (worst == SIZE_MAX || block.size > manager->blocks[worst].size)
            worst = i;
    }

    return worst;
}

size_t find_suitable_block(MemoryManager *manager, size_t size){
    switch (manager->strategy)
    {
    case FIRST_FIT:
        return first_fit(manager, size);
    case BEST_FIT:
        return best_fit(manager, size);
    case WORST_FIT:
        return worst_fit(manager, size);
    default:
        return SIZE_MAX;
    }
}

size_t find_allocation(MemoryManager *manager, unsigned int id){
    for (size_t i = 0; i < manager->count; i++)
    {
        if (manager->blocks[i].allocated && manager->blocks[i].id == id)
            return i;
    }

    return SIZE_MAX;
}

size_t find_block_start(MemoryManager *manager, size_t start){
    for (size_t i = 0; i < manager->count; i++)
    {
        if (manager->blocks[i].start == start)
            return i;
    }

    return SIZE_MAX;
}

void merge_free_blocks(MemoryManager *manager){
    size_t i = 0;

    while (i + 1 < manager->count)
    {
        Block *current = &manager->blocks[i];
        Block *next = &manager->blocks[i + 1];

        if (!current->allocated && !next->allocated)
        {
            current->size += next->size;
            remove_block(manager, i + 1);
        }
        else
        {
            i++;
        }
    }
}

void report_fragmentation(MemoryManager *manager)
{
    size_t total_free = 0;
    size_t largest_free = 0;

    for (size_t i = 0; i < manager->count; i++)
    {
        Block block = manager->blocks[i];

        if (!block.allocated)
        {
            total_free += block.size;

            if (block.size > largest_free)
                largest_free = block.size;
        }
    }

    printf("Total free memory: %zu\n", total_free);
    printf("Largest free block: %zu\n", largest_free);

    double fragmentation = total_free ? 100.0 * (total_free - largest_free) / total_free : 0.0;

    printf("External fragmentation: %.2f%%\n", fragmentation);
}

void allocate_memory(MemoryManager *manager, size_t size)
{
    if (size == 0 || manager->next_id == 0)
    {
        puts("Invalid allocation size or ID limit reached.");
        return;
    }

    size_t index = find_suitable_block(manager, size);

    if (index == SIZE_MAX)
    {
        puts("Allocation failed.");
        report_fragmentation(manager);
        return;
    }

    Block free_block = manager->blocks[index];
    size_t remaining = free_block.size - size;

    if (remaining && !reserve_blocks(manager, manager->count + 1))
    {
        puts("Unable to allocate metadata.");
        return;
    }

    unsigned int id = manager->next_id++;

    manager->blocks[index] = (Block){
        free_block.start, size, id, true};

    if (remaining)
    {
        insert_block(manager, index + 1, (Block){free_block.start + size, remaining, 0, false});
    }

    printf("Allocated ID: %u | Start: %zu | Size: %zu\n", id, free_block.start, size);
}

void release_memory(MemoryManager *manager, unsigned int id)
{
    size_t index = find_allocation(manager, id);

    if (index == SIZE_MAX)
    {
        puts("Allocation ID not found.");
        return;
    }

    manager->blocks[index].allocated = false;
    manager->blocks[index].id = 0;

    merge_free_blocks(manager);
    puts("Memory released.");
}

void resize_memory(MemoryManager *manager, unsigned int id, size_t new_size)
{
    size_t index = find_allocation(manager, id);

    if (index == SIZE_MAX || new_size == 0)
    {
        puts("Invalid allocation ID or size.");
        return;
    }

    Block current = manager->blocks[index];

    if (new_size == current.size)
    {
        puts("Allocation size unchanged.");
        return;
    }

    if (new_size < current.size)
    {
        if (!reserve_blocks(manager, manager->count + 1))
        {
            puts("Unable to allocate metadata.");
            return;
        }

        manager->blocks[index].size = new_size;

        insert_block(manager, index + 1, (Block){current.start + new_size, current.size - new_size, 0, false});

        merge_free_blocks(manager);
        puts("Allocation resized.");
        return;
    }

    size_t extra = new_size - current.size;

    if (index + 1 < manager->count && !manager->blocks[index + 1].allocated && manager->blocks[index + 1].size >= extra)
    {

        Block *next = &manager->blocks[index + 1];

        if (next->size == extra)
        {
            manager->blocks[index].size = new_size;
            remove_block(manager, index + 1);
        }
        else
        {
            next->start += extra;
            next->size -= extra;
            manager->blocks[index].size = new_size;
        }

        puts("Allocation resized in place.");
        return;
    }

    size_t destination = find_suitable_block(manager, new_size);

    if (destination == SIZE_MAX)
    {
        puts("Resize failed.");
        report_fragmentation(manager);
        return;
    }

    Block target = manager->blocks[destination];
    size_t remaining = target.size - new_size;

    if (remaining && !reserve_blocks(manager, manager->count + 1))
    {
        puts("Unable to allocate metadata.");
        return;
    }

    memmove(manager->memory + target.start, manager->memory + current.start, current.size);

    manager->blocks[destination] = (Block){
        target.start, new_size, id, true};

    if (remaining)
    {
        insert_block(manager, destination + 1, (Block){target.start + new_size, remaining, 0, false});
    }

    size_t old_index = find_block_start(manager, current.start);

    manager->blocks[old_index].allocated = false;
    manager->blocks[old_index].id = 0;

    merge_free_blocks(manager);

    printf("Allocation moved and resized. New start: %zu\n",
           target.start);
}

void display_memory_map(MemoryManager *manager)
{
    puts("\n--- Memory Map ---");

    for (size_t i = 0; i < manager->count; i++)
    {
        Block block = manager->blocks[i];

        printf("[%zu - %zu] %s | Size: %zu", block.start, block.start + block.size - 1, block.allocated ? "Allocated" : "Free", block.size);

        if (block.allocated)
            printf(" | ID: %u", block.id);

        putchar('\n');
    }
}

void display_statistics(MemoryManager *manager)
{
    size_t used = 0;
    size_t free_size = 0;
    size_t allocations = 0;
    size_t free_blocks = 0;

    for (size_t i = 0; i < manager->count; i++)
    {
        Block block = manager->blocks[i];

        if (block.allocated)
        {
            used += block.size;
            allocations++;
        }
        else
        {
            free_size += block.size;
            free_blocks++;
        }
    }

    printf("\nTotal memory: %zu\n", manager->total_size);
    printf("Used memory: %zu\n", used);
    printf("Free memory: %zu\n", free_size);
    printf("Allocations: %zu\n", allocations);
    printf("Free blocks: %zu\n", free_blocks);

    report_fragmentation(manager);
}

void change_strategy(MemoryManager *manager)
{
    int choice;

    printf("1. First Fit\n2. Best Fit\n3. Worst Fit\nChoice: ");

    if (scanf("%d", &choice) != 1)
        return;

    if (choice < 1 || choice > 3)
    {
        puts("Invalid strategy.");
        return;
    }

    manager->strategy = (Strategy)(choice - 1);
    puts("Allocation strategy updated.");
}

void printMenu()
{
    printf("\n--- Smart Memory Manager ---\n");
    printf("1. Allocate memory\n");
    printf("2. Release memory\n");
    printf("3. Resize allocation\n");
    printf("4. Display memory map\n");
    printf("5. Display statistics\n");
    printf("6. Change allocation strategy\n");
    printf("7. Exit\n");
    printf("Choice: ");
}

int main()
{
    MemoryManager manager;
    size_t memory_size;
    int choice;

    printf("Enter total memory size: ");
    scanf("%zu", &memory_size);
    if (memory_size < 1 || memory_size > 1000000)
    {
        printf("The memory pool size must be between 1and 1,000,000 bytes.\n");
        return 1;
    }
    if (!initialize_manager(&manager, memory_size))
    {
        puts("Unable to initialize memory manager.");
        return 1;
    }

    int strategy_choice;

    printf("Select allocation strategy (1. First Fit, 2. Best Fit, 3. Worst Fit): ");

    if (scanf("%d", &strategy_choice) != 1 || strategy_choice < 1 || strategy_choice > 3)
    {
        puts("Invalid strategy.");
        free(manager.blocks);
        free(manager.memory);
        return 1;
    }

    manager.strategy = (Strategy)(strategy_choice - 1);

    while (true)
    {
        printMenu();
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
        {
            size_t size;
            printf("Enter allocation size: ");
            if (scanf("%zu", &size) == 1)
                allocate_memory(&manager, size);
            break;
        }

        case 2:
        {
            unsigned int id;
            printf("Enter allocation ID: ");
            if (scanf("%u", &id) == 1)
                release_memory(&manager, id);
            break;
        }

        case 3:
        {
            unsigned int id;
            size_t size;
            printf("Enter allocation ID and new size: ");

            if (scanf("%u %zu", &id, &size) == 2)
                resize_memory(&manager, id, size);
            break;
        }

        case 4:
            display_memory_map(&manager);
            break;

        case 5:
            display_statistics(&manager);
            break;

        case 6:
            change_strategy(&manager);
            break;

        case 7:
            puts("Exiting.");
            free(manager.blocks);
            free(manager.memory);
            return 0;
            break;

        default:
            puts("Invalid choice.");
        }
    }

    return 0;
}
