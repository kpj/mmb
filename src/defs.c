#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "defs.h"

static struct timespec tp;

void pointPrint(Point point, char *tail) {
	long long int x = point.x - HALFCOMP;
	long long int y = point.y - HALFCOMP;
	long long int z = point.z - HALFCOMP;

	printf("%lli/%lli/%lli%s", x, y, z, tail);
}

void *knalloc(size_t size) {
	/* The german word "Knall" means bang/explosion.
	 * This function's name is thus to be considered a pun. */
	void *mem = malloc(size);
	if (mem == NULL) {
		fprintf(stderr, "malloc() failed. out of memory?\n");
		exit(EXIT_FAILURE);
	}
	return mem;
}

List *listNew() {
	List *list = knalloc(sizeof(List));

	ssize_t size = 8;
	list->mem = knalloc(size * sizeof(void*));
	list->nextFree = list->mem;
	list->end = list->mem + size;

	return list;
}

void listFree(List *list) {
	free(list->mem);
	free(list);
}

void listInsert(List *list, void *ptr) {
	if (list->end <= list->nextFree + 2) {
		off_t freeOffset = list->nextFree - list->mem;
		ssize_t size = (list->end - list->mem) * 2;
		//printf("listInsert(%p): new size is %llu\n", list,
		//		(long long unsigned int)size);

		list->mem = realloc(list->mem, size * sizeof(void*));
		list->end = list->mem + size;
		list->nextFree = list->mem + freeOffset;

		if (list->mem == NULL) {
			fprintf(stderr, "realloc() failed. out of memory?\n");
			exit(EXIT_FAILURE);
		}
	}

	*(list->nextFree) = ptr;
	list->nextFree++;
}

void **listSearch(List *list, void *ptr) {
	// place sentinel
	*(list->nextFree) = ptr;

	void **pos = list->mem;
	for (; ; pos++) {
		if (*pos == ptr) {
			if (pos != list->nextFree)
				return pos;
			else
				return NULL;
		}
	}
	return NULL;
}

long startTimer() {
	clock_gettime(CLOCK_REALTIME, &tp);
	return (tp.tv_sec % 1000) * 1000000 + tp.tv_nsec / 1000;
}

long stopTimer(long start) {
	return (startTimer() + 1000000000 - start) % 1000000000;
}
