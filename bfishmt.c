/****************************************************************************
FILE          : bfishmt.c
LAST REVISION : 2008-09-15
SUBJECT       : File encryption program using Blowfish.
PROGRAMMER    : (C) Copyright 2008 by Peter C. Chapin

This program allows the user to encrypt or decryption a file using the
Blowfish algorithm. It is multithreaded and attempts to overlap the
file I/O operations with the encryption operation.

To Do:

+ Add error handling on the dynamic memory allocation functions.
  
+ Consider using a better way to convert pass phrases into keys.

Please send comments or bug reports to

     Peter C. Chapin
     Computer Information Systems
     Vermont Technical College
     Randolph Center, VT 05061
     pchapin@ecet.vtc.edu
****************************************************************************/

// Standard
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Unix specific
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <pthread.h>
#include "pcbuffer.h"

// OpenSSL
#include <openssl/blowfish.h>

#define BUFFER_SIZE 4096

extern int optind;

// This should be passed as an argument to the encryptor thread. I'm
// making it global because I'm too lazy to set up a structure, etc.
//
unsigned char raw_key[16];

// This should also be passed as an argument to the threads (all
// threads). My laziness knows no bounds!
//
int do_verbose = 0;

// Used to hold information about a chunk of data from the file.
struct file_chunk {
  unsigned char buffer[BUFFER_SIZE];
  int count;
  int ID;
};

pcbuffer incoming;
pcbuffer outgoing;

void *reader_thread(void *arg)
{
  int *in = (int *)arg;
  int  counter = 1;
  struct file_chunk *current;

  current = malloc(sizeof(struct file_chunk));
  current->ID = counter++;
  while ((current->count = read(*in, current->buffer, BUFFER_SIZE)) > 0) {
    if (do_verbose) {
      printf("Pushing incoming chunk of size %4d (ID=%04d)\n",
              current->count, current->ID);
    }
    pcbuffer_push(&incoming, current);

    // Get next chunk structure ready.
    current = malloc(sizeof(struct file_chunk));
    current->ID = counter++;
  }

  // Add the zero sized chunk to mark end-of-file.
  if (do_verbose) {
    printf("Pushing incoming chunk of size %4d (ID=%04d)\n",
            current->count, current->ID);
  }
  pcbuffer_push(&incoming, current);

  return NULL;
}


void *encryptor_thread(void *arg)
{
  BF_KEY        key;
  unsigned char IV[8];
  int           IV_index;
  int           direction = *(int *)arg;
  struct file_chunk *current;

  // Prepare the key.
  BF_set_key(&key, 16, raw_key);

  // Prepare the IV.
  memset(IV, 0, 8);
  IV_index = 0;

  current = pcbuffer_pop(&incoming);
  while (current->count != 0) {

    // Do the deed.
    BF_cfb64_encrypt(current->buffer,
                     current->buffer,
                     current->count,
                     &key,
                     IV,
                     &IV_index,
                     direction);

    if (do_verbose) {
      printf("Pushing outgoing chunk of size %4d (ID=%04d)\n",
              current->count, current->ID);
    }
    pcbuffer_push(&outgoing, current);

    // Get next chunk.
    current = pcbuffer_pop(&incoming);
  }

  // Send the zero sized chunk on to the next stage.
  if (do_verbose) {
    printf("Pushing outgoing chunk of size %4d (ID=%04d)\n",
            current->count, current->ID);
  }
  pcbuffer_push(&outgoing, current);

  return NULL;
}


void *writer_thread(void *arg)
{
  int *out = (int *)arg;
  struct file_chunk *current;

  current = pcbuffer_pop(&outgoing);
  while (current->count != 0) {
    write(*out, current->buffer, current->count);
    if (do_verbose) {
      printf("Wrote outgoing chunk of size %4d to disk (ID=%04d)\n",
              current->count, current->ID);
    }

    free(current);

    // Get next chunk.
    current = pcbuffer_pop(&outgoing);
  }

  if (do_verbose) {
    printf("Writer terminated on chunk of size %4d (ID=%04d)\n",
            current->count, current->ID);
  }

  // Release the end-of-file marker chunk.
  free(current);

  return NULL;
}


// ============
// Main Program
// ============

int main(int argc, char **argv)
{
  int  option;
  int  do_encrypt = 0;
  int  do_decrypt = 0;
  int  direction;
  int  in;            // Input file handle.
  int  out;           // Output file handle.
  pthread_t     reader_ID, encryptor_ID, writer_ID;
  
  while ((option = getopt(argc, argv, "edv")) != -1) {
    switch (option) {
      case 'e': do_encrypt = 1; direction = BF_ENCRYPT; break;
      case 'd': do_decrypt = 1; direction = BF_DECRYPT; break;
      case 'v': do_verbose = 1; break;
    }
  }

  if (argc - optind != 3) {
    fprintf(stderr,
      "Usage: %s -e|-d [-v] infile outfile \"pass phrase\"\n", argv[0]);
    return 1;
  }

  if ((do_encrypt == 1 && do_decrypt == 1) ||
      (do_encrypt == 0 && do_decrypt == 0)) {
    fprintf(stderr, "Exactly one of -e or -d must be specified.\n");
    return 1;
  }

  // Prepare the key.
  strncpy(raw_key, argv[optind + 2], 16);

  // Open the files.
  if ((in = open(argv[optind + 0], O_RDONLY)) == -1) {
    perror("Error opening input file");
    return 1;
  }
  if ((out = open(argv[optind + 1], O_WRONLY|O_CREAT|O_TRUNC, 0666)) == -1) {
    perror("Error opening output file");
    close(in);
    return 1;
  }

  // Initialize the producer/consumer buffers.  
  pcbuffer_init(&incoming);
  pcbuffer_init(&outgoing);

  // Create the threads.
  pthread_create(&reader_ID, NULL, reader_thread, &in);
  pthread_create(&encryptor_ID, NULL, encryptor_thread, &direction);
  pthread_create(&writer_ID, NULL, writer_thread, &out);

  // Wait for them to terminate.
  pthread_join(reader_ID, NULL);
  pthread_join(encryptor_ID, NULL);
  pthread_join(writer_ID, NULL);

  // Clean up.
  pcbuffer_destroy(&outgoing);
  pcbuffer_destroy(&incoming);
  close(in);
  close(out);
  
  return 0;
}
