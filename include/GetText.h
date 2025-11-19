#ifndef GETTEXT_H
#define GETTEXT_H

// Placeholder GetText.h header file
// This is a stub to allow compilation until the actual GetText library is properly configured

#include <string>

// If GetText library is available, define the standard functions
#ifdef HAVE_GETTEXT
    #include <libintl.h>
    #define SOLDAT_GETTEXT_DEFINED
    #define _(String) gettext(String)
    #define gettext_noop(String) String
    #define N_(String) gettext_noop(String)
#else
    // If GetText is not available, define macros as no-ops
    #define SOLDAT_GETTEXT_DEFINED
    #define _(String) (String)
    #define gettext_noop(String) String
    #define N_(String) gettext_noop(String)
#endif

// Ensure wstring is properly declared if needed elsewhere
// This file shouldn't declare wstring functions, that's for other headers to handle

#endif // GETTEXT_H