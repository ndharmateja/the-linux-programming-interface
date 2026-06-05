/**************************************************************************
* Copyright (C) Michael Kerrisk, 2026.                                    *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Lesser General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the files COPYING.lgpl-v3 and COPYING.gpl-v3 for details.           *
*                                                                         *
* The docstrings are AI generated                                         *
\*************************************************************************/

/* Listing 3-2 */

/* error_functions.h

   Header file for error_functions.c.
*/
#ifndef ERROR_FUNCTIONS_H
#define ERROR_FUNCTIONS_H

#ifdef __GNUC__
/* This macro stops 'gcc -Wall' complaining that "control reaches
   end of non-void function" if we use the following functions to
   terminate main() or some other non-void function. */
#define NORETURN __attribute__((__noreturn__))
#else
#define NORETURN
#endif

/* ===================================================================== */
/* System Call and Library Error Diagnostic Routines          */
/* ===================================================================== */

/**
 * @brief Prints an error message to standard error based on the current errno.
 *
 * This function extracts the error text corresponding to the current global
 * value of 'errno' (combining the symbolic error name like EPERM and the
 * description provided by strerror()). It appends a formatted message custom-built
 * via standard printf() style formatting rules.
 *
 * @param format A printf-style format string.
 * @param ...    Variadic arguments matching the format specifiers.
 * @note         A terminating newline character is automatically appended.
 * This function does NOT terminate the program.
 */
void errMsg(const char *format, ...);

/**
 * @brief Prints an errno-based error message and terminates the program.
 *
 * Operates identically to errMsg() regarding message composition, but gracefully
 * shuts down the application after printing.
 *
 * @param format A printf-style format string.
 * @param ...    Variadic arguments matching the format specifiers.
 * @return       This function never returns (marked NORETURN).
 * @note         Termination behavior relies on the environment:
 * - Default: Calls exit() to terminate normally.
 * - If the environment variable 'EF_DUMPCORE' is defined with a
 * nonempty string: Calls abort() to induce a core dump file
 * for debugging analysis.
 */
void errExit(const char *format, ...) NORETURN;

/**
 * @brief Prints an errno-based error message and immediately terminates the process.
 *
 * A specialized version of errExit() designed primarily for child processes created via
 * fork() that encounter critical internal failures. It provides clean isolation from
 * parent process resources.
 *
 * @param format A printf-style format string.
 * @param ...    Variadic arguments matching the format specifiers.
 * @return       This function never returns (marked NORETURN).
 * @note         Differs from errExit() in two crucial architectural respects:
 * 1. It does NOT flush standard output (stdout) buffers before printing.
 * 2. It terminates the process via _exit() instead of exit(), skipping
 * the invocation of user-established exit handlers and standard
 * stdio buffer flush cycles.
 */
void err_exit(const char *format, ...) NORETURN;

/**
 * @brief Prints an explicit error-number message and terminates the program.
 *
 * Identical to errExit() except that it skips the global 'errno' variable entirely,
 * resolving error strings using the explicitly provided 'errnum' integer parameter.
 *
 * @param errnum The explicit error integer code to diagnose.
 * @param format A printf-style format string.
 * @param ...    Variadic arguments matching the format specifiers.
 * @return       This function never returns (marked NORETURN).
 * @note         Mainly utilized across POSIX threads (pthreads) configurations. Threaded
 * functions return error numbers directly as their functional status
 * (returning 0 on success) rather than interacting with the global errno
 * mechanism, which carries high function-call overhead due to thread-specific
 * macro lookups.
 */
void errExitEN(int errnum, const char *format, ...) NORETURN;

/* ===================================================================== */
/* General Error Diagnostic Routines                 */
/* ===================================================================== */

/**
 * @brief Diagnoses general application errors unrelated to errno status.
 *
 * Intended for runtime errors where errno is not set or irrelevant (e.g., general logic
 * failures or library calls that do not provide standard system error symbols).
 *
 * @param format A printf-style format string.
 * @param ...    Variadic arguments matching the format specifiers.
 * @return       This function never returns (marked NORETURN).
 * @note         Formats output onto standard error, automatically appends a trailing
 * newline, and cleanly terminates the system runtime following the identical
 * exit/abort logic established by errExit().
 */
void fatal(const char *format, ...) NORETURN;

/**
 * @brief Diagnoses command-line parameter syntax errors.
 *
 * Automatically prefixes the diagnostic message with the standardized "Usage: " string
 * context before printing to stderr.
 *
 * @param format A printf-style format string.
 * @param ...    Variadic arguments matching the format specifiers.
 * @return       This function never returns (marked NORETURN).
 * @note         Terminates execution via standard exit(). Used typically to catch faulty
 * argument counts or missing parameters directly at the beginning of main().
 */
void usageErr(const char *format, ...) NORETURN;

/**
 * @brief Diagnoses specific data validation errors within command-line arguments.
 *
 * Closely matches usageErr(), but refined for pointing out specific semantic errors in
 * user parameters (e.g., an argument was successfully provided, but its parsed value
 * falls outside an acceptable numeric range).
 *
 * @param format A printf-style format string.
 * @param ...    Variadic arguments matching the format specifiers.
 * @return       This function never returns (marked NORETURN).
 * @note         Outputs formatting to stderr and handles termination via standard exit().
 */
void cmdLineErr(const char *format, ...) NORETURN;

#endif