#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
//#include"custmer.c"
#include"f.c"
int main()
{
	store n;
	inIt(&n);
	printf("Welcome to our Online_Bazar\n");
	int a=1;
	while(a==1)
	{
		int b;
		printf("1. Add New Products to store\n2. Shopping \n");	
		scanf("%d",&b);
		switch(b)
		{
			case 1 :
				{
					
					
					boolean b;
					int i,j=0;
					printf("Enter no of entry you want to enter\n");
					scanf("%d",&i);
					while(j<i)
					{
						b=addnewproduct(&n);
						if(b==YES)
						{
							printf("Entry successfull\n");
						}
						j++;
					}
				}break;
			case 2 :
				{
					ctype c;
					ini(&c);
					signINUP(&c,&n);
				}break;
		}
	}
	return 0;
}
