/*Version 3.2*/

/*
Version 1: Huffman compression using strings for storage
					 text files can contain only letters

Version Update(2.1.1): Leaves created only for those characters whose freq is gretater than 0

Version Update(3.1.1): Compatible with entire character set

Version Update(3.2.1): Nodes created using ASCII values
Version Update(3.2.2): File handling
Version Update(3.2.3): Seperate files for compression and decompression
Version Update(3.2.4): Error handling for files
*/

#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#define MC 256
#define INTER '*'

/*EOF is stored as 1st character in freq array*/

struct node{
  char c;
  long long int freq;
  struct node *left,*right;
};

enum insertionMode{
	normalMode,
	flushMode
};

typedef struct node node;
typedef enum insertionMode insertionMode;

int checkFile(int ,char **);
void getFreqCount(long long int* ,char* );
void createMetaFile(long long int* ,char* );
node* makeHuffmanTree(long long int* );
int setLeaves(long long int* ,node** );
node* newNode(char ,long long int ,node* ,node* );
void sortNodes(node** ,int );
node* getMinNode(node** ,node** ,int* ,int*);
void shiftArr(node** ,int );
char* changeFileName(char * ,char* );
void getHuffmanCodes(char [][100] ,node*,int );
void compress(char [][100] ,char* );
void insertBit(int ,FILE* ,insertionMode );
long long int fileDetails(char* );

int main(int argc,char *argv[]){
	if(checkFile(argc,argv)) return 1;

	char *fileName=argv[1];
  long long int freq[MC]={0};

  getFreqCount(freq,fileName);
	createMetaFile(freq,fileName);

  node *start=makeHuffmanTree(freq);
  char codes[MC][100];

  getHuffmanCodes(codes,start,0);
	compress(codes,fileName);
	return 0;
}

int checkFile(int argc,char *argv[]){
	if (argc==1){
		printf("Enter file to compress as command line argument!\n");
		return 1;
	}

	char *fileName=argv[1];

	if (strlen(fileName)<5 || strcmp(fileName+strlen(fileName)-4,".txt")!=0){
		printf("Invalid File Name\n");
		return 1;
	}

	FILE *temp=fopen(fileName,"r");

	if(temp==NULL){
		printf("File missing\n");
		return 1;
	}
	fclose(temp);
	return 0;
}

void createMetaFile(long long int freq[],char* source){
	struct{
		char ch;
		long long int freq;
	}s;

	int i;
	FILE *meta=fopen(changeFileName(source,".meta"),"wb");

	for(i=0;i<MC;++i){
		if(freq[i]!=0){
			s.freq=freq[i];
			s.ch=i;
			fwrite(&s,sizeof(s),1,meta);
		}
	}
	fclose(meta);
}

void getFreqCount(long long int freq[],char *source){     /*Creates the freq distribution table*/
	FILE *src=fopen(source,"r");
	char ch;

	while(1){
		ch=getc(src);
		if(ch==EOF){
			break;
		}
		freq[ch]+=1;
	}
	freq[0]+=1;

	fclose(src);
}

node* makeHuffmanTree(long long int freq[]){      /*Creates the binary tree needed for huffman coding and returns a pointer to the root*/
  node *arr1[MC],*arr2[MC],*min1,*min2;
  int l1,l2=0;

  l1=setLeaves(freq,arr1);
  sortNodes(arr1,l1);

  while(l1+l2>1){
    min1=getMinNode(arr1,arr2,&l1,&l2);
    min2=getMinNode(arr1,arr2,&l1,&l2);
    shiftArr(arr2,l2);
    l2++;
    arr2[0]=newNode(INTER,min1->freq+min2->freq,min1,min2);
  }
  return getMinNode(arr1,arr2,&l1,&l2);
}

node* getMinNode(node **arr1,node** arr2,int *l1,int *l2){      /*Returns pointer to the node having minimum most freq among arr1 and arr2*/
  if(!(*l1)){
    return arr2[--(*l2)];
  }
  if(!(*l2)){
    return arr1[--(*l1)];
  }
  if(arr1[(*l1)-1]->freq<=arr2[(*l2)-1]->freq){
    return arr1[--(*l1)];
  }
  return arr2[--(*l2)];
}

void shiftArr(node **arr,int n){     /*Shifts all nodes in array by 1 to the right*/
  int i;

  for(i=n;i>0;i--){
    arr[i]=arr[i-1];
  }
}

int setLeaves(long long int freq[],node* arr[]){      /*creates nodes for all char having freq>0*/
  int i,length=0;     //length stores number of leaves

  for(i=0;i<MC;++i){
    if(freq[i]){
      arr[length]=newNode(i,freq[i],NULL,NULL);
      length++;
    }
  }
  return length;      //returning the number of leaves
}

node* newNode(char a,long long int f,node* left,node* right){     /*dynamically allocates memory for new node*/
  node* newn=(node*)malloc(sizeof(node));

  newn->c=a;
  newn->freq=f;
  newn->left=left;
  newn->right=right;
  return newn;
}

void sortNodes(node** arr,int length){     /*Sorts the nodes using buublesort*/
  int i,j;

  for(i=0;i<length-1;i++){
    for(j=i+1;j<length;j++){
      if(arr[i]->freq < arr[j]->freq){
        node *temp=arr[i];
        arr[i]=arr[j];
        arr[j]=temp;
      }
    }
  }
}

void getHuffmanCodes(char codes[][100],node* nd,int depth){      /*Returns array of strings for the bit codes for each char*/
   static char arr[100];

   if(nd->left){
     arr[depth]='0';
     getHuffmanCodes(codes,nd->left,depth+1);
   }
   if(nd->right){
     arr[depth]='1';
     getHuffmanCodes(codes,nd->right,depth+1);
   }
   if (!nd->left&&!nd->right){
     int i;
     for(i=0;i<depth;i++){
       codes[nd->c][i]=arr[i];
     }
     codes[nd->c][depth]='\0';
   }
}

char* changeFileName(char *source,char* change){
	char *dest=malloc((strlen(source)+10)*sizeof(char));
	int i;

	for(i=0;source[i]!='.';i++){
		dest[i]=source[i];
	}
	dest[i]='\0';
	return strcat(dest,change);
}

long long int fileDetails(char *name){
		FILE* f=fopen(name,"r");

		fseek(f,0,SEEK_END);
		long long int len = ftell(f);
		fclose(f);

		printf("%s : %lld Bytes\n",name,len);
		return len;
}

void compress(char codes[][100],char* source){     /*Uses the codes to compress the string*/
  int i,j,t;
	char ch;
	char *dest=changeFileName(source,".hfc");
	FILE *cmprs=fopen(dest,"w");
	FILE *src=fopen(source,"r");

  while(1){
		ch=getc(src);
		if(ch==EOF){
			break;
		}
    for(j=0;codes[ch][j];++j){
      if(codes[ch][j]=='0'){
        insertBit(0,cmprs,normalMode);
      }
      else{
        insertBit(1,cmprs,normalMode);
      }
    }
  }
  for(j=0;codes[0][j];++j){
    if(codes[0][j]=='0'){
      insertBit(0,cmprs,normalMode);
    }
    else{
      insertBit(1,cmprs,normalMode);
    }
  }

  insertBit(0,cmprs,flushMode);
	fclose(cmprs);
	fclose(src);

	long long int len1=fileDetails(source);
	fileDetails(changeFileName(source,".meta"));
	long long int len2=fileDetails(dest);
	printf("compression Ratio: %2.2f %%\n",(len1-len2)*100.0/len1);
}

void insertBit(int bit,FILE *cmprs,insertionMode mode){      /*Inserts a bit into a string, mode 0 indicates insertion of a single bit while mode 1 indicates a flush*/
  static int n=0;
  static char c=0;

  if(mode&&n%8){
    putc(c<<(7-(n%8)),cmprs);
  }
  c+=bit;
  n++;
  if(!(n%8)){
    putc(c,cmprs);
    c=0;
  }
  c<<=1;
}
