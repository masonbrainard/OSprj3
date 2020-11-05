#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pzip.h"

/**
 * pzip() - zip an array of characters in parallel
 *
 * Inputs:
 * @n_threads:		   The number of threads to use in pzip
 * @input_chars:		   The input characters (a-z) to be zipped
 * @input_chars_size:	   The number of characaters in the input file
 *
 * Outputs:
 * @zipped_chars:       The array of zipped_char structs
 * @zipped_chars_count:   The total count of inserted elements into the zippedChars array.
 * @char_frequency[26]: Total number of occurences
 *
 * NOTE: All outputs are already allocated. DO NOT MALLOC or REASSIGN THEM !!!
 *
 */

pthread_barrier_t barrier;
pthread_mutex_t lock1;
//pthread_mutex_t lock2;

void pzip(int n_threads, char *input_chars, int input_chars_size,
	  struct zipped_char *zipped_chars, int *zipped_chars_count,
	  int *char_frequency)
{
		
	/*
	int n_threads, 			ie 4 threads
	char *input_chars, 		ie ddyyyssssapppzzdddqqqqq
	int input_chars_size 	ie 28
	struct zipped_char *zipped_chars, ie output 
	int *zipped_chars_count, num of different letters
	int *char_frequency[26]  ie [1,0,0,7...]
	*/

	pthread_t tid[n_threads]; 

	//call pthread_barrier()
	pthread_barrier_init(&barrier, NULL, n_threads);
	pthread_mutex_init(&lock1, NULL);
	//pthread_mutex_init(&lock2, NULL);

	struct reader info[n_threads];
	
	//create threads and give places to start
	for(int i = 0; i < n_threads; i = i + 1)
	{
		info[i].input_chars = input_chars + i*(input_chars_size / n_threads);
		info[i].iterate_num = input_chars_size / n_threads;
		info[i].local_zip_char = malloc(info[i].iterate_num*sizeof(struct zipped_char)); // need to free this later
		info[i].char_frequency = char_frequency;
		
		info[i].local_zip_char_size = 0;

		pthread_create(&tid[i], NULL, threaded_zip, (void*) &info[i]);
	}
	//each pthread has completed its count
	// pthread_barrier_wait(&barrier);

	*zipped_chars_count = 0;
	//call pthread_join()
	
	for(int i = 0; i < n_threads; i = i + 1)
	{
		pthread_join(tid[i], NULL);
		for(int j = 0; j < info[i].local_zip_char_size; j = j + 1)
		{
			zipped_chars[*zipped_chars_count + j] = info[i].local_zip_char[j];
		}
		free(info[i].local_zip_char);
		*zipped_chars_count = *zipped_chars_count + info[i].local_zip_char_size;
	}
	pthread_mutex_destroy(&lock1);
	//pthread_mutex_destroy(&lock2);
	/*
	for(int i = 0; i < n_threads; i = i + 1)
	{

	}
	*/	

	return;
}

void* threaded_zip(void *test)
{
	struct reader *info = (struct reader*) test;
	
	//info->local_zip_char = malloc(info->iterate_num*sizeof(struct zipped_char)); // need to free this later

	int occurence = 1;
	int zip_num = 0;

	for(int i = 0; i < info->iterate_num - 1; i++)
	{
		if(info->input_chars[i] == info->input_chars[i + 1])
		{
			occurence = occurence + 1;
		}
		else
		{
			info->local_zip_char[zip_num].character = info->input_chars[i];
			info->local_zip_char[zip_num].occurence = occurence;

			occurence = 1;
			zip_num = zip_num + 1;
		}
	}
	//for last instance which is guarenteed
	info->local_zip_char[zip_num].character = info->input_chars[info->iterate_num - 1];
	info->local_zip_char[zip_num].occurence = occurence;
	zip_num = zip_num + 1;

	info->local_zip_char_size = zip_num;
	
	//pthread_barrier_wait(&barrier);

	
	for(int i = 0; i < zip_num; i = i + 1)
	{
		info->char_frequency[info->local_zip_char[i].character - 97] = info->char_frequency[info->local_zip_char[i].character - 97] + info->local_zip_char[i].occurence;	
	}
	//info->zipped_chars_count = info->zipped_chars_count + zip_num;
	/*
	pthread_barrier_wait(&barrier);
	pthread_mutex_lock(&lock1);
	*info->zipped_chars_count = *info->zipped_chars_count + zip_num;
	pthread_mutex_unlock(&lock1);
	*/
	//pthread_mutex_lock(&lock2);
	
	//pthread_mutex_unlock(&lock2);
	
	//pthread_barrier_wait(&barrier);

	//sleep(0.00002);
	
	return 0;
}
