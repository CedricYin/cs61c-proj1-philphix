#include "hashtable.h"

/*
 * General utility routines (including malloc()).
 */
#include <stdlib.h>

/*
 * Standard IO and file routines.
 */
#include <stdio.h>

/*
 * String utility routines.
 */
#include <string.h>

/*
 * This creates a new hash table of the specified size and with
 * the given hash function and comparison function.
 */
HashTable *createHashTable(int size, unsigned int (*hashFunction)(void *),
                           int (*equalFunction)(void *, void *)) {
  int i = 0;
  HashTable *newTable = malloc(sizeof(HashTable));
  if (NULL == newTable) {
    fprintf(stderr, "malloc failed \n");
    exit(1);
  }
  newTable->size = size;
  newTable->buckets = malloc(sizeof(struct HashBucketEntry *) * size);
  if (NULL == newTable->buckets) {
    fprintf(stderr, "malloc failed \n");
    exit(1);
  }
  for (i = 0; i < size; i++) {
    newTable->buckets[i] = NULL;
  }
  newTable->hashFunction = hashFunction;
  newTable->equalFunction = equalFunction;
  return newTable;
}

/* Task 1.2 */
void insertData(HashTable *table, void *key, void *data) {
  // 1. Find the right hash bucket location with table->hashFunction.
  unsigned int loc = table->hashFunction(key) % table->size;
  // 2. Allocate a new hash bucket entry struct.
  HashBucketEntry *newBucketEntry = malloc(sizeof(HashBucketEntry));
  newBucketEntry->key = key;
  newBucketEntry->data = data;
  newBucketEntry->next = NULL;
  // 3. Append to the linked list or create it if it does not yet exist. 
  if (table->buckets[loc] == NULL) {
    table->buckets[loc] = newBucketEntry;
  } else {
    HashBucketEntry *p = table->buckets[loc];
    while (p->next) {
      p = p->next;
    }
    p->next = newBucketEntry;
  }
}

/* Task 1.3 */
void *findData(HashTable *table, void *key) {
  // 1. Find the right hash bucket with table->hashFunction.
  unsigned int loc = table->hashFunction(key) % table->size;
  // 2. Walk the linked list and check for equality with table->equalFunction.
  HashBucketEntry *p = table->buckets[loc];
  while (p) {
    if (table->equalFunction(key, p->key)) {
      return p->data;
    }
    p = p->next;
  }
  return NULL;
}

/* Task 2.1 */
unsigned int stringHash(void *s) {
  unsigned int seed = 131;
  unsigned int hash = 0;
  while (*((char *) s)) {
    hash = hash * seed + *((char *) s);
    s++;
  }
  return (hash & 0x7FFFFFFF);
}

/* Task 2.2 */
int stringEquals(void *s1, void *s2) {
  return !strcmp(s1, s2);
}