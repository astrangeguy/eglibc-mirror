/* Copyright (C) 1999-2013 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Andreas Jaeger <aj@suse.de>, 1999.

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

#include <math.h>
#include <float.h>
#include <stdio.h>

static int errors = 0;


static void
check (const char *testname, int result)
{
  if (!result) {
    printf ("Failure: %s\n", testname);
    errors++;
  }
}

#define TEST_FUNC(NAME, FLOAT, SUFFIX, EPSILON, HUGEVAL)		      \
static void								      \
NAME (void)								      \
{									      \
  /* Variables are declared volatile to forbid some compiler		      \
     optimizations.  */							      \
  volatile FLOAT Inf_var, qNaN_var, zero_var, one_var;			      \
  FLOAT x1, x2;								      \
									      \
  zero_var = 0.0;							      \
  one_var = 1.0;							      \
  qNaN_var = __builtin_nan ## SUFFIX ("");				      \
  Inf_var = one_var / zero_var;						      \
									      \
  (void) &zero_var;							      \
  (void) &one_var;							      \
  (void) &qNaN_var;							      \
  (void) &Inf_var;							      \
									      \
									      \
  check (#FLOAT " isinf (inf) == 1", isinf (Inf_var) == 1);		      \
  check (#FLOAT " isinf (-inf) == -1", isinf (-Inf_var) == -1);		      \
  check (#FLOAT " !isinf (1)", !(isinf (one_var)));			      \
  check (#FLOAT " !isinf (qNaN)", !(isinf (qNaN_var)));			      \
									      \
  check (#FLOAT " isnan (qNaN)", isnan (qNaN_var));			      \
  check (#FLOAT " isnan (-qNaN)", isnan (-qNaN_var));			      \
  check (#FLOAT " !isnan (1)", !(isnan (one_var)));			      \
  check (#FLOAT " !isnan (inf)", !(isnan (Inf_var)));			      \
									      \
  check (#FLOAT " inf == inf", Inf_var == Inf_var);			      \
  check (#FLOAT " -inf == -inf", -Inf_var == -Inf_var);			      \
  check (#FLOAT " inf != -inf", Inf_var != -Inf_var);			      \
  check (#FLOAT " qNaN != qNaN", qNaN_var != qNaN_var);			      \
									      \
  /*									      \
     the same tests but this time with NAN from <bits/nan.h>		      \
     NAN is a double const						      \
   */									      \
  check (#FLOAT " isnan (NAN)", isnan (NAN));				      \
  check (#FLOAT " isnan (-NAN)", isnan (-NAN));				      \
  check (#FLOAT " !isinf (NAN)", !(isinf (NAN)));			      \
  check (#FLOAT " !isinf (-NAN)", !(isinf (-NAN)));			      \
  check (#FLOAT " NAN != NAN", NAN != NAN);				      \
									      \
  /*									      \
     And again with the value returned by the `nan' function.		      \
   */									      \
  check (#FLOAT " isnan (nan (\"\"))", isnan (nan ## SUFFIX ("")));	      \
  check (#FLOAT " isnan (-nan (\"\"))", isnan (-nan ## SUFFIX ("")));	      \
  check (#FLOAT " !isinf (nan (\"\"))", !(isinf (nan ## SUFFIX (""))));	      \
  check (#FLOAT " !isinf (-nan (\"\"))", !(isinf (-nan ## SUFFIX (""))));     \
  check (#FLOAT " nan (\"\") != nan (\"\")",				      \
	 nan ## SUFFIX ("") != nan ## SUFFIX (""));			      \
									      \
  /* test if EPSILON is ok */						      \
  x1 = 1.0;								      \
  x2 = x1 + EPSILON;							      \
  check (#FLOAT " 1 != 1+EPSILON", x1 != x2);				      \
									      \
  x1 = 1.0;								      \
  x2 = x1 - EPSILON;							      \
  check (#FLOAT " 1 != 1-EPSILON", x1 != x2);				      \
									      \
  /* test if HUGE_VALx is ok */						      \
  x1 = HUGEVAL;								      \
  check (#FLOAT " isinf (HUGE_VALx) == +1", isinf (x1) == +1);		      \
  x1 = - HUGEVAL;							      \
  check (#FLOAT " isinf (-HUGE_VALx) == -1", isinf (x1) == -1);		      \
}

#define TEST_TRUNC(NAME, FLOAT, DOUBLE, SUFFIX)				      \
void									      \
NAME (void)								      \
{									      \
  volatile DOUBLE Inf_var, qNaN_var, zero_var, one_var;			      \
  FLOAT x1, x2;								      \
									      \
  zero_var = 0.0;							      \
  one_var = 1.0;							      \
  qNaN_var = __builtin_nan ## SUFFIX ("");				      \
  Inf_var = one_var / zero_var;						      \
									      \
  (void) &qNaN_var;							      \
  (void) &Inf_var;							      \
									      \
  x1 = (FLOAT) qNaN_var;						      \
  check (" "#FLOAT" x = ("#FLOAT") ("#DOUBLE") qNaN", isnan (x1) != 0);	      \
  x2 = (FLOAT) Inf_var;							      \
  check (" "#FLOAT" x = ("#FLOAT") ("#DOUBLE") Inf", isinf (x2) != 0);	      \
}

TEST_FUNC (float_test, float, f, FLT_EPSILON, HUGE_VALF)
TEST_FUNC (double_test, double, , DBL_EPSILON, HUGE_VAL)
TEST_TRUNC (truncdfsf_test, float, double, )
#ifndef NO_LONG_DOUBLE
TEST_FUNC (ldouble_test, long double, l, LDBL_EPSILON, HUGE_VALL)
TEST_TRUNC (trunctfsf_test, float, long double, l)
TEST_TRUNC (trunctfdf_test, double, long double, l)
#endif

int
do_test (void)
{
  float_test ();
  double_test ();
  truncdfsf_test();

#ifndef NO_LONG_DOUBLE
  ldouble_test ();
  trunctfsf_test();
  trunctfdf_test();
#endif

  return errors != 0;
}

#define TEST_FUNCTION do_test ()
#include "../test-skeleton.c"
