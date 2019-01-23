/*Version 3.2*/

/*
Version Update(3.2.4): Error handling for files
*/

#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#define MC 256
#define INTER '*'

struct node{
  char c;
  long long int freq;
  struct node *left,*right;
};

typedef struct node node;

int checkFile(int argc,char *argv[]);
void getFreqCount(char* ,long long int* );
node* makeHuffmanTree(long long int* );
int setLeaves(long long int* ,node** );
void shiftArr(node** ,int );
node* newNode(char ,long long int ,node* ,node* );
node* getMinNode(node** ,node** ,int* ,int*);
void sortNodes(node** ,int );
void decompress(char* ,node* );
int extractBit(FILE* );
char* changeFileName(char* ,char* );
long long int fileDetails(char* );

int main(int argc,char *argv[]){
	if (checkFile(argc,argv))return 1;

	char *fileName=argv[1];
	long long int freq[MC]={0};

	getFreqCount(fileName,freq);

  node *start=makeHuffmanTree(freq);

	decompress(fileName,start);
	return 0;
}

char* changeFileName(char *source,char* change){
	char *dest=malloc((strlen(source)+10)*sizeof(char));
	int i=0;

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

int checkFile(int argc,char *argv[]){
	if (argc==1){
		printf("Enter file to decompress as command line argument!\n");
		return 1;
	}

	char *fileName=argv[1];

	if (strlen(fileName)<5 || strcmp(fileName+strlen(fileName)-4,".hfc")!=0){
		printf("Invalid File Name\n");
		return 1;
	}

	int i;
	FILE *temp2=fopen(changeFileName(fileName,".meta"),"rb");
	FILE *temp1=fopen(fileName,"r");

	if(temp1==NULL){
		printf("Compressed File Missing\n");
		return 1;
	}
	if(temp2==NULL){
		printf("Meta File Missing\n");
		return 1;
	}
	fclose(temp1);
	fclose(temp2);
	return 0;
}

void getFreqCount(char* source,long long int freq[]){
	struct{
		char ch;
		long long int freq;
	}s;

	char *dest=changeFileName(source,".meta");
	FILE *meta=fopen(dest,"rb");

	while(!feof(meta)){
			fread(&s,sizeof(s),1,meta);
			freq[s.ch]=s.freq;
	}
	fclose(meta);
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

void decompress(char *source,node* start){     /*Decompresses the compressed string*/
  node* temp=start;
	char *destination=changeFileName(source,"_decmprsd.txt");
	FILE *dest=fopen(destination,"w");
	FILE *src=fopen(source,"r");
  int l=0;

  while(1){
    if(extractBit(src)){
      temp=temp->right;
    }
    else{
      temp=temp->left;
    }
    if(!temp->left && !temp->right){
			if(temp->c==0){
				break;
			}
			putc(temp->c,dest);
      temp=start;
    }
  }
	fclose(dest);
	fclose(src);

  fileDetails(source);
	fileDetails(destination);
}

int extractBit(FILE* src){      /*Extracts a bit from the given string*/
  static int n=0;

	fseek(src,n/8,SEEK_SET);
  char c=getc(src);

  if(c&(1<<(7-((n++)%8)))){
    return 1;
  }
  return 0;
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

void sortNodes(node** arr,int length){     /*Sorts the nodes using bubblesort*/
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

node* newNode(char a,long long int f,node* left,node* right){     /*dynamically allocates memory for new node*/
  node* newn=(node*)malloc(sizeof(node));

  newn->c=a;
  newn->freq=f;
  newn->left=left;
  newn->right=right;
  return newn;
}
