/* Test and measure strncat functions.
   Copyright (C) 2011-2013 Free Software Foundation, Inc.
   Contributed by Intel Corporation.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#define TEST_MAIN
#define TEST_NAME "strncat"
#include "test-string.h"

typedef char *(*proto_t) (char *, const char *, size_t);
char *stupid_strncat (char *, const char *, size_t);
char *simple_strncat (char *, const char *, size_t);

IMPL (stupid_strncat, 0)
IMPL (strncat, 2)

char *
stupid_strncat (char *dst, const char *src, size_t n)
{
  char *ret = dst;
  while (*dst++ != '\0');
  --dst;
  while (n--)
    if ( (*dst++ = *src++) == '\0')
      return ret;
  *dst = '\0';
  return ret;
}

static void
do_one_test (impl_t *impl, char *dst, const char *src, size_t n)
{
  size_t k = strlen (dst);
  if (CALL (impl, dst, src, n) != dst)
    {
      error (0, 0, "Wrong result in function %s %p != %p", impl->name,
	     CALL (impl, dst, src, n), dst);
      ret = 1;
      return;
    }

  size_t len = strlen (src);
  if (memcmp (dst + k, src, len + 1 > n ? n : len + 1) != 0)
    {
      error (0, 0, "Incorrect cancatination in function %s",
	     impl->name);
      ret = 1;
      return;
    }
  if (n < len && dst[k + n] != '\0')
    {
      error (0, 0, "There is no zero in the end of output string in %s",
	     impl->name);
      ret = 1;
      return;
    }
  if (HP_TIMING_AVAIL)
    {
      hp_timing_t start __attribute ((unused));
      hp_timing_t stop __attribute ((unused));
      hp_timing_t best_time = ~ (hp_timing_t) 0;
      size_t i;

      for (i = 0; i < 32; ++i)
	{
	  dst[k] = '\0';
	  HP_TIMING_NOW (start);
	  CALL (impl, dst, src, n);
	  HP_TIMING_NOW (stop);
	  HP_TIMING_BEST (best_time, start, stop);
	}

      printf ("\t%zd", (size_t) best_time);
    }
}

static void
do_test (size_t align1, size_t align2, size_t len1, size_t len2,
	 size_t n, int max_char)
{
  size_t i;
  char *s1, *s2;

  align1 &= 7;
  if (align1 + len1 >= page_size)
    return;
  if (align1 + n > page_size)
    return;
  align2 &= 7;
  if (align2 + len1 + len2 >= page_size)
    return;
  if (align2 + len1 + n > page_size)
    return;
  s1 = (char *) (buf1 + align1);
  s2 = (char *) (buf2 + align2);

  for (i = 0; i < len1; ++i)
    s1[i] = 32 + 23 * i % (max_char - 32);
  s1[len1] = '\0';

  for (i = 0; i < len2; i++)
    s2[i] = 32 + 23 * i % (max_char - 32);

  if (HP_TIMING_AVAIL)
    printf ("Length %4zd/%4zd, alignment %2zd/%2zd, N %4zd:",
	    len1, len2, align1, align2, n);

  FOR_EACH_IMPL (impl, 0)
    {
      s2[len2] = '\0';
      do_one_test (impl, s2, s1, n);
    }

  if (HP_TIMING_AVAIL)
    putchar ('\n');
}

static void
do_random_tests (void)
{
  size_t i, j, n, align1, align2, len1, len2, N;
  unsigned char *p1 = buf1 + page_size - 512;
  unsigned char *p2 = buf2 + page_size - 512;
  unsigned char *res;
  fprintf (stdout, "Number of iterations in random test = %zd\n",
	   ITERATIONS);
  for (n = 0; n < ITERATIONS; n++)
    {
      N = random () & 255;
      align1 = random () & 31;
      if (random () & 1)
	align2 = random () & 31;
      else
	align2 = align1 + (random () & 24);
      len1 = random () & 511;
      if (len1 + align2 > 512)
	len2 = random () & 7;
      else
	len2 = (512 - len1 - align2) * (random () & (1024 * 1024 - 1))
	  / (1024 * 1024);
      j = align1;
      if (align2 + len2 > j)
	j = align2 + len2;
      if (len1 + j >= 511)
	len1 = 510 - j - (random () & 7);
      if (len1 >= 512)
	len1 = 0;
      if (align1 + len1 < 512 - 8)
	{
	  j = 510 - align1 - len1 - (random () & 31);
	  if (j > 0 && j < 512)
	    align1 += j;
	}
      j = len1 + align1 + 64;
      if (j > 512)
	j = 512;
      for (i = 0; i < j; i++)
	{
	  if (i == len1 + align1)
	    p1[i] = 0;
	  else
	    {
	      p1[i] = random () & 255;
	      if (i >= align1 && i < len1 + align1 && !p1[i])
		p1[i] = (random () & 127) + 3;
	    }
	}
      for (i = 0; i < len2; i++)
	{
	  buf1[i] = random () & 255;
	  if (!buf1[i])
	    buf1[i] = (random () & 127) + 3;
	}
      buf1[len2] = 0;

      FOR_EACH_IMPL (impl, 1)
	{
	  memset (p2 - 64, '\1', align2 + 64);
	  memset (p2 + align2 + len2 + 1, '\1', 512 - align2 - len2 - 1);
	  memcpy (p2 + align2, buf1, len2 + 1);
	  res = (unsigned char *) CALL (impl, (char *) (p2 + align2),
					(char *) (p1 + align1), N);
	  if (res != p2 + align2)
	    {
	      error (0, 0, "Iteration %zd - wrong result in function %s "
		     "(%zd, %zd, %zd, %zd, %zd) %p != %p",
		     n, impl->name, align1, align2, len1, len2, N,
		     res, p2 + align2);
	      ret = 1;
	    }
	  for (j = 0; j < align2 + 64; ++j)
	    {
	      if (p2[j - 64] != '\1')
		{
		  error (0, 0, "Iteration %zd - garbage before dst, %s "
			 "%zd, %zd, %zd, %zd, %zd)",
			 n, impl->name, align1, align2, len1, len2, N);
		  ret = 1;
		  break;
		}
	    }
	  if (memcmp (p2 + align2, buf1, len2))
	    {
	      error (0, 0, "Iteration %zd - garbage in string before, %s "
		     "(%zd, %zd, %zd, %zd, %zd)",
		     n, impl->name, align1, align2, len1, len2, N);
	      ret = 1;
	    }

	  if ((len1 + 1) > N)
	    j = align2 + N + 1 + len2;
	  else
	    j = align2 + len1 + 1 + len2;
	  for (; j < 512; ++j)
	    {
	      if (p2[j] != '\1')
		{
		  error (0, 0, "Iteration %zd - garbage after, %s "
			 "(%zd, %zd, %zd, %zd, %zd)",
			 n, impl->name, align1, align2, len1, len2, N);
		  ret = 1;
		  break;
		}
	    }
	  if (len1 + 1 > N)
	    {
	      if (p2[align2 + N + len2] != '\0')
		{
		  error (0, 0, "Iteration %zd - there is no zero at the "
			 "end of output string, %s (%zd, %zd, %zd, %zd, %zd)",
			 n, impl->name, align1, align2, len1, len2, N);
		  ret = 1;
		}
	    }
	  if (memcmp (p1 + align1, p2 + align2 + len2,
		      (len1 + 1) > N ? N : len1 + 1))
	    {
	      error (0, 0, "Iteration %zd - different strings, %s "
		     "(%zd, %zd, %zd, %zd, %zd)",
		     n, impl->name, align1, align2, len1, len2, N);
	      ret = 1;
	    }
	}
    }
}

int
main (void)
{
  size_t i, n;

  test_init ();

  printf ("%28s", "");
  FOR_EACH_IMPL (impl, 0)
    printf ("\t%s", impl->name);
  putchar ('\n');

  for (n = 2; n <= 2048; n*=4)
    {
      do_test (0, 2, 2, 2, n, 127);
      do_test (0, 0, 4, 4, n, 127);
      do_test (4, 0, 4, 4, n, 255);
      do_test (0, 0, 8, 8, n, 127);
      do_test (0, 8, 8, 8, n, 127);

      for (i = 1; i < 8; ++i)
	{
	  do_test (0, 0, 8 << i, 8 << i, n, 127);
	  do_test (8 - i, 2 * i, 8 << i, 8 << i, n, 127);
	  do_test (0, 0, 8 << i, 2 << i, n, 127);
	  do_test (8 - i, 2 * i, 8 << i, 2 << i, n, 127);
	}

      for (i = 1; i < 8; ++i)
	{
	  do_test (i, 2 * i, 8 << i, 1, n, 127);
	  do_test (2 * i, i, 8 << i, 1, n, 255);
	  do_test (i, i, 8 << i, 10, n, 127);
	}
    }

  do_random_tests ();
  return ret;
}
