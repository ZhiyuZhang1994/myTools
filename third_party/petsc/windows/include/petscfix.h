#if !defined(INCLUDED_PETSCFIX_H)
#define INCLUDED_PETSCFIX_H

typedef int uid_t;
typedef int gid_t;
typedef int int32_t;
#if defined(__cplusplus)
extern "C" {
}
#else
#include <stddef.h>
int getdomainname(char *, size_t);
double drand48(void);
void   srand48(long int);
#endif
#endif
