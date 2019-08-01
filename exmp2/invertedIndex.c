#include<stdio.h>
#include<err.h>
#include<stdlib.h>
#include<ctype.h>
#include<sysexits.h>
#include<string.h>
#include<math.h>
#include "invertedIndex.h"


void splitlist(TfIdfList head,TfIdfList *a, TfIdfList *b){
    TfIdfList front;
    TfIdfList back;
    back  = head;
    front = head->next;

    while(front!=NULL){
        front = front->next;
        if (front != NULL){
            back = back->next;
            front = front->next;
        }
    }

    //now back is the midpoint
    *a = head;
    *b = back->next;
    back->next = NULL;
}


TfIdfList sortedList(TfIdfList a, TfIdfList b){
    TfIdfList result;
    //base case
    if(a==NULL){
        return b;
    }
    if(b==NULL){
        return a;
    }
    if(a->tfidf_sum > b->tfidf_sum){
        result = a;
        result->next = sortedList(a->next,b);
    }else if (a->tfidf_sum < b->tfidf_sum){
        result= b;
        result->next = sortedList(a,b->next);
    }else{
        if(strcmp(a->filename,b->filename)>0){
            result = b;
            result->next = sortedList(a,b->next);
        }else{
            result = a;
            result->next = sortedList(a->next,b);
        }
    }

    return result;
}



TfIdfList reorderList(TfIdfList result){
    //merge sort
    TfIdfList head =result;
    TfIdfList a  = NULL;
    TfIdfList b = NULL;

    

    if (head ==NULL || head->next ==NULL){
        return result;
    }
    splitlist(head,&a,&b);
    reorderList(a);
    reorderList(b);
    head = sortedList(a,b);
    return head;

}



TfIdfList newLIST(char *str){
    TfIdfList new = malloc(sizeof(*new));
    if (new==NULL) err(EX_OSERR,"couldn't allocate");
    new->filename = strdup(str);
    new->tfidf_sum = 0;
    new->next = NULL;

    return new;
}

double countidf(FileList file,char *str,double D){
    double num = 0;
    double result = 0;
    FileList curr = file;
    while (curr!=NULL){
        num++;
        curr = curr->next;
    }
    result = log10(D/num);
    return result;
}

double counttf(FileList file,char *str){
    double num =0 ;
    double total = 0;
    char word[100];
    FILE *fp;
    fp = fopen(file->filename,"r");
    while(fscanf(fp,"%s",word)!=EOF){
        normaliseWord(word);
        //printf("(%s)\n",word);
        if (strcmp(word,str)==0){
            num++;
        }
        total++;
    }

    return (num/total);
}


double calcuationtfidf(FileList loc,FileList curr_loc,char *str,double D){
    
    double result = 0;
    double tf =0 ;
    double idf=0;
    //FileList curr = loc;
    idf = countidf(loc,str,D);
    //loop for everyfile

    //count for the number of word
    tf = counttf(curr_loc,str);

    result =tf*idf;
    

    //printf("%s,%f",loc->filename,result);
    return result;
    

}


int txtDetect(char * str){
    int result=0;
    if (str[strlen(str)-4] == '.') return 1;
    return result;
}

int checkBST(InvertedIndexBST beginning,char * str){
    int result = 0;
    InvertedIndexBST curr = beginning;
    while(curr!=NULL){
        if(strcmp(curr->word,str)==0){
            result = 1;
            return result;
        }
        curr = curr->right;
    }
    return result;
}
InvertedIndexBST locateBST(InvertedIndexBST beginning,char * str){
    InvertedIndexBST result;
    InvertedIndexBST curr = beginning;
    while(curr!=NULL){
        if(strcmp(curr->word,str)==0){
            result = curr;
            return result;
        }
        curr = curr->right;
    }
    return NULL;
}

FileList newFile(char *str){
    FileList new = malloc(sizeof(*new));
    if (new==NULL) err(EX_OSERR,"couldn't allocate");
    new->filename = strdup(str);
    new->tf = 0.0;
    new->next = NULL;
    return new;
}

TfIdfList addList(TfIdfList start,TfIdfList add){
    TfIdfList curr = start;
    TfIdfList tmp;
    TfIdfList tmp1;
    //empty
    if (strcmp(curr->filename,add->filename)==0){
        start->tfidf_sum = add->tfidf_sum;
        return start;
    }
    tmp = newLIST(add->filename);
    tmp->tfidf_sum = add->tfidf_sum;
    // if two time is the same 
    if(curr->tfidf_sum==add->tfidf_sum){
        if(strcmp(curr->filename,add->filename)>0){
            tmp->next = curr;
            return tmp;
        }
        tmp1 = curr->next;
        curr->next = tmp;
        tmp->next = tmp1;
        return start;
    }


    //greater than first one
    if ((curr->tfidf_sum - add->tfidf_sum)<0){
        
        tmp->next = curr;
        return tmp;
    }
    //middle
    while(curr->next!=NULL){
        if((curr->next->tfidf_sum - add->tfidf_sum)<0){
            tmp1 = curr->next;
            curr->next = tmp;
            tmp->next = tmp1;
            return start;
        }

        curr = curr->next;
    }
    curr->next = tmp;
    return start;
}

void addFile(InvertedIndexBST location,FileList file ){
    FileList tmp;
    FileList tmp1;
    tmp = location->fileList;
    //filter existing one
    while(tmp!=NULL){
        if(strcmp(tmp->filename,file->filename)==0){
            return;
        }
        tmp = tmp->next;
    }

    tmp = location->fileList;
    //filter the existing one and order the sequence
    //if smaller than the first one
    if(strcmp(tmp->filename,file->filename)>0){
        location->fileList = file;
        file->next = tmp;
        return;
    }
    while(tmp->next!=NULL){
        

        if (strcmp(tmp->next->filename,file->filename)>0){
            tmp1 = tmp->next;
            tmp->next = file;
            file->next = tmp1;
            return;
        }
        tmp = tmp->next;
    }
    tmp->next = file;
}
InvertedIndexBST newBST (char *str){
    InvertedIndexBST new = malloc(sizeof(*new));
    if (new==NULL) err(EX_OSERR,"couldn't allocate");
    new->word = strdup(str);
    new->fileList = NULL;
    new->right = NULL;
    new->left = NULL;
    return new;
}


InvertedIndexBST addBST (InvertedIndexBST beginning,InvertedIndexBST name){
   
    InvertedIndexBST tmp;
    //if it's the begining
    if(strcmp(beginning->word ,"")==0){
        beginning = name;
        
        //printf("|%s|",beginning->word);
        return beginning;
    }
  
    
    
    //only one there
    if(beginning->right==NULL){

        beginning->right = name;
        name->left = beginning;

        return beginning;

    }
    
    //if str is greater than the first one
    if (strcmp(beginning->word,name->word)>0){
        beginning->left = name;
        beginning->left->right = beginning;
        return beginning->left;
    }
    InvertedIndexBST curr = beginning;
    //find the position just bigger than that
    
    while (curr->right!=NULL){ //not the last one
        if(strcmp(curr->right->word,name->word)>0){
            break;
        }
        curr = curr->right;
    }
    //stop before the bigger one
    //concren about the condition of first / middle /last
    //if curr is the last
    if(curr->right==NULL){
        //printf("3");
        curr->right = name;
        curr->right->left = curr;
        
        return beginning;
    }
    

    tmp = curr->right;
    curr->right =name ;
    curr->right->left = curr;
    curr->right->right = tmp;
    tmp->left = curr->right;
    return beginning;
    

}

TfIdfList combineTfidf(TfIdfList result,TfIdfList tmp){
    //for first shoot
    TfIdfList curr =tmp;
    TfIdfList curr1 = result;
    int detect = 0;
    if(tmp == result){
        return result;
    }
    while (curr!=NULL)
    {   
        detect = 0;
        curr1 = result;
        while(curr1!=NULL){
            //if have the same name add the tfidf value together
            if(strcmp(curr1->filename,curr->filename)==0){
                curr1->tfidf_sum = curr1->tfidf_sum + curr->tfidf_sum;
                detect = 1;
                break;
            }
            curr1 = curr1->next;
        }
        //if not part of the exsiting list, add it at last
        if (detect ==0){
            curr1 = result;
            while(curr1->next!=NULL){
                curr1 = curr1->next;
            }
            curr1->next = newLIST(curr->filename);
            curr1->next->tfidf_sum = curr->tfidf_sum;
        }

        curr = curr->next;
    }
    return result;

}

char * normaliseWord(char *str){
    
    for(int i = 0;str[i];i++){
        *(str+i)= tolower(str[i]);
        
    }
    if (str[strlen(str)-1] == '.' || str[strlen(str)-1] == ','||str[strlen(str)-1] == ';'||str[strlen(str)-1] == '?' ){
        *(str+strlen(str)-1)='\0';
    }
    return str;
}

InvertedIndexBST generateInvertedIndex(char *collectionFilename){

    FILE *fp;
    FILE *fp1;
    //InvertedIndexBST curr;
    InvertedIndexBST beginning;
    
    beginning = newBST("");
    
    
    char word[100];
    char filename[100];

    fp = fopen(collectionFilename,"r");
     while(fscanf(fp,"%s",filename)!=EOF){
        fp1 = fopen(filename,"r");
        while(fscanf(fp1,"%s",word)!=EOF){
            FileList file = newFile(filename);
            
            normaliseWord(word);
            //detect if exist
            
            if(checkBST(beginning,word)){
                //printf("exist the dulicate\n");
                InvertedIndexBST location;
                //give the position of the word
                location = locateBST(beginning,word);
                addFile(location,file);
                
            }else{

                InvertedIndexBST curr = newBST(word);
                beginning=addBST(beginning,curr);
                curr->fileList = file;
                //printf("<%s>\n",beginning->word);
            }


        }
        fclose(fp1);
        

    }
    fclose(fp);

    return beginning;
}
void printInvertedIndex(InvertedIndexBST tree){
    InvertedIndexBST copy = tree;
    FileList copy1;
    while(copy!=NULL){
        printf("%s ",copy->word);
        copy1 = copy->fileList;
        while(copy1!=NULL){
            printf("%s ",copy1->filename);
            copy1 = copy1->next;
        }
        printf("\n");
        copy = copy->right;
    }
}


TfIdfList calculateTfIdf(InvertedIndexBST tree, char *searchWord , int D){
    char *word;
    word = strdup(searchWord);
    normaliseWord(word);
    TfIdfList start;
    //locate the begining for one term
    if(locateBST(tree,word)==NULL){
        return NULL;
    }
    FileList loc_start = locateBST(tree,word)->fileList;
    // alternative for term start
    FileList loc = loc_start;
    //the list going to return
    start = newLIST(loc->filename);
    //the alternative of return list
    TfIdfList curr = start; //one for hold poistion
    //TfIdfList curr1 = start;
   while(loc!=NULL){
        
        //curr = newLIST(loc->filename);
        //how many word in this document
        
        curr->tfidf_sum = calcuationtfidf(loc_start,loc,word,(double)D);
        
        start = addList(start,curr);
       
        loc = loc->next; //go through all the file
        if(loc!=NULL){
            curr = newLIST(loc->filename);
        }
        //curr = curr->next;
        //run without sorting and ignore the case with two same
   }
   //start = reorderList(start);
   return start;
}

TfIdfList retrieve(InvertedIndexBST tree, char* searchWords[] , int D){
    int i = 0;
    TfIdfList  result;
    TfIdfList tmp;
    result = calculateTfIdf(tree,searchWords[0],D);
    tmp = result;
    while(searchWords[i]){
  
        if(tmp!=NULL){
            result = combineTfidf(result,tmp);
        }
        i++;
        if(searchWords[i]){
            tmp = calculateTfIdf(tree,searchWords[i],D);
        }
    }
    result = reorderList(result);
    // order the list after summation

    return result;
}
