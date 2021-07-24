# MemoryPool

The idea behind this memory pool is to create a fast allocator that allows for getting memory chunks different in size and providing logics for uniting neighboring deallocated chunks.
So, the naive idea is to allocate the volume of memory once and to create a linked-list based structure with nodes that keep information about every allocated chunk:

![mpool1](https://user-images.githubusercontent.com/59689769/126879376-bb12721c-fdc7-446d-b698-10dc92d0e342.png)

But with every allocation/deallocation of memory chunk from the pool the allocation/deallocation of memory for nodes is also required. So, no significant speed in comparison with .malloc()/.free() is achieved. To avoid such a drawback the realized approach uses internal vector of nodes with pre-reserved memory for their total needed quantity. The adding node to the internal vector realised in a special way and costs O(n).

![image](https://user-images.githubusercontent.com/59689769/126879507-f7e03900-fdd8-4594-af1c-3f01e292babc.png)

The effectiveness of allocator was tested using worst-case scenario (with maximum number of blocks uniting operations), the size of allocate chunks was 4096 bytes, mode – Release, platform Win 86. Each test run included 8 operations of allocation/deallocation. The results are the following:

![image](https://user-images.githubusercontent.com/59689769/126879531-daed89a1-25ff-449a-a432-86ffd854ae73.png)

The drawback of this allocator is the size of the node that contains a lot of information. For example, if we create the memory pool with size of 10 bytes, the size of the vector with reserved memory for 10 nodes could be 20 times larger. The one way to avoid this is to restrict the minimal size of the allocated memory chunk.
