#ifndef VECTOR_H
#define VECTOR_H

//*******************************************************************************
//                                                                              
//       Vector Unit for SOLDAT                                                 
//                                                                              
//       Copyright (c) 2013 Gregor A. Cieslak                                   
//                                                                              
//         Modification of MyD3DX8 Unit                                         
//           Copyright (c) 2001 by Michal Marcinkowski                          
//                                                                              
//*******************************************************************************
//*                                                                            
//*        Based upon :                                                        
//*          Direct3DX 8.1 Delphi adaptation by Alexey Barkovoy                
//*          E-Mail: directx@clootie.ru                                        
//*                                                                            
//*******************************************************************************
//*                                                                            
//*  Copyright (C) Microsoft Corporation.  All Rights Reserved.                
//*                                                                            
//*  File:       d3dx8.h, d3dx8core.h, d3dx8math.h, d3dx8math.inl,             
//*              d3dx8effect.h, d3dx8mesh.h, d3dx8shape.h, d3dx8tex.h          
//*  Content:    Direct3DX 8.1 headers                                         
//*                                                                            
//*  Direct3DX 8.1 Delphi adaptation by Alexey Barkovoy                        
//*  E-Mail: directx@clootie.ru                                                
//*                                                                            
//*  Modified: 12-Feb-2005                                                     
//*                                                                            
//*  Partly based upon :                                                       
//*    Direct3DX 7.0 Delphi adaptation by                                      
//*      Arne Schäpers, e-Mail: [look at www.delphi-jedi.org/DelphiGraphics/]  
//*                                                                            
//*  Latest version can be downloaded from:                                    
//*    http://clootie.ru                                                       
//*    http://sourceforge.net/projects/delphi-dx9sdk                           
//*                                                                            
//*  This File contains only Direct3DX 8.x Definitions.                        
//*  If you want to use D3DX7 version of D3DX use translation by Arne Schäpers 
//*                                                                            
//*******************************************************************************
//                                                                              
// Obtained through: Joint Endeavour of Delphi Innovators (Project JEDI)        
//                                                                              
// The contents of this file are used with permission, subject to the Mozilla   
// Public License Version 1.1 (the "License"); you may not use this file except 
// in compliance with the License. You may obtain a copy of the License at      
// http://www.mozilla.org/MPL/MPL-1.1.html                                      
//                                                                              
// Software distributed under the License is distributed on an "AS IS" basis,   
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for 
// the specific language governing rights and limitations under the License.    
//                                                                              
// Alternatively, the contents of this file may be used under the terms of the  
// GNU Lesser General Public License (the  "LGPL License"), in which case the   
// provisions of the LGPL License are applicable instead of those above.        
// If you wish to allow use of your version of this file only under the terms   
// of the LGPL License and not to allow others to use your version of this file 
// under the MPL, indicate your decision by deleting  the provisions above and  
// replace  them with the notice and other provisions required by the LGPL      
// License.  If you do not delete the provisions above, a recipient may use     
// your version of this file under either the MPL or the LGPL License.          
//                                                                              
// For more information about the LGPL: http://www.gnu.org/copyleft/lesser.html 
//                                                                              
//*******************************************************************************

#include <cmath>

// Type definitions
struct TVector2 {
    float x, y;
};

struct TVector3 {
    float x, y, z;
};

using PVector2 = TVector2*;
using PVector3 = TVector3*;

// Function implementations
namespace VectorImpl {
    inline TVector2 Vector2(float x, float y) {
        TVector2 result;
        result.x = x;
        result.y = y;
        return result;
    }

    inline float Vec2Length(TVector2 v) {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }

    inline float Vec2Length2(TVector2 v) {
        return v.x * v.x + v.y * v.y;
    }

    inline float Vec2Dot(TVector2 v1, TVector2 v2) {
        return v1.x * v2.x + v1.y * v2.y;
    }

    inline TVector2 Vec2Add(TVector2 v1, TVector2 v2) {
        TVector2 result;
        result.x = v1.x + v2.x;
        result.y = v1.y + v2.y;
        return result;
    }

    inline TVector2 Vec2Subtract(TVector2 v1, TVector2 v2) {
        TVector2 result;
        result.x = v1.x - v2.x;
        result.y = v1.y - v2.y;
        return result;
    }

    inline PVector2 Vec2Scale(TVector2& vOut, TVector2 v, float s) {
        vOut.x = v.x * s;
        vOut.y = v.y * s;
        return &vOut;
    }

    inline PVector2 Vec2Normalize(TVector2& vOut, TVector2 v) {
        float len = Vec2Length(v);
        if (len < 0.001f && len > -0.001f) {
            vOut.x = 0.0f;
            vOut.y = 0.0f;
        } else {
            vOut.x = v.x / len;
            vOut.y = v.y / len;
        }
        return &vOut;
    }

    inline float Vec3Length(TVector3 v) {
        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }
}

// Using declarations to bring the functions into the global scope
using VectorImpl::Vector2;
using VectorImpl::Vec2Length;
using VectorImpl::Vec2Length2;
using VectorImpl::Vec2Dot;
using VectorImpl::Vec2Add;
using VectorImpl::Vec2Subtract;
using VectorImpl::Vec2Scale;
using VectorImpl::Vec2Normalize;
using VectorImpl::Vec3Length;

#endif // VECTOR_H