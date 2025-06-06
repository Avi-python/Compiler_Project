/* ANSI-C code produced by gperf version 3.1 */
/* Command-line: gperf -S 1 -t -P -N is_resword -c reswords.gperf  */
/* Computed positions: -k'1' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gperf@gnu.org>."
#endif

#line 3 "reswords.gperf"

#include <string.h> // Needed for strcmp
#include "tokens.h"
#line 8 "reswords.gperf"
struct resword { const char *name; int type; };

#define TOTAL_KEYWORDS 8
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 6
#define MIN_HASH_VALUE 2
#define MAX_HASH_VALUE 10
/* maximum key range = 9, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (register const char *str, register size_t len)
{
  static unsigned char asso_values[] =
    {
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11,  0,
       5,  5, 11, 11, 11,  0, 11, 11, 11, 11,
      11, 11, 11, 11,  0, 11, 11, 11, 11,  5,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11
    };
  return len + asso_values[(unsigned char)str[0]];
}

struct stringpool_t
  {
    char stringpool_str0[sizeof("if")];
    char stringpool_str1[sizeof("int")];
    char stringpool_str2[sizeof("char")];
    char stringpool_str3[sizeof("const")];
    char stringpool_str4[sizeof("return")];
    char stringpool_str5[sizeof("do")];
    char stringpool_str6[sizeof("else")];
    char stringpool_str7[sizeof("while")];
  };
static struct stringpool_t stringpool_contents =
  {
    "if",
    "int",
    "char",
    "const",
    "return",
    "do",
    "else",
    "while"
  };
#define stringpool ((const char *) &stringpool_contents)
struct resword *
is_resword (register const char *str, register size_t len)
{
  static struct resword wordlist[] =
    {
#line 14 "reswords.gperf"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str0, IF},
#line 11 "reswords.gperf"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str1, INT},
#line 12 "reswords.gperf"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str2, CHAR},
#line 18 "reswords.gperf"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str3, CONST},
#line 13 "reswords.gperf"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str4, RETURN},
#line 16 "reswords.gperf"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str5, DO},
#line 15 "reswords.gperf"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str6, ELSE},
#line 17 "reswords.gperf"
      {(int)(size_t)&((struct stringpool_t *)0)->stringpool_str7, WHILE}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register unsigned int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= MIN_HASH_VALUE)
        {
          register struct resword *resword;

          switch (key - 2)
            {
              case 0:
                resword = &wordlist[0];
                goto compare;
              case 1:
                resword = &wordlist[1];
                goto compare;
              case 2:
                resword = &wordlist[2];
                goto compare;
              case 3:
                resword = &wordlist[3];
                goto compare;
              case 4:
                resword = &wordlist[4];
                goto compare;
              case 5:
                resword = &wordlist[5];
                goto compare;
              case 7:
                resword = &wordlist[6];
                goto compare;
              case 8:
                resword = &wordlist[7];
                goto compare;
            }
          return 0;
        compare:
          {
            register const char *s = (int)resword->name + stringpool;

            if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
              return resword;
          }
        }
    }
  return 0;
}
