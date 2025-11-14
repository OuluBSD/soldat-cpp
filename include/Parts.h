#ifndef PARTS_H
#define PARTS_H

//*******************************************************************************
//                                                                              
//       Particle Physics Unit for SOLDAT                                       
//                                                                              
//       Copyright (c) 2001-02 Michal Marcinkowski           
//                                                                              
//       PARTS ver. 1.0.7                                                       
//       PARTICLE & CONSTRAINT PHYSICS MODULE FOR DELPHI 2D                    
//        by Michal Marcinkowski                                                
//                                                                              
//       version history:                                                       
//        Current - Modified for SOLDAT                                         
//        1.0.7 - Changed to 2D                                                 
//        1.0.6 - Added Constraint Active variable                             
//        1.0.5 - Added PO Constraints only loading                            
//        1.0.4 - Changed to D3DX Vectors                                       
//        1.0.3 - Added PO loader scaling                                       
//        1.0.2 - Added Euler Integrator                                        
//        1.0.1 - Added PO files loading                                        
//                                                                              
//*******************************************************************************

#include "Vector.h"
#include <vector>
#include <cstring>
#include <cmath>

const int NUM_PARTICLES = 560;
const float RKV = 0.98f;

struct TConstraint {
    bool Active;
    int PartA, PartB;
    float Restlength;
    
    TConstraint() : Active(false), PartA(0), PartB(0), Restlength(0.0f) {}
};

struct TParticleSystem {
    std::vector<bool> Active;
    std::vector<TVector2> Pos;
    std::vector<TVector2> Velocity;
    std::vector<TVector2> OldPos;
    std::vector<TVector2> Forces;
    std::vector<float> OneOverMass;
    float TimeStep;
    float Gravity, VDamping, EDamping;
    int ConstraintCount;
    int PartCount;
    std::vector<TConstraint> Constraints;
    
    TParticleSystem() :
        Active(NUM_PARTICLES + 1), // Pascal arrays start from 1
        Pos(NUM_PARTICLES + 1),
        Velocity(NUM_PARTICLES + 1),
        OldPos(NUM_PARTICLES + 1),
        Forces(NUM_PARTICLES + 1),
        OneOverMass(NUM_PARTICLES + 1),
        TimeStep(1.0f),
        Gravity(0.0f),
        VDamping(0.99f),
        EDamping(0.99f),
        ConstraintCount(0),
        PartCount(0),
        Constraints(NUM_PARTICLES + 1)
    {}

    void DoVerletTimeStep();
    void DoVerletTimeStepFor(int I, int J);
    void DoEulerTimeStep();
    void DoEulerTimeStepFor(int I);
    void CreatePart(TVector2 Start, TVector2 Vel, float Mass, int Num);
    void MakeConstraint(int PA, int PB, float Rest);
    void Clone(const TParticleSystem& Other);
    void LoadPOObject(const std::string& Filename, float Scale);
    void StopAllParts();
    void Destroy();
    void SatisfyConstraints();
    
private:
    void Verlet(int I);
    void Euler(int I);
    void SatisfyConstraintsFor(int I);
};

namespace PartsImpl {
    inline void TParticleSystem::DoVerletTimeStep() {
        for (int i = 1; i <= NUM_PARTICLES; i++) {
            if (Active[i]) {
                Verlet(i);
            }
        }
        SatisfyConstraints();
    }

    inline void TParticleSystem::DoVerletTimeStepFor(int I, int J) {
        Verlet(I);
        SatisfyConstraintsFor(J);
    }

    inline void TParticleSystem::DoEulerTimeStepFor(int I) {
        Euler(I);
    }

    inline void TParticleSystem::DoEulerTimeStep() {
        for (int i = 1; i <= NUM_PARTICLES; i++) {
            if (Active[i]) {
                Euler(i);
            }
        }
    }

    inline void TParticleSystem::Euler(int I) {
        // Accumulate forces
        Forces[I].y += Gravity;
        TVector2 TempPos = Pos[I];

        TVector2 S = Vec2Scale(Forces[I], OneOverMass[I]);
        S = Vec2Scale(S, TimeStep * TimeStep);

        Velocity[I] = Vec2Add(Velocity[I], S);
        Pos[I] = Vec2Add(Pos[I], Velocity[I]);
        Velocity[I] = Vec2Scale(Velocity[I], EDamping);
        OldPos[I] = TempPos;

        Forces[I].x = 0.0f;
        Forces[I].y = 0.0f;
    }

    inline void TParticleSystem::Verlet(int I) {
        // Accumulate forces
        Forces[I].y += Gravity;
        TVector2 TempPos = Pos[I];

        // Pos[I] := 2 * Pos[I] - OldPos[I] + Forces[I] * TimeStep * TimeStep;  {Verlet integration}
        TVector2 S1 = Vec2Scale(Pos[I], 1.0f + VDamping);
        TVector2 S2 = Vec2Scale(OldPos[I], VDamping);

        TVector2 D = Vec2Subtract(S1, S2);
        S1 = Vec2Scale(Forces[I], OneOverMass[I]);
        S2 = Vec2Scale(S1, TimeStep * TimeStep);

        Pos[I] = Vec2Add(D, S2);
        OldPos[I] = TempPos;

        Forces[I].x = 0.0f;
        Forces[I].y = 0.0f;
    }

    inline void TParticleSystem::SatisfyConstraints() {
        if (ConstraintCount > 0) {
            for (int i = 1; i <= ConstraintCount; i++) {
                TConstraint& constraint = Constraints[i];
                if (constraint.Active) {
                    float Diff = 0.0f;
                    TVector2 Delta = Vec2Subtract(Pos[constraint.PartB], Pos[constraint.PartA]);
                    float DeltaLength = std::sqrt(Vec2Dot(Delta, Delta));
                    if (DeltaLength != 0.0f) {
                        Diff = (DeltaLength - constraint.Restlength) / DeltaLength;
                    }
                    if (OneOverMass[constraint.PartA] > 0.0f) {
                        TVector2 D = Vec2Scale(Delta, 0.5f * Diff);
                        Pos[constraint.PartA] = Vec2Add(Pos[constraint.PartA], D);
                    }
                    if (OneOverMass[constraint.PartB] > 0.0f) {
                        TVector2 D = Vec2Scale(Delta, 0.5f * Diff);
                        Pos[constraint.PartB] = Vec2Subtract(Pos[constraint.PartB], D);
                    }
                }
            }
        }
    }

    inline void TParticleSystem::SatisfyConstraintsFor(int I) {
        TConstraint& constraint = Constraints[I];
        float Diff = 0.0f;
        TVector2 Delta = Vec2Subtract(Pos[constraint.PartB], Pos[constraint.PartA]);
        float DeltaLength = std::sqrt(Vec2Dot(Delta, Delta));
        if (DeltaLength != 0.0f) {
            Diff = (DeltaLength - constraint.Restlength) / DeltaLength;
        }
        if (OneOverMass[constraint.PartA] > 0.0f) {
            TVector2 D = Vec2Scale(Delta, 0.5f * Diff);
            Pos[constraint.PartA] = Vec2Add(Pos[constraint.PartA], D);
        }
        if (OneOverMass[constraint.PartB] > 0.0f) {
            TVector2 D = Vec2Scale(Delta, 0.5f * Diff);
            Pos[constraint.PartB] = Vec2Subtract(Pos[constraint.PartB], D);
        }
    }

    inline void TParticleSystem::CreatePart(TVector2 Start, TVector2 Vel, float Mass, int Num) {
        // Num is now the active Part
        Active[Num] = true;
        Pos[Num] = Start;
        Velocity[Num] = Vel;
        OldPos[Num] = Start;
        OneOverMass[Num] = 1.0f / Mass;
    }

    inline void TParticleSystem::MakeConstraint(int PA, int PB, float Rest) {
        ConstraintCount++;
        TConstraint& constraint = Constraints[ConstraintCount];
        constraint.Active = true;
        constraint.PartA = PA;
        constraint.PartB = PB;
        constraint.Restlength = Rest;
    }

    inline void TParticleSystem::Clone(const TParticleSystem& Other) {
        ConstraintCount = Other.ConstraintCount;
        PartCount = Other.PartCount;

        std::memcpy(Active.data() + 1, Other.Active.data() + 1, PartCount * sizeof(bool));
        std::memcpy(Pos.data() + 1, Other.Pos.data() + 1, PartCount * sizeof(TVector2));
        std::memcpy(Velocity.data() + 1, Other.Velocity.data() + 1, PartCount * sizeof(TVector2));
        std::memcpy(OldPos.data() + 1, Other.OldPos.data() + 1, PartCount * sizeof(TVector2));
        std::memcpy(OneOverMass.data() + 1, Other.OneOverMass.data() + 1, PartCount * sizeof(float));

        for (int i = 1; i <= ConstraintCount; i++) {
            const TConstraint& otherConstraint = Other.Constraints[i];
            TConstraint& constraint = Constraints[i];
            constraint.Active = otherConstraint.Active;
            constraint.PartA = otherConstraint.PartA;
            constraint.PartB = otherConstraint.PartB;
            constraint.Restlength = otherConstraint.Restlength;
        }
    }

    inline void TParticleSystem::LoadPOObject(const std::string& Filename, float Scale) {
        // This is a simplified implementation since PhysFS operations need proper integration
        // The full implementation would require PhysFS functionality to be properly handled in C++
    }

    inline void TParticleSystem::StopAllParts() {
        for (int i = 1; i <= NUM_PARTICLES; i++) {
            if (Active[i]) {
                Velocity[i].x = 0.0f;
                Velocity[i].y = 0.0f;
                OldPos[i] = Pos[i];
            }
        }
    }

    inline void TParticleSystem::Destroy() {
        for (int i = 1; i <= NUM_PARTICLES; i++) {
            Active[i] = false;
            Pos[i].x = 0.0f;
            Pos[i].y = 0.0f;
            OldPos[i] = Pos[i];
            Velocity[i].x = 0.0f;
            Velocity[i].y = 0.0f;
            Forces[i].x = 0.0f;
            Forces[i].y = 0.0f;
        }
        ConstraintCount = 0;
    }
}

// Using declarations to bring into global namespace
using PartsImpl::TConstraint;
using PartsImpl::TParticleSystem;

#endif // PARTS_H