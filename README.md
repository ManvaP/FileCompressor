# FileCompressor

The fileCompressor program has a lot of working parts and different aspects so that it can 
compress and decompress files using the Huffman algorithm. We first start with an 
implementation of a min-heap. The min-heap keeps the words in the heap-order property, so that 
later we can give the words different bit strings and compress/decompress. The heap has 
functions such as insert, reallocateHeap, siftUp, siftDown, siftDownUpdate, and deleteMin. 
Insert simply inserts a token from the file into the heap, along with an occurence of one. If the 
number of items in the heap is filled up, insert makes a call to reallocateHeap, which doubles the 
size of the heap. SiftUp is also called within insert, as the node will be inserted at the end of the 
heap and needs to be sifted up the heap to maintain the heap-ordering property. When we find a 
token that is already in the heap, we need to update its frequency by one, thus the new 
occurrence of that word needs to be siftedDown to maintain the heap-ordering property. 
SiftDown and deleteMin are functions that are used to build the Huffman encoding tree, which 
will be explained later on in this document.

When tokenizing the file for words, we start building the token by reading one character 
from the file at a time. If the token is full, then we reallocate the space for it, and proceed to keep 
adding one character from the file at a time, providing we do not hit a delimiter. A delimiter is an 
escape sequence in our program like new lines, tabs, spaces, etc. Then we take the token and 
insert the full token into the heap. Afterwards the token is reset, and the file reading character by 
character keeps looping until the end of the file and repeating this process of tokenizing.

After tokenizing the words in the entire file, we proceed to do the Huffman algorithm on 
the heap to generate a Huffman encoding tree. The Huffman algorithm takes the two minimum 
frequency nodes from the heap and proceeds to build an internal node from it with a combined 
frequency of the two min nodes, and a left and right pointer to each node respectively. This 
process continues until there is only one node left in the heap at the first index, which is the 
entire Huffman tree.

After the Huffman tree is built, we begin to build the codebook when -b is called, in 
preparation for the Huffman compression/decompression. Now we need togenerate the Huffman
codes from the tree. We do this by traversing the tree, goin to the left of the root adds a 0, and 
going to the right of the root adds a 1, building the token as we traverse through the tree. All 
these codes are stored in a file called HuffmanCodebook, and now we are ready to do
compression/decompression.

Compression and decompression are both relatively similar in their methodology. We 
read through the given file byte by byte and compare it to the words in the codebook. If we find the word in the codebook, then we write the bit string to a .hcz file (in compression). In 
decompression we write the word to the file instead of the bit string. If we do not find the word 
in the codebook, we keep comparing byte by byte until we do, or we run out of the file, causing
an error. When the codebook is computed, the maximum length of a single Huffman encoding is 
stored, so that we can check that number of bytes at a time, for efficiency sake. If a token is 
bigger than that previously computed maximum size, we know it is not in the codebook, and we 
return with an error.

Most of the spatial complexity of fileCompressor is within the heap itself. An array of 
structs takes up quite a bit of memory, so to save memory in terms of space efficiency, we start 
with a heap that has a small amount of values and reallocate space for more as it gets filled up. 
The time complexity of our fileCompressor implementation is k*m*n. First you must calculate 
making the heap from the codebook which takes k, then you must run through the input string, 
which contains m bytes, and then you must compare the string to everything in the heap, n items. 
Thus, the big O is k*m*n
