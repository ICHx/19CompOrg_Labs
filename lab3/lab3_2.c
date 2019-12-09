#include<stdio.h>
#include <stdlib.h>
#include <sys/time.h>
int main()
{
	struct timeval a;
		FILE *my_file_pointer;
	if ( (my_file_pointer = fopen("lab3_2_out.txt", "w")) == NULL)
	{
		printf("Error opening the file, so exiting\n");
		exit(1);
	}


	printf(" unsigned int data type is %d bytes or %d bits long\n",sizeof(unsigned int), sizeof(unsigned int)*8 );
	printf(" double data type is %d bytes or %d bits long\n",sizeof(double), sizeof(double)*8 );
	printf("long data type is %d bytes or %d bits long\n",sizeof(long), sizeof(long)*8 );
	printf("long long data type is %d bytes or %d bits long\n",sizeof(long long), sizeof(long long)*8 );
	printf(" char data type is %d bytes or %d bits long\n",sizeof(char), sizeof(char)*8 );
	printf("float data type is %d bytes or %d bits long\n",sizeof(float), sizeof(float)*8 );
	printf(" struct timeval data type is %d bytes or %d bits long\n",sizeof(struct timeval), sizeof(struct timeval)*8 );
fclose(my_file_pointer); 
	return 0;
 } 
