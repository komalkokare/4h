#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>
#define send_data_tag 2001
#define return_data_tag 2002

int bin_search(int *arr, int key, int start,int end, int id);
int main(int argc, char **argv)
{
	int rank,size;
	int ierr;
	ierr=MPI_Init(&argc, &argv);
	ierr=MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	ierr=MPI_Comm_size(MPI_COMM_WORLD,&size);

	int n;
	MPI_Status status;
	int root=0;
	n=100;
	int isFound,n_per_process,n_in_slave,key_in_slave,start_index;
	int array[n];
	int key=56;
	int arr[n];

	if(rank==root)
	{	
		for(int i=0;i<n;i++)
		{
			arr[i]=i+1;
		}

		n_per_process = n/size;
		printf("Number of processes=%d\nKey=%d",n_per_process,key);

		for(int i=1;i<size;i++)
		{
 			start_index = i*n_per_process;
			ierr=MPI_Send(&n_per_process,1,MPI_INT,i,send_data_tag,MPI_COMM_WORLD);
			ierr=MPI_Send(&arr[start_index],n_per_process,MPI_INT,i,send_data_tag,MPI_COMM_WORLD);
			ierr=MPI_Send(&key,1,MPI_INT,i,send_data_tag,MPI_COMM_WORLD);
		}

		isFound=-1;
		isFound = bin_search(arr,key,0,n_per_process-1,0);
		if(isFound>=0)
		{
			printf("\nElement found by master at %d\n",isFound);
		}
	}
	else
	{
		ierr=MPI_Recv(&n_in_slave,1,MPI_INT,root,send_data_tag,MPI_COMM_WORLD,&status);
		ierr=MPI_Recv(&array,n_in_slave,MPI_INT,root,send_data_tag,MPI_COMM_WORLD,&status);
		ierr=MPI_Recv(&key,1,MPI_INT,root,send_data_tag,MPI_COMM_WORLD,&status);

		isFound=-1;
		isFound=bin_search(array,key,0,n_in_slave,rank);
		if(isFound>=0)
		{
			printf("\nElement found by slave %d at %d\n",rank,rank*n_in_slave+isFound);
		}
	}
	ierr=MPI_Finalize();

	return 0;
}
int bin_search(int arr[], int key, int start,int end, int id)
{
	while(start<=end)
	{
		int mid=(start+end)/2;
		if(arr[mid]==key)
		{
			return mid;
		}
		if(arr[mid]>key)
		{
			end=mid-1;
		}
		else 
		{
			start=mid+1;
		}
	}
	return -1;
}
