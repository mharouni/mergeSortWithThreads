//
//  main.c
//  mergeSort
//
//  Created by Mohamed Harouni on 10/27/19.
//  Copyright © 2019 Mohamed Harouni. All rights reserved.
//

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
typedef struct{// struct to pass the first and last indecies of the array to be sorted to the merge sort function.
	int low;
	int high;
}input;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;//lock to prevent multiple threads from manipulating the shared variables.
bool readArray(FILE * fp);//reads the unsorted array from the file.
bool writeResult(FILE * fp);// writes the sorted array to the output file
void mergeArray(int low, int mid, int high);//a helper function in merge sort which merges and sorts two sorted subarrays.
void * threadMerge(void * args);//the merge sort msin function which is executed by seperate threads.
int size = 0;
clock_t timee;// variable to calculate time.
//int threadCounter = 0;
int * array = NULL;//pointer to the block of memory which will hold the input.

int main(int argc, const char * argv[]) {
	FILE *fp = NULL;
	int i;
	if (readArray(fp))//does nothing if the readArray fails becsause of inconsistency in file format.
	{
		pthread_t threads;
		input * in = malloc(sizeof(input));// allocate memory for the struct pointer, populate the memory whith the first and last indecies
		in->high = size -1;
		in->low = 0;
		timee = clock();
		pthread_create(&threads, NULL, threadMerge, in);//spawn a thread and call the mergesort to start merginng the array.
		pthread_join(threads, NULL);// forces the main thread to wait for the newly spawned thread to finish execution before continuing its execution
		timee = clock() - timee;// together with the previous time = clock expression the number of cpu cycles it took to complete the task is recorded.
		for ( i = 0; i< size; i++)
		{
			printf("%d\n",array[i]);
		}
		if(writeResult(fp))
			printf("Done\n");
		else
			printf("There was a problem with the file\n");
	}
	else
		printf("Please check the file format\n");

	return 0;
}




bool readArray(FILE * fp)
{
	int i;
	if((fp = fopen("input.txt", "r")))
	{
		fscanf(fp, "%d", &size);// read the size of the given input.
		array = malloc(size * sizeof(int));
		for (i=0;i<size;i++)
		{
			fscanf(fp, "%d", (array+ i));//read the array and populate the memory block allocated to hold it.
		}
		fclose(fp);
		return true;
	}
	else
		return false;
}


void mergeArray(int low, int mid, int high)
{
	int sizeLeft = mid - low + 1;
	int sizeRight = high - mid;
	int left[sizeLeft];
	int right[sizeRight];
	int i, j ,k;

	for (i=0; i<sizeLeft; i++)
	{
		left[i] = array[i+ low];
	}
	for (i=0; i<sizeRight; i++)
	{
		right[i] = array[mid + i +1];
	}//divide the two sorted subarrays into a left and right arrays

	k = low;
	i = j = 0;
	while (i < sizeLeft && j < sizeRight)// rearrange so that to two sub arrays are sorted.
	{
		if (left[i] <= right[j])
		{
			array[k] = left[i];
			k++;
			i++;
		}
		else
		{
			array[k] = right[j];
			k++;
			j++;
		}
	}
	while ( i < sizeLeft)
	{
		array[k] = left[i];
		k++;
		i++;
	}
	while (j < sizeRight)
	{
		array[k] = right[j];
		k++;
		j++;
	}

}



void * threadMerge(void * args)
{

	input * in = (input*) args;
	int low = in->low;
	int high = in->high;
	if (low < high)//terminating condition of the recursive function.
	{
		int mid = (low + high)/2;
		pthread_t thread1, thread2;
		input * in1 = malloc(sizeof(input));// allocate memory for the struct pointer, populate the memory whith the first and last indecies
		in1->low = low;
		in1-> high = mid;
		pthread_create(&thread1, NULL, threadMerge, in1);// spawn a thread to handle the left subarray
		input * in2 = malloc(sizeof(input));
		in2-> low = mid +1;
		in2-> high = high;
		pthread_create(&thread2, NULL, threadMerge, in2);// spawn a thread to handle the right subarray
		pthread_join(thread1, NULL);
		pthread_join(thread2, NULL);// wait for the newly spawned threads to finish before merging (needed to force the multithreaded program to behave as if there was a recursion stack)
		pthread_mutex_lock(&lock);//lock other threads while merging the array.
		mergeArray(low, mid, high);
		pthread_mutex_unlock(&lock);

	}

	return NULL;
}

bool writeResult(FILE * fp)
{
	if ((fp =fopen("output.txt", "w")))
	{
		int i;
		for (i = 0; i< size; i++) {
			fprintf(fp, "%d\t",array[i]);
		}
		fprintf(fp, "\n%lf Seconds\n", (double)timee/CLOCKS_PER_SEC);//prints the sorted array and the cpu cycles it took to the file/



		return true;
	}
	else return false;
}
