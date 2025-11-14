#ifndef VERSION_H
#define VERSION_H

//*******************************************************************************
//                                                                              
//       Version Unit                                                           
//                                                                              
//       Copyright (c) 2011 Gregor A. Cieslak           
//                                                                              
//*******************************************************************************

#include <string>
#include <cstddef>

// Note: In Pascal version, these were loaded from external files (Version.txt and %BUILD_ID%)
// For C++ implementation, we'll define them directly or through build system
const std::string SOLDAT_VERSION = "2.9.0";  // This would typically come from Version.txt
const size_t SOLDAT_VERSION_CHARS = SOLDAT_VERSION.length();
const int SOLDAT_VERSION_LONG = 0x290;       // This would typically come from %BUILD_ID%

#ifdef SERVER_CODE
const std::string DEDVERSION = "2.9.0";
const std::string COREVERSION = "2.5";
#else
const std::string DEDVERSION = "GAME";
#endif

const int DEMO_VERSION = 0x0000;  // version number for soldat demo files

#endif // VERSION_H