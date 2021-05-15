// this is it manav
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>
#include "heaptest.c"
#include "codebookHeap.c"

void printCodes(struct heapNode* root, int arr[], int top);

char* openD(char** name){
		int fh, rd, n;
		fh = open(*name, O_RDONLY);
		if(fh == -1){
			printf("0ERROR with %s\n", *name);
			perror("Error");
			return *name;
		} else {
			// First find the size of the file using stat() system call
			struct stat v;
			stat(*name, &v);
			int size = v.st_size;
			// Create a dynamic array equal to file size
			char *input = malloc(size * sizeof(char));

			// Read data from file and copy into dynamic array and print
			int ret = read(fh, input, size);
			if(ret == -1){
				printf("1ERROR with %s\n", *name);
				perror("read");
				exit(1);
			} else {
				return input;
			}
		}
}

// traverse is for the recursive protocol of building the codebook (Noted by -R)
void traverse(DIR *dr, char* start, char* flag){
    
	struct dirent *de;
	DIR *dire = opendir(start);
	 
	int heapIndex = 0;
	int heapSize = 10;
	int heapSizeC = 0;
	int maxHuffman = 0;
       	struct heapNode** heap = (struct heapNode**)malloc(heapSize * sizeof(struct heapNode*));
	int foundCodebook = 0;

	int y = 0;
	for(y = 0; y < heapSize; y++){
		heap[y] = (struct heapNode*) malloc(sizeof(struct heapNode));
	}	

	struct stat buffer;
	if( stat("HuffmanCodebook", &buffer) == 0){
		foundCodebook = 1;
	} 

	while(((de = readdir(dire)) != NULL)){
        	if(de->d_type == DT_DIR){
            		// is a directory, make the recursive call 
            
            		if(strcmp(de->d_name, ".")==0 || strcmp(de->d_name, "..") == 0 || strcmp(de->d_name, ".git") == 0){
                		continue;
            		}

            		DIR *dir = opendir(de->d_name);
            		chdir(de->d_name);
            		traverse(dir, start, flag);
			chdir("..");
        	}else{
            		//printf("%s\n", de->d_name);
			char* temp = de->d_name; // temp holds file name
			char* input = openD(&(temp)); // So testing holds the string we're going to use 
			//printf("%s\n", input);

			// If for some reason we couldn't open the file properly
			if(strcmp(input, de->d_name) == 0){
				printf("Something went wrong with opening files\n");
				exit(1);
			}

			if(strcmp(flag, "-b") == 0){
				char *token = malloc(sizeof(char)); // String that will be used to hold our token(s)
				int i; // Loop Counter
				int indexNum = 0; // Used to count the size of a single token, reset after each token
				int stringSize = sizeof(char); // Used as a tracker for the size of a string
	
				for(i = 0; i < strlen(input); ++i){
					//printf("here's i  %d  and inputLength  %d  and heres input[i]:%c.\n", i, strlen(input), input[i]);
					if( ((int)input[i] <= (char)32) || ((int)input[i] == (char)172) ) {
						//printf("here's the token '%s' and here's the escape code we found '%c'\n", token, input[i]);
						if(i == 0){ // This escape sequence is the first character in the string, so add the escape sequence to the heap
							heap[heapIndex]->tokenName = (char *)malloc(2*sizeof(char));
							char* newLine = (char*) malloc(2*sizeof(char));
							newLine[0] = input[i];
							newLine[1] = '\0';
							
							int occ = 1;
							insert(&newLine, &occ, &heap, &heapIndex, &heapSize);
							siftDownUpdate(&heap, &heapIndex, heapIndex-1);
						} else if(indexNum == 0){ // Our token is empty, so just add the escape sequence or increment its frequency
							//printf("We have no token! Here is the escape '%c'\n", input[i]);
							if(heapIndex == 0){
								//printf("heap is empty and no token! Here is the escape again '%c'\n", input[i]);
								heap[heapIndex]->tokenName = (char *)malloc(2*sizeof(char));
								char* newLine = (char*) malloc(2*sizeof(char));
								newLine[0] = input[i];
								newLine[1] = '\0';
								
								int occ = 1;
								insert(&newLine, &occ, &heap, &heapIndex, &heapSize);
								siftDownUpdate(&heap, &heapIndex, heapIndex-1);
							} else {
								// We need to search the heap to see if the heap contains our escape sequence
								int y, foundCode = 0;
								for(y = 0; y < (heapIndex); y++){
									if( (int)(((heap[y]))->tokenName[0]) == (int)input[i] ){
									//if(strcmp( ((heap[i]))->tokenName, (char*)input[i]) == 0){
										if(foundCode == 0){
											foundCode = 1;
											((heap[y])->frequency) += 1;
											//printf("Increasing freq of escape '%s', freq is now '%d'\n", i, heap[y]->tokenName, heap[y]->frequency);
											// <============================================================================= SIFTDOWN WILL CHANGE
											siftDownUpdate(&heap, &heapIndex, y);
											//printf("we found t, here is the freq %d\n", heap[y]->frequency);
											break;
										}
									}
								}
								// If it doesnt find it in the heap, add it to the heap
								if(foundCode == 0){
									//printf("Our escape '%c' isn't in the heap so insert it\n", input[i]);
									heap[heapIndex]->tokenName = (char *)malloc(2*sizeof(char));
									char* newLine = (char*) malloc(2*sizeof(char));
									newLine[0] = input[i];
									newLine[1] = '\0';
					
									int occ = 1;
									insert(&newLine, &occ, &heap, &heapIndex, &heapSize);
									siftDownUpdate(&heap, &heapIndex, heapIndex-1);
								}
							}	
						} else { // We have SOMETHING in our token, so we're gonna have to add that AND the escape sequence
							// Close off the excess space in our array to save space
							token = realloc(token, sizeof(char)*indexNum+1);
							if(token == NULL){
								printf("Something went wrong with allocating\n");
								return;
							}
							//printf("We have something in our token '%s' and an escape char '%c'\n", token, input[i]);
							if(heapIndex == 0){ // If there's nothing in the heap, add both to the heap
								int occurence = 1;
								heap[heapIndex]->tokenName = (char *)malloc(sizeof(char)*(strlen(token)+1));
								insert(&token, &occurence, &heap, &heapIndex, &heapSize);	
								//printf("inserted %s, %d, %d\n", heap[0]->tokenName, heap[0]->frequency, heapIndex-1);
	
								heap[heapIndex]->tokenName = (char *)malloc(2*sizeof(char));
								char* newLine = (char*) malloc(2*sizeof(char));
								newLine[0] = input[i];
								newLine[1] = '\0';
								
								int occ = 1;
								insert(&newLine, &occ, &heap, &heapIndex, &heapSize);
								siftDownUpdate(&heap, &heapIndex, heapIndex-1);
								//printf("inserted %s, %d, %d\n", heap[1]->tokenName, heap[1]->frequency, heapIndex-1);
							} else {
								// We need to search the heap to see if the heap contains our word
								int y, foundToken = 0, foundCode = 0;
								for(y = 0; y < (heapIndex); y++){
									//printf("%d Comparing '%s' and '%s' at %d \n", i,token, heap[y]->tokenName,y);
									if(strcmp( ((heap[y])->tokenName), token) == 0){
										if(foundToken == 0){
											foundToken = 1;
											((heap[y])->frequency) += 1;
											//printf("%d %d Increasing freq of token '%s', freq is now '%d'\n", i, y, heap[y]->tokenName, heap[y]->frequency);
											// <======================================================= SIFTDOWN WILL CHANGE
											siftDownUpdate(&heap, &heapIndex, y);
											break;
										}
									}
									
								}
								
								for(y = 0; y < (heapIndex); y++){
									// MIGHT WANNA CHECK IF TOKENNAME IS NULL HERE. IF IT IS WE MIGHT SEGFAULT <===========
									if( (int)(((heap[y])->tokenName)[0]) == (int)(input[i]) ){
										if(foundCode == 0){
											foundCode = 1;
											((heap[y])->frequency) += 1;
											//printf("increasing freq of escape '%s', freq is now '%d'\n", heap[y]->tokenName, heap[y]->frequency);
											// <====================================================== SIFTDOWN WILL CAHNGE
											siftDownUpdate(&heap, &heapIndex, y);
											break;
										}
									}
								}	

								if(foundToken == 0){
									int occ = 1;
									//printf("%d foundToken is 0 for token %s\n", i, token);
									heap[heapIndex]->tokenName = (char *)malloc(sizeof(char)*(strlen(token)+1));
									insert(&token, &occ, &heap, &heapIndex, &heapSize);
									siftDownUpdate(&heap, &heapIndex, heapIndex-1);	
									/* int k;
									for(k = 0; k < heapIndex; k++){
										printf("token heap[heapToken]->name = %s, freq = %d\n", heap[k]->tokenName, heap[k]->frequency);
									} */
								}
	
								if(foundCode == 0){
									heap[heapIndex]->tokenName = (char *)malloc(2*sizeof(char));
									char* newLine = (char*) malloc(2*sizeof(char));
									newLine[0] = input[i];
									newLine[1] = '\0';
					
									int occ = 1;
									insert(&newLine, &occ, &heap, &heapIndex, &heapSize);
									siftDownUpdate(&heap, &heapIndex, heapIndex-1);
								}	
							}			
						}

						// Remove the previous token from and restart the index counter
						token = malloc(sizeof(char));
						stringSize = sizeof(char);
						indexNum = 0;
					} else { // This is the only case where we continue making the token
						if(indexNum >= stringSize-1){
							stringSize *= 2;
							token = realloc(token, sizeof(char)*stringSize);
							if(token == NULL){
								printf("Something went wrong with allocating\n");
								return;
							}
						}
						token[indexNum] = input[i];
						//printf("Here is the string now %s\n", token);
						++indexNum;
					}	
				}
			} else if(strcmp(flag, "-c") == 0){
				// Check if the Codebook exists
				// Done Before the whle loop

				// Build a Heap from the Codebook
				struct heapNodeC** heapC = buildHeap(&heapSizeC, &maxHuffman);
				if(heapSizeC <= 0){
					printf("Error with Codebook\n");
					return;
				}

				char *token = malloc(sizeof(char)); // String that will be used to hold our token(s)
				int i; // Loop Counter
				int indexNum = 0; // Used to count the size of a single token, reset after each token
				int stringSize = sizeof(char); // Used as a tracker for the size of a string
				
				int t;
				char* given = temp;
				int newFileNameSize = strlen(given);
				char* newFileName = malloc( (5+strlen(given))*sizeof(char) ); // New File Name will be 5+argv[2] because we're adding '.' , 'h' , 'c' , 'z' , '\0'
				newFileName = given;
				newFileName[newFileNameSize] = '.';
				newFileName[++newFileNameSize] = 'h';
				newFileName[++newFileNameSize] = 'c';
				newFileName[++newFileNameSize] = 'z';
				newFileName[++newFileNameSize] = '\0';
				//printf("NewFileName '%s'\n", newFileName);
				
				// fd creates the file and also replaces it. wd makes it so we can only append to the file after we create it
				int fd = open(newFileName, O_RDWR | O_CREAT | O_TRUNC, 0600); // <===========================================================================
				int wd = open(newFileName, O_RDWR | O_CREAT | O_APPEND, 0600);				

				for(i = 0; i < strlen(input); ++i){
					if( ((int)input[i] <= (char)32) || ((int)input[i] == (char)172) ) {
						//printf("THis is the delimiter '%c' and this is the token '%s'\n", input[i], token);
						if(indexNum == 0){ // If token is empty, we only search the heap for the delimiter only
							int y;
							int foundEscape = 0;
							for(y = 0; y < heapSizeC; y++){
								if( (int)(((heapC[y])->word)[0]) == (int)(input[i]) ){
								//if(strcmp(token, heapC[y]->word) == 0){
									foundEscape = 1;
									// WRITE TO OUTPUT FILE
									//printf("1Here is the escape code I'm printing '%c'\n", input[i]);
									int size = strlen( heapC[y]->bitString );
									int ez = write(wd, heapC[y]->bitString, size);
								} 
							}

							if(foundEscape == 0){
								printf("1Error: This file contains words that aren't in the codebook!\n");
								remove(newFileName);
								exit(1);
							}
						} else { // SOMETHING is in our token, so we have to search the heap for the token AND delimiter
							int y;
							int foundToken = 0;
							int foundEscape = 0;
							
							// Loop to search for the token
							for(y = 0; y < heapSizeC; y++){
								if(strcmp(token, heapC[y]->word) == 0){
									foundToken = 1;
									// WRITE TO OUTPUT FILE
									printf("2Here is the token I'm printing '%s'\n", token);
									int size = strlen( heapC[y]->bitString );
									int ez = write(wd, heapC[y]->bitString, size);
								} 
							}

							// Loop to search for the delimiter
							for(y = 0; y < heapSizeC; y++){
								if( (int)(((heapC[y])->word)[0]) == (int)(input[i]) ){
								//if(strcmp(token, heapC[y]->word) == 0){
									foundEscape = 1;
									// WRITE TO OUTPUT FILE
									printf("3Here is the escape code I'm printing '%c'\n", input[i]); 
									int size = strlen( heapC[y]->bitString );
									int ez = write(wd, heapC[y]->bitString, size);
								} 
							}
						
							// If either were not in the codebook, exit with error
							if(foundToken == 0){
								printf("Token: '%s' 2Error: This file contains words that aren't in the codebook!\n", token);
								remove(newFileName);
								exit(1);
							}
							if(foundEscape == 0){
								printf("3Error: This file contains words that aren't in the codebook!\n");
								remove(newFileName);
								exit(1);
							}
						}

						token = malloc(sizeof(char));
						stringSize = sizeof(char);
						indexNum = 0;
					} else { // This is the only case where we continue making the token
						if(indexNum >= stringSize-1){
							stringSize *= 2;
							token = realloc(token, sizeof(char)*stringSize);
							if(token == NULL){
								printf("Something went wrong with allocating\n");
								return;
							}
						}
						token[indexNum] = input[i];
						//printf("Here is the string now %s\n", token);
						++indexNum;
					}
				}

			}else if(strcmp(flag, "-d") == 0){
				// Check to see if the file is a .hcz file
				int length = strlen(temp) - 1;
				if(temp[length] != 'z' && temp[--length] != 'c' && temp[--length] != 'h' && temp[--length] != '.'){
					printf("Not a .hcz file!\n");
					continue;
				}
		
				// Build a Heap from the Codebook
				struct heapNodeC** heapC = buildHeap(&heapSizeC, &maxHuffman);
				if(heapSizeC <= 0){
					printf("Error with Codebook\n");
					return;
				}

				char *token = malloc(sizeof(char)); // String that will be used to hold our token(s)
				int i; // Loop Counter
				int indexNum = 0; // Used to count the size of a single token, reset after each token
				int stringSize = sizeof(char); // Used as a tracker for the size of a string
				
				int t;
				char* given = temp;
				int newFileNameSize = strlen(given) - 4;
				char* newFileName = given;
				newFileName[newFileNameSize] = '\0';
				//printf("NewFileName '%s'\n", newFileName);

				// fd creates the file and also replaces it. wd makes it so we can only append to the file after we create it
				int fd = open(newFileName, O_RDWR | O_CREAT | O_TRUNC, 0600); // <===========================================================================
				int wd = open(newFileName, O_RDWR | O_CREAT | O_APPEND, 0600);				

				for(i = 0; i < strlen(input); ++i){
					// In decompression, we're always building the token, so no if else statement needed
					if(indexNum >= stringSize-1){
						stringSize *= 2;
						token = realloc(token, sizeof(char)*stringSize);
						if(token == NULL){
							printf("Something went wrong with allocating\n");
							return;
						}
					}
					token[indexNum] = input[i];
					//printf("Here is the string now %s\n", token);
					++indexNum;

					if(indexNum > maxHuffman){
						printf("Error: This file contains encoding which isn't in the codebook!\n");
						remove(newFileName);
						exit(1);
					}

					int y;
					int foundWord = 0;
					for(y = 0; y < heapSizeC; y++){
						if( strcmp(token, heapC[y]->bitString) == 0 ){
							foundWord = 1;
							// WRITE TO OUTPUT FILE
							int size = strlen( heapC[y]->word );
							int ez = write(wd, heapC[y]->word, size);
							
							// Now that we've added, we can reset our token to prepare for the next one
							token = malloc(sizeof(char));
							stringSize = sizeof(char);
							indexNum = 0;
						}
					}

				}

			}
        	}
    }
	
	// HUFFMAN
	if( strcmp(flag, "-b") == 0){
		int heapItems = heapIndex;
		int heapSize2 = heapIndex;
		while(heapItems != 0){
			// Get two minimums
			struct heapNode* left = deleteMin(&heap, &heapIndex, &heapSize);
			struct heapNode* right = deleteMin(&heap, &heapIndex, &heapSize);

			struct heapNode* tree = (struct heapNode*)malloc(sizeof(struct heapNode));
			tree->tokenName = NULL;
			int freq = (left->frequency) + (right->frequency);
			tree -> left = left;
			tree -> right = right;
					
			insertTree(&tree, &freq, &heap, &heapIndex, &heapSize);
			heapItems--;
		}

		// heap[0] is now the Huffman Tree
		//printf("\nHere is the Huffman Tree:\n");
		int arr[heapSize2], top = 0;	
		int fd = open("HuffmanCodebook", O_RDWR | O_CREAT | O_TRUNC, 0600);	
		int sz = write(fd, "\\\n", 2*sizeof(char));
		printCodes(heap[0], arr, top);
	} 
    closedir(dr);
}


void segfault_sigaction(int signal, siginfo_t *si, void *arg){
	printf("Unfortunately something went wrong. Exiting\n");
	exit(0);
}

int main(int argc, char** argv){
	// Catch seg faults
	struct sigaction sa;
	memset(&sa, 0,sizeof(struct sigaction));
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = segfault_sigaction;
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &sa, NULL);	

	// There are 4 arguments for nonrecursive , 5 for recursive
	if(strcmp(argv[1], "-R") == 0 || strcmp(argv[2], "-R") == 0){
		// RECURSIVE CASE
		// argv[3] should hold the recursive directory
		DIR *temp = opendir(argv[3]);
		if( strcmp(argv[1], "-R") == 0){
			traverse(temp, argv[3], argv[2]);
		} else if( strcmp(argv[2], "-R") == 0){
			traverse(temp, argv[3], argv[2]);
		} 
		return 0;
	} 
	 
	int heapIndex = 0;
	int heapSize = 10;
	int heapSizeC = 0;
	int maxHuffman = 0;
       	struct heapNode** heap = (struct heapNode**)malloc(heapSize * sizeof(struct heapNode*));

	int y = 0;
	for(y = 0; y < heapSize; y++){
		heap[y] = (struct heapNode*) malloc(sizeof(struct heapNode));
	}	


	// NON RECURSIVE CASE
    	if(strcmp(argv[1], "-b") == 0){
		// In the case of a nonrecursive build, our input would look like
		// ./fileCompressor -b <fileName>
		int fh, rd, n;
		fh = open(argv[2], O_RDONLY);
		if(fh == -1){
			printf("ERROR with %s\n", argv[2]);
			perror("open");
			return;
		} else {
			// First find the size of the file using stat() system call
			struct stat v;
			stat(argv[2], &v);
			int size = v.st_size;
			// Create a dynamic array equal to file size
			char *input = malloc(size * sizeof(char));

			// Read data from file and copy into dynamic array and print
			int ret = read(fh, input, size);
			if(ret == -1){
				printf("ERROR with %s\n", argv[2]);
				perror("read");
				return;
			} else {
				//printf("data read from file %s is: \n", argv[2]);
				//tokenize(p, &heap, &heapIndex, &heapSize);
				//printf("%s\n", p);
				char *token = malloc(sizeof(char)); // String that will be used to hold our token(s)
				int i; // Loop Counter
				int indexNum = 0; // Used to count the size of a single token, reset after each token
				int stringSize = sizeof(char); // Used as a tracker for the size of a string
	
				for(i = 0; i < strlen(input); ++i){
					//printf("here's i  %d  and inputLength  %d  and heres input[i]:%c.\n", i, strlen(input), input[i]);
					if( ((int)input[i] <= (char)32) || ((int)input[i] == (char)172) ) {
						//printf("here's the token '%s' and here's the escape code we found '%c'\n", token, input[i]);
						if(i == 0){ // This escape sequence is the first character in the string, so add the escape sequence to the heap
							heap[heapIndex]->tokenName = (char *)malloc(2*sizeof(char));
							char* newLine = (char*) malloc(2*sizeof(char));
							newLine[0] = input[i];
							newLine[1] = '\0';
							
							int occ = 1;
							insert(&newLine, &occ, &heap, &heapIndex, &heapSize);
							siftDownUpdate(&heap, &heapIndex, heapIndex-1);
						} else if(indexNum == 0){ // Our token is empty, so just add the escape sequence or increment its frequency
							//printf("We have no token! Here is the escape '%c'\n", input[i]);
							if(heapIndex == 0){
								//printf("heap is empty and no token! Here is the escape again '%c'\n", input[i]);
								heap[heapIndex]->tokenName = (char *)malloc(2*sizeof(char));
								char* newLine = (char*) malloc(2*sizeof(char));
								newLine[0] = input[i];
								newLine[1] = '\0';
								
								int occ = 1;
								insert(&newLine, &occ, &heap, &heapIndex, &heapSize);
								siftDownUpdate(&heap, &heapIndex, heapIndex-1);
							} else {
								// We need to search the heap to see if the heap contains our escape sequence
								int y, foundCode = 0;
								//printf("Breka Heap is NOT empty but we still have no token. Escape is '%c'\n", input[i]);
								for(y = 0; y < (heapIndex); y++){
									if( (int)(((heap[y]))->tokenName[0]) == (int)input[i] ){
									//if(strcmp( ((heap[i]))->tokenName, (char*)input[i]) == 0){
										if(foundCode == 0){
											foundCode = 1;
											((heap[y])->frequency) += 1;
											//printf("Increasing freq of escape '%s', freq is now '%d'\n", i, heap[y]->tokenName, heap[y]->frequency);
											// <============================================================================= SIFTDOWN WILL CHANGE
											siftDownUpdate(&heap, &heapIndex, y);
											//printf("we found t, here is the freq %d\n", heap[y]->frequency);
											break;
										}
									}
								}
								// If it doesnt find it in the heap, add it to the heap
								if(foundCode == 0){
									//printf("Our escape '%c' isn't in the heap so insert it\n", input[i]);
									heap[heapIndex]->tokenName = (char *)malloc(2*sizeof(char));
									char* newLine = (char*) malloc(2*sizeof(char));
									newLine[0] = input[i];
									newLine[1] = '\0';
					
									int occ = 1;
									insert(&newLine, &occ, &heap, &heapIndex, &heapSize);
									siftDownUpdate(&heap, &heapIndex, heapIndex-1);
								}
							}	
						} else { // We have SOMETHING in our token, so we're gonna have to add that AND the escape sequence
							// Close off the excess space in our array to save space
							token = realloc(token, sizeof(char)*indexNum+1);
							if(token == NULL){
								printf("Something went wrong with allocating\n");
								return;
							}
							//printf("We have something in our token '%s' and an escape char '%c'\n", token, input[i]);
							if(heapIndex == 0){ // If there's nothing in the heap, add both to the heap
								int occurence = 1;
								heap[heapIndex]->tokenName = (char *)malloc(sizeof(char)*(strlen(token)+1));
								insert(&token, &occurence, &heap, &heapIndex, &heapSize);	
								//printf("inserted %s, %d, %d\n", heap[0]->tokenName, heap[0]->frequency, heapIndex-1);
	
								heap[heapIndex]->tokenName = (char *)malloc(2*sizeof(char));
								char* newLine = (char*) malloc(2*sizeof(char));
								newLine[0] = input[i];
								newLine[1] = '\0';
								
								int occ = 1;
								insert(&newLine, &occ, &heap, &heapIndex, &heapSize);
								siftDownUpdate(&heap, &heapIndex, heapIndex-1);
								//printf("inserted %s, %d, %d\n", heap[1]->tokenName, heap[1]->frequency, heapIndex-1);
							} else {
								// We need to search the heap to see if the heap contains our word
								int y, foundToken = 0, foundCode = 0;
								for(y = 0; y < (heapIndex); y++){
									//printf("%d Comparing '%s' and '%s' at %d \n", i,token, heap[y]->tokenName,y);
									if(strcmp( ((heap[y])->tokenName), token) == 0){
										if(foundToken == 0){
											foundToken = 1;
											((heap[y])->frequency) += 1;
											//printf("%d %d Increasing freq of token '%s', freq is now '%d'\n", i, y, heap[y]->tokenName, heap[y]->frequency);
											// <======================================================= SIFTDOWN WILL CHANGE
											siftDownUpdate(&heap, &heapIndex, y);
											break;
										}
									}
									
								}
								
								for(y = 0; y < (heapIndex); y++){
									// MIGHT WANNA CHECK IF TOKENNAME IS NULL HERE. IF IT IS WE MIGHT SEGFAULT <===========
									if( (int)(((heap[y])->tokenName)[0]) == (int)(input[i]) ){
										if(foundCode == 0){
											foundCode = 1;
											((heap[y])->frequency) += 1;
											//printf("increasing freq of escape '%s', freq is now '%d'\n", heap[y]->tokenName, heap[y]->frequency);
											// <====================================================== SIFTDOWN WILL CAHNGE
											siftDownUpdate(&heap, &heapIndex, y);
											break;
										}
									}
								}	

								if(foundToken == 0){
									int occ = 1;
									//printf("%d foundToken is 0 for token %s\n", i, token);
									heap[heapIndex]->tokenName = (char *)malloc(sizeof(char)*(strlen(token)+1));
									insert(&token, &occ, &heap, &heapIndex, &heapSize);
									siftDownUpdate(&heap, &heapIndex, heapIndex-1);	
									/* int k;
									for(k = 0; k < heapIndex; k++){
										printf("token heap[heapToken]->name = %s, freq = %d\n", heap[k]->tokenName, heap[k]->frequency);
									} */
								}
	
								if(foundCode == 0){
									heap[heapIndex]->tokenName = (char *)malloc(2*sizeof(char));
									char* newLine = (char*) malloc(2*sizeof(char));
									newLine[0] = input[i];
									newLine[1] = '\0';
					
									int occ = 1;
									insert(&newLine, &occ, &heap, &heapIndex, &heapSize);
									siftDownUpdate(&heap, &heapIndex, heapIndex-1);
								}	
							}			
						}

						// Remove the previous token from and restart the index counter
						token = malloc(sizeof(char));
						stringSize = sizeof(char);
						indexNum = 0;
					} else { // This is the only case where we continue making the token
						if(indexNum >= stringSize-1){
							stringSize *= 2;
							token = realloc(token, sizeof(char)*stringSize);
							if(token == NULL){
								printf("Something went wrong with allocating\n");
								return;
							}
						}
						token[indexNum] = input[i];
						//printf("Here is the string now %s\n", token);
						++indexNum;
					}	
				}
			
				//printf("made it here lol\n\n");	
				
				//int q;
				//for(q = 0; q < heapIndex; q++){
				//	printf("Here is the tokenName: %s, and here is the freq: %d\n", heap[q]->tokenName, heap[q]->frequency);
				//}
	
				heapSizeC = heapIndex;				

				// HUFFMAN
				int heapItems = heapIndex;
				int heapSize2 = heapIndex;
				while(heapItems != 0){
					// Get two minimums
					struct heapNode* left = deleteMin(&heap, &heapIndex, &heapSize);
					struct heapNode* right = deleteMin(&heap, &heapIndex, &heapSize);

					struct heapNode* tree = (struct heapNode*)malloc(sizeof(struct heapNode));
					tree->tokenName = NULL;

					int freq = (left->frequency) + (right->frequency);
					tree -> left = left;
					tree -> right = right;
					
					insertTree(&tree, &freq, &heap, &heapIndex, &heapSize);
					heapItems--;
				}

				// heap[0] is now the Huffman Tree
				//printf("\nHere is the Huffman Tree:\n");
				int arr[heapSize2], top = 0;	
				int fd = open("HuffmanCodebook", O_RDWR | O_CREAT | O_TRUNC, 0600);	
				int sz = write(fd, "\\\n", 2*sizeof(char));
				printCodes(heap[0], arr, top);
			}
			free(input);
		}
	} else if( strcmp(argv[1], "-c") == 0 ) {
		//printf("hello\n");
		// Check if the Codebook exists
		struct stat buffer;
		if( stat ("HuffmanCodebook", &buffer) != 0){
			printf("Error: No Codebook to compress with\n");
			exit(1);
		}

		// Build a Heap from the Codebook
		struct heapNodeC** heapC = buildHeap(&heapSizeC, &maxHuffman);
		if(heapSizeC <= 0){
			printf("Error with Codebook\n");
			return;
		}

		// In the case of a nonrecursive build, our input would look like
		// ./fileCompressor -c <fileName> |codebook|
		int fh, rd, n;
		fh = open(argv[2], O_RDONLY);
		if(fh == -1){
			printf("ERROR with %s\n", argv[2]);
			perror("open");
			return;
		} else {
			struct stat v;
			stat(argv[2], &v);
			int size = v.st_size;
			// Create a dynamic array equal to file size
			char *input = malloc(size * sizeof(char));

			// Read data from file and copy into dynamic array and print
			int ret = read(fh, input, size);
			if(ret == -1){
				printf("ERROR with %s\n", argv[2]);
				perror("read");
				return;
			} else {
				//printf("data read from file %s is: \n", argv[2]);
				//tokenize(p, &heap, &heapIndex, &heapSize);
				//printf("%s\n", p);
				char *token = malloc(sizeof(char)); // String that will be used to hold our token(s)
				int i; // Loop Counter
				int indexNum = 0; // Used to count the size of a single token, reset after each token
				int stringSize = sizeof(char); // Used as a tracker for the size of a string
				
				int t;
				char* given = argv[2];
				int newFileNameSize = strlen(given);
				char* newFileName = malloc( (5+strlen(argv[2]))*sizeof(char) ); // New File Name will be 5+argv[2] because we're adding '.' , 'h' , 'c' , 'z' , '\0'
				newFileName = given;
				newFileName[newFileNameSize] = '.';
				newFileName[++newFileNameSize] = 'h';
				newFileName[++newFileNameSize] = 'c';
				newFileName[++newFileNameSize] = 'z';
				newFileName[++newFileNameSize] = '\0';
				//printf("NewFileName '%s'\n", newFileName);
				
				// fd creates the file and also replaces it. wd makes it so we can only append to the file after we create it
				int fd = open(newFileName, O_RDWR | O_CREAT | O_TRUNC, 0600); // <===========================================================================
				int wd = open(newFileName, O_RDWR | O_CREAT | O_APPEND, 0600);				

				for(i = 0; i < strlen(input); ++i){
					if( ((int)input[i] <= (char)32) || ((int)input[i] == (char)172) ) {
						//printf("THis is the delimiter '%c' and this is the token '%s'\n", input[i], token);
						if(indexNum == 0){ // If token is empty, we only search the heap for the delimiter only
							int y;
							int foundEscape = 0;
							for(y = 0; y < heapSizeC; y++){
								if( (int)(((heapC[y])->word)[0]) == (int)(input[i]) ){
								//if(strcmp(token, heapC[y]->word) == 0){
									foundEscape = 1;
									// WRITE TO OUTPUT FILE
									//printf("1Here is the escape code I'm printing '%c'\n", input[i]);
									int size = strlen( heapC[y]->bitString );
									int ez = write(wd, heapC[y]->bitString, size);
								} 
							}

							if(foundEscape == 0){
								printf("1Error: This file contains words that aren't in the codebook!\n");
								remove(newFileName);
								exit(1);
							}
						} else { // SOMETHING is in our token, so we have to search the heap for the token AND delimiter
							int y;
							int foundToken = 0;
							int foundEscape = 0;
							
							// Loop to search for the token
							for(y = 0; y < heapSizeC; y++){
								if(strcmp(token, heapC[y]->word) == 0){
									foundToken = 1;
									// WRITE TO OUTPUT FILE
									//printf("3Here is the token I'm printing '%s'\n", token);
									int size = strlen( heapC[y]->bitString );
									int ez = write(wd, heapC[y]->bitString, size);
								} 
							}

							// Loop to search for the delimiter
							for(y = 0; y < heapSizeC; y++){
								if( (int)(((heapC[y])->word)[0]) == (int)(input[i]) ){
								//if(strcmp(token, heapC[y]->word) == 0){
									foundEscape = 1;
									// WRITE TO OUTPUT FILE
									//printf("2Here is the escape code I'm printing '%c'\n", input[i]); 
									int size = strlen( heapC[y]->bitString );
									int ez = write(wd, heapC[y]->bitString, size);
								} 
							}
						
							// If either were not in the codebook, exit with error
							if(foundToken == 0){
								printf("Token: '%s' 2Error: This file contains words that aren't in the codebook!\n", token);
								remove(newFileName);
								exit(1);
							}
							if(foundEscape == 0){
								printf("3Error: This file contains words that aren't in the codebook!\n");
								remove(newFileName);
								exit(1);
							}
						}

						token = malloc(sizeof(char));
						stringSize = sizeof(char);
						indexNum = 0;
					} else { // This is the only case where we continue making the token
						if(indexNum >= stringSize-1){
							stringSize *= 2;
							token = realloc(token, sizeof(char)*stringSize);
							if(token == NULL){
								printf("Something went wrong with allocating\n");
								return;
							}
						}
						token[indexNum] = input[i];
						//printf("Here is the string now %s\n", token);
						++indexNum;
					}
				}
			}
		}	
	} else if( strcmp(argv[1], "-d") == 0){
		// Check if the file exists
		struct stat buffer;
		if( stat (argv[2], &buffer) != 0){
			printf("Error: File doesn't exist!\n");
			exit(1);
		}
		
		// Check to see if the file is a .hcz file
		int length = strlen(argv[2]) - 1;
		if(argv[2][length] != 'z' && argv[2][--length] != 'c' && argv[2][--length] != 'h' && argv[2][--length] != '.'){
			printf("Not a .hcz file!\n");
			return;
		}
		
		// Build a Heap from the Codebook
		struct heapNodeC** heapC = buildHeap(&heapSizeC, &maxHuffman);
		if(heapSizeC <= 0){
			printf("Error with Codebook\n");
			return;
		}

		// In the case of a nonrecursive build, our input would look like
		// ./fileCompressor -d <fileName> |codebook|
		int fh, rd, n;
		fh = open(argv[2], O_RDONLY);
		if(fh == -1){
			printf("ERROR with %s\n", argv[2]);
			perror("open");
			return;
		} else {
			struct stat v;
			stat(argv[2], &v);
			int size = v.st_size;
			// Create a dynamic array equal to file size
			char *input = malloc(size * sizeof(char));

			// Read data from file and copy into dynamic array and print
			int ret = read(fh, input, size);
			if(ret == -1){
				printf("ERROR with %s\n", argv[2]);
				perror("read");
				return;
			} else {
				//printf("data read from file %s is: \n", argv[2]);
				//tokenize(p, &heap, &heapIndex, &heapSize);
				//printf("%s\n", p);
				char *token = malloc(sizeof(char)); // String that will be used to hold our token(s)
				int i; // Loop Counter
				int indexNum = 0; // Used to count the size of a single token, reset after each token
				int stringSize = sizeof(char); // Used as a tracker for the size of a string
				
				int t;
				char* given = argv[2];
				int newFileNameSize = strlen(given) - 4;
				char* newFileName = given;
				newFileName[newFileNameSize] = '\0';
				//printf("NewFileName '%s'\n", newFileName);

				// fd creates the file and also replaces it. wd makes it so we can only append to the file after we create it
				int fd = open(newFileName, O_RDWR | O_CREAT | O_TRUNC, 0600); // <===========================================================================
				int wd = open(newFileName, O_RDWR | O_CREAT | O_APPEND, 0600);				

				for(i = 0; i < strlen(input); ++i){
					// In decompression, we're always building the token, so no if else statement needed
					if(indexNum >= stringSize-1){
						stringSize *= 2;
						token = realloc(token, sizeof(char)*stringSize);
						if(token == NULL){
							printf("Something went wrong with allocating\n");
							return;
						}
					}
					token[indexNum] = input[i];
					//printf("Here is the string now %s\n", token);
					++indexNum;

					if(indexNum > maxHuffman){
						printf("Error: This file contains encoding which isn't in the codebook!\n");
						remove(newFileName);
						exit(1);
					}

					int y;
					int foundWord = 0;
					for(y = 0; y < heapSizeC; y++){
						if( strcmp(token, heapC[y]->bitString) == 0 ){
							foundWord = 1;
							// WRITE TO OUTPUT FILE
							int size = strlen( heapC[y]->word );
							int ez = write(wd, heapC[y]->word, size);
							
							// Now that we've added, we can reset our token to prepare for the next one
							token = malloc(sizeof(char));
							stringSize = sizeof(char);
							indexNum = 0;
						}
					}

				}
			}
		}	

	}
    
    
    	return 0;
}

void printArr(int arr[], int n, char* tokenName){
	int i;
	int fd = open("HuffmanCodebook", O_RDWR | O_CREAT | O_APPEND, 0600); // Code to open HuffmanCodebook. O_RDWR lets us read and write, O_CREAT will create the file if it 
									     // doesn't exist (this shouldn't happen), O_APPEND makes sure we don't overwrite anything
	char* code = (char*) malloc(n*sizeof(char)); // code is to store the Huffman Encoding

	// Exit if something went wrong with our open
	if(fd < 0){
		perror("r1");
		exit(1);
	}
	// Fill up the code string with the proper Huffman Encoding
	for(i = 0; i < n; ++i){
		code[i] = arr[i] + '0';
		//printf("%d", arr[i]);
	}

	// As per the formatting, we first write the Huffman Encoding, a tab, and then the word
	int sz = write(fd, code, n); // Write the Huffman Encoding
	int bz = write(fd, "\t", sizeof(char)); // Write the tab
	
	// If we need to write a control code, the cases change
	if(tokenName[0] == ' '){
		int iz = write(fd, "\\32", 3*sizeof(char));
	} else if(iscntrl(tokenName[0]) == 0){
		int wz = write(fd, tokenName, strlen(tokenName)); // If not a control code, just write the word
	}  else {
		// If it is a control code, we have to add our escape value. This part still needs to be tweaked a little
		int tokenNameInt = (int)(tokenName[0]);
		//printf("This is tokenName: '%c' and Int: %d\n", tokenName[0], tokenNameInt);
		char *tokenNameASCII = (char *)malloc(2*sizeof(char));
		tokenNameASCII[1] = '\0';
		tokenNameASCII[2] = '\0';
		sprintf(tokenNameASCII, "%d", tokenNameInt);
		//printf("This istokenNameASCII: '%s'\n", tokenNameASCII);	
		
		int pz = write(fd, "\\", sizeof(char));	
		if( (tokenNameASCII[1] >= '0' && tokenNameASCII[1] <= '9') ){
			int iz = write(fd, tokenNameASCII, 2*sizeof(char));
		} else {
			int iz = write(fd, tokenNameASCII, 1*sizeof(char));	
		}
	}
	int nz = write(fd, "\n", sizeof(char)); // Write a new line so in our next iteration we can add more
	//printf("\n");
}

void printCodes(struct heapNode* root, int arr[], int top){
	if(root->left){
		arr[top] = 0;
		printCodes(root->left, arr, top+1);
	}
	if(root->right){
		arr[top] = 1;
		printCodes(root->right, arr, top+1);
	}

	if( !(root->left) && !(root->right) ){
		//printf("%s: ", root->tokenName);
		printArr(arr, top, root->tokenName);
	}
}
