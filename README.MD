# ft_malloc
malloc project @ 42

## Mandatory part
Rewrite `malloc` `free` and `realloc` functions.

also write a `show_alloc_mem` function to printing memory allocations.

Like the real malloc, the allocated memory must be stored in pre-allocated 
"zones" (**bins**), unless the requested size is too big, then the allocation must
be **fitted**.

Two zones are required: 
- Tiny zones of **N** size, storing allocations from 0 to **n** size
- Small zones of **M** size, storing allocations from **n**+1 to **m** size
- Larges allocations, fitted allocations beginning from **m**+1

Zones size must be a multiple of page size, acquired through `getpagesize()`.
Allocations (and internally required allocations) must be executed with `mmap`,
de-allocations with `munmap`

## Bonus part
- Making mandatory functions thread safe
- Debug flags
- Additional info print (mem dump, allocations history...) through said flags or extra functions
- De-fragmented bins on free and realloc

## Mandatory part code explanation
Lightly inspired from [debian glibc malloc](https://sources.debian.org/src/glibc/2.28-10/malloc/malloc.c/)

When words related to allocation will be put between double quotes
it means that it is not necessarily an allocation, attributing a portion
of a bin is not an allocation, a fitted allocation is indeed an allocation.

Since most of the explanations are not linked to real memory allocations made
with `mmap` and applies to bin or fitted "allocations", whether it is an
attribution or an allocation do not change how it is organised.

### Brief explanation of memory alignement

The computer read memory by sections of **4/8 bytes**, or **32/64 bits**, this determines
the name of the architecture.

Aligning memory corresponds to set variables on start of **4/8 bytes** sections.
If this is not respected, it raises :

- **Constraint violation**: an alignment error interrupts the execution of 
instructions.

- **Non-optimal alignment**: twice 4 well-aligned bytes are read, 
then the 4 misaligned bytes are extracted from these 8 bytes, 
which takes much longer than reading 4 well-aligned bytes.

Considering this, compilers will automatically align memory when possible
by padding memory with `char`, meaning that, when the fields of a structure
are not positioned properly or the total size of a structure is not a multiple
of 4/8, it will take more memory than it should, due to padding.

However, when freely attributing values in memory, it is required to be
careful with alignement, to prevent unnecessary slowdowns.

### Chunks
Every memory "allocation" (bin or fitted) is preceded by a header.
The combination of a memory "allocation" (**data**) and it's header is called a **chunk**.

The header bears the following information : 
- Allocation size (`unsigned long`)
- Allocation real size (`unsigned long`)
- Pointer to next chunk (`void *`)
- Pointer to previous chunk (`void *`)

For a total of **16/32 bytes** on **32/64 bits** architectures. The structure
itself if memory aligned and the start of the data is also aligned.
The data size is always set to a multiple of 8, this forces
memory alignement on every architecture.

The data size is often larger than what was asked, to keep memory aligned.
The portion of data that matches the size asked by the user (**real size**) is called 
**provided data**, the remainder is called **tail**.

To sum things up, this is how a chunk is organised in memory (assuming a line is 4/8 bytes) :

|        |                                   |
|--------|-----------------------------------|
| Header | size of data (size)               |
|        | size of provided data (real size) |
|        | pointer to next chunk (next)      |
|        | pointer to previous chunk (prev)  |
| data   | provided data (could be any size) |
|        | tail (could be any size)          |

The size of the provided data is only here for debug information and `show_alloc_mem`.

#### Pros and cons
The pros of overhead instead of exported bookkeeping is that it is way faster
to retrieve data related to a chunk with a pointer to its data field (in free or realloc for example)
since it only required to subtract the header size to the pointer to retrieve it header.

It also takes less space since it does not contain pointers to data.

It also requires fewer calls to `mmap` (which are fairly slow) since it is not allocated elsewhere, data and header are allocated in a single call.

The cons are that a non-cautious user writing out of provided data bounds might ruin
the bookkeeping data and ruin malloc... But well, nobody is supposed to write out of bounds right ?

### Bins
In the similar way as a chunk, bins have a preceding header, 
the only differences are that the size corresponds to the full zone size, header included,
and that the real size field is unused and that pointers point to bins instead of chunks.


### malloc
When calling malloc, if the size is not too high for a bin (then it just will perform a fitted allocation)
it will iterate in the proper bin corresponding to the size and find a free chunk with the proper size, this helps
to prevent bin fragmentation.
if it reaches the end of a bin and there is enough space left in the bin, it will attribute a chunk in the bin.
If there is not enough space left in the bin, it will create a new bin.

If several contiguous freed chunks total size matches the new chunks size
they will be joined as a single chunk, again, to prevent fragmentation.

### free
When calling free, if the size is not too high for a bin (then it will just unmap the chunk and sort out next/prev pointers)
it will check if the provided chunk is at the end of a bin, if so, the chunk will be 
de-attributed, then it will iterate backward to find contiguous chunks to de-attribute them,
This helps to prevent bin fragmentation.

If the bin ends up being empty, it is de-allocated to reduce the program's memory footprint.

### realloc
When calling realloc, if the size is not too high for a bin (which will be covered in a few lines)
it will run a few checks :

- If the new size is equal the current real size, if so, it will just return the provided pointer.

- If the new size is lesser than the current real size, it will adjust the chunk's real size and return the provided pointer.

- If it is greater, but lesser than the chunk's data size, it will adjust the chunk's real size and return the provided pointer.

In the two previous cases, if the chunk is at the end of a bin, the chunk will be expanded (if there is enough space left)
or trimmed to fit the new size.

- If the new size is greater than the chunk's data size (or bin's left size if it is at the end of a bin)
it will simply store the chunk's data in a buffer, free the chunk, call malloc to get a new chunk and copy the buffer
into the new chunk's data.

In a similar fashion. if the size is too big for a bin, it will try to expand or trim the chunk with `mmap`,
if it worked, chunk's size and real size is changed accordingly and the provided pointer is returned. 
If it did not work, it will copy the chunk's data to the new chunk and return a pointer to the new chunk's data.

## Bonus part code explanation (WIP)
None of the bonuses really requires an explanation, besides eventually de-fragmentation...

Preventing fragmentation is already covered in the mandatory part (to be fair, if one wants to do this bonus, it is recommended to code the mandatory part
with de-fragmentation in mind...).

Planning make malloc thread-safe since it seems to be quite easy.
