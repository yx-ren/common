
#ifndef CBASE_EXPORT_H
#define CBASE_EXPORT_H

#ifdef CBASE_STATIC_DEFINE
#  define CBASE_EXPORT
#  define CBASE_NO_EXPORT
#else
#  ifndef CBASE_EXPORT
#    ifdef cbase_EXPORTS
        /* We are building this library */
#      define CBASE_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define CBASE_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef CBASE_NO_EXPORT
#    define CBASE_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef CBASE_DEPRECATED
#  define CBASE_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef CBASE_DEPRECATED_EXPORT
#  define CBASE_DEPRECATED_EXPORT CBASE_EXPORT CBASE_DEPRECATED
#endif

#ifndef CBASE_DEPRECATED_NO_EXPORT
#  define CBASE_DEPRECATED_NO_EXPORT CBASE_NO_EXPORT CBASE_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef CBASE_NO_DEPRECATED
#    define CBASE_NO_DEPRECATED
#  endif
#endif

#endif
