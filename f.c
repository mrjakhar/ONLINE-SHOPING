#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
typedef enum {NO,YES} boolean;
typedef struct purchase_history{
	char ctg[20];
	char product_name[20];
	int quantity;
	float charge;
	struct purchase_history *no;
}orders;
typedef struct wish{
	char productname[20];
	struct wish *nextlist;
} wish;
typedef struct custmer_tag{
	char name[20];
	long int phone;
	char address[50];
	char payment_mode[20];
	struct purchase_history *h,*t,*favoutate;
	struct wish *wishlist;
}custmer_database;
typedef struct custmer_list{
	char user_Id[20];
	char password[20];
	char E_mail_address[30];
	struct custmer_tag *custmer;
	struct custmer_list *next;
}custmer_list;
typedef struct list{
	struct custmer_list *head;
	struct custmer_list *tail;
}ctype;

typedef struct node_type {
	char product_name[20];
	int availability;
	float price;
	struct node_type * next1;
}product_type;
typedef struct category_type {
	char category[20];
	struct category_type *down;
	 struct node_type *next;
	 struct node_type *tail;
}categoty_type;
typedef struct stock {
	struct category_type *head;
	struct category_type *last;
} store;
void inIt(store *n)
{	
	n->head=n->last=NULL;
}
product_type* newproduct(char name[], int avail, float price)
{
	product_type *new;
	new=(product_type*)malloc(sizeof(product_type));
	strcpy(new->product_name,name);
	new->availability=avail;
	new->price=price;
	new->next1=NULL;
	return new;
}
categoty_type* newcategory(char name[])
{
	categoty_type *new;
	new=(categoty_type*)malloc(sizeof(categoty_type));
	strcpy(new->category,name);
	new->down=NULL;
	new->next=new->tail=NULL;
	return new;
}
boolean isProductOf(char name[],product_type *n,product_type **m)
{
	boolean b=NO;
	product_type *p;
	p=n;
	while(p!=NULL && b==NO)
	{
		if(strcmp(p->product_name,name)==0)
		{
			b=YES;
			*m=p;
		}
		else
		{
			p=p->next1;
		}
	}
	if(b==YES)
	{
		n=p;
	}
	return b;
}
boolean iscategoryOf(char name[],categoty_type *n,categoty_type **m)
{
	boolean b=NO;
	categoty_type *p;
	p=n;
	while(p!=NULL && b==NO)
	{
		if(strcmp(p->category,name)==0)
		{
			b=YES;
			*m=p;
		}
		else
		{
			p=p->down;
		}
	}
	if(b==YES)
	{
		n=p;
	}
	return b;
}
boolean addnewproduct(store *s)
{
	char category_name[20],pname[20];
	float newprice;
	int quantity;
	printf("Enter category :\n");
	scanf("%s",category_name);
	printf("Enter product name:\n");
	scanf("%s",pname);
	printf("Enter products price:\n");
	scanf("%f",&newprice);
	printf("Enter products quantity:\n");
	scanf("%d",&quantity);
	categoty_type *c,*c1;
	c=s->head;
	boolean b=iscategoryOf(category_name,c,&c1);
	if(b==YES)
	{
		product_type *p,*q,*r;
		p=c->next;
		b=isProductOf(pname,p,&r);
		if(b==YES)
		{
			p->availability=p->availability+quantity;
			p->price=newprice;
		}
		else
		{
			q=newproduct(pname,quantity, newprice);
			p=c->tail;
			if(p==NULL)
			{
				c->next=c->tail=q;
			}
			else
			{
				c->tail=p->next1=q;
			}
		}
	}
	else
	{
		categoty_type *n,*bc;
		product_type *q;
		bc=s->last;
		n=newcategory(category_name);
		if(n!=NULL)
		{
			if(bc==NULL)
			{
				s->head=n;
			}
			else
			{
				bc->down=n;
			}
			s->last=n;
			q=newproduct(pname,quantity, newprice);
			if(q!=NULL)
			{
				n->next=n->tail=q;
				b=YES;
			}
		}
	}
	return b;
}
void printproduct(product_type *p)
{
	product_type *m=p;
	if(m!=NULL)
	{
		printf("	product :     %s\n",m->product_name);
		printf("	price:        %f\n",m->price);
		printf("	availability: %d\n",m->availability);
		printproduct(m->next1);
	}
}
void printcategory( categoty_type *p )
{
	categoty_type *n=p;
	product_type *p1=p->next;
	if(n!=NULL);	
	{
		printf("- %s\n",n->category);
		printproduct(p1);
		printcategory(n->down);
	}
}
void print(store *s)
{
	categoty_type *p=s->head;
	printcategory(p);
}	
void ini(ctype *c)
{
	c->head=c->tail=NULL;
}
void history(orders *o)
{
	if(o!=NULL)
	{
		printf("%s",o->ctg);
		printf("%s",o->product_name);
		printf("%d",o->quantity);
		printf("%f",o->charge);
		history(o->no);
	}
}
void wishL(wish *w)
{
	if(w!=NULL)
	{
		printf("%s",w->productname);
		 wishL(w->nextlist);
	}
}
void printuser(custmer_database *c)
{
	printf("user name:");
	printf(" %s\n",c->name);
	printf("phone no:");
	printf(" %ld\n",c->phone);
	printf("Address:");
	printf(" %s\n",c->address);
}
boolean addToWishList(wish **w,char p[])
{
	wish *n,*p1;
	boolean b=NO;
	n=(wish*)malloc(sizeof(wish));
	n->nextlist=NULL;
	strcpy(n->productname,p);
	p1=*w;
	if(p1==NULL)
	{
		b=YES;
		*w=n;
	}
	else
	{
		while(p1->nextlist!=NULL)
		{
			p1=p1->nextlist;
		}
		p1->nextlist=n;
		b=YES;
	}
	return b;
}
boolean ReturnProduct(store *s,orders **o,char pd[],char ct[])
{
	orders *p,*pr;
	boolean b,b1=NO;
	p=*o;
	if(p!=NULL)
	{
		while(strcmp(p->product_name,pd)!=0 && p!=NULL)
		{
			pr=p;
			p=p->no;
		}
		if(p!=NULL)
		{
			pr->no=p->no;
			categoty_type *cr;
			b=iscategoryOf(ct,s->head,&cr);
			if(b==YES)
			{
				product_type *p2,*p1;
				p2=cr->next;
				b=isProductOf(pd,p2,&p1);
				p1->availability=p1->availability+p->quantity;
				b1=YES;
			}
		}
	}
	return b1;
}
		
orders* orderit(char ct[] ,char pn[],int q,float c)
{
	orders *o;
	o=(orders*)malloc(sizeof(orders));
	strcpy(o->product_name,pn);
	strcpy(o->ctg,ct);
	o->quantity=q;
	o->charge=c;
	o->no=NULL;
	return o;
}
custmer_database* addcustmer(char name[],char add[],long int mb,char pm[])
{
	custmer_database *n;
	n=(custmer_database*)malloc(sizeof(custmer_database));
	n->wishlist=NULL;
	n->h=n->t=n->favoutate=NULL;
	strcpy(n->name,name);
	strcpy(n->address,add);
	strcpy(n->payment_mode,pm);
	n->phone=mb;
	return n;
}
custmer_list* custmernode(char uid[],char pass[],char email[])
{
	custmer_list *n;
	n=(custmer_list*)malloc(sizeof(custmer_list));
	n->next=NULL;
	n->custmer=NULL;
	strcpy(n->user_Id,uid);
	strcpy(n->password,pass);
	strcpy(n->E_mail_address,email);
	return n;
}
	
boolean iscustmerRegistered(ctype *c,char custmer[],char email[] ,custmer_list **r)
{
	boolean b=NO;
	custmer_list *p;
	p=c->head;
	while(p!=NULL && b==NO)
	{
		if(strcmp(p->user_Id,custmer)==0 && strcmp(p->E_mail_address,email)==0)
		{
			b=YES;
			*r=p;
			printuser(p->custmer);
		}
		else
		{
			p=p->next;
		}
	}
	return b;
}
void resetPassword(custmer_list *c)
{
	char pass1[20],pass2[20];
	printf("Enter new password:\n");
	scanf("%s",pass1);
	int a=0;
	while(a==0)
	{
		printf("Re-ente password:\n");
		scanf("%s",pass2);
		if(strcmp(pass1,pass2)==0)
		{
			a=1;
			strcpy(c->password,pass1);
			printf("password reseted successfully\n");
		}
		else
		{
			printf("re-enter the same password\n");
		}
	}
}
void shopping(custmer_list *p,store *s)
{
	int e=1;
	while(e==1)
	{
		int a;
		printf("1. HOME\n2. PROFILE\n3. PLACE ORDER\n4. ORDER HISTORY \n5. WISHLIST \n6. FAVOURATE PURCHASE\n");
		scanf("%d",&a);
		switch(a)
		{
			case 1 :
				{
					print(s);
				}break;
			case 2 :
				{
					printf("user id:");
					printf(" %s\n",p->user_Id);
					printf("email:");
					printf(" %s\n",p->E_mail_address);
					printf("password:");
					printf(" %s\n",p->password);
					custmer_database *q;
					q=p->custmer;
					printuser(q);
				}break;
		
			case 3 :
				{
					print(s);
					char ct[20],pd[20];
					int r;
					custmer_database *cd,*n;
					n=cd=p->custmer;
					orders *or;
					printf("choose category(enter category) \n");
					scanf("%s",ct);
					printf("choose product \n");
					scanf("%s",pd);
					printf("enter quantity \n");
					scanf("%d",&r);
					boolean b;
					categoty_type *cr;
					b=iscategoryOf(ct,s->head,&cr);
					if(b==YES)
					{
						product_type *p2,*p1;
						p2=cr->next;
						b=isProductOf(pd,p2,&p1);
						if(b==YES)
						{
							p1->availability=p1->availability-r;
							if(p1->availability>=0)
							{
								orders *o,*o1;
								float rate=(float)r*p1->price;
								o=orderit(ct,pd,r,rate);
								if(o!=NULL)
								{
									o1=n->t;
									if(o1==NULL){
										n->h=n->t=n->favoutate=o;
									}
									else
									{
										o1->no=n->t=o;
									}
								}
								else
								{
									printf("order cannot be placed plz try again\n");
								}
							}
							else
							{
								p1->availability=p1->availability+r;
								printf("we have shortage of quantity\n");
							}
							
						}
						else
						{
							printf("this product is not available\n");
						}
					}
					else
					{
						printf("this product is not available\n");
					}	
						
				} break;
					
			case 4 :
				{
					custmer_database *q;
					q=p->custmer;
					orders *o;
					o=q->h;
					history(o);
					char c;
					printf("do you want to return any product y/n \n");
					scanf("%c",&c);
					if(c=='y')
					{
						char pd[20],ct[20];
						printf("Enter category name\n");
						scanf("%s",ct);
						printf("Enter product name\n");
						scanf("%s",pd);
						boolean b;
						b=ReturnProduct(s,&o,pd,ct);
						if(b==YES)
						{
							printf("Product Returned  successfully\n");
						}
						else
						{
							printf("product cannot be returned/this product is not in your order history try again\n");
						}
					}
				}break;
		
			case 5 :
				{
					custmer_database *q;
					q=p->custmer;
					wishL(q->wishlist);
					char c;
					printf("Edit  wish list y/n \n");
					scanf("%c",&c);
					if(c=='y')
					{	
						wish *w;
						w=q->wishlist;
						print(s);
						char pd[20];
						printf("choose product \n");
						scanf("%s",pd);
						boolean b;
						b=addToWishList(&w,pd);
						if(b==YES)
						{
							printf("added to wish list successfully\n");
						}
						else
						{
							printf("product cannot added to wishlist\n");
						}
					}
				}break;
	
			case 6 :
				{
					custmer_database *q;
					q=p->custmer;
					orders *o;
					o=q->favoutate;
					printf("%s",o->product_name);
				}break;
	
		}
	printf("press 1 to continue \npress 0 to logout\n");
	scanf("%d",&e);
	}
}
void signINUP(ctype *c,store *s)
{
	int x=1;
	while(x==1)
	{
		boolean b;
		int a;
		char userid[20],email[30],pass[20];
		printf("1. signIn  \n2. register  \n");
		scanf("%d",&a);
		printf("Enter user Id :\n");
		scanf("%s",userid);
		printf("Enter email address :\n");
		scanf("%s",email);
		switch(a)
		{
			case 1 :
				{
					custmer_list *p;
					b=iscustmerRegistered(c,userid,email,&p);
					printf("%s\n",p->password);
					if(b==YES)
					{
						int found=0;
						while(found<3)
						{
							
							printf("Enter password:\n");
							scanf("%s",pass);
							if(strcmp(p->password,pass)==0)
							{
								found=4;
								printf("you are successfully logged in welcome to online bajar \n");
								shopping(p,s);
							}
							else
							{
								printf("incorrect password\n");
								found++;;
							}
						}
					if(found==3)
					{
						printf("1. forgot password \n2. try again\n");
						int a;
						scanf("%d",&a);
						if(a==1)
						{
							resetPassword(p);
						}
					
					}}
					
					else
					{
						printf("User is not registered please register/enter a valid userId and email\n");
					}
				}break;
		
			case 2 :
				{
					custmer_list *p,*n;
					custmer_database *q;
					b=iscustmerRegistered(c,userid,email,&p);
					
					if(b==YES)
					{
						printf("User is already registered go to sign in\n");
					}
					else
					{
						char pass[20], name[20],address[50],payment[20];
						long int phone;
						printf("Enter password\n");
						scanf("%s",pass);
						p=custmernode(userid,pass,email	);
						printf("%s\n%s\n%s\n",p->user_Id,p->E_mail_address,p->password);
						printf("Enter your name:\n");
						scanf("%s",name);
						printf("Enter your phone no:\n");
						scanf("%ld",&phone);
						printf("Enter your address:\n");
						scanf("%s",address);
						printf("Enter your payment mod:\n	1.buy now pay later BNPL\n	2.cash on delivery COD \n	3.Debit card \n	4.credit card \n	5.Net banking \n	6.UPI enabled \n	7.PhonePe\n");
						scanf("%s",payment);
						q=addcustmer(name,address,phone,payment);
						p->custmer=q;
						n=c->tail;
						if(n==NULL)
						{
							c->tail=c->head=p;
						}
						else
						{
							n->next=c->tail=p;
						}
						shopping(p,s);
									
					}
				}break;
		}
	printf("press 1 to continue \npress 0 to exit\n");
	scanf("%d",&x);
	}
}
