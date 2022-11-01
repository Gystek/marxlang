 /*
  * ----------------------------------------------------------------------------
  * "THE BEER-WARE LICENSE" (Revision 42):
  * <gustek@riseup.net> wrote this file.  As long as you retain this notice you
  * can do whatever you want with this stuff. If we meet some day, and you think
  * this stuff is worth it, you can buy me a beer in return.
  * ----------------------------------------------------------------------------
  */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <limits.h>

#define WORDS (11518)
#define WORD_MAX (46) /* Longest word in English is 45 letters long: Pneumonoultramicroscopicsilicovolcanoconiosis. */

static const char* const dictionnary[WORDS] = {
#include "the_communist_manifesto.txt"
};

static uint8_t usage[WORDS / 8 + 1] = { 0 };

enum {
  LOADA = 0x3,
  LOADB = 0x1,
  READA = 0x4,
  READB = 0x15,
  JUMP  = 0x9,
  VBOP  = 0x2,
  ROT   = 0x6,
  OUT   = 0x5,
  IN    = 0x35,
  HALT  = 0x8
};

#define MEMORY (0xDEAD)
static uint8_t memory[MEMORY] = { 0 };
static uint8_t flippin = 0;
static uint8_t a = 0;
static uint8_t b = 0;

int
run (void)
{
  uint16_t i = 0;

  for (;;i++)
    {
      uint8_t opcode = memory[i];

      switch (opcode)
	{
	case LOADA:
	  a = flippin;
	  break;
	case LOADB:
	  b = flippin;
	  break;
	case READA:
	  flippin = a;
	  break;
	case READB:
	  flippin = b;
	  break;
	case JUMP:
	  if (a)
	    i = (((uint16_t) a) << 8) | b;
	  break;
	case VBOP:
	  switch (i % 3)
	    {
	    case 0:
	      flippin = a ^ b;
	      break;
	    case 1:
	      flippin = a | b;
	      break;
	    default:
	      flippin = ~(a & b);
	      break;
	    }
	  break;
	case ROT:
	  if ((i % 6) & 1)
	    a = (a << 1) | (a & 010000000);
	  else
	    a = (a >> 1) | (a & 1);
	  break;
	case OUT:
	  putchar (flippin);
	  break;
	case IN:
	  a = getchar ();
	  break;
	case HALT:
	  return 0;
	}

      if (i + 1 >= MEMORY)
	{
	  fputs ("Out of memory\n", stderr);
	  return -1;
	}
    }

  return 0;
}

static uint16_t instr = 0;
static uint8_t prev = 0;

void
unlock (word)
     const char *word;
{
  size_t i = 0;

  for (; i < WORDS; i++)
    {
      if (!strcmp (word, dictionnary[i]))
	usage[i / 8] &= ~(1 << (i % 8));
    }
}


long
search (word)
     const char *word;
{
  size_t i = 0;
  long f = -1;

  for (; i < WORDS; i++)
    {
      if (!strcmp (word, dictionnary[i]))
	{
	  if ((usage[i / 8] >> (i % 8)) & 1)
	    {
	      if (f == -1) f = i;
	    }
	  else
	    {
	      usage[i / 8] |= (1 << (i % 8));
	      return i;
	    }
	}
    }

  if (f != -1)
    {
      unlock (word);
      usage[f / 8] |= (1 << (f % 8));
    }

  return f;
}

int
compile_instr (word)
     const char *word;
{
  long gd = search (word);
  uint8_t v = 0;

  if (gd == -1)
    {
      fprintf (stderr, "unknown instruction: '%s'\n", word);
      return -1;
    }

  v = (instr % 2) ? (gd >> 8) : gd;
  v ^= (uint8_t) ((instr >> 8) + (instr & 255));
  v += (prev & v);
  v %= 112;

  memory[instr] = v;

  switch (v)
    {
    case LOADA:
    case LOADB:
    case READA:
    case READB:
    case JUMP:
    case VBOP:
    case ROT:
    case OUT:
    case IN:
    case HALT:
      instr++;
      break;
    }

  prev = v;

  return 0;
}

int
compile (fi)
     FILE *fi;
{
  char c;
  char word[WORD_MAX] = { 0 };

  instr = 0;

  while (c = fgetc (fi), c != EOF)
    {
      if (isalnum(c))
	word[strlen(word)] = toupper(c);
      else
	{
	  if (compile_instr (word))
	    return 1;
	  memset (word, 0, WORD_MAX * sizeof(char));

	  while (c = fgetc (fi), !isalnum(c) && c != EOF)
	    ;

	  word[0] = toupper(c);
	}
    }

  return compile_instr (word);
}

int
main (argc, argv)
     int argc;
     char *const argv[];
{
  int i = 1;

  if (argc == 1)
    {

      fputs ("usage: argv[0] <file>...\n", stderr);
      return EXIT_FAILURE;
    }

  for (; i < argc; i++)
    {
      const char *fname = argv[i];
      FILE *f = (strcmp(fname, "-") == 0 ? stdin : fopen (fname, "r"));

      if (f == NULL)
	perror (fname);
      else
	{
	  if (compile (f))
	    return EXIT_FAILURE;
	}

      if (f != stdin)
	fclose (f);

      if (run ())
	return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
