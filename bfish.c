/****************************************************************************
FILE          : bfish.c
LAST REVISION : 2008-09-15
SUBJECT       : File encryption program using Blowfish.
PROGRAMMER    : (C) Copyright 2008 by Peter C. Chapin

This program allows the user to encrypt or decryption a file using the
Blowfish algorithm.

To Do:

+ Consider using a better way to convert pass phrases into keys.

Please send comments or bug reports to

     Peter C. Chapin
     Computer Information Systems
     Vermont Technical College
     Randolph Center, VT 05061
     Peter.Chapin@vtc.vsc.edu
****************************************************************************/

// Standard
#include <stdio.h>
#include <string.h>

// Unix specific
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

// OpenSSL
#include <openssl/blowfish.h>

#define BUFFER_SIZE 4096

extern int optind;

char buffer[BUFFER_SIZE];
int  count;

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
  unsigned char raw_key[16];
  BF_KEY        key;
  unsigned char IV[8];
  int           IV_index;

  while ((option = getopt(argc, argv, "ed")) != -1) {
    switch (option) {
      case 'e': do_encrypt = 1; direction = BF_ENCRYPT; break;
      case 'd': do_decrypt = 1; direction = BF_DECRYPT; break;
    }
  }

  if (argc - optind != 3) {
    fprintf(stderr,
      "Usage: %s -e|-d infile outfile \"pass phrase\"\n", argv[0]);
    return 1;
  }

  if ((do_encrypt == 1 && do_decrypt == 1) ||
      (do_encrypt == 0 && do_decrypt == 0)) {
    fprintf(stderr, "Exactly one of -e or -d must be specified.\n");
    return 1;
  }

  // Prepare the key.
  strncpy(raw_key, argv[optind + 2], 16);
  BF_set_key(&key, 16, raw_key);

  // Prepare the IV.
  memset(IV, 0, 8);
  IV_index = 0;

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
  
  while ((count = read(in, buffer, BUFFER_SIZE)) > 0) {
    BF_cfb64_encrypt(buffer, buffer, count, &key, IV, &IV_index, direction);
    if (write(out, buffer, count) != count) {
      perror("Error writing output");
      break;
    }
  }
  
  close(in);
  close(out);
  
  return 0;
}
