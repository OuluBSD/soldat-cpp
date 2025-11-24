#ifndef SPARKS_H
#define SPARKS_H

//*******************************************************************************
//
//       Sparks Unit for SOLDAT
//
//       Copyright (c) 2003 Michal Marcinkowski
//
//*******************************************************************************

#include "Vector.h"
#include "Constants.h" // MAX_SPARKS is defined here
#include <string>

// Forward declaration
struct TVector2;

namespace SparksImpl {
    void CreateSpark(TVector2 Pos, TVector2 Vel, uint8_t Type, uint8_t TTL, uint8_t Intensity);
}

// Use the namespace function
using SparksImpl::CreateSpark;

#endif // SPARKS_H