/*
Project: BS842 Tools
File: bs842_2dprim.h
Author: Brock Salmon
Notice: (C) Copyright 2021 by Brock Salmon. All Rights Reserved
*/

// TODO(bSalmon): SIMD

#ifndef BS842_2DPRIM_H

//// INTERNAL ////
#define bsint_function static

typedef unsigned __int8 bsint_u8;
typedef unsigned __int32 bsint_u32;
typedef __int32 bsint_s32;
typedef size_t bsint_mem_index;
typedef float bsint_f32;

inline bsint_s32 bs842_prim_internal_RoundF32ToS32(bsint_f32 value)
{
    bsint_s32 result = (bsint_s32)(value + 0.5f);
    return result;
}

inline void *bs842_internal_SetMem(void *block, bsint_u32 value, bsint_mem_index size)
{
    bsint_u32 *pointer = (bsint_u32 *)block;
    while (size--)
    {
        *pointer++ = value;
    }
    
    return block;
}

inline bsint_s32 bs842_internal_Lerp(bsint_s32 a, bsint_s32 b, bsint_f32 t)
{
    bsint_s32 result = (bsint_s32)((1.0f - t) * a + t * b);
    return result;
}

inline bsint_s32 bs842_internal_Abs(bsint_s32 value)
{
    bsint_s32 result = (value < 0) ? -value : value;
    return result;
}

inline bsint_f32 bs842_internal_SqRt(bsint_f32 value)
{
    bsint_f32 temp = 0.0f;
    bsint_f32 result = value / 2.0f;
    
    while (result != temp)
    {
        temp = result;
        result = (value / temp + temp) / 2.0f;
    }
    
    return result;
}

#ifndef BSINTERNAL_BACKBUFFER
struct BSInternal_BackBuffer
{
    bsint_s32 width;
    bsint_s32 height;
    void *memory;
    bsint_s32 pitch;
};
#define BSINTERNAL_BACKBUFFER
#endif

struct BSInternal_SizeSpec
{
    bsint_s32 x1;
    bsint_s32 x2;
    bsint_s32 y1;
    bsint_s32 y2;
};

#define INTERNAL_BITMAP_BYTES_PER_PIXEL 4

#if 1
#define INTERNAL_ASSERT(expr) if (!(expr)) {*(bsint_s32 *)0 = 0;}
#else
#define INTERNAL_ASSERT(expr) 
#endif
#define CHECK_INTERNAL_BACKBUFFER(backBuffer) \
INTERNAL_ASSERT(backBuffer->width > 0); \
INTERNAL_ASSERT(backBuffer->height > 0); \
INTERNAL_ASSERT(backBuffer->pitch > 0); \
INTERNAL_ASSERT(backBuffer->memory);

#define INTERNAL_SWAP(a, b) {decltype(a) temp = a; a = b; b = temp;}
//////////////////

// NOTE(bSalmon): For Lines: x1 & y1 designate one end of the line, x2 & y2 designate the other
// NOTE(bSalmon): For Boxes: x1 & y1 designate the top left corner, x2 & y2 designate the bottom right corner
struct BS842_Prim_SizeSpec
{
    bsint_f32 x1;
    bsint_f32 x2;
    
    bsint_f32 y1;
    bsint_f32 y2;
};


bsint_function BSInternal_SizeSpec bs842_internal_ConvertSizeSpec(BSInternal_BackBuffer *backBuffer, BS842_Prim_SizeSpec sizeSpec)
{
    BSInternal_SizeSpec result = {};
    
    result.x1 = bs842_prim_internal_RoundF32ToS32(sizeSpec.x1 * backBuffer->width);
    result.x1 = (result.x1 < 0) ? 0 : ((result.x1 > backBuffer->width) ? backBuffer->width : result.x1);
    result.x2 = bs842_prim_internal_RoundF32ToS32(sizeSpec.x2 * backBuffer->width);
    result.x2 = (result.x2 < 0) ? 0 : ((result.x2 > backBuffer->width) ? backBuffer->width : result.x2);
    result.y1 = bs842_prim_internal_RoundF32ToS32(sizeSpec.y1 * backBuffer->height);
    result.y1 = (result.y1 < 0) ? 0 : ((result.y1 > backBuffer->height) ? backBuffer->height : result.y1);
    result.y2 = bs842_prim_internal_RoundF32ToS32(sizeSpec.y2 * backBuffer->height);
    result.y2 = (result.y2 < 0) ? 0 : ((result.y2 > backBuffer->height) ? backBuffer->height : result.y2);
    
    return result;
}

bsint_function BS842_Prim_SizeSpec BS842_FillSizeSpec(bsint_f32 x1, bsint_f32 x2, bsint_f32 y1, bsint_f32 y2)
{
    BS842_Prim_SizeSpec result = {};
    
    result.x1 = x1;
    result.x2 = x2;
    result.y1 = y1;
    result.y2 = y2;
    
     return result;
}

bsint_function BSInternal_SizeSpec BS842_FillSizeSpec(bsint_s32 x1, bsint_s32 x2, bsint_s32 y1, bsint_s32 y2)
{
    BSInternal_SizeSpec result = {};
    
    result.x1 = x1;
    result.x2 = x2;
    result.y1 = y1;
    result.y2 = y2;
    
    return result;
}

bsint_function void BS842_Clear(void *buffer, bsint_u32 colour)
{
    BSInternal_BackBuffer *backBuffer = (BSInternal_BackBuffer *)buffer;
    CHECK_INTERNAL_BACKBUFFER(backBuffer);
    
    // NOTE(bSalmon): Goes by pixel rather than byte so height * pitch causes an exception
    bs842_internal_SetMem(backBuffer->memory, colour, backBuffer->height * backBuffer->width);
}

// NOTE(bSalmon): Not Anti-Aliased
bsint_function void BS842_DrawLine(void *buffer, BS842_Prim_SizeSpec sizeSpec, bsint_f32 lineThickness, bsint_u32 colour)
{
    BSInternal_BackBuffer *backBuffer = (BSInternal_BackBuffer *)buffer;
    CHECK_INTERNAL_BACKBUFFER(backBuffer);
    
    BSInternal_SizeSpec int_sizeSpec = bs842_internal_ConvertSizeSpec(backBuffer, sizeSpec);
    
    // NOTE(bSalmon): Modified Bresenham Line Algorithm: http://members.chello.at/~easyfilter/bresenham.html
    bsint_s32 dx = bs842_internal_Abs(int_sizeSpec.x2 - int_sizeSpec.x1);
    bsint_s32 dy = bs842_internal_Abs(int_sizeSpec.y2 - int_sizeSpec.y1);
    
    bsint_s32 sx = (int_sizeSpec.x1 < int_sizeSpec.x2) ? 1 : -1;
    bsint_s32 sy = (int_sizeSpec.y1 < int_sizeSpec.y2) ? 1 : -1;
    
    bsint_s32 err = dx - dy;
    
    bsint_f32 ed = ((dx + dy) == 0) ? 1 : bs842_internal_SqRt((bsint_f32)dx * dx + (bsint_f32)dy * dy);
    bsint_s32 y3;
    
    for (lineThickness = (lineThickness + 1) / 2;;)
    {
        *((bsint_u32 *)backBuffer->memory + (backBuffer->width * int_sizeSpec.y1) + int_sizeSpec.x1) = colour;
        
        bsint_s32 e2 = err;
        bsint_s32 x3 = int_sizeSpec.x1;
        
        if (2 * e2 >= -dx)
        {
            for (e2 += dy, y3 = int_sizeSpec.y1; (e2 < ed * lineThickness) && ((int_sizeSpec.y2 != y3) || (dx > dy)); e2 += dx)
            {
                *((bsint_u32 *)backBuffer->memory + (backBuffer->width * (y3 += sy)) + int_sizeSpec.x1) = colour;
            }
            
            if (int_sizeSpec.x1 == int_sizeSpec.x2)
            {
                break;
            }
            
            e2 = err;
            err -= dy;
            int_sizeSpec.x1 += sx;
        }
        if (2 * e2 <= dy)
        {
            for (e2 = dx - e2; (e2 < ed * lineThickness) && ((int_sizeSpec.x2 != x3) || (dx < dy)); e2 += dy)
            {
                *((bsint_u32 *)backBuffer->memory + (backBuffer->width * int_sizeSpec.y1) + (x3 += sx)) = colour;
            }
            
            if (int_sizeSpec.y1 == int_sizeSpec.y2)
            {
                break;
            }
            
            err += dx;
            int_sizeSpec.y1 += sy;
        }
    }
}

bsint_function void BS842_DrawLine(void *buffer, BSInternal_SizeSpec sizeSpec, bsint_f32 lineThickness, bsint_u32 colour)
{
    BSInternal_BackBuffer *backBuffer = (BSInternal_BackBuffer *)buffer;
    CHECK_INTERNAL_BACKBUFFER(backBuffer);
    
    // NOTE(bSalmon): Modified Bresenham Line Algorithm: http://members.chello.at/~easyfilter/bresenham.html
    bsint_s32 dx = bs842_internal_Abs(sizeSpec.x2 - sizeSpec.x1);
    bsint_s32 dy = bs842_internal_Abs(sizeSpec.y2 - sizeSpec.y1);
    
    bsint_s32 sx = (sizeSpec.x1 < sizeSpec.x2) ? 1 : -1;
    bsint_s32 sy = (sizeSpec.y1 < sizeSpec.y2) ? 1 : -1;
    
    bsint_s32 err = dx - dy;
    
    bsint_f32 ed = ((dx + dy) == 0) ? 1 : bs842_internal_SqRt((bsint_f32)dx * dx + (bsint_f32)dy * dy);
    bsint_s32 y3;
    
    for (lineThickness = (lineThickness + 1) / 2;;)
    {
        *((bsint_u32 *)backBuffer->memory + (backBuffer->width * sizeSpec.y1) + sizeSpec.x1) = colour;
        
        bsint_s32 e2 = err;
        bsint_s32 x3 = sizeSpec.x1;
        
        if (2 * e2 >= -dx)
        {
            for (e2 += dy, y3 = sizeSpec.y1; (e2 < ed * lineThickness) && ((sizeSpec.y2 != y3) || (dx > dy)); e2 += dx)
            {
                *((bsint_u32 *)backBuffer->memory + (backBuffer->width * (y3 += sy)) + sizeSpec.x1) = colour;
            }
            
            if (sizeSpec.x1 == sizeSpec.x2)
            {
                break;
            }
            
            e2 = err;
            err -= dy;
            sizeSpec.x1 += sx;
        }
        if (2 * e2 <= dy)
        {
            for (e2 = dx - e2; (e2 < ed * lineThickness) && ((sizeSpec.x2 != x3) || (dx < dy)); e2 += dy)
            {
                *((bsint_u32 *)backBuffer->memory + (backBuffer->width * sizeSpec.y1) + (x3 += sx)) = colour;
            }
            
            if (sizeSpec.y1 == sizeSpec.y2)
            {
                break;
            }
            
            err += dx;
            sizeSpec.y1 += sy;
        }
    }
}

bsint_function void BS842_DrawSolidBox(void *buffer, BS842_Prim_SizeSpec sizeSpec, bsint_u32 colour)
{
    BSInternal_BackBuffer *backBuffer = (BSInternal_BackBuffer *)buffer;
    CHECK_INTERNAL_BACKBUFFER(backBuffer);
    
    if (sizeSpec.x1 > sizeSpec.x2)
    {
        INTERNAL_SWAP(sizeSpec.x1, sizeSpec.x2);
    }
    if (sizeSpec.y1 > sizeSpec.y2)
    {
        INTERNAL_SWAP(sizeSpec.y1, sizeSpec.y2);
    }
    
    BSInternal_SizeSpec int_sizeSpec = bs842_internal_ConvertSizeSpec(backBuffer, sizeSpec);
    
    for (bsint_s32 y = int_sizeSpec.y1; y <= int_sizeSpec.y2; ++y)
    {
        bs842_internal_SetMem((bsint_u8 *)backBuffer->memory + (y * backBuffer->pitch) + (int_sizeSpec.x1 * INTERNAL_BITMAP_BYTES_PER_PIXEL), colour, int_sizeSpec.x2 - int_sizeSpec.x1);
    }
}

bsint_function void BS842_DrawSolidBox(void *buffer, BSInternal_SizeSpec sizeSpec, bsint_u32 colour)
{
    BSInternal_BackBuffer *backBuffer = (BSInternal_BackBuffer *)buffer;
    CHECK_INTERNAL_BACKBUFFER(backBuffer);
    
    if (sizeSpec.x1 > sizeSpec.x2)
    {
        INTERNAL_SWAP(sizeSpec.x1, sizeSpec.x2);
    }
    if (sizeSpec.y1 > sizeSpec.y2)
    {
        INTERNAL_SWAP(sizeSpec.y1, sizeSpec.y2);
    }
    
    for (bsint_s32 y = sizeSpec.y1; y <= sizeSpec.y2; ++y)
    {
        bs842_internal_SetMem((bsint_u8 *)backBuffer->memory + (y * backBuffer->pitch) + (sizeSpec.x1 * INTERNAL_BITMAP_BYTES_PER_PIXEL), colour, sizeSpec.x2 - sizeSpec.x1);
    }
}

bsint_function void BS842_DrawHollowBox(void *buffer, BS842_Prim_SizeSpec sizeSpec, bsint_f32 lineThickness, bsint_u32 colour)
{
    if (sizeSpec.x1 > sizeSpec.x2)
    {
        INTERNAL_SWAP(sizeSpec.x1, sizeSpec.x2);
    }
    if (sizeSpec.y1 > sizeSpec.y2)
    {
        INTERNAL_SWAP(sizeSpec.y1, sizeSpec.y2);
    }
    
    BS842_Prim_SizeSpec top = BS842_FillSizeSpec(sizeSpec.x1, sizeSpec.x2, sizeSpec.y1, sizeSpec.y1);
        BS842_DrawLine(buffer, top, lineThickness, colour);
    
    BS842_Prim_SizeSpec bottom = BS842_FillSizeSpec(sizeSpec.x1, sizeSpec.x2, sizeSpec.y2, sizeSpec.y2);
        BS842_DrawLine(buffer, bottom, lineThickness, colour);
    
    BS842_Prim_SizeSpec left = BS842_FillSizeSpec(sizeSpec.x1, sizeSpec.x1, sizeSpec.y1, sizeSpec.y2);
        BS842_DrawLine(buffer, left, lineThickness, colour);
    
    BS842_Prim_SizeSpec right = BS842_FillSizeSpec(sizeSpec.x2, sizeSpec.x2, sizeSpec.y1, sizeSpec.y2);
        BS842_DrawLine(buffer, right, lineThickness, colour);
}

bsint_function void BS842_DrawHollowBox(void *buffer, BSInternal_SizeSpec sizeSpec, bsint_f32 lineThickness, bsint_u32 colour)
{
    if (sizeSpec.x1 > sizeSpec.x2)
    {
        INTERNAL_SWAP(sizeSpec.x1, sizeSpec.x2);
    }
    if (sizeSpec.y1 > sizeSpec.y2)
    {
        INTERNAL_SWAP(sizeSpec.y1, sizeSpec.y2);
    }
    
    BSInternal_SizeSpec top = BS842_FillSizeSpec(sizeSpec.x1, sizeSpec.x2, sizeSpec.y1, sizeSpec.y1);
    BS842_DrawLine(buffer, top, lineThickness, colour);
    
    BSInternal_SizeSpec bottom = BS842_FillSizeSpec(sizeSpec.x1, sizeSpec.x2, sizeSpec.y2, sizeSpec.y2);
    BS842_DrawLine(buffer, bottom, lineThickness, colour);
    
    BSInternal_SizeSpec left = BS842_FillSizeSpec(sizeSpec.x1, sizeSpec.x1, sizeSpec.y1, sizeSpec.y2);
    BS842_DrawLine(buffer, left, lineThickness, colour);
    
    BSInternal_SizeSpec right = BS842_FillSizeSpec(sizeSpec.x2, sizeSpec.x2, sizeSpec.y1, sizeSpec.y2);
    BS842_DrawLine(buffer, right, lineThickness, colour);
}

bsint_function void BS842_DrawOutlinedBox(void *buffer, BS842_Prim_SizeSpec sizeSpec, bsint_f32 lineThickness, bsint_u32 colour1, bsint_u32 colour2)
{
    BS842_DrawSolidBox(buffer, sizeSpec, colour1);
    BS842_DrawHollowBox(buffer, sizeSpec, lineThickness, colour2);
}


bsint_function void BS842_DrawOutlinedBox(void *buffer, BSInternal_SizeSpec sizeSpec, bsint_f32 lineThickness, bsint_u32 colour1, bsint_u32 colour2)
{
    BS842_DrawSolidBox(buffer, sizeSpec, colour1);
    BS842_DrawHollowBox(buffer, sizeSpec, lineThickness, colour2);
}


#define BS842_2DPRIM_H
#endif // BS842_2DPRIM_H