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
typedef struct{
	int low;
	int high;
}input;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
bool readArray(FILE * fp);
bool writeResult(FILE * fp);
void mergeArray(int low, int mid, int high);
void * threadMerge(void * args);
int size = 0;
clock_t timee;
//int threadCounter = 0;
int * array = NULL;

int main(int argc, const char * argv[]) {
	// insert code here...
	FILE *fp = NULL;
	int i;
	if (readArray(fp))
	{
		pthread_t threads;
		input * in = malloc(sizeof(input));
		in->high = size -1;
		in->low = 0;
		timee = clock();
		pthread_create(&threads, NULL, threadMerge, in);
		pthread_join(threads, NULL);
		timee = clock() - timee;
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
		fscanf(fp, "%d", &size);
		array = malloc(size * sizeof(int));
		for (i=0;i<size;i++)
		{
			fscanf(fp, "%d", (array+ i));
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
	}

	k = low;
	i = j = 0;
	while (i < sizeLeft && j < sizeRight)
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
	if (low < high)
	{
		int mid = (low + high)/2;
		pthread_t thread1, thread2;
		input * in1 = malloc(sizeof(input));
		in1->low = low;
		in1-> high = mid;
		pthread_create(&thread1, NULL, threadMerge, in1);
		input * in2 = malloc(sizeof(input));
		in2-> low = mid +1;
		in2-> high = high;
		pthread_create(&thread2, NULL, threadMerge, in2);
		pthread_join(thread1, NULL);
		pthread_join(thread2, NULL);
		pthread_mutex_lock(&lock);
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
		fprintf(fp, "\n%0.2f CPU cycles\n", (float)timee);
		
		
		
		return true;
	}
	else return false;
}
