// codebook heap
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

struct heapNodeC{
    char * bitString;
    char * word;
    int frequency;

};


void freeHeapC(struct heapNodeC** heap, int* heapSize);
void insertC(char** token, char** bits, int*occurence, struct heapNodeC*** heap, int* heapIndex);
void siftUpC(struct heapNodeC**** heap, int ** heapIndex);


struct heapNodeC** buildHeap(int* heapSize){
    
    int heapIndex = 0;

    struct heapNodeC** heap = (struct heapNodeC**)malloc((*heapSize) * sizeof(struct heapNodeC*));
    
    
    
    int q = 0;
    for(q = 0; q < (*heapSize); q++){
        heap[q] = (struct heapNodeC*)malloc(sizeof(struct heapNodeC));
    }




	int i;
	int fd = open("HuffmanCodebook", O_RDONLY); // Code to open HuffmanCodebook. O_RDONLY makes sure we only read

	// Exit if something went wrong with our open
	if(fd < 0){
		perror("r1");
		exit(1);
	}

	struct stat v;
	stat("HuffmanCodebook", &v);
	int size = v.st_size; // size now holds the max number of bytes in the codebook
	char *input = malloc(size * sizeof(char));
	int ret = read(fd, input, size);	
	int occurence = 1;
    char * bits;

	if(ret == -1){
		perror("read");
		exit(1);
	} else {
		char *token = malloc(sizeof(char));
		int i;
		int indexNum = 0;
		int stringSize = sizeof(char);

		printf("THis is the whole string '%s'\n", input);	
		for(i = 2; i < strlen(input); ++i){
			if(input[i] == '\t'){ // If we reached here, we've finished building our Huffman Encoding string
				token = realloc(token, sizeof(char)*indexNum+1);
				token[indexNum] = '\0';
				if(token == NULL){
					printf("Something went wrong with allocating\n");
					exit(1);
				}	

				// TOKEN NOW HOLDS HUFFMAN ENCODING STRING. INSERT INTO HEAP HERE <======================
				printf("This should be Huffman Encoding: '%s'\n", token);
                // insert into heap
                bits = (char*)malloc((strlen(token)+1) * sizeof(char));
                strcpy(bits, token);


				// Remove previous token and reset counter
				token = malloc(sizeof(char));
				stringSize = sizeof(char);				
				indexNum = 0;
			} else if(input[i] == '\n'){
				token = realloc(token, sizeof(char)*indexNum+1);
				token[indexNum] = '\0';

				if(token == NULL){
					printf("Something went wrong with allocating\n");
					exit(1);
				}	
				// Check if we have a control code in the string
				if(token[0] == '\\'){
					char* ASCII = &token[1]; //Start at token[1] to ignore the escape char
					int ASCIINum = atoi(ASCII);	
					char escape = (char)ASCIINum;
					token[0] = escape;
					token[1] = '\0';
				}
				
				// TOKEN NOW HOLDS THE CORRESPONDING STRING. INSERT INTO HEAP HERE <===========================
				char * tokenName = (char*)malloc((strlen(token)+1) * sizeof(char));
                strcpy(tokenName, token);

                insertC(&tokenName, &bits, &occurence, &heap, &heapIndex);
                
                
                printf("This should be the string: '%s'\n", token);

				token = malloc(sizeof(char));
				stringSize = sizeof(char);
				indexNum = 0;
			} else { // This is the case of building the token 
				if(indexNum >= stringSize-1){
					
					stringSize *= 2;
					token = realloc(token, sizeof(char)*stringSize);
					if(token == NULL){
						printf("Something went wrong with allocation\n");
						exit(1);
					}
				}
				//token[0] = 'a';	
				token[indexNum] = input[i];
				//printf("token[%d] = '%s', here's input[%d] = '%c'\n", indexNum, token, i, input[i]);
				++indexNum;
			
			}	
		}
	}


    return heap;
}







// This method takes in a size 
void printArrC(){
	int i;
	int fd = open("HuffmanCodebook", O_RDONLY); // Code to open HuffmanCodebook. O_RDONLY makes sure we only read

	// Exit if something went wrong with our open
	if(fd < 0){
		perror("r1");
		exit(1);
	}

	struct stat v;
	stat("HuffmanCodebook", &v);
	int size = v.st_size; // size now holds the max number of bytes in the codebook
	char *input = malloc(size * sizeof(char));
	int ret = read(fd, input, size);	
	
	if(ret == -1){
		perror("read");
		exit(1);
	} else {
		char *token = malloc(sizeof(char));
		int i;
		int indexNum = 0;
		int stringSize = sizeof(char);

		printf("THis is the whole string '%s'\n", input);	
		for(i = 2; i < strlen(input); ++i){
			if(input[i] == '\t'){ // If we reached here, we've finished building our Huffman Encoding string
				token = realloc(token, sizeof(char)*indexNum+1);
				token[indexNum] = '\0';
				if(token == NULL){
					printf("Something went wrong with allocating\n");
					exit(1);
				}	

				// TOKEN NOW HOLDS HUFFMAN ENCODING STRING. INSERT INTO HEAP HERE <======================
				printf("This should be Huffman Encoding: '%s'\n", token);

				// Remove previous token and reset counter
				token = malloc(sizeof(char));
				stringSize = sizeof(char);				
				indexNum = 0;
			} else if(input[i] == '\n'){
				token = realloc(token, sizeof(char)*indexNum+1);
				token[indexNum] = '\0';

				if(token == NULL){
					printf("Something went wrong with allocating\n");
					exit(1);
				}	
				// Check if we have a control code in the string
				if(token[0] == '\\'){
					char* ASCII = &token[1]; //Start at token[1] to ignore the escape char
					int ASCIINum = atoi(ASCII);	
					char escape = (char)ASCIINum;
					token[0] = escape;
					token[1] = '\0';
				}
				
				// TOKEN NOW HOLDS THE CORRESPONDING STRING. INSERT INTO HEAP HERE <===========================
				printf("This should be the string: '%s'\n", token);

				token = malloc(sizeof(char));
				stringSize = sizeof(char);
				indexNum = 0;
			} else { // This is the case of building the token 
				if(indexNum >= stringSize-1){
					
					stringSize *= 2;
					token = realloc(token, sizeof(char)*stringSize);
					if(token == NULL){
						printf("Something went wrong with allocation\n");
						exit(1);
					}
				}
				//token[0] = 'a';	
				token[indexNum] = input[i];
				//printf("token[%d] = '%s', here's input[%d] = '%c'\n", indexNum, token, i, input[i]);
				++indexNum;
			
			}	
		}
	}
}



void freeHeapC(struct heapNodeC** heap, int* heapSize){

    int i;

    for(i = 0; i< (*heapSize); i++){
        free(heap[i]->bitString);
    }

    for(i = 0; i < (*heapSize); i++){
        free(heap[i]->word);
    }

    for(i = 0; i < (*heapSize); i++){
        free(heap[i]);
    }
    free(heap);
}



void insertC(char ** token, char ** bits, int* occurence, struct heapNodeC*** heap, int* heapIndex){

    int i;


         struct heapNodeC* temp = (struct heapNodeC*)malloc(sizeof(struct heapNodeC));
        
         (*heap)[(*heapIndex)]->word = (char*)malloc((strlen(*token)+1) * sizeof(char));
         (*heap)[(*heapIndex)]->bitString = (char*)malloc((strlen(*bits)+1)*sizeof(char));
         
         temp -> word = *token;
        temp -> bitString = *bits;
	temp -> frequency = (*occurence);
    (*occurence)++;
        
         (*heap)[(*heapIndex)] = temp;         
        siftUpC(&heap, &heapIndex);
      
        (*heapIndex)++;
    
                
        }               


void siftUpC(struct heapNodeC**** heap, int **heapIndex){
    int i = 0;
    int k = **heapIndex;
    
    
    while(k > 0){ // 0 is index of the root
                    
        int p = (k-1)/2; // k's parent index number
                
        struct heapNodeC* parent; 
        parent = (**heap)[p];
        
        struct heapNodeC* child; 
        child = (**heap)[k];

  
	if( (child->frequency) < (parent -> frequency) ){
    	// swap them
    	    struct heapNodeC* temp = child;
	    (**heap)[k] = parent;
	    (**heap)[p] = temp;
	    k=p;                                                         
     	}else{
	     break;
        }

   }

}

