#ifndef BINPACK_H
#define BINPACK_H

//*******************************************************************************
//                                                                              
//       BinPack Unit for SOLDAT                                                 
//                                                                              
//       Copyright (c) 2015 Mariano Cuatrin            
//                                                                              
//*******************************************************************************

#include <vector>
#include <memory>
#include <algorithm>
#include <climits>
#include <cstring>

// Structure representing a rectangle for packing
struct TBPRect {
    int x, y;      // Position
    int w, h;      // Dimensions
    void* Data;    // User-defined pointer data
    
    TBPRect() : x(0), y(0), w(0), h(0), Data(nullptr) {}
    TBPRect(int x_, int y_, int w_, int h_, void* data_) : 
        x(x_), y(y_), w(w_), h(h_), Data(data_) {}
};

using TBPRectArray = std::vector<TBPRect>;

// Custom dynamic array class to simulate Pascal's TRectList class behavior
class TRectList {
private:
    std::unique_ptr<TBPRect[]> FData;
    int FSize;
    int FCapacity;
    
public:
    TRectList() : FSize(0), FCapacity(16) {
        FData = std::make_unique<TBPRect[]>(FCapacity);
    }
    
    void Push(const TBPRect& rect) {
        if (FSize == FCapacity) {
            FCapacity *= 2;
            std::unique_ptr<TBPRect[]> newData = std::make_unique<TBPRect[]>(FCapacity);
            std::memcpy(newData.get(), FData.get(), FSize * sizeof(TBPRect));
            FData = std::move(newData);
        }
        
        FData[FSize] = rect;
        FSize++;
    }
    
    void Remove(int index) {
        if (index >= 0 && index < FSize) {
            // Shift elements to fill the gap
            for (int i = index; i < FSize - 1; i++) {
                FData[i] = FData[i + 1];
            }
            FSize--;
        }
    }
    
    int GetSize() const { return FSize; }
    
    const TBPRect& GetItem(int index) const {
        static TBPRect empty; // Static default rect for out of bounds access
        if (index >= 0 && index < FSize) {
            return FData[index];
        }
        return empty;
    }
    
    void SetItem(int index, const TBPRect& value) {
        if (index >= 0 && index < FCapacity) {
            if (index >= FSize) {
                FSize = index + 1;  // Expand the size if needed
            }
            FData[index] = value;
        }
    }
    
    const TBPRect& operator[](int index) const {
        return GetItem(index);
    }
    
    TBPRect& operator[](int index) {
        if (index >= 0 && index >= FSize) {
            FSize = index + 1;  // Expand the size if needed
        }
        return FData[index];
    }
};

// Internal bin packing structure
struct TBinPack {
    TRectList Used;
    TRectList Free;
    
    TBinPack() = default;
};

// Internal functions declarations
TBPRect ScoreRect(TBinPack& bp, int w, int h, int& BestY, int& BestX);
void PlaceRect(TBinPack& bp, const TBPRect& Rect);
bool SplitFreeRect(TBinPack& bp, TBPRect FreeRect, const TBPRect& UsedRect);
void PruneFreeList(TBinPack& bp);
bool IsContainedIn(const TBPRect& a, const TBPRect& b);

/**
 * Function to pack rectangles into a bin of specified width and height
 * 
 * @param w Width of the bin
 * @param h Height of the bin 
 * @param Rects Array of rectangles to pack (will be modified with placements)
 * @return Number of successfully placed rectangles
 */
int PackRects(int w, int h, std::vector<TBPRect>& Rects);

namespace BinPackImpl {
    inline TBPRect ScoreRect(TBinPack& bp, int w, int h, int& BestY, int& BestX) {
        TBPRect bestRect;
        bestRect.x = 0;
        bestRect.y = 0;
        bestRect.w = 0;
        bestRect.h = 0;
        
        BestX = INT_MAX;
        BestY = INT_MAX;
        
        for (int i = 0; i < bp.Free.GetSize(); i++) {
            TBPRect freeRect = bp.Free[i];
            if ((freeRect.w >= w) && (freeRect.h >= h)) {
                int topSideY = freeRect.y + h;
                
                if ((topSideY < BestY) || ((topSideY == BestY) && (freeRect.x < BestX))) {
                    bestRect.x = freeRect.x;
                    bestRect.y = freeRect.y;
                    bestRect.w = w;
                    bestRect.h = h;
                    BestY = topSideY;
                    BestX = freeRect.x;
                }
            }
        }
        
        if (bestRect.h == 0) {
            BestX = INT_MAX;
            BestY = INT_MAX;
        }
        
        return bestRect;
    }

    inline void PlaceRect(TBinPack& bp, const TBPRect& Rect) {
        int i = 0;
        int n = bp.Free.GetSize();
        
        while (i < n) {
            if (SplitFreeRect(bp, bp.Free[i], Rect)) {
                bp.Free.Remove(i);
                n--;
                // Don't increment i since we removed an element, so next element shifted down
            } else {
                i++;  // Only increment if we didn't remove
            }
        }
        
        PruneFreeList(bp);
        bp.Used.Push(Rect);
    }

    inline bool SplitFreeRect(TBinPack& bp, TBPRect FreeRect, const TBPRect& UsedRect) {
        // Check if rectangles don't overlap
        if ((UsedRect.x >= (FreeRect.x + FreeRect.w)) ||
            ((UsedRect.x + UsedRect.w) <= FreeRect.x) ||
            (UsedRect.y >= (FreeRect.y + FreeRect.h)) ||
            ((UsedRect.y + UsedRect.h) <= FreeRect.y)) {
            return false;
        }
        
        // Horizontal splits - split along Y-axis
        if ((UsedRect.x < (FreeRect.x + FreeRect.w)) &&
            ((UsedRect.x + UsedRect.w) > FreeRect.x)) {
            
            // Top part of free rect above used rect
            if ((UsedRect.y > FreeRect.y) && (UsedRect.y < (FreeRect.y + FreeRect.h))) {
                TBPRect rect = FreeRect;
                rect.h = UsedRect.y - rect.y;
                bp.Free.Push(rect);
            }
            
            // Bottom part of free rect below used rect
            if ((UsedRect.y + UsedRect.h) < (FreeRect.y + FreeRect.h)) {
                TBPRect rect = FreeRect;
                rect.y = UsedRect.y + UsedRect.h;
                rect.h = (FreeRect.y + FreeRect.h) - (UsedRect.y + UsedRect.h);
                bp.Free.Push(rect);
            }
        }
        
        // Vertical splits - split along X-axis
        if ((UsedRect.y < (FreeRect.y + FreeRect.h)) &&
            ((UsedRect.y + UsedRect.h) > FreeRect.y)) {
            
            // Left part of free rect to the left of used rect
            if ((UsedRect.x > FreeRect.x) && (UsedRect.x < (FreeRect.x + FreeRect.w))) {
                TBPRect rect = FreeRect;
                rect.w = UsedRect.x - rect.x;
                bp.Free.Push(rect);
            }
            
            // Right part of free rect to the right of used rect
            if ((UsedRect.x + UsedRect.w) < (FreeRect.x + FreeRect.w)) {
                TBPRect rect = FreeRect;
                rect.x = UsedRect.x + UsedRect.w;
                rect.w = (FreeRect.x + FreeRect.w) - (UsedRect.x + UsedRect.w);
                bp.Free.Push(rect);
            }
        }
        
        return true;
    }

    inline void PruneFreeList(TBinPack& bp) {
        int i = 0;
        int n = bp.Free.GetSize();
        
        while (i < n) {
            int j = i + 1;
            
            while (j < n) {
                if (IsContainedIn(bp.Free[i], bp.Free[j])) {
                    bp.Free.Remove(i);
                    i--;
                    n--;
                    break; // Exit the inner loop after removing
                }
                
                if (IsContainedIn(bp.Free[j], bp.Free[i])) {
                    bp.Free.Remove(j);
                    j--;
                    n--;
                }
                
                j++;
            }
            
            i++;
        }
    }

    inline bool IsContainedIn(const TBPRect& a, const TBPRect& b) {
        return (a.x >= b.x) && (a.y >= b.y) && 
               ((a.x + a.w) <= (b.x + b.w)) && 
               ((a.y + a.h) <= (b.y + b.h));
    }

    inline int PackRects(int w, int h, std::vector<TBPRect>& Rects) {
        TBinPack bp;
        
        // Create initial free space covering the entire bin
        TBPRect initialRect(0, 0, w, h, nullptr);
        bp.Free.Push(initialRect);
        
        int i = 0;
        
        while (i <= static_cast<int>(Rects.size()) - 1) {
            int bestIndex = -1;
            int bestScore1 = INT_MAX;
            int bestScore2 = INT_MAX;
            
            for (int j = i; j <= static_cast<int>(Rects.size()) - 1; j++) {
                int score1 = 0, score2 = 0;
                TBPRect rect = ScoreRect(bp, Rects[j].w, Rects[j].h, score1, score2);

                if ((score1 < bestScore1) || ((score1 == bestScore1) && (score2 < bestScore2))) {
                    bestScore1 = score1;
                    bestScore2 = score2;
                    // Note: rect is already the best rectangle here
                    // But we need to use it properly as BestRect
                    bestIndex = j;
                }
            }

            if (bestIndex == -1) {
                break; // No more rectangles can be placed
            }
            
            // Get the best rectangle to place (recompute since ScoreRect is called again)
            int score1 = 0, score2 = 0;
            TBPRect bestRect = ScoreRect(bp, Rects[bestIndex].w, Rects[bestIndex].h, score1, score2);

            if (bestRect.h == 0) {
                break; // Can't place any more rectangles
            }

            PlaceRect(bp, bestRect);

            bestRect.Data = Rects[bestIndex].Data;  // Preserve data pointer
            // Swap bestIndex with i and place the best rectangle at index i
            TBPRect temp = Rects[bestIndex];
            Rects[bestIndex] = Rects[i];
            Rects[i] = bestRect;
            
            i++;
        }
        
        return i;  // Return number of placed rectangles
    }
}

// Using declarations to bring functions into global namespace
using BinPackImpl::TBPRect;
using BinPackImpl::TBPRectArray;
using BinPackImpl::TRectList;
using BinPackImpl::TBinPack;
using BinPackImpl::ScoreRect;
using BinPackImpl::PlaceRect;
using BinPackImpl::SplitFreeRect;
using BinPackImpl::PruneFreeList;
using BinPackImpl::IsContainedIn;
using BinPackImpl::PackRects;

#endif // BINPACK_H