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
#include <string>

const int MAX_SPARKS = 1000;

// Forward declaration
struct TVector2;

void CreateSpark(TVector2 Pos, TVector2 Vel, uint8_t Type, uint8_t TTL, uint8_t Intensity);

namespace SparksImpl {
    void CreateSpark(TVector2 Pos, TVector2 Vel, uint8_t Type, uint8_t TTL, uint8_t Intensity);
}

using SparksImpl::CreateSpark;

#endif // SPARKS_H