/*
 * Include the provided hash table library.
 */
#include "hashtable.h"

/*
 * Include the header file.
 */
#include "philphix.h"

/*
 * Standard IO and file routines.
 */
#include <stdio.h>

/*
 * General utility routines (including malloc()).
 */
#include <stdlib.h>

/*
 * Character utility routines.
 */
#include <ctype.h>

/*
 * String utility routines.
 */
#include <string.h>

/*
 * This hash table stores the dictionary.
 */
HashTable *dictionary;

/*
 * The MAIN routine.  You can safely print debugging information
 * to standard error (stderr) as shown and it will be ignored in 
 * the grading process.
 */
#ifndef _PHILPHIX_UNITTEST
int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Specify a dictionary\n");
    return 1;
  }
  /*
   * Allocate a hash table to store the dictionary.
   */
  fprintf(stderr, "Creating hashtable\n");
  dictionary = createHashTable(0x61C, &stringHash, &stringEquals);

  fprintf(stderr, "Loading dictionary %s\n", argv[1]);
  readDictionary(argv[1]);
  fprintf(stderr, "Dictionary loaded\n");

  fprintf(stderr, "Processing stdin\n");
  processInput();

  /*
   * The MAIN function in C should always return 0 as a way of telling
   * whatever program invoked this that everything went OK.
   */
  return 0;
}
#endif /* _PHILPHIX_UNITTEST */

/* Task 3 */
void readDictionary(char *dictName) {
  FILE *fp = fopen(dictName, "r");
  if (fp == NULL) {
    fprintf(stderr, "dictName file not found!\n");
    exit(61);
  }

  int isEndLine = 0, readKey = 1;
  int size = 60, key_size = 0, data_size = 0;
  char *key = (char *) calloc(size, 1), *keyp = key;
  char *data = (char *) calloc(size, 1), *datap = data;
  char c = 0;

  while (!feof(fp)) {
    if (isEndLine) {
      insertData(dictionary, key, data);
      // free(key);  // 释放旧内存
      // free(data); // 释放旧内存
      key = (char *) calloc(size, 1);
      keyp = key;
      data = (char *) calloc(size, 1);
      datap = data;
      key_size = data_size = 0; // 重置大小
      isEndLine = 0;
    }

    c = fgetc(fp);

    if (c == ' ' || c == '\t' || c == '\n') {
      readKey = 0;
      if (c == '\n') {
        isEndLine = 1;
        readKey = 1;
      }
      continue;
    }

    if (readKey) {
      if (key_size >= size - 1) { // 保留空字符位置
        char *new_key = (char *) realloc(key, size * 2);
        // if (new_key == NULL) {
        //   free(key);
        //   free(data);
        //   fprintf(stderr, "Memory allocation failed!\n");
        //   exit(1);
        // }
        key = new_key;
        keyp = key + key_size;
        memset(keyp, 0, size); // 初始化新内存
        size *= 2;
      }
      *keyp++ = c;
      key_size++;
    } else {
      if (data_size >= size - 1) { // 保留空字符位置
        char *new_data = (char *) realloc(data, size * 2);
        // if (new_data == NULL) {
        //   free(key);
        //   free(data);
        //   fprintf(stderr, "Memory allocation failed!\n");
        //   exit(1);
        // }
        data = new_data;
        datap = data + data_size;
        memset(datap, 0, size); // 初始化新内存
        size *= 2;
      }
      *datap++ = c;
      data_size++;
    }
  }

  if (key_size > 0 || data_size > 0) { // 检查是否有最后一个数据没有插入
    *(datap - 1) = 0;  // 最后读入eof(-1)
    insertData(dictionary, key, data);
  }

  fclose(fp);
  // free(key); // 释放内存
  // free(data); // 释放内存
}


char *strLower(char *str, int size) {
  char *buf = (char *) calloc(size, 1);
  memcpy(buf, str, strlen(str));
  char *p = buf;
  while (*p) {
    *p = tolower(*p);
    p++;
  }
  return buf;
}

int isAlphaStr(char *str) {
  while (*str) {
    if (!isalpha(*str)) {
      return 0;
    }
    str++;
  }
  return 1;
}

int isAlphaNum(char c) {
  if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
    return 1;
  }
  return 0;
}

void replace(char *input, int size) {
  char *output = findData(dictionary, input);
  if (output) {
    ;
  } else {
    if (isAlphaStr(input)) {
      char *lowerStr = strLower(input, size);
      lowerStr[0] = input[0];
      output = findData(dictionary, lowerStr);
      if (output) {
        ;
      } else {
        lowerStr[0] = tolower(lowerStr[0]);
        output = findData(dictionary, lowerStr);
      }
      free(lowerStr);
      lowerStr = NULL;
    }
  }
  if (output != NULL) {
    fputs(output, stdout);
  } else {
    fputs(input, stdout);
  }
  free(input);
}

/* Task 4 */
void processInput() {
  int size = 60, init_size = 60;
  int isEnd = 0;
  char *input = (char *) calloc(init_size, 1), *p = input;
  char c;
  while ((c = fgetc(stdin)) != -1) {
    if (isAlphaNum(c)) {
      isEnd = 0;
      
      if (strlen(input) >= size) {
        input = (char *) realloc(input, size * 2);
        p = input + size;
        memset(p, 0, size);
        size *= 2;
      }
      *p++ = c;
    } else {
      if (strlen(input) > 0) {
        replace(input, size);
        input = (char *) calloc(init_size, 1);
        p = input;
        size = 60;
      }
      fputc(c, stdout);
      isEnd = 1;
    }
  }

  if (!isEnd) {
    replace(input, size);
  } else {
    free(input);
    input = NULL;
  }
}
