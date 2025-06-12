/**************
*
* SG_types.h
*
* Standartises the types
* OS: __linux, _AIX, _WIN32, DARWIN, __APPLE__
*
* (c) Sébastien Galvagno - 2008
* 
***************/

#ifndef __SG_TYPES_H__
#define __SG_TYPES_H__

//#pragma pack(1)



#ifdef __linux_
# include <unistd.h>
#endif

#ifdef _WIN32
# include <WinNT.h>
#endif

/*--- 8bit ------------------------------------------*/
#if !defined(DARWIN)
   typedef signed char     SInt8;
   typedef unsigned char   UInt8;
#endif /* DARWIN */
   typedef signed char     SChar;
   typedef unsigned char   UChar;
/*--- end of 8bit -----------------------------------*/

/*--- 16bit ------------------------------------------*/
#if !defined(DARWIN)
   typedef signed short int   SInt16;
   typedef unsigned short int UInt16;
#endif /* DARWIN */
/*--- end of 16bit -----------------------------------*/

/*--- 32bit ------------------------------------------*/
#if !defined(DARWIN)

// ok: hpux/32/ia64(gcc, cc), aix/32/64(gcc), sparc/32/64(gcc), linux/32/64(gcc), mac/32/64(gcc)
# if __LONG_MAX__ == 9223372036854775807LL
   typedef signed int   SInt32;
   typedef unsigned int UInt32;
#  define TB_DEF_SNPRINTF_I32_WO "d"
#  define is32Defined

# else

#  if defined(__L64_MODE__)
      typedef signed int   SInt32;
      typedef unsigned int UInt32;
#  define TB_DEF_SNPRINTF_I32_WO "d"

#   define is32Defined
#  endif /* #if defined(__L64_MODE__) */

# endif /* #if __LONG_MAX__ == 9223372036854775807 */

# if !defined(is32Defined)
   typedef signed long int   SInt32;
   typedef unsigned long int UInt32;
#  define TB_DEF_SNPRINTF_I32_WO "ld"
# endif // #if !defined(is32Defined)

#endif /* DARWIN */
/*--- end of 32bit -----------------------------------*/

/*--- 64bit -----------------------------------------*/
//#warning 64bits
#if defined(_MSC_VER) || defined(_M_IX86)
    typedef   signed __int64                SInt64;
    typedef unsigned __int64                UInt64;

#else


# if defined(__APPLE_CC__) || defined(__APPLE__)
/* from File: CarbonCore/MacTypes.h */
  typedef   signed long long              SInt64;
  typedef unsigned long long              UInt64;
/* *** */

# elif defined(__linux)

#  include <stdint.h>
   typedef int64_t  SInt64;
   typedef uint64_t UInt64;

# elif defined (EXE_IOS)
   typedef int64_t  SInt64;
   typedef uint64_t UInt64;

# else // defined(EXE_LINUX)

#  if defined(_MSC_VER)
#   error OS inconnu
#  endif //#if defined(_MSC_VER)

#endif // defined(DARWIN)

#endif

       

/*--- end of 64bit ----------------------------------*/


#ifdef _WIN32

#else

typedef SInt8               CHAR;
typedef unsigned int        UINT;
typedef UInt8		    UINT8;  
typedef UInt16		    UINT16;
typedef UInt32       	    UINT32;
typedef UInt64		    UINT64;
typedef SInt64	            INT64;
typedef UInt8		    BYTE;
typedef UInt16		    WORD;
typedef UInt16      	    USHORT;
typedef UInt32		    DWORD;
typedef UInt32              LONG;
typedef int                 BOOL;
typedef BYTE                BOOLEAN;
typedef char *              LPSTR, LPTSTR;
typedef const char *        LPCSTR;
typedef void *              PVOID;

typedef void * LPVOID;
typedef void * HANDLE;
typedef HANDLE SERVICE_STATUS_HANDLE;
typedef HANDLE SC_HANDLE;

typedef void * LPHANDLER_FUNCTION;


#endif /* _WIN32 */


#if !defined(DARWIN) //&& !defined (EXE_IOS)
// Desole mais cela rentre en conflit avec une definition correcte du type Boolean sur Macintosh.

# ifndef __MACTYPES__

typedef enum
{ False, True }
Boolean;
# endif
#endif /* DARWIN */


#endif /* __SG_TYPES_H__ */
;
/* *** */

