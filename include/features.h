#ifndef _FEATURES_H
#define _FEATURES_H

#if !defined(GCC_VERSION)
#define GCC_VERSION (__GNUC__ * 10000 \
          + __GNUC_MINOR__ * 100 \
          + __GNUC_PATCHLEVEL__)
#endif  // !GCC_VERSION
          
#ifndef __GNUC_PREREQ
# if defined __GNUC__ && defined __GNUC_MINOR__
#  define __GNUC_PREREQ(maj, min) \
         ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
# else
#  define __GNUC_PREREQ(maj, min) 0
# endif
#endif

#endif // _FEATURES_H
