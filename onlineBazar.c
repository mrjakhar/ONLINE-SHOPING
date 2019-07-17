#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define M 5
typedef enum{FAILURE,SUCCESS} status_code;
int customer_count=0;
typedef struct product_tag
{
  int P_ID;
  char name[40];
  int cost;
  int items_available;
  int sold;
}product_type;

typedef struct history_tag
{
  int P_ID;
  char product_name[40];
  int qty;
  struct history_tag *next;
}history_type;


typedef struct customer_tag
{
  int C_ID;
  char name[30];
  char address[40];
  char email[50];
  char password[20];
  char contact[15];
  int cash_back;
  history_type *history_ptr;
  history_type *wish_ptr;
  int payment_history[7]; // 0-BNPL 1-COD 2-debit card 3-Credit card 4-Netbanking 5-UPI 6-Phone
  struct customer_tag *next;
}customer_type;

typedef struct customer_data_tag{
  customer_type* data[4];
  int no_of_data;
  struct customer_data_tag *next;
  struct customer_data_tag *prev;
}customer_data_node;

typedef struct customer_node_tag{
  int key[4];
  int no_of_keys;
  int pointingto;           //0 if its pointing to data_ptr and 1 if its pointing to another node and -1 if not pointing to anything
  union customer_pointer_tag{
    customer_data_node *data_ptr[5];
    struct customer_node_tag *next_ptr[5];
  }ptr;
}customer_node_type;

typedef struct product_data_tag{
  product_type* data[4];
  int no_of_data;
  struct product_data_tag *next;
  struct product_data_tag *prev;
}product_data_node;

typedef struct product_node_tag{
  int key[4];
  int no_of_keys;
  int pointingto;           //0 if its pointing to data_ptr and 1 if its pointing to another node and -1 if not pointing to anything
  union product_pointer_tag{
    product_data_node *data_ptr[5];
    struct product_node_tag *next_ptr[5];
  }ptr;
}product_node_type;

typedef struct product_category_tag{
  product_node_type *root;
}product_category;

product_category pc[6]; //0-mobile 1-mobile accessories 2-Laptop and Computer Accessories 3-Electronic gadgets 4-games and gaming accesories 5-home and kitchen appliances
history_type *global_history=NULL;

enum KeyStatus { Duplicate,SearchFailure,Success,InsertIt,LessKeys };

//*****************************************************************//
//                            PRODUCT                              //
//*****************************************************************//

product_data_node* initialise_product_dptr(product_data_node* dptr)
{
  dptr->no_of_data=0;
  dptr->next=NULL;
  dptr->prev=NULL;
}

product_data_node* insert_product_data(product_data_node* dptr,product_type* data)
{
  int i;
  i=0;
  while(i<dptr->no_of_data && dptr->data[i]->P_ID<(data->P_ID))
  {
    i++;
  }
  int j=dptr->no_of_data;
  while(i<j)
  {
    dptr->data[j]=dptr->data[j-1];
    j--;
  }
  dptr->data[j]=data;
  //dptr->no_of_data++;
  return dptr;
}

product_data_node* insert_product_double(product_data_node *dptr,product_data_node *ndptr,product_data_node *nndptr)
{
  if(nndptr==NULL)
  {
    ndptr->next=NULL;
    dptr->next=ndptr;
    ndptr->prev=dptr;
  }
  else
  {
    //printf("condition\n");
    nndptr->prev=ndptr;
    ndptr->next=nndptr;
    dptr->next=ndptr;
    ndptr->prev=dptr;
  }
}

int searchPos_product(int key, int *key_arr, int n)
{
    int pos=0;
    while (pos < n && key > key_arr[pos])
        pos++;
    return pos;
}

enum KeyStatus ins_product(product_node_type *ptr, int key, int *upKey,product_node_type
                   **newnode,product_node_type *nptr)
{
    product_node_type *newPtr, *lastPtr;
    int pos, i, n,splitPos;
    int newKey, lastKey;
    enum KeyStatus value;
    if (ptr->pointingto == 0)
    {
        *newnode = nptr;
        *upKey = key;
        return InsertIt;
    }
    n = ptr->no_of_keys;
    pos = searchPos_product(key, ptr->key, n);
    if (pos < n && key == ptr->key[pos])
        return Duplicate;
    value = ins_product(ptr->ptr.next_ptr[pos], key, &newKey, &newPtr,nptr);
    if (value != InsertIt)
        return value;
    /*If keys in node is less than M-1 where M is order of B tree*/
    if (n < M - 1)
    {
        pos = searchPos_product(newKey, ptr->key, n);
        /*Shifting the key and pointer right for inserting the new key*/
        for (i=n; i>pos; i--)
        {
            ptr->key[i] = ptr->key[i-1];
            ptr->ptr.next_ptr[i+1] = ptr->ptr.next_ptr[i];
        }
        /*Key is inserted at exact location*/
        ptr->key[pos] = newKey;
        ptr->ptr.next_ptr[pos+1] = newPtr;
        ++ptr->no_of_keys;
        ptr->pointingto=1; /*incrementing the number of keys in node*/
        return Success;
    }/*End of if */
    /*If keys in nodes are maximum and position of node to be inserted is
    last*/
    if (pos == M - 1)
    {
        lastKey = newKey;
        lastPtr = newPtr;
    }
    else /*If keys in node are maximum and position of node to be inserted
is not last*/
    {
        lastKey = ptr->key[M-2];
        lastPtr = ptr->ptr.next_ptr[M-1];
        for (i=M-2; i>pos; i--)
        {
            ptr->key[i] = ptr->key[i-1];
            ptr->ptr.next_ptr[i+1] = ptr->ptr.next_ptr[i];
        }
        ptr->key[pos] = newKey;
        ptr->ptr.next_ptr[pos+1] = newPtr;
    }
    splitPos = (M - 1)/2;
    (*upKey) = ptr->key[splitPos];

    (*newnode)=malloc(sizeof(product_node_type));/*Right node after split*/
    ptr->no_of_keys = splitPos; /*No. of keys for left splitted node*/
    (*newnode)->no_of_keys = M-1-splitPos;/*No. of keys for right splitted node*/
    for (i=0; i < (*newnode)->no_of_keys; i++)
    {
        (*newnode)->ptr.next_ptr[i] = ptr->ptr.next_ptr[i + splitPos + 1];
        if(i < (*newnode)->no_of_keys - 1)
            (*newnode)->key[i] = ptr->key[i + splitPos + 1];
        else
            (*newnode)->key[i] = lastKey;
    }
    (*newnode)->ptr.next_ptr[(*newnode)->no_of_keys] = lastPtr;
    (*newnode)->pointingto=1;
    return InsertIt;
}

product_node_type* Insert_product(product_node_type *root,product_type* data)
{
    if(root->no_of_keys==0)                             //no key in root node
    {
        if(root->ptr.data_ptr[0]==NULL)                    //no data present at all
        {
          //printf("inserting %d\n",data);
          product_data_node *dptr=(product_data_node*)malloc(sizeof(product_data_node));
          dptr->data[0]=data;
          dptr->no_of_data=1;
          dptr->next=NULL;
          dptr->prev=NULL;
          root->ptr.data_ptr[0]=dptr;
          root->pointingto=0;
        }
        else
        {
          product_data_node *dptr=root->ptr.data_ptr[0];
          if(dptr->no_of_data<4)                      //node has data<4 and no root key
          {
            //printf("inserting %d\n",data);
            dptr=insert_product_data(dptr,data);
            dptr->no_of_data++;
          }
          else                                          //node has data>4 need to create a key in root
          {
            //printf("inserting %d\n",data);
            int key=2;
            root->key[0]=data->P_ID;                            //finding the key value
            if(data->P_ID<dptr->data[1]->P_ID)
            {
              key=1;
              root->key[0]=dptr->data[1]->P_ID;
            }
            else if(data->P_ID>dptr->data[2]->P_ID)
            {
              key=2;
              root->key[0]=dptr->data[2]->P_ID;
            }
            root->no_of_keys++;
            product_data_node *ndptr=(product_data_node*)malloc(sizeof(product_data_node));             //product_data_node to split the prev data node
            ndptr=initialise_product_dptr(ndptr);
            int i;
            for(i=key;i<dptr->no_of_data;i++)                                    //copying value after the key node
            {
              ndptr->data[ndptr->no_of_data]=dptr->data[i];
              ndptr->no_of_data++;
            }
            if (key==1)
            {
              dptr=insert_product_data(dptr,data);
            }
            else
            {
              ndptr=insert_product_data(ndptr,data);
            }
            dptr->no_of_data=2;
            ndptr->no_of_data=3;
            dptr->next=ndptr;
            ndptr->prev=dptr;
            root->ptr.data_ptr[1]=ndptr;
            root->pointingto=0;
          }
        }
    }
    else                                          //atleast one key value is there in tree and two data nodes
    {

      product_node_type *ptr=root;
      int index,i,found=0;
      //search for ptr start
      while(ptr->pointingto!=0)
      {
        i=0;
        while(i<ptr->no_of_keys && found==0)
        {
          //printf("index2 in loop %d\n",index);
          if(data->P_ID<ptr->key[i])
          {
            index=i;
            found=1;
          }
          i++;
          //printf("index2 in loop %d\n",index);
        }
        if(found==0)
        {
          ptr=ptr->ptr.next_ptr[ptr->no_of_keys];
        }
        else
        {
          ptr=ptr->ptr.next_ptr[index];
        }
        //printf("index2 in loop %d\n",index);
      }
      int index2=0,found2=0;
      i=0;
      while(i<ptr->no_of_keys && found2==0)
      {
        if(data->P_ID<ptr->key[i])
        {
          index2=i;
          found2=1;
        }
        //printf("index2 in loop %d\n",index2);
        i++;
      }
      product_data_node *dptr;
      if(found2==0)
      {
        //printf("inserting %d\n",data);
        index2=ptr->no_of_keys;
      }
      dptr=ptr->ptr.data_ptr[index2];
      //search for ptr ends
      if(dptr->no_of_data<4)                      //node has data<4 and no root key
      {

        dptr=insert_product_data(dptr,data);
        dptr->no_of_data++;
      }
      else                                          //node has data>4 need to create a key in root
      {
        int key=2;
        int keyelement=data->P_ID;
        //ptr->key[no_of_keys]=data;                            //finding the key value
        if(data->P_ID<dptr->data[1]->P_ID)
        {
          key=1;
          keyelement=dptr->data[1]->P_ID;
          //ptr->key[no_of_keys]=dptr->data[1];
        }
        else if(data->P_ID>dptr->data[2]->P_ID)
        {
          key=2;
          keyelement=dptr->data[2]->P_ID;
          //ptr->key[no_of_keys]=dptr->data[2];
        }
        product_data_node *ndptr=(product_data_node*)malloc(sizeof(product_data_node));             //product_data_node to split the prev data node
        ndptr=initialise_product_dptr(ndptr);
        int i;
        //printf("inserting %d\n",data);
        for(i=key;i<dptr->no_of_data;i++)                                    //copying value after the key node
        {
          //printf("inserting %d\n",data);
          ndptr->data[ndptr->no_of_data]=dptr->data[i];
          ndptr->no_of_data++;
        }
        if (key==1)
        {
          dptr=insert_product_data(dptr,data);
        }
        else
        {
          //printf("inserting %d\n",data);
          ndptr=insert_product_data(ndptr,data);
        }
        dptr->no_of_data=2;
        ndptr->no_of_data=3;
        // dptr->next=ndptr;
        // ndptr->prev=dptr;
        if(ptr->no_of_keys<4)
        {
          //printf("inserting %d\n",data);
          //printf("keys= %d\n",ptr->no_of_keys);
          int i;
          for(i=ptr->no_of_keys-1; i>=index2 ; i--)
          {
            ptr->key[i+1]=ptr->key[i];
            ptr->ptr.data_ptr[i+2]=ptr->ptr.data_ptr[i+1];
          }
          ptr->key[index2]=keyelement;
          ptr->ptr.data_ptr[index2+1]=ndptr;
          ptr->no_of_keys++;
          // dptr->next=ndptr;
          // ndptr->prev=dptr;
          //printf("index2 = %d\n",index2);
          product_data_node *nndptr=dptr->next;
          ndptr=insert_product_double(dptr,ndptr,nndptr);
        }
        else
        {
          //printf("inserting %d\n",data);
          int keyelement;
          product_node_type* nptr;
          nptr=(product_node_type*)malloc(sizeof(product_node_type));
          nptr->no_of_keys=0;
          product_data_node *nndptr=dptr->next;
          ndptr=insert_product_double(dptr,ndptr,nndptr);
          if(index2==2)
          {
            //  printf("inserting %d\n",data);
            keyelement=data->P_ID;
            nptr->ptr.data_ptr[0]=ndptr;
            nptr->key[0]=ptr->key[2];
            nptr->ptr.data_ptr[1]=ptr->ptr.data_ptr[3];
            nptr->key[1]=ptr->key[3];
            nptr->ptr.data_ptr[2]=ptr->ptr.data_ptr[4];
            nptr->no_of_keys=2;
            ptr->no_of_keys=2;
            nptr->pointingto=0;
          }
          else if(index2<2)
          {
            //printf("inserting %d\n",data);
            keyelement=ptr->key[1];
            nptr->pointingto=0;
            nptr->ptr.data_ptr[0]=ptr->ptr.data_ptr[2];
            nptr->key[0]=ptr->key[2];
            nptr->ptr.data_ptr[1]=ptr->ptr.data_ptr[3];
            nptr->key[1]=ptr->key[3];
            nptr->ptr.data_ptr[2]=ptr->ptr.data_ptr[4];
            if(index2==0)
            {
              ptr->key[1]=ptr->key[0];
              ptr->ptr.data_ptr[2]=ptr->ptr.data_ptr[1];
              ptr->key[0]=data->P_ID;
              ptr->ptr.data_ptr[1]=ndptr;
            }
            else
            {
              //index2==1
              ptr->key[1]=ndptr->data[0]->P_ID;
              ptr->ptr.data_ptr[2]=ndptr;
            }
            nptr->no_of_keys=2;
            ptr->no_of_keys=2;
          }
          else
          {
            keyelement=ptr->key[2];
            nptr->pointingto=0;
            if(index2==3)
            {
              //printf("inserting %d\n",data);
              nptr->key[1]=ptr->key[3];
              nptr->key[0]=ndptr->data[0]->P_ID;
              nptr->ptr.data_ptr[0]=ptr->ptr.data_ptr[3];
              nptr->ptr.data_ptr[1]=ndptr;
              nptr->ptr.data_ptr[2]=ptr->ptr.data_ptr[4];
            }
            else
            {
              //index2==4
              nptr->key[1]=ndptr->data[0]->P_ID;
              nptr->ptr.data_ptr[1]=ndptr;
              nptr->key[0]=ptr->key[3];
              nptr->ptr.data_ptr[0]=ptr->ptr.data_ptr[3];
              nptr->ptr.data_ptr[1]=ptr->ptr.data_ptr[4];
              nptr->ptr.data_ptr[2]=ndptr;
            }
            nptr->no_of_keys=2;
            ptr->no_of_keys=2;
          }
          // dptr->next=ndptr;
          // ndptr->prev=dptr;

          //printf("key element = %d\n",keyelement);
          //printf("index2 = %d\n",index2);
          product_node_type *newnode;
          int upKey;
          enum KeyStatus value;
          value = ins_product(root, keyelement, &upKey, &newnode, nptr);
          if (value == Duplicate)
              printf("Key already available\n");
          if (value == InsertIt)
          {
              product_node_type *uproot = root;
              root=malloc(sizeof(product_node_type));
              root->no_of_keys = 1;
              root->key[0] = upKey;
              root->ptr.next_ptr[0] = uproot;
              root->ptr.next_ptr[1] = newnode;
              root->pointingto=1;
          }
          //root=insert_in_btree(root,&keyelement,ptr,nptr,index2);
        }
      }
    }
    //printf("inserting %d\n",data);
    return root;
}

void showproduct(product_type* nptr)
{
  printf("\t|| Product id : ");
  printf("%d\n",nptr->P_ID);
  printf("\t|| Name : ");
  printf("%s\n",nptr->name);
  printf("\t|| Price : ");
  printf("%d\n",nptr->cost);
  printf("\t|| available : ");
  printf("%d\n",nptr->items_available);
  printf("\t|| Sold : ");
  printf("%d\n",nptr->sold);
}

void display_product(product_node_type *ptr, int blanks)
{
    if (ptr->pointingto==1)
    {
        int i;
        for(i=1; i<=blanks; i++)
            printf(" ");
        for (i=0; i < ptr->no_of_keys; i++)
            printf("%d ",ptr->key[i]);
        printf("\n");
        for (i=0; i <= ptr->no_of_keys; i++)
            display_product(ptr->ptr.next_ptr[i], blanks+10);
    }
    else
    {
      int i,j;
      for(i=1; i<=blanks; i++)
          printf(" ");
      for (i=0; i < ptr->no_of_keys; i++)
          printf("%d ",ptr->key[i]);
      printf("\n");
      for (j=0; j <= ptr->no_of_keys; j++)
      {
        //printf("%d\n",ptr->no_of_keys);
        product_data_node* dptr=ptr->ptr.data_ptr[j];
      for(i=1; i<=blanks+10; i++)
          printf(" ");
      for (i=0; i < dptr->no_of_data; i++)
          //printf("innner i = %d\n",i);
          //printf("%d ",dptr->data[i]);
          showproduct(dptr->data[i]);
          // printf("%d",dptr->data[i]->C_ID);
          // printf("%s")
          printf("\n");
      }
      //    display_customer(ptr->ptr.next_ptr[i], blanks+10);
    }/*End of if*/
}/*End of display_customer()*/
void traverse_product(product_node_type *root)
{
  product_node_type *ptr=root;
  int i;
  while(ptr->pointingto!=0)
  {
    ptr=ptr->ptr.next_ptr[0];
  }
  product_data_node *lptr=ptr->ptr.data_ptr[0];
	product_data_node *nptr = lptr;
  printf("\t---------------------------------\n");
  printf("\t||      PRODUCTS AVAILABLE     ||\n");
  printf("\t---------------------------------\n");
	while(nptr != NULL)
	{
    for (i=0; i < nptr->no_of_data; i++)
    {
        //printf("innner i = %d\n",i);
        //showproduct(nptr->data[i]);
        printf("\t---------------------------------\n");
        printf("\t|| Product id : ");
        printf("%d\n",nptr->data[i]->P_ID);
        printf("\t|| Name : ");
        printf("%s\n",nptr->data[i]->name);
        printf("\t|| Price : ");
        printf("%d\n",nptr->data[i]->cost);
        //printf("\t---------------------------------\n");
    }
		nptr = nptr -> next;
	}
  printf("\t---------------------------------\n");
	printf("\n");
}

void traverse_all_product()
{
  product_node_type* ptr;
  product_type* nptr;
	int i,flag=1;
  for(i=0;i<6;i++)
  {
    ptr=pc[i].root;
    int j;
    while(ptr->pointingto!=0)
    {
      ptr=ptr->ptr.next_ptr[0];
    }
    product_data_node *lptr=ptr->ptr.data_ptr[0];
    while (lptr!=NULL)
  	{
      for(j=0;j<lptr->no_of_data;j++)
      {
        nptr=lptr->data[j];
        printf("\t---------------------------------\n");
        showproduct(nptr);
      }
  		lptr=lptr->next;
  	}
  }
  printf("\t---------------------------------\n");
}

product_type* search_product(product_node_type* root,int P_ID,status_code *sc)
{
  int i=0;
  int found=0;
  int index=0;
  product_type* ret_val;
  product_node_type *ptr=root;
  while(ptr->pointingto!=0)
  {
    i=0;
    while(i<ptr->no_of_keys && found==0)
    {
      //printf("index2 in loop %d\n",index);
      if(P_ID<ptr->key[i])
      {
        index=i;
        found=1;
      }
      i++;
      //printf("index2 in loop %d\n",index);
    }
    if(found==0)
    {
      ptr=ptr->ptr.next_ptr[ptr->no_of_keys];
    }
    else
    {
      ptr=ptr->ptr.next_ptr[index];
    }
    //printf("index2 in loop %d\n",index);
  }
  int index2=0,found2=0;
  i=0;
  while(i<ptr->no_of_keys && found2==0)
  {
    if(P_ID<ptr->key[i])
    {
      index2=i;
      found2=1;
    }
    //printf("index2 in loop %d\n",index2);
    i++;
  }
  product_data_node *dptr;
  if(found2==0)
  {
    //printf("inserting %d\n",data);
    index2=ptr->no_of_keys;
  }
  dptr=ptr->ptr.data_ptr[index2];
  i=0;
  int index3=0,found3=0;
  while(i<dptr->no_of_data && found3==0)
  {
    if(P_ID==dptr->data[i]->P_ID)
    {
      index3=i;
      found3=1;
    }
    i++;
  }
  if(found3==1)
  {
    ret_val=dptr->data[index3];
    *sc=SUCCESS;
  }
  else
  {
    ret_val=NULL;
    *sc=FAILURE;
  }
  return ret_val;
}

void createlist()
{
  product_type* ptr;
  FILE *fptr;
	int i,flag=1;
  product_node_type *root=NULL;
  root=(product_node_type*)malloc(sizeof(product_node_type));
  root->no_of_keys=0;
  root->ptr.data_ptr[0]=NULL;
  char str[100]="product_data_1.txt";
  for(i=0;i<6;i++)
  {
    root=(product_node_type*)malloc(sizeof(product_node_type));
    root->no_of_keys=0;
    root->ptr.data_ptr[0]=NULL;
    if(fptr=fopen(str,"r"))
    {
    	while (!feof (fptr))
    	{
        ptr=(product_type*)malloc(sizeof(product_type));
        fscanf(fptr,"%d ",&ptr->P_ID);
        printf("%d ",ptr->P_ID);
        fscanf(fptr,"%s ",ptr->name);

        fscanf(fptr,"%d ",&ptr->cost);
        fscanf(fptr,"%d ",&ptr->items_available);
        fscanf(fptr,"%d ",&ptr->sold);
        //showproduct(ptr);
    		root=Insert_product(root,ptr);
    	}
    }
    else
    {
      printf("not opened");
    }
    fclose(fptr);
    pc[i].root=root;
    str[13]+=1;
  }
}

void storelist(status_code *sc)
{
  product_node_type* ptr;
  product_type* nptr;
  FILE *fptr;
	int i,flag=1;
  char str[100]="product_data_1.txt";
  for(i=0;i<6;i++)
  {
    ptr=pc[i].root;
    int j;
    while(ptr->pointingto!=0)
    {
      ptr=ptr->ptr.next_ptr[0];
    }
    product_data_node *lptr=ptr->ptr.data_ptr[0];
    if(fptr=fopen(str,"w"))
    {
    	while (lptr!=NULL)
    	{
        for(j=0;j<lptr->no_of_data;j++)
        {
          nptr=lptr->data[j];
          fprintf(fptr,"%d\n",nptr->P_ID);
          fprintf(fptr,"%s\n",nptr->name);
          fprintf(fptr,"%d\n",nptr->cost);
          fprintf(fptr,"%d\n",nptr->items_available);
          fprintf(fptr,"%d\n",nptr->sold);
        }
    		lptr=lptr->next;
    	}
    }
    else
    {
      printf("not opened");
    }
    fclose(fptr);
    str[13]+=1;
  }
}
//*****************************************************************//
//                            History                              //
//*****************************************************************//

void traverse_history(history_type *list_ptr)
{
	history_type *ptr;
  ptr=list_ptr;
	if(list_ptr==NULL)
	{
		printf("\tNo Products\n");
	}
	else
	{
		while(ptr!=NULL)
		{
      printf("\tProduct Name : %s \n",ptr->product_name);
      printf("\tQuantity     : %d \n",ptr->qty);
      printf("\n");
			ptr=ptr->next;
		}
	}
}

history_type* search_history(char name[],history_type *list_ptr,status_code *sc)
{
	history_type *ptr,*prev,*ret_val=NULL;
  int found=0;
	if(list_ptr==NULL)
	{
		*sc=FAILURE;
  }
	else
	{
    ptr=list_ptr;
		while(ptr!=NULL && found==0)
		{
      prev=ptr;
      if(strcmp(name,ptr->product_name)==0)
      {
        found=1;
      }
			ptr=ptr->next;
		}
    if(found==1)
    {
      ret_val=prev;
      *sc=SUCCESS;
    }
	}
  return ret_val;
}

history_type* add_history(history_type *list_ptr, char prod_name[], int qty)
{

	history_type *nptr,*ptr;
  status_code sc;
  ptr=search_history(prod_name,list_ptr,&sc);
  if(sc==SUCCESS)
  {
    ptr->qty+=qty;
  }
  else
  {
    nptr=(history_type*)malloc(sizeof(history_type));
    strcpy(nptr->product_name,prod_name);
    nptr->qty=qty;
    nptr->next=NULL;
  	if(list_ptr==NULL)
  	{
  		list_ptr=nptr;
  	}
  	else
  	{
  		ptr=list_ptr;
  		while(ptr->next!=NULL)
  		{
  			ptr=ptr->next;
  		}
  		ptr->next=nptr;
  	}
  }
	return list_ptr;
}

history_type* delete_history(history_type *list_ptr, char prod_name[])
{
	history_type *nptr,*ptr,*prev;

  int found=0;
	if(list_ptr!=NULL)
	{
		if(list_ptr->next==NULL)
		{
      if(strcmp(list_ptr->product_name,prod_name)==0)
      {
  			free(list_ptr);
  			list_ptr=NULL;
      }
		}
		else
		{
      ptr=list_ptr;
      if(strcmp(ptr->product_name,prod_name)==0)
      {
        found=1;
        list_ptr=ptr->next;
        free(ptr);
      }
      else
      {
        while(ptr->next!=NULL && found==0)
			     {
            prev=ptr;
    				ptr=ptr->next;
            if(strcmp(ptr->product_name,prod_name)==0)
            {
              found=1;
            }
    			}
          if(found==1)
          {
            prev->next=ptr->next;
            free(ptr);
          }
      }
		}
	}
	return (list_ptr);
}

void read_history()
{
  FILE *fptr=fopen("global_history.txt","r");
	history_type ptr;

	while(!feof(fptr))
		{
      fscanf(fptr,"%s ",ptr.product_name);
      fscanf(fptr,"%d ",&ptr.qty);
      global_history=add_history(global_history,ptr.product_name,ptr.qty);
		}
  fclose(fptr);
}

void save_history(history_type *list_ptr, FILE *fptr)
{
	history_type *ptr;
  ptr=list_ptr;
	if(list_ptr!=NULL)
	{
		while(ptr!=NULL)
		{
      //fprintf(fptr,"%s\n",ptr->P_ID);
      fprintf(fptr,"%s\n",ptr->product_name);
      fprintf(fptr,"%d\n",ptr->qty);
			ptr=ptr->next;
		}
	}
}
//*****************************************************************//
//                            Customer                             //
//*****************************************************************//

customer_type* customer_intialise(customer_type* ptr)
{
  ptr->C_ID=-1;
  strcpy(ptr->name,"");
  strcpy(ptr->address,"");
  strcpy(ptr->email,"");
  strcpy(ptr->contact,"");
  ptr->wish_ptr=NULL;
  ptr->history_ptr=NULL;
  ptr->cash_back=0;
  int i=0;
  for(;i<7;i++)
  {
    ptr->payment_history[i]=0;
  }
  ptr->next=NULL;
  return ptr;
}

customer_data_node* initialise_customer_dptr(customer_data_node* dptr)
{
  dptr->no_of_data=0;
  dptr->next=NULL;
  dptr->prev=NULL;
}

customer_data_node* insert_customer_data(customer_data_node* dptr,customer_type* data)
{
  int i;
  i=0;
  while(i<dptr->no_of_data && dptr->data[i]->C_ID<(data->C_ID))
  {
    i++;
  }
  int j=dptr->no_of_data;
  while(i<j)
  {
    dptr->data[j]=dptr->data[j-1];
    j--;
  }
  dptr->data[j]=data;
  //dptr->no_of_data++;
  return dptr;
}

customer_data_node* insert_double(customer_data_node *dptr,customer_data_node *ndptr,customer_data_node *nndptr)
{
  if(nndptr==NULL)
  {
    ndptr->next=NULL;
    dptr->next=ndptr;
    ndptr->prev=dptr;
  }
  else
  {
    //printf("condition\n");
    nndptr->prev=ndptr;
    ndptr->next=nndptr;
    dptr->next=ndptr;
    ndptr->prev=dptr;
  }
}
int searchPos_customer(int key, int *key_arr, int n)
{
    int pos=0;
    while (pos < n && key > key_arr[pos])
        pos++;
    return pos;
}

enum KeyStatus ins_customer(customer_node_type *ptr, int key, int *upKey,customer_node_type
                   **newnode,customer_node_type *nptr)
{
    customer_node_type *newPtr, *lastPtr;
    int pos, i, n,splitPos;
    int newKey, lastKey;
    enum KeyStatus value;
    if (ptr->pointingto == 0)
    {
        *newnode = nptr;
        *upKey = key;
        return InsertIt;
    }
    n = ptr->no_of_keys;
    pos = searchPos_customer(key, ptr->key, n);
    if (pos < n && key == ptr->key[pos])
        return Duplicate;
    value = ins_customer(ptr->ptr.next_ptr[pos], key, &newKey, &newPtr,nptr);
    if (value != InsertIt)
        return value;
    /*If keys in node is less than M-1 where M is order of B tree*/
    if (n < M - 1)
    {
        pos = searchPos_customer(newKey, ptr->key, n);
        /*Shifting the key and pointer right for inserting the new key*/
        for (i=n; i>pos; i--)
        {
            ptr->key[i] = ptr->key[i-1];
            ptr->ptr.next_ptr[i+1] = ptr->ptr.next_ptr[i];
        }
        /*Key is inserted at exact location*/
        ptr->key[pos] = newKey;
        ptr->ptr.next_ptr[pos+1] = newPtr;
        ++ptr->no_of_keys;
        ptr->pointingto=1; /*incrementing the number of keys in node*/
        return Success;
    }/*End of if */
    /*If keys in nodes are maximum and position of node to be inserted is
    last*/
    if (pos == M - 1)
    {
        lastKey = newKey;
        lastPtr = newPtr;
    }
    else /*If keys in node are maximum and position of node to be inserted
is not last*/
    {
        lastKey = ptr->key[M-2];
        lastPtr = ptr->ptr.next_ptr[M-1];
        for (i=M-2; i>pos; i--)
        {
            ptr->key[i] = ptr->key[i-1];
            ptr->ptr.next_ptr[i+1] = ptr->ptr.next_ptr[i];
        }
        ptr->key[pos] = newKey;
        ptr->ptr.next_ptr[pos+1] = newPtr;
    }
    splitPos = (M - 1)/2;
    (*upKey) = ptr->key[splitPos];

    (*newnode)=malloc(sizeof(customer_node_type));/*Right node after split*/
    ptr->no_of_keys = splitPos; /*No. of keys for left splitted node*/
    (*newnode)->no_of_keys = M-1-splitPos;/*No. of keys for right splitted node*/
    for (i=0; i < (*newnode)->no_of_keys; i++)
    {
        (*newnode)->ptr.next_ptr[i] = ptr->ptr.next_ptr[i + splitPos + 1];
        if(i < (*newnode)->no_of_keys - 1)
            (*newnode)->key[i] = ptr->key[i + splitPos + 1];
        else
            (*newnode)->key[i] = lastKey;
    }
    (*newnode)->ptr.next_ptr[(*newnode)->no_of_keys] = lastPtr;
    (*newnode)->pointingto=1;
    return InsertIt;
}

customer_node_type* Insert_customer(customer_node_type *root,customer_type* data)
{
    if(root->no_of_keys==0)                             //no key in root node
    {
        if(root->ptr.data_ptr[0]==NULL)                    //no data present at all
        {
          //printf("inserting %d\n",data);
          customer_data_node *dptr=(customer_data_node*)malloc(sizeof(customer_data_node));
          dptr->data[0]=data;
          dptr->no_of_data=1;
          dptr->next=NULL;
          dptr->prev=NULL;
          root->ptr.data_ptr[0]=dptr;
          root->pointingto=0;
        }
        else
        {
          customer_data_node *dptr=root->ptr.data_ptr[0];
          if(dptr->no_of_data<4)                      //node has data<4 and no root key
          {
            //printf("inserting %d\n",data);
            dptr=insert_customer_data(dptr,data);
            dptr->no_of_data++;
          }
          else                                          //node has data>4 need to create a key in root
          {
            //printf("inserting %d\n",data);
            int key=2;
            root->key[0]=data->C_ID;                            //finding the key value
            if(data->C_ID<dptr->data[1]->C_ID)
            {
              key=1;
              root->key[0]=dptr->data[1]->C_ID;
            }
            else if(data->C_ID>dptr->data[2]->C_ID)
            {
              key=2;
              root->key[0]=dptr->data[2]->C_ID;
            }
            root->no_of_keys++;
            customer_data_node *ndptr=(customer_data_node*)malloc(sizeof(customer_data_node));             //customer_data_node to split the prev data node
            ndptr=initialise_customer_dptr(ndptr);
            int i;
            for(i=key;i<dptr->no_of_data;i++)                                    //copying value after the key node
            {
              ndptr->data[ndptr->no_of_data]=dptr->data[i];
              ndptr->no_of_data++;
            }
            if (key==1)
            {
              dptr=insert_customer_data(dptr,data);
            }
            else
            {
              ndptr=insert_customer_data(ndptr,data);
            }
            dptr->no_of_data=2;
            ndptr->no_of_data=3;
            dptr->next=ndptr;
            ndptr->prev=dptr;
            root->ptr.data_ptr[1]=ndptr;
            root->pointingto=0;
          }
        }
    }
    else                                          //atleast one key value is there in tree and two data nodes
    {

      customer_node_type *ptr=root;
      int index,i,found=0;
      //search for ptr start
      while(ptr->pointingto!=0)
      {
        i=0;
        while(i<ptr->no_of_keys && found==0)
        {
          //printf("index2 in loop %d\n",index);
          if(data->C_ID<ptr->key[i])
          {
            index=i;
            found=1;
          }
          i++;
          //printf("index2 in loop %d\n",index);
        }
        if(found==0)
        {
          ptr=ptr->ptr.next_ptr[ptr->no_of_keys];
        }
        else
        {
          ptr=ptr->ptr.next_ptr[index];
        }
        //printf("index2 in loop %d\n",index);
      }
      int index2=0,found2=0;
      i=0;
      while(i<ptr->no_of_keys && found2==0)
      {
        if(data->C_ID<ptr->key[i])
        {
          index2=i;
          found2=1;
        }
        //printf("index2 in loop %d\n",index2);
        i++;
      }
      customer_data_node *dptr;
      if(found2==0)
      {
        //printf("inserting %d\n",data);
        index2=ptr->no_of_keys;
      }
      dptr=ptr->ptr.data_ptr[index2];
      //search for ptr ends
      if(dptr->no_of_data<4)                      //node has data<4 and no root key
      {

        dptr=insert_customer_data(dptr,data);
        dptr->no_of_data++;
      }
      else                                          //node has data>4 need to create a key in root
      {
        int key=2;
        int keyelement=data->C_ID;
        //ptr->key[no_of_keys]=data;                            //finding the key value
        if(data->C_ID<dptr->data[1]->C_ID)
        {
          key=1;
          keyelement=dptr->data[1]->C_ID;
          //ptr->key[no_of_keys]=dptr->data[1];
        }
        else if(data->C_ID>dptr->data[2]->C_ID)
        {
          key=2;
          keyelement=dptr->data[2]->C_ID;
          //ptr->key[no_of_keys]=dptr->data[2];
        }
        customer_data_node *ndptr=(customer_data_node*)malloc(sizeof(customer_data_node));             //customer_data_node to split the prev data node
        ndptr=initialise_customer_dptr(ndptr);
        int i;
        //printf("inserting %d\n",data);
        for(i=key;i<dptr->no_of_data;i++)                                    //copying value after the key node
        {
          //printf("inserting %d\n",data);
          ndptr->data[ndptr->no_of_data]=dptr->data[i];
          ndptr->no_of_data++;
        }
        if (key==1)
        {
          dptr=insert_customer_data(dptr,data);
        }
        else
        {
          //printf("inserting %d\n",data);
          ndptr=insert_customer_data(ndptr,data);
        }
        dptr->no_of_data=2;
        ndptr->no_of_data=3;
        // dptr->next=ndptr;
        // ndptr->prev=dptr;
        if(ptr->no_of_keys<4)
        {
          //printf("inserting %d\n",data);
          //printf("keys= %d\n",ptr->no_of_keys);
          int i;
          for(i=ptr->no_of_keys-1; i>=index2 ; i--)
          {
            ptr->key[i+1]=ptr->key[i];
            ptr->ptr.data_ptr[i+2]=ptr->ptr.data_ptr[i+1];
          }
          ptr->key[index2]=keyelement;
          ptr->ptr.data_ptr[index2+1]=ndptr;
          ptr->no_of_keys++;
          // dptr->next=ndptr;
          // ndptr->prev=dptr;
          //printf("index2 = %d\n",index2);
          customer_data_node *nndptr=dptr->next;
          ndptr=insert_double(dptr,ndptr,nndptr);
        }
        else
        {
          //printf("inserting %d\n",data);
          int keyelement;
          customer_node_type* nptr;
          nptr=(customer_node_type*)malloc(sizeof(customer_node_type));
          nptr->no_of_keys=0;
          customer_data_node *nndptr=dptr->next;
          ndptr=insert_double(dptr,ndptr,nndptr);
          if(index2==2)
          {
            //  printf("inserting %d\n",data);
            keyelement=data->C_ID;
            nptr->ptr.data_ptr[0]=ndptr;
            nptr->key[0]=ptr->key[2];
            nptr->ptr.data_ptr[1]=ptr->ptr.data_ptr[3];
            nptr->key[1]=ptr->key[3];
            nptr->ptr.data_ptr[2]=ptr->ptr.data_ptr[4];
            nptr->no_of_keys=2;
            ptr->no_of_keys=2;
            nptr->pointingto=0;
          }
          else if(index2<2)
          {
            //printf("inserting %d\n",data);
            keyelement=ptr->key[1];
            nptr->pointingto=0;
            nptr->ptr.data_ptr[0]=ptr->ptr.data_ptr[2];
            nptr->key[0]=ptr->key[2];
            nptr->ptr.data_ptr[1]=ptr->ptr.data_ptr[3];
            nptr->key[1]=ptr->key[3];
            nptr->ptr.data_ptr[2]=ptr->ptr.data_ptr[4];
            if(index2==0)
            {
              ptr->key[1]=ptr->key[0];
              ptr->ptr.data_ptr[2]=ptr->ptr.data_ptr[1];
              ptr->key[0]=data->C_ID;
              ptr->ptr.data_ptr[1]=ndptr;
            }
            else
            {
              //index2==1
              ptr->key[1]=ndptr->data[0]->C_ID;
              ptr->ptr.data_ptr[2]=ndptr;
            }
            nptr->no_of_keys=2;
            ptr->no_of_keys=2;
          }
          else
          {
            keyelement=ptr->key[2];
            nptr->pointingto=0;
            if(index2==3)
            {
              //printf("inserting %d\n",data);
              nptr->key[1]=ptr->key[3];
              nptr->key[0]=ndptr->data[0]->C_ID;
              nptr->ptr.data_ptr[0]=ptr->ptr.data_ptr[3];
              nptr->ptr.data_ptr[1]=ndptr;
              nptr->ptr.data_ptr[2]=ptr->ptr.data_ptr[4];
            }
            else
            {
              //index2==4
              nptr->key[1]=ndptr->data[0]->C_ID;
              nptr->ptr.data_ptr[1]=ndptr;
              nptr->key[0]=ptr->key[3];
              nptr->ptr.data_ptr[0]=ptr->ptr.data_ptr[3];
              nptr->ptr.data_ptr[1]=ptr->ptr.data_ptr[4];
              nptr->ptr.data_ptr[2]=ndptr;
            }
            nptr->no_of_keys=2;
            ptr->no_of_keys=2;
          }
          // dptr->next=ndptr;
          // ndptr->prev=dptr;

          //printf("key element = %d\n",keyelement);
          //printf("index2 = %d\n",index2);
          customer_node_type *newnode;
          int upKey;
          enum KeyStatus value;
          value = ins_customer(root, keyelement, &upKey, &newnode, nptr);
          if (value == Duplicate)
              printf("Key already available\n");
          if (value == InsertIt)
          {
              customer_node_type *uproot = root;
              root=malloc(sizeof(customer_node_type));
              root->no_of_keys = 1;
              root->key[0] = upKey;
              root->ptr.next_ptr[0] = uproot;
              root->ptr.next_ptr[1] = newnode;
              root->pointingto=1;
          }
          //root=insert_in_btree(root,&keyelement,ptr,nptr,index2);
        }
      }
    }
    //printf("inserting %d\n",data);
    return root;
}

int favpayment(int arr[])
{
    int i;
    int pos;
    int n=7;
    // Initialize maximum element
    int max = arr[0];
    pos=0;
    // Traverse array elements
    // from second and compare
    // every element with current max
    for (i = 1; i < n; i++)
    {
        if (arr[i] > max)
        {
            max = arr[i];
            pos=i;
        }
    }
    return pos+1;
}
void showcustomer(customer_type* nptr)
{
  int x;
  printf("\t|| Name : ");
  printf("%s\n",nptr->name);
  printf("\t|| Email : ");
  printf("%s\n",nptr->email);
  printf("\t|| Contact No : ");
  printf("%s\n",nptr->contact);
  printf("\t|| Address : ");
  printf("%s\n",nptr->address);
  printf("\t|| Customer ID : ");
  printf("%d\n",nptr->C_ID);
  printf("\t|| Return Cash : ");
  printf("%d\n",nptr->cash_back);
  x=favpayment(nptr->payment_history);
  printf("\t|| Favourite payment Mode : ");
  switch(x)
  {
    case 1 :{
              printf("%s\n","BNPL");
              break;
            }
    case 2: {
              printf("%s\n","COD");
              break;
            }
    case 3: {
              printf("%s\n","Debit Card");
              break;
            }
    case 4: {
              printf("%s\n","Credit Card");
              break;
            }
    case 5: {
              printf("%s\n","Net Banking");
              break;
            }
    case 6:
            {
              printf("%s\n","UPI");
              break;
            }
    case 7:
            {
              printf("%s\n","Phone Pe");
              break;
            }
  }
}

void display_customer(customer_node_type *ptr, int blanks)
{
    if (ptr->pointingto==1)
    {
        int i;
        for(i=1; i<=blanks; i++)
            printf(" ");
        for (i=0; i < ptr->no_of_keys; i++)
            printf("%d ",ptr->key[i]);
        printf("\n");
        for (i=0; i <= ptr->no_of_keys; i++)
            display_customer(ptr->ptr.next_ptr[i], blanks+10);
    }
    else
    {
      int i,j;
      for(i=1; i<=blanks; i++)
          printf(" ");
      for (i=0; i < ptr->no_of_keys; i++)
          printf("%d ",ptr->key[i]);
      printf("\n");
      for (j=0; j <= ptr->no_of_keys; j++)
      {
        //printf("%d\n",ptr->no_of_keys);
        customer_data_node* dptr=ptr->ptr.data_ptr[j];
      for(i=1; i<=blanks+10; i++)
          printf(" ");
      for (i=0; i < dptr->no_of_data; i++)
          //printf("innner i = %d\n",i);
          showcustomer(dptr->data[i]);
          // printf("%d",dptr->data[i]->C_ID);
          // printf("%s")
          printf("\n");
      }
      //    display_customer(ptr->ptr.next_ptr[i], blanks+10);
    }/*End of if*/
}/*End of display_customer()*/
void traverse_customer(customer_node_type *root)
{
  customer_node_type *ptr=root;
  int i;
  while(ptr->pointingto!=0)
  {
    ptr=ptr->ptr.next_ptr[0];
  }
  customer_data_node *lptr=ptr->ptr.data_ptr[0];
	customer_data_node *nptr = lptr;
	while(nptr != NULL)
	{
    for (i=0; i < nptr->no_of_data; i++)
    {
        //printf("innner i = %d\n",i);
        printf("%d ",nptr->data[i]->C_ID);
    }
		nptr = nptr -> next;
	}
	printf("\n");
}

customer_node_type * readrecords_customer()
{
  customer_type* nptr;
  customer_node_type *root=NULL;
  root=(customer_node_type*)malloc(sizeof(customer_node_type));
  root->no_of_keys=0;
  root->ptr.data_ptr[0]=NULL;
  history_type hptr;
  FILE *fptr;
	int i;
  char tempstart[30],tempend[30];
  if(fptr=fopen("customer_data1.txt","r"))
  {
    int x;
		while(!feof(fptr))
		{
      nptr=(customer_type*)malloc(sizeof(customer_type));
      nptr=customer_intialise(nptr);
      fscanf(fptr,"%d ",&nptr->C_ID);
      fscanf(fptr,"%s ",nptr->email);
      fscanf(fptr,"%s ",nptr->password);
      fscanf(fptr,"%s ",nptr->name);
      fscanf(fptr,"%s ",nptr->address);
      fscanf(fptr,"%s ",nptr->contact);
      fscanf(fptr,"%d ",&nptr->cash_back);
      fscanf(fptr,"%s ",tempend);
      fscanf(fptr,"%s ",hptr.product_name);
      while(strcmp("Historyends",hptr.product_name)!=0)
      {
        fscanf(fptr,"%d ",&hptr.qty);
        nptr->history_ptr=add_history(nptr->history_ptr,hptr.product_name,hptr.qty);
        fscanf(fptr,"%s ",hptr.product_name);
      }
      //printf("FileOPENED");
      fscanf(fptr,"%s ",tempend);
      fscanf(fptr,"%s ",hptr.product_name);
      while(strcmp("wishends",hptr.product_name)!=0)
      {
        fscanf(fptr,"%d ",&hptr.qty);
        nptr->wish_ptr=add_history(nptr->wish_ptr,hptr.product_name,hptr.qty);
        fscanf(fptr,"%s ",hptr.product_name);
      }
      for(i=0;i<7;i++)
      {
        fscanf(fptr,"%d ",&nptr->payment_history[i]);
      }
    //traverse_customer(nptr);
      //showcustomer(nptr);
      root=Insert_customer(root,nptr);
      customer_count++;
    }
  }
  fclose(fptr);
  return root;
}

customer_type* search_customer(customer_node_type* root,int C_ID,status_code *sc)
{
  int i=0;
  int found=0;
  int index=0;
  customer_type* ret_val;
  customer_node_type *ptr=root;
  while(ptr->pointingto!=0)
  {
    i=0;
    while(i<ptr->no_of_keys && found==0)
    {
      //printf("index2 in loop %d\n",index);
      if(C_ID<ptr->key[i])
      {
        index=i;
        found=1;
      }
      i++;
      //printf("index2 in loop %d\n",index);
    }
    if(found==0)
    {
      ptr=ptr->ptr.next_ptr[ptr->no_of_keys];
    }
    else
    {
      ptr=ptr->ptr.next_ptr[index];
    }
    //printf("index2 in loop %d\n",index);
  }
  int index2=0,found2=0;
  i=0;
  while(i<ptr->no_of_keys && found2==0)
  {
    if(C_ID<ptr->key[i])
    {
      index2=i;
      found2=1;
    }
    //printf("index2 in loop %d\n",index2);
    i++;
  }
  customer_data_node *dptr;
  if(found2==0)
  {
    //printf("inserting %d\n",data);
    index2=ptr->no_of_keys;
  }
  dptr=ptr->ptr.data_ptr[index2];
  i=0;
  int index3=0,found3=0;
  while(i<dptr->no_of_data && found3==0)
  {
    if(C_ID==dptr->data[i]->C_ID)
    {
      index3=i;
      found3=1;
    }
    i++;
  }
  if(found3==1)
  {
    ret_val=dptr->data[index3];
    *sc=SUCCESS;
  }
  else
  {
    ret_val=NULL;
    *sc=FAILURE;
  }
  return ret_val;
}

customer_type* add_customer(customer_node_type **root)
{
  system("cls");
	customer_type* nptr;
  //status_code sc;
  char temp;
  nptr=(customer_type*)malloc(sizeof(customer_type));
  nptr=customer_intialise(nptr);
  nptr->C_ID=customer_count+1;
  printf("\t|| Email : ");
  scanf("%s[^\n]",nptr->email);
  printf("\t|| Password : ");
  scanf("%s[^\n]",nptr->password);
  printf("\t|| Name : ");
  scanf("%s[^\n]",nptr->name);
  printf("\t|| Address : ");
  scanf("%s[^\n]",nptr->address);
  printf("\t|| Contact No : ");
  scanf("%s[^\n]",nptr->contact);
  *root=Insert_customer(*root,nptr);
	return nptr;
}

void saverecords(customer_node_type *root)
{
  customer_node_type *ptr=root;
  int i,j;
  while(ptr->pointingto!=0)
  {
    ptr=ptr->ptr.next_ptr[0];
  }
  customer_data_node *lptr=ptr->ptr.data_ptr[0];
  customer_data_node *dptr=lptr;
	customer_type *nptr;
  FILE *fptr;
  if(fptr=fopen("customer_data1.txt","w"))
  {
    int x;
  	//if(lptr!=NULL)
  	//{
  		while(dptr!=NULL)
  		{
        for(j=0;j<dptr->no_of_data;j++)
        {
          nptr=dptr->data[j];
          fprintf(fptr,"%d\n",nptr->C_ID);
          fprintf(fptr,"%s\n",nptr->email);
          fprintf(fptr,"%s\n",nptr->password);
          fprintf(fptr,"%s\n",nptr->name);
          fprintf(fptr,"%s\n",nptr->address);
          fprintf(fptr,"%s\n",nptr->contact);
          fprintf(fptr,"%d\n",nptr->cash_back);
          fprintf(fptr,"Historystarts\n");
          save_history(nptr->history_ptr,fptr);
          fprintf(fptr,"Historyends\n");
          fprintf(fptr,"wishstarts\n");
          save_history(nptr->wish_ptr,fptr);
          fprintf(fptr,"wishends\n");
          for(i=0;i<7;i++)
          {
            fprintf(fptr,"%d ",nptr->payment_history[i]);
          }
          fprintf(fptr,"\n");
        }
        dptr=dptr->next;
      }
    //}
  }
  fclose(fptr);
}

void Customers_range(customer_node_type *root,int start,int end)
{
  int i=0;
  int C_ID=start;
  int found=0;
  int index=0;
  customer_type* ret_val;
  customer_node_type *ptr=root;
  while(ptr->pointingto!=0)
  {
    i=0;
    while(i<ptr->no_of_keys && found==0)
    {
      //printf("index2 in loop %d\n",index);
      if(C_ID<ptr->key[i])
      {
        index=i;
        found=1;
      }
      i++;
      //printf("index2 in loop %d\n",index);
    }
    if(found==0)
    {
      ptr=ptr->ptr.next_ptr[ptr->no_of_keys];
    }
    else
    {
      ptr=ptr->ptr.next_ptr[index];
    }
    //printf("index2 in loop %d\n",index);
  }
  int index2=0,found2=0;
  i=0;
  while(i<ptr->no_of_keys && found2==0)
  {
    if(C_ID<ptr->key[i])
    {
      index2=i;
      found2=1;
    }
    //printf("index2 in loop %d\n",index2);
    i++;
  }
  customer_data_node *dptr;
  if(found2==0)
  {
    //printf("inserting %d\n",data);
    index2=ptr->no_of_keys;
  }
  dptr=ptr->ptr.data_ptr[index2];
  i=0;
  int index3=0,found3=0;
  while(i<dptr->no_of_data && found3==0)
  {
    if(C_ID<=dptr->data[i]->C_ID)
    {
      index3=i;
      found3=1;
    }
    i++;
  }
  if(found3==1)
  {
    ret_val=dptr->data[index3];
    int m;
    for(m=index3; m<dptr->no_of_data && dptr->data[m]->C_ID!=end ;m++)
    {
      showcustomer(dptr->data[m]);
    }
    if(dptr->data[m]->C_ID==end)
    {
      showcustomer(dptr->data[m]);
    }
    while(dptr->data[m]->C_ID!=end)
    {
      dptr=dptr->next;
      for(m=0; m<dptr->no_of_data && dptr->data[m]->C_ID!=end ;m++)
      {
        showcustomer(dptr->data[m]);
      }
      if(dptr->data[m]->C_ID==end)
      {
        showcustomer(dptr->data[m]);
      }
    }
  }
  else
  {
    ret_val=NULL;
  }
}

void traverse_customer_names(customer_type *list_ptr)
{
	customer_type *nptr;
	nptr=list_ptr;
  int x;
	if(list_ptr==NULL)
	{
		printf("List Empty");
	}
	else
	{
		printf("\n\tCustomer Details\n");
		while(nptr!=NULL && nptr->history_ptr!=NULL)
		{
      printf("\n---------------------------------------------\n");
      printf("Name : ");
      printf("%s\n",nptr->name);
      printf("Customer ID : ");
      printf("%d\n",nptr->C_ID);
      printf("\n Purchases: \n");
      traverse_history(nptr->history_ptr);
      printf("\n---------------------------------------------\n");
			nptr=nptr->next;
		}
	}
}

customer_type* temp_list(customer_node_type *root)
{
  customer_type* list;
  customer_type* nlist;
  customer_type* next_list;
  list=NULL;
  customer_node_type *ptr=root;
  int i;
  while(ptr->pointingto!=0)
  {
    ptr=ptr->ptr.next_ptr[0];
  }
  customer_data_node *lptr=ptr->ptr.data_ptr[0];
	customer_data_node *nptr = lptr;
	while(nptr != NULL)
	{
    for (i=0; i < nptr->no_of_data; i++)
    {
        if(list==NULL)
        {
          list=(customer_type*)malloc(sizeof(customer_type));
          list=customer_intialise(list);
          list=nptr->data[i];
          list->next=NULL;
          nlist=list;
        }
        else
        {
          next_list=(customer_type*)malloc(sizeof(customer_type));
          next_list=customer_intialise(next_list);
          next_list=nptr->data[i];
          next_list->next=nlist;
          nlist=next_list;
        }
    }
		nptr = nptr -> next;
	}
  return nlist;
}
//*****************************************************************//
//                            Order                             //
//*****************************************************************//
status_code Order(customer_type *cptr, int P_ID, int qty, product_node_type *root)
{
  system("cls");
  product_type* ptr;
  int paymode;
  // customer_type cptr;
  status_code sc;
  //sc=FAILURE;
  // cptr=search_customer(C_ID);
  ptr=search_product(root,P_ID,&sc);
  if(sc=SUCCESS)
  {
    system("cls");
    printf("\t---------------------------------\n");
    printf("\t|| Product You have selected : %s \n",ptr->name);
    printf("\t|| Amount To be Paid : %d\n",ptr->cost);
    printf("\t---------------------------------\n");
    printf("\t=================================\n");
    printf("\t||       PAYMENT OPTIONS       ||\n");
    printf("\t=================================\n");
    printf("\t=================================\n");
    printf("\n\t|| 1. BNPL\n\t|| 2. COD\n\t|| 3. debit card\n\t|| 4. Credit card\n\t|| 5. Netbanking\n\t|| 6. UPI\n\t|| 7. PhonePe\n");
    printf("\t=================================\n");
    printf("\tEnter Your Choice : ");
    scanf("%d",&paymode);
    if(ptr->items_available >= qty )
    {
      ptr->items_available-=qty;
      ptr->sold+=qty;
      cptr->payment_history[paymode-1]+=1;
      sc=SUCCESS;
      cptr->history_ptr=add_history(cptr->history_ptr,ptr->name,qty);
      global_history=add_history(global_history,ptr->name,qty);
      // printf("\nadding to history %s \n",(**cptr).history_ptr->product_name);
      cptr->wish_ptr=delete_history(cptr->wish_ptr,ptr->name);
      // printf("\nadding to history %s \n",(**cptr).history_ptr->product_name);
    }
  }
  return sc;
}

status_code AddWishList(customer_type *cptr,int P_ID, int qty,product_node_type *root)
{
  product_type *ptr;
  // customer_type cptr;
  status_code sc;
  sc=FAILURE;
  // cptr=search_customer(C_ID);
  ptr=search_product(root,P_ID,&sc);
  if(sc==SUCCESS)
  {
    if(ptr->items_available >= qty )
    {
      sc=SUCCESS;
      cptr->wish_ptr=add_history(cptr->wish_ptr,ptr->name,qty);
    }
  }
  return sc;
}

status_code ReturnProduct(customer_type *cptr, int P_ID, product_node_type *root)
{
  product_type *ptr;
  // customer_type cptr;
  status_code sc;
  // cptr=search_customer(C_ID);
  history_type *hptr=cptr->history_ptr;
  ptr=search_product(root,P_ID,&sc);
  if(sc==SUCCESS)
  {
    hptr=search_history(ptr->name,hptr,&sc);
    if(sc==SUCCESS)
    {
      ptr->items_available+=hptr->qty;
      ptr->sold-=hptr->qty;
      sc=SUCCESS;
      cptr->history_ptr=delete_history(cptr->history_ptr,ptr->name);
      global_history=delete_history(global_history,ptr->name);
      cptr->cash_back=ptr->cost*hptr->qty;
    }
  }
  return sc;
}

//*****************************************************************//
//                        Merge Sort                             //
//*****************************************************************//
history_type* SortedMerge(history_type* a, history_type* b)
{
history_type* result = NULL;

/* Base cases */
if (a == NULL)
    return(b);
else if (b==NULL)
    return(a);

/* Pick either a or b, and recur */
if (a->qty >= b->qty)
{
    result = a;
    result->next = SortedMerge(a->next, b);
}
else
{
    result = b;
    result->next = SortedMerge(a, b->next);
}
return(result);
}

void FrontBackSplit(history_type* source,
        history_type** frontRef, history_type** backRef)
{
        history_type* fast;
        history_type* slow;
    slow = source;
    fast = source->next;

    /* Advance 'fast' two nodes, and advance 'slow' one node */
    while (fast != NULL)
    {
    fast = fast->next;
    if (fast != NULL)
    {
        slow = slow->next;
        fast = fast->next;
    }
    }

    /* 'slow' is before the midpoint in the list, so split it in two
    at that point. */
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}

void MergeSort(history_type** headRef)
{
  history_type* head = *headRef;
  history_type* a;
  history_type* b;

  /* Base case -- length 0 or 1 */
  if ((head == NULL) || (head->next == NULL))
  {
      return;
  }

  /* Split head into 'a' and 'b' sublists */
  FrontBackSplit(head, &a, &b);

  /* Recursively sort the sublists */
  MergeSort(&a);
  MergeSort(&b);

  /* answer = merge the two sorted lists together */
  *headRef = SortedMerge(a, b);
}

int sum_history(history_type *list_ptr)
{
  int sum=0;
	history_type *ptr;
  ptr=list_ptr;
	if(list_ptr==NULL)
	{
		sum=0;
	}
	else
	{
		while(ptr!=NULL)
		{
      sum+=ptr->qty;
			ptr=ptr->next;
		}
	}
  return sum;
}

customer_type* SortedMerge_customer(customer_type* a, customer_type* b)
{
customer_type* result = NULL;
int sum1,sum2;

/* Base cases */
if (a == NULL)
    return(b);
else if (b==NULL)
    return(a);

/* Pick either a or b, and recur */
sum1=sum_history(a->history_ptr);
sum2=sum_history(b->history_ptr);
if (sum1 >= sum2)
{
    result = a;
    result->next = SortedMerge_customer(a->next, b);
}
else
{
    result = b;
    result->next = SortedMerge_customer(a, b->next);
}
return(result);
}

void FrontBackSplit_customer(customer_type* source,customer_type** frontRef, customer_type** backRef)
{
        customer_type* fast;
        customer_type* slow;
    slow = source;
    fast = source->next;
    /* Advance 'fast' two nodes, and advance 'slow' one node */
    while (fast != NULL)
    {
      fast = fast->next;

        if (fast != NULL)
        {
          slow = slow->next;
          fast = fast->next;
        }
    }

    /* 'slow' is before the midpoint in the list, so split it in two
    at that point. */
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}

void MergeSort_customer(customer_type** headRef)
{
customer_type* head = *headRef;
customer_type* a;
customer_type* b;

/* Base case -- length 0 or 1 */
if ((head == NULL) || (head->next == NULL))
{
    return;
}
/* Split head into 'a' and 'b' sublists */
FrontBackSplit_customer(head, &a, &b);
/* Recursively sort the sublists */
MergeSort_customer(&a);
MergeSort_customer(&b);

/* answer = merge the two sorted lists together */
*headRef = SortedMerge_customer(a, b);
}


//*****************************************************************//
//                        Product Menu                             //
//*****************************************************************//
void product_menu(customer_type *cptr)
{
  customer_type *current_customer;
  current_customer=cptr;
  char name[20];
  int P_ID;
  int qty;
  int exit=0;
  int back=0;
  status_code sc;
  int choice,category;
  int choice2;
  while(exit==0 && back==0)
  {
    printf("\t---------------------------------\n");
    printf("\t||          CATEGORIES          ||\n");
    printf("\t---------------------------------\n");
    printf("\t=================================\n");
    printf("\t|| 1. Mobile\n\t|| 2. Mobile accessories \n\t|| 3. Laptop and Computer Accessories \n\t|| 4. Electronic gadgets \n\t|| 5. Games and Gaming accesories \n\t|| 6. Home and Kitchen appliances \n\t|| 7. Back\n");
    printf("\t=================================\n");
    printf("\tEnter Your Choice : ");
    scanf("%d",&category);
    if(category<1 || category>6)
    {
      back=1;
    }
    else
    {
      category-=1;
      product_node_type *root=pc[category].root;
      system("cls");
      traverse_product(root);
      printf("\t---------------------------------\n");
      printf("\t||        PRODUCT MENU          ||\n");
      printf("\t---------------------------------\n");
      printf("\t=================================\n");
      printf("\t|| 1. Product Optionn\n\t|| 2. Back\n");
      printf("\t=================================\n");
      printf("\tEnter Your Choice : ");
      scanf("%d",&choice2);
      switch(choice2)
      {
        case 1:
                {
                  choice=1;
                  printf("\t-------------------------------\n");
                  printf("\t|| Product ID : ");
                  scanf("%d",&P_ID);
                  printf("\t-------------------------------\n");
                  printf("\t|| Quantity : ");
                  scanf("%d",&qty);
                  system("cls");
                  printf("\t=================================\n");
                  printf("\t||       Product Options         ||\n");
                  printf("\t=================================\n");
                  printf("\t=================================\n");
                  printf("\t|| 1. Order\n\t|| 2. Add To Wish List\n\t|| 3. Return Product\n");
                  printf("\t=================================\n");
                  printf("\t\tEnter Your Choice : ");
                  scanf("%d",&choice);
                    switch(choice)
                    {
                      case 1: {

                                sc=Order(cptr, P_ID, qty,root);
                                system("cls");
                                if(sc=SUCCESS)
                                {
                                  printf("\t-----------------------------------------\n");
                                  printf("\t|| Thankyou, %s\n",current_customer->name);
                                  printf("\t|| Your Order for Product ID = %d is Placed Successfully \n",P_ID);
                                  printf("\t|| It Will Be Deliever to %s soon\n",current_customer->address);
                                  printf("\t-----------------------------------------\n");
                                }
                                else if(sc=FAILURE)
                                {
                                  printf("\t-----------------------------------------\n");
                                  printf("\t|| Sorry, %s",current_customer->name);
                                  printf("\t|| We Do not have this much Quantity \n");
                                  printf("\t|| Please Enter Less Quantity\n");
                                  printf("\t-----------------------------------------\n");
                                }
                                break;
                              }
                      case 2: {
                                sc=AddWishList(cptr, P_ID, qty,root);
                                system("cls");
                                if(sc=SUCCESS)
                                {
                                  printf("\t----------------------------------------------------------\n");
                                  printf("\t|| The product, Product ID = %d, is added to your wishlist\n",P_ID);
                                  printf("\t----------------------------------------------------------\n");
                                }
                                else if(sc=SUCCESS)
                                {
                                  printf("\t----------------------------------------------------------\n");
                                  printf("\t|| Please Enter Less Quantity\n");
                                  printf("\t----------------------------------------------------------\n");
                                }
                                break;
                              }
                      case 3:
                              {
                                sc=ReturnProduct(cptr, P_ID,root);
                                system("cls");
                                if(sc=SUCCESS)
                                {
                                  printf("\t-----------------------------------------\n");
                                  printf("\t|| Return of the Product intiated\n");
                                  printf("\t|| Our Executive will contact you soon\n");
                                  printf("\t-----------------------------------------\n");
                                }
                                else if(sc=FAILURE)
                                {
                                  printf("\t-----------------------------------------\n");
                                  printf("\t|| Sorry, %s",current_customer->name);
                                  printf("\t|| You cant Return a product you havent bought\n");
                                  printf("\t-----------------------------------------\n");
                                }
                                break;
                              }
                      default:
                              {
                                printf("\n\t !!!Invalid Choice!!! ");
                              }
                    }

                  break;
                }
          case 2:
                  {
                    exit=1;
                    break;
                  }
          default:
                  {
                    printf("\n\t\tInvalid Option\n");
                    break;
                  }
      }
      pc[category].root=root;
    }
  }
}

//*****************************************************************//
//                        Print Menu                             //
//*****************************************************************//
void print_menu(customer_node_type *head_ptr)
{
  int choice;
  int temp=1;
  status_code sc;
  customer_type *cptr;
  int C_ID;
  int exit=0;
  while(exit==0)
  {
    system("cls");
    printf("\t---------------------------------------------------------------------\n");
    printf("\t||                            MENU                                 ||\n");
    printf("\t---------------------------------------------------------------------\n");
    printf("\t======================================================================\n");
    printf("\t|| 1. Print WishList\n\t|| 2. Print Customer info\n\t|| 3. Print Items Purchased Frequently By a Customer\n\t|| 4. Print Items Purchased Frequently By all Customers\n\t|| 5. Customer list sorted according to Frequency of purchases\n\t|| 6. Print Product list\n\t|| 7. Customers in Given Range\n\t|| 8. Exit \n");
    printf("\t======================================================================\n");
    printf("\tEnter Your Choice : ");
    scanf("%d",&choice);
    switch(choice)
    {
      case 1:
              {
                system("cls");
                printf("\n\tEnter Customer ID : ");
                scanf("%d",&C_ID);
                cptr=search_customer(head_ptr,C_ID,&sc);
                if(cptr!=NULL)
                {
                  printf("\tCustomer Name : %s\n",cptr->name);
                  printf("\t=================================\n");
                  traverse_history(cptr->wish_ptr);
                  printf("\t=================================\n");
                  getchar();
                  getchar();
                }
                else
                {
                  system("cls");
                  printf("\n\n\t\tIncorrect ID");
                  getchar();
                  getchar();
                }
                break;
              }
      case 2:
              {
                system("cls");
                printf("\n\tEnter Customer ID : ");
                scanf("%d",&C_ID);
                cptr=search_customer(head_ptr,C_ID,&sc);
                if(cptr!=NULL)
                {
                  printf("\t=================================\n");
                  showcustomer(cptr);
                  printf("\t=================================\n");
                  getchar();
                  getchar();
                }
                else
                {
                  printf("\n\n\t\tIncorrect ID");
                  getchar();
                  getchar();
                }
                break;
              }
      case 3:
              {
                system("cls");
                printf("\n\tEnter Customer ID : ");
                scanf("%d",&C_ID);
                cptr=search_customer(head_ptr,C_ID,&sc);
                if(cptr!=NULL)
                {
                  printf("\tCustomer Name : %s\n",cptr->name);
                  MergeSort(&(cptr->history_ptr));
                  printf("\t=================================\n");
                  traverse_history(cptr->history_ptr);
                  printf("\t=================================\n");
                  getchar();
                  getchar();
                }
                else
                {
                  system("cls");
                  printf("\n\n\t\tIncorrect ID");
                  getchar();
                  getchar();
                }
                break;
              }
      case 4:
              {
                system("cls");
                MergeSort(&global_history);
                printf("\t---------------------------------\n");
                printf("\t||Frequently Purchased Products||\n");
                printf("\t---------------------------------\n");
                printf("\t=================================\n");
                traverse_history(global_history);
                printf("\t=================================\n");
                getchar();
                getchar();
                break;
              }
      case 5:
              {
                system("cls");
                customer_type *lptr;
                lptr=NULL;
                lptr=temp_list(head_ptr);
                MergeSort_customer(&lptr);
                printf("\t---------------------------------\n");
                printf("\t||        Active Buyers         ||\n");
                printf("\t---------------------------------\n");
                traverse_customer_names(lptr);
                getchar();
                getchar();
                break;
              }
      case 6:
              {
                system("cls");
                printf("\t---------------------------------\n");
                printf("\t||         Products            ||\n");
                printf("\t---------------------------------\n");
                traverse_all_product();
                getchar();
                getchar();
                break;
              }
      case 7:
              {
                int start,end;
                system("cls");
                printf("\n\tEnter starting Customer ID : ");
                scanf("%d",&start);
                printf("\n\tEnter ending Customer ID : ");
                scanf("%d",&end);
                system("cls");
                Customers_range(head_ptr,start,end);
                getchar();
                getchar();
                break;
              }
      case 8:
              {
                exit=1;
                break;
              }
      default :
              {
                printf("Invalid Option");
                break;
              }
    }
  }
}

//*****************************************************************//
//                        LOGIN                                   //
//*****************************************************************//
status_code login(customer_node_type* head_customer,customer_type **current_customer,int C_ID,char password[])
{
  status_code sc=FAILURE;
  customer_type* cptr=search_customer(head_customer,C_ID,&sc);
  if(cptr!=NULL)
  {
    if(strcmp(cptr->password,password)==0)
    {
      sc=SUCCESS;
      *current_customer=cptr;
    }
  }
  return sc;
}

void main()
{
  int choice=0;
  status_code sc;
  product_type *list_prod;
	list_prod=NULL;
  createlist();
  read_history();
  //traverse_product(list_prod);
  customer_node_type *head_customer=NULL;
  head_customer=readrecords_customer();
  //traverse_customer(head_customer);
  customer_type *current_customer;
  //head_customer=createcustomer(&sc);
  while(choice!=4)
  {
    system("cls");
    printf("\t=================================\n");
    printf("\t||                              ||\n");
    printf("\t|| !!Welcome To Online Bazaar!! ||\n");
    printf("\t||                              ||\n");
    printf("\t=================================\n");

    printf("\t---------------------------------\n");
    printf("\t||          MENU BAR           ||\n");
    printf("\t---------------------------------\n");

    printf("\t=================================\n");
    printf("\t|| 1. Create User\n\t|| 2. Login\n\t|| 3. Admin\n\t|| 4. Exit\n");
    printf("\t=================================\n");
    printf("\n\tEnter Your Choice : ");
    scanf("%d",&choice);
    switch(choice)
    {
      case 1: {
                current_customer=add_customer(&head_customer);
                system("cls");
                printf("\t---------------------------------\n");
                printf("\t||   Registered Successfully   ||\n");
                printf("\t---------------------------------\n");
                printf("\t---------------------------------\n");
                printf("\t||   Your Customer ID is %d    ||\n",current_customer->C_ID);
                printf("\t---------------------------------\n");
                getchar();
                getchar();
                system("cls");
                product_menu(current_customer);
                break;
              }
      case 2:
              {
                system("cls");
                int C_ID;
                char password[30];
                printf("\t---------------------------------\n");
                printf("\t|| Enter User ID : ");
                scanf("%d",&C_ID);
                getchar();
                printf("\t--------------------------------2-\n");
                printf("\t|| Enter Password : ");
                scanf("%s",password);

                sc=login(head_customer,&current_customer,C_ID,password);
                if(sc==SUCCESS)
                {
                  int lchoice;
                  int exit_login=0;
                  while(exit_login!=1)
                  {
                    system("cls");
                    printf("\t---------------------------------\n");
                    printf("\t||          MENU BAR           ||\n");
                    printf("\t---------------------------------\n");

                    printf("\t=================================\n");
                    printf("\t|| 1. My Orders\n\t|| 2. My Wishlist\n\t|| 3. Buy Product\n\t|| 4. Back\n");
                    printf("\t=================================\n");
                    printf("\n\tEnter Your Choice : ");
                    scanf("%d",&lchoice);
                    switch(lchoice)
                    {
                      case 1:
                              {
                                system("cls");
                                printf("\t---------------------------------\n");
                                printf("\t||            Orders             ||\n");
                                printf("\t---------------------------------\n");

                                printf("\t=================================\n");
                                traverse_history(current_customer->history_ptr);
                                printf("\t=================================\n");
                                getchar();
                                getchar();
                                break;
                              }
                     case 2:
                              {
                                system("cls");
                                printf("\t---------------------------------\n");
                                printf("\t||          Wish List           ||\n");
                                printf("\t---------------------------------\n");

                                printf("\t=================================\n");
                                traverse_history(current_customer->wish_ptr);
                                printf("\t=================================\n");
                                getchar();
                                getchar();
                                break;
                              }
                      case 3:
                               {
                                 system("cls");
                                 product_menu(current_customer);
                                 break;
                               }
                      case 4:
                              {
                                exit_login=1;
                                break;
                              }
                      default:
                              {
                                printf("\n\tInvalid option");
                                break;
                              }
                    }
                  }
                }
                else
                {
                  system("cls");
                  printf("\n\n\t\tIncorrect ID or password");
                  getchar();
                  getchar();
                }
                break;
              }
      case 3: {
                system("cls");
                print_menu(head_customer);
                break;
              }
      case 4: { break;}
      default:
              {
                printf("Invalid Choice");
              }
    }
  }
  FILE *fptr=fopen("global_history.txt","w");
  save_history(global_history,fptr);
  fclose(fptr);
  saverecords(head_customer);
  storelist(&sc);
}
