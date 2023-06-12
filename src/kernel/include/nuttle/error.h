#ifndef __NUTTLE_ERROR_H__
#define __NUTTLE_ERROR_H__

// Checks whether a provided integer value represents an error or not.

#define ISERR(value) ((int) (value) < 0)

// Check whether a returned pointer type value represents an error.

#define ISERRP(value) ((int) (value) == 0)

// Use this if the pointer type value may represents an error with a code.

#define ERROR_P(value) (void*) (value)

// Use this to extract pointer type errors with error codes.

#define ERROR_I(value) (int) (value)

#endif    // __NUTTLE_ERROR_H__