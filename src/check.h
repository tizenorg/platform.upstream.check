/*-*- mode:C; -*- */
/*
 * Check: a unit test framework for C
 * Copyright (C) 2001, 2002, Arien Malec
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef CHECK_H
#define CHECK_H

#include <stddef.h>
#include <string.h>

#include <check_stdint.h>

/* Check: a unit test framework for C

   Check is a unit test framework for C. It features a simple
   interface for defining unit tests, putting little in the way of the
   developer. Tests are run in a separate address space, so Check can
   catch both assertion failures and code errors that cause
   segmentation faults or other signals. The output from unit tests
   can be used within source code editors and IDEs.

   Unit tests are created with the START_TEST/END_TEST macro
   pair. The mark_point macro is useful for trapping the location 
   of signals and/or early exits. The following calls are used for
   creating checks within unit tests, where ... represents arguments
   given to a printf:
      Asserting an expression:
         ck_assert(expr)
         ck_assert_msg(expr, ...)
      Aborting a test:
         ck_abort()
         ck_abort_msg(...)
      Check the relationship between two integers:
         ck_assert_int_eq
         ck_assert_int_ne
         ck_assert_int_lt
         ck_assert_int_le
         ck_assert_int_gt
         ck_assert_int_ge
      Check the relationship between two strings:
         ck_assert_str_eq
         ck_assert_str_ne
         ck_assert_str_lt
         ck_assert_str_le
         ck_assert_str_gt
         ck_assert_str_ge
      Check the relationship between two pointers:
         ck_assert_ptr_eq
         ck_assert_ptr_ne

   Test cases are created with tcase_create, unit tests are added
   with tcase_add_test


   Suites are created with suite_create; test cases are added
   with suite_add_tcase

   Suites are run through an SRunner, which is created with
   srunner_create. Additional suites can be added to an SRunner with
   srunner_add_suite. An SRunner is freed with srunner_free, which also
   frees all suites added to the runner. 

   Use srunner_run_all to run a suite and print results.

   Macros and functions starting with _ (underscore) are internal and
   may change without notice. You have been warned!.

*/


#ifdef __cplusplus
#define CK_CPPSTART extern "C" {
#define CK_CPPEND }
CK_CPPSTART
#endif

#if defined(__GNUC__) && defined(__GNUC_MINOR__)
#define GCC_VERSION_AT_LEAST(major, minor) \
((__GNUC__ > (major)) || \
 (__GNUC__ == (major) && __GNUC_MINOR__ >= (minor)))
#else
#define GCC_VERSION_AT_LEAST(major, minor) 0
#endif

#if GCC_VERSION_AT_LEAST(2,95)
#define CK_ATTRIBUTE_UNUSED __attribute__ ((unused))
#else
#define CK_ATTRIBUTE_UNUSED              
#endif /* GCC 2.95 */

#if GCC_VERSION_AT_LEAST(2,5)
#define CK_ATTRIBUTE_NORETURN __attribute__ ((noreturn))
#else
#define CK_ATTRIBUTE_NORETURN
#endif /* GCC 2.5 */

#include <sys/types.h>

/* Used to create the linker script for hiding lib-local symbols. Shall
   be put directly in front of the exported symbol. */
#define CK_EXPORT

/*
 * Used for MSVC to create the export attribute
 * CK_DLL_EXP is defined during the compilation of the library
 * on the command line.
 */
#ifndef CK_DLL_EXP
#define CK_DLL_EXP
#endif

/* check version numbers */
  
#define CHECK_MAJOR_VERSION (0)
#define CHECK_MINOR_VERSION (9)
#define CHECK_MICRO_VERSION (12)

CK_DLL_EXP extern int CK_EXPORT check_major_version;
CK_DLL_EXP extern int CK_EXPORT check_minor_version;
CK_DLL_EXP extern int CK_EXPORT check_micro_version;

#ifndef NULL
#define NULL ((void*)0)
#endif

/* opaque type for a test case

   A TCase represents a test case.  Create with tcase_create, free
   with tcase_free.  For the moment, test cases can only be run
   through a suite
*/
typedef struct TCase TCase; 

/* type for a test function */
typedef void (*TFun) (int);

/* type for a setup/teardown function */
typedef void (*SFun) (void);
 
/* Opaque type for a test suite */
typedef struct Suite Suite;
 
/* Creates a test suite with the given name */
CK_DLL_EXP Suite * CK_EXPORT suite_create (const char *name);

/* Determines whether a given test suite contains a case named after a
   given string.  */
CK_DLL_EXP int CK_EXPORT suite_tcase (Suite *s, const char *tcname);

/* Add a test case to a suite */
CK_DLL_EXP void CK_EXPORT suite_add_tcase (Suite *s, TCase *tc);

/* Create a test case */
CK_DLL_EXP TCase * CK_EXPORT tcase_create (const char *name);

/* Add a test function to a test case (macro version) */
#define tcase_add_test(tc,tf) tcase_add_test_raise_signal(tc,tf,0)

/* Add a test function with signal handling to a test case (macro version) */
#define tcase_add_test_raise_signal(tc,tf,signal) \
   _tcase_add_test((tc),(tf),"" # tf "",(signal), 0, 0, 1)

/* Add a test function with an expected exit value to a test case (macro version) */
#define tcase_add_exit_test(tc, tf, expected_exit_value) \
  _tcase_add_test((tc),(tf),"" # tf "",0,(expected_exit_value),0,1)

/* Add a looping test function to a test case (macro version)

   The test will be called in a for(i = s; i < e; i++) loop with each
   iteration being executed in a new context. The loop variable 'i' is
   available in the test.
 */
#define tcase_add_loop_test(tc,tf,s,e) \
  _tcase_add_test((tc),(tf),"" # tf "",0,0,(s),(e))
 
/* Signal version of loop test.  
   FIXME: add a test case; this is untested as part of Check's tests.
 */
#define tcase_add_loop_test_raise_signal(tc,tf,signal,s,e) \
  _tcase_add_test((tc),(tf),"" # tf "",(signal),0,(s),(e))

/* allowed exit value version of loop test. */
#define tcase_add_loop_exit_test(tc,tf,expected_exit_value,s,e) \
  _tcase_add_test((tc),(tf),"" # tf "",0,(expected_exit_value),(s),(e))

/* Add a test function to a test case
  (function version -- use this when the macro won't work
*/
CK_DLL_EXP void CK_EXPORT _tcase_add_test (TCase *tc, TFun tf, const char *fname, int _signal, int allowed_exit_value, int start, int end);

/* Add unchecked fixture setup/teardown functions to a test case

   If unchecked fixture functions are run at the start and end of the
   test case, and not before and after unit tests. Note that unchecked
   setup/teardown functions are not run in a separate address space,
   like test functions, and so must not exit or signal (e.g.,
   segfault)

   Also, when run in CK_NOFORK mode, unchecked fixture functions may
   lead to different unit test behavior IF unit tests change data
   setup by the fixture functions.
*/
CK_DLL_EXP void CK_EXPORT tcase_add_unchecked_fixture (TCase *tc, SFun setup, SFun teardown);

/* Add fixture setup/teardown functions to a test case

   Checked fixture functions are run before and after unit
   tests. Unlike unchecked fixture functions, checked fixture
   functions are run in the same separate address space as the test
   program, and thus the test function will survive signals or
   unexpected exits in the fixture function. Also, IF the setup
   function is idempotent, unit test behavior will be the same in
   CK_FORK and CK_NOFORK modes.

   However, since fixture functions are run before and after each unit
   test, they should not be expensive code.

*/ 
CK_DLL_EXP void CK_EXPORT tcase_add_checked_fixture (TCase *tc, SFun setup, SFun teardown);

/* Set the timeout for all tests in a test case. A test that lasts longer
   than the timeout (in seconds) will be killed and thus fail with an error.
   The timeout can also be set globaly with the environment variable
   CK_DEFAULT_TIMEOUT, the specific setting always takes precedence.
*/
CK_DLL_EXP void CK_EXPORT tcase_set_timeout (TCase *tc, double timeout);
 
/* Internal function to mark the start of a test function */
CK_DLL_EXP void CK_EXPORT tcase_fn_start (const char *fname, const char *file, int line);

/* Start a unit test with START_TEST(unit_name), end with END_TEST
   One must use braces within a START_/END_ pair to declare new variables
*/ 
#define START_TEST(__testname)\
static void __testname (int _i CK_ATTRIBUTE_UNUSED)\
{\
  tcase_fn_start (""# __testname, __FILE__, __LINE__);

/* End a unit test */
#define END_TEST }

/* Old check fail API, use new check fail API for newly written code. */

/* Fail the test case unless expr is false */
#define fail_unless ck_assert_msg

/* Fail the test case if expr is false */
/* The space before the comma sign before ## is essential to be compatible
   with gcc 2.95.3 and earlier.
   FIXME: these macros may conflict with C89 if expr is
   FIXME:   strcmp (str1, str2) due to excessive string length.
*/
#define fail_if(expr, ...)\
  (expr) ? \
     _ck_assert_failed(__FILE__, __LINE__, "Failure '"#expr"' occured" , ## __VA_ARGS__, NULL) \
     : _mark_point(__FILE__, __LINE__)

#define fail ck_abort_msg

/*
 * This is called whenever an assertion fails.
 * Note that it only has the noreturn modifier when
 * using fork. If fork is unavailable, the function
 * calls longjmp() when a test assertion fails. Marking
 * the function as noreturn causes gcc to make assumptions
 * which are not valid, as longjmp() is like a return.
 */
#if 1
CK_DLL_EXP void CK_EXPORT _ck_assert_failed (const char *file, int line, const char *expr, ...) CK_ATTRIBUTE_NORETURN;
#else
CK_DLL_EXP void CK_EXPORT _ck_assert_failed (const char *file, int line, const char *expr, ...);
#endif

/* New check fail API. */

/* Fail the test case if expr is false */
/* The space before the comma sign before ## is essential to be compatible
   with gcc 2.95.3 and earlier.
*/
#define ck_assert(C) ck_assert_msg(C, NULL)
#define ck_assert_msg(expr, ...) \
  (expr) ? \
     _mark_point(__FILE__, __LINE__) : \
     _ck_assert_failed(__FILE__, __LINE__, "Assertion '"#expr"' failed" , ## __VA_ARGS__, NULL)

/* Always fail */
#define ck_abort() ck_abort_msg(NULL)
#define ck_abort_msg(...) _ck_assert_failed(__FILE__, __LINE__, "Failed" , ## __VA_ARGS__, NULL)

/* Signed and unsigned integer comparsion macros with improved output compared to ck_assert(). */
/* OP may be any comparion operator. */
#define _ck_assert_int(X, OP, Y) do { \
  intmax_t _ck_x = (X); \
  intmax_t _ck_y = (Y); \
  ck_assert_msg(_ck_x OP _ck_y, "Assertion '"#X#OP#Y"' failed: "#X"==%jd, "#Y"==%jd", _ck_x, _ck_y); \
} while (0)
#define ck_assert_int_eq(X, Y) _ck_assert_int(X, ==, Y)
#define ck_assert_int_ne(X, Y) _ck_assert_int(X, !=, Y)
#define ck_assert_int_lt(X, Y) _ck_assert_int(X, <, Y)
#define ck_assert_int_le(X, Y) _ck_assert_int(X, <=, Y)
#define ck_assert_int_gt(X, Y) _ck_assert_int(X, >, Y)
#define ck_assert_int_ge(X, Y) _ck_assert_int(X, >=, Y)

#define _ck_assert_uint(X, OP, Y) do { \
  uintmax_t _ck_x = (X); \
  uintmax_t _ck_y = (Y); \
  ck_assert_msg(_ck_x OP _ck_y, "Assertion '"#X#OP#Y"' failed: "#X"==%ju, "#Y"==%ju", _ck_x, _ck_y); \
} while (0)
#define ck_assert_uint_eq(X, Y) _ck_assert_uint(X, ==, Y)
#define ck_assert_uint_ne(X, Y) _ck_assert_uint(X, !=, Y)
#define ck_assert_uint_lt(X, Y) _ck_assert_uint(X, <, Y)
#define ck_assert_uint_le(X, Y) _ck_assert_uint(X, <=, Y)
#define ck_assert_uint_gt(X, Y) _ck_assert_uint(X, >, Y)
#define ck_assert_uint_ge(X, Y) _ck_assert_uint(X, >=, Y)

/* String comparsion macros with improved output compared to ck_assert() */
/* OP might be any operator that can be used in '0 OP strcmp(X,Y)' comparison */
/* The x and y parameter swap in strcmp() is needed to handle >, >=, <, <= operators */
#define _ck_assert_str(X, OP, Y) do { \
  const char* _ck_x = (X); \
  const char* _ck_y = (Y); \
  ck_assert_msg(0 OP strcmp(_ck_y, _ck_x), \
    "Assertion '"#X#OP#Y"' failed: "#X"==\"%s\", "#Y"==\"%s\"", _ck_x, _ck_y); \
} while (0)
#define ck_assert_str_eq(X, Y) _ck_assert_str(X, ==, Y)
#define ck_assert_str_ne(X, Y) _ck_assert_str(X, !=, Y)
#define ck_assert_str_lt(X, Y) _ck_assert_str(X, <, Y)
#define ck_assert_str_le(X, Y) _ck_assert_str(X, <=, Y)
#define ck_assert_str_gt(X, Y) _ck_assert_str(X, >, Y)
#define ck_assert_str_ge(X, Y) _ck_assert_str(X, >=, Y)

/* Pointer comparsion macros with improved output compared to ck_assert(). */
/* OP may only be == or !=  */
#define _ck_assert_ptr(X, OP, Y) do { \
  const void* _ck_x = (X); \
  const void* _ck_y = (Y); \
  ck_assert_msg(_ck_x OP _ck_y, "Assertion '"#X#OP#Y"' failed: "#X"==%#x, "#Y"==%#x", _ck_x, _ck_y); \
} while (0)
#define ck_assert_ptr_eq(X, Y) _ck_assert_ptr(X, ==, Y)
#define ck_assert_ptr_ne(X, Y) _ck_assert_ptr(X, !=, Y)

/* Mark the last point reached in a unit test
   (useful for tracking down where a segfault, etc. occurs)
*/
#define mark_point() _mark_point(__FILE__,__LINE__)

/* Non macro version of #mark_point */
CK_DLL_EXP void CK_EXPORT _mark_point (const char *file, int line);

/* Result of a test */
enum test_result {
  CK_TEST_RESULT_INVALID, /* Default value; should not encounter this */
  CK_PASS, /* Test passed*/
  CK_FAILURE, /* Test completed but failed */
  CK_ERROR /* Test failed to complete
	      (unexpected signal or non-zero early exit) */ 
};

/* Specifies the how much output an SRunner should produce */
enum print_output {
  CK_SILENT, /* No output */
  CK_MINIMAL, /* Only summary output */
  CK_NORMAL, /* All failed tests */
  CK_VERBOSE, /* All tests */
  CK_ENV, /* Look at environment var */
#if 1
  CK_SUBUNIT, /* Run as a subunit child process */
#endif
  CK_LAST
};

/* Holds state for a running of a test suite */
typedef struct SRunner SRunner;

/* Opaque type for a test failure */
typedef struct TestResult TestResult;

/* accessors for tr fields */
enum ck_result_ctx {
  CK_CTX_INVALID, /* Default value; should not encounter this */
  CK_CTX_SETUP,
  CK_CTX_TEST,
  CK_CTX_TEARDOWN
};

/* Type of result */
CK_DLL_EXP int CK_EXPORT tr_rtype (TestResult *tr);
/* Context in which the result occurred */ 
CK_DLL_EXP enum ck_result_ctx CK_EXPORT tr_ctx (TestResult *tr); 
/* Failure message */
CK_DLL_EXP const char * CK_EXPORT tr_msg (TestResult *tr);
/* Line number at which failure occured */
CK_DLL_EXP int CK_EXPORT tr_lno (TestResult *tr);
/* File name at which failure occured */
CK_DLL_EXP const char * CK_EXPORT tr_lfile (TestResult *tr);
/* Test case in which unit test was run */
CK_DLL_EXP const char * CK_EXPORT tr_tcname (TestResult *tr);

/* Creates an SRunner for the given suite */
CK_DLL_EXP SRunner * CK_EXPORT srunner_create (Suite *s);

/* Adds a Suite to an SRunner */
CK_DLL_EXP void CK_EXPORT srunner_add_suite (SRunner *sr, Suite *s);

/* Frees an SRunner, all suites added to it and all contained test cases */
CK_DLL_EXP void CK_EXPORT srunner_free (SRunner *sr);

 
/* Test running */

/* Runs an SRunner, printing results as specified (see enum print_output) */
CK_DLL_EXP void CK_EXPORT srunner_run_all (SRunner *sr, enum print_output print_mode);

/* Runs an SRunner specifying test suite and test case by name,
   printing results as specified (see enum print_output).  A NULL
   value means "any test suite" or "any test case".  */
CK_DLL_EXP void CK_EXPORT srunner_run (SRunner *sr, const char *sname, const char *tcname, enum print_output print_mode);

 
/* Next functions are valid only after the suite has been
   completely run, of course */

/* Number of failed tests in a run suite. Includes failures + errors */
CK_DLL_EXP int CK_EXPORT srunner_ntests_failed (SRunner *sr);

/* Total number of tests run in a run suite */
CK_DLL_EXP int CK_EXPORT srunner_ntests_run (SRunner *sr);

/* Return an array of results for all failures
  
   Number of failures is equal to srunner_nfailed_tests.  Memory for
   the array is malloc'ed and must be freed, but individual TestResults
   must not
*/
CK_DLL_EXP TestResult ** CK_EXPORT srunner_failures (SRunner *sr);

/* Return an array of results for all run tests

   Number of results is equal to srunner_ntests_run, and excludes
   failures due to setup function failure.

   Memory is malloc'ed and must be freed, but individual TestResults
   must not
*/  
CK_DLL_EXP TestResult ** CK_EXPORT srunner_results (SRunner *sr);

 
/* Printing */

/* Print the results contained in an SRunner */
CK_DLL_EXP void CK_EXPORT srunner_print (SRunner *sr, enum print_output print_mode);
  
  
/* Set a log file to which to write during test running.

  Log file setting is an initialize only operation -- it should be
  done immediatly after SRunner creation, and the log file can't be
  changed after being set.
*/
CK_DLL_EXP void CK_EXPORT srunner_set_log (SRunner *sr, const char *fname);

/* Does the SRunner have a log file? */
CK_DLL_EXP int CK_EXPORT srunner_has_log (SRunner *sr);

/* Return the name of the log file, or NULL if none */
CK_DLL_EXP const char * CK_EXPORT srunner_log_fname (SRunner *sr);

/* Set a xml file to which to write during test running.

  XML file setting is an initialize only operation -- it should be
  done immediatly after SRunner creation, and the XML file can't be
  changed after being set.
*/
CK_DLL_EXP void CK_EXPORT srunner_set_xml (SRunner *sr, const char *fname);

/* Does the SRunner have an XML log file? */
CK_DLL_EXP int CK_EXPORT srunner_has_xml (SRunner *sr);

/* Return the name of the XML file, or NULL if none */
CK_DLL_EXP const char * CK_EXPORT srunner_xml_fname (SRunner *sr);

/* Set a tap file to which to write during test running.

  Log file setting is an initialize only operation -- it should be
  done immediatly after SRunner creation, and the log file can't be
  changed after being set.
*/
CK_DLL_EXP void CK_EXPORT srunner_set_tap (SRunner *sr, const char *fname);

/* Does the SRunner have a tap file? */
CK_DLL_EXP int CK_EXPORT srunner_has_tap (SRunner *sr);

/* Return the name of the tap file, or NULL if none */
CK_DLL_EXP const char * CK_EXPORT srunner_tap_fname (SRunner *sr);


/* Control forking */
enum fork_status {
  CK_FORK_GETENV, /* look in the environment for CK_FORK */
  CK_FORK,        /* call fork to run tests */
  CK_NOFORK       /* don't call fork */
};
 
/* Get the current fork status */
CK_DLL_EXP enum fork_status CK_EXPORT srunner_fork_status (SRunner *sr);

/* Set the current fork status */
CK_DLL_EXP void CK_EXPORT srunner_set_fork_status (SRunner *sr, enum fork_status fstat); 
  
/* Fork in a test and make sure messaging and tests work. */
CK_DLL_EXP pid_t CK_EXPORT check_fork(void);

/* Wait for the pid and exit. If pid is zero, just exit. */
CK_DLL_EXP void CK_EXPORT check_waitpid_and_exit(pid_t pid) CK_ATTRIBUTE_NORETURN;

#ifdef __cplusplus 
CK_CPPEND
#endif

#endif /* CHECK_H */
