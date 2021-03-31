/*
Project: BS842 Text Rendering w/ stb_truetype
File: bs842_text.h
Author: Brock Salmon
Notice: (C) Copyright 2021 by Brock Salmon. All Rights Reserved
Dependencies:
/ stb_truetype: https://github.com/nothings/stb/blob/master/stb_truetype.h
*/

#ifndef BS842_TEXT_H

#ifndef STB_TRUETYPE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#include "../stb/stb_truetype.h"
#endif

//// INTERNAL ////
#define bsint_function static

typedef unsigned __int8 bsint_u8;
typedef unsigned __int32 bsint_u32;
typedef __int32 bsint_s32;
typedef __int32 bsint_b32;
typedef size_t bsint_mem_index;
typedef float bsint_f32;

inline bsint_s32 bs842_text_internal_RoundF32ToS32(bsint_f32 value)
{
    bsint_s32 result = 0;
    
    result = (bsint_s32)(value + 0.5f);
    
    return result;
}

inline void *bs842_internal_SetMem(void *block, bsint_u8 value, bsint_mem_index size)
{
    bsint_u8 *pointer = (bsint_u8 *)block;
    while (size--)
    {
        *pointer++ = value;
    }
    
    return block;
}

#define bs842_internal_ZeroMem(block, size) bs842_internal_SetMem(block, (bsint_u8)0, size)

inline bsint_u8 bs842_internal_Lerp(bsint_u8 a, bsint_u8 b, bsint_f32 t)
{
    bsint_u8 result = (bsint_u8)((1.0f - t) * a + t * b);
    return result;
}

struct Text_BackBuffer
{
    bsint_s32 width;
    bsint_s32 height;
    void *memory;
    bsint_s32 pitch;
};
#define INTERNAL_ASSERT(expr) if (!(expr)) {*(bsint_s32 *)0 = 0;}
#define CHECK_TEXT_BACKBUFFER(backBuffer) \
INTERNAL_ASSERT(backBuffer->width > 0); \
INTERNAL_ASSERT(backBuffer->height > 0); \
INTERNAL_ASSERT(backBuffer->pitch > 0); \
INTERNAL_ASSERT(backBuffer->memory);

#define BITMAP_BYTES_PER_PIXEL 4

//////////////////

bsint_function void BS842_CreateTextBitmap(unsigned char *result, stbtt_fontinfo *fontInfo, char *text, bsint_f32 lineHeight, bsint_s32 textSizeX, bsint_f32 *charX)
{
    bsint_f32 scale = stbtt_ScaleForPixelHeight(fontInfo, lineHeight);
    
    bsint_s32 ascent, baseline;
    stbtt_GetFontVMetrics(fontInfo, &ascent, 0, 0);
    baseline = (bsint_s32)(ascent * scale);
    
    bsint_s32 ch = 0;
    while (text[ch])
    {
        bsint_s32 advanceX, leftSideBearing;
        stbtt_GetCodepointHMetrics(fontInfo, text[ch], &advanceX, &leftSideBearing);
        
        bsint_s32 chXMin, chXMax, chYMin, chYMax;
        bsint_f32 xShift = *charX - (bsint_s32)*charX;
        stbtt_GetCodepointBitmapBoxSubpixel(fontInfo, text[ch], scale, scale, xShift, 0, &chXMin, &chYMin, &chXMax, &chYMax);
        
        bsint_s32 y = baseline + chYMin;
        
        bsint_s32 byteOffset = (bsint_s32)*charX + (bsint_s32)(leftSideBearing * scale) + (y * textSizeX);
        stbtt_MakeCodepointBitmapSubpixel(fontInfo, result + byteOffset,
                                          chXMax - chXMin, chYMax - chYMin, textSizeX, scale, scale, xShift, 0, text[ch]);
        
        *charX += (advanceX * scale);
        if (text[ch + 1])
        {
            *charX += scale * stbtt_GetCodepointKernAdvance(fontInfo, text[ch], text[ch + 1]);
        }
        
        ++ch;
    }
}

bsint_function void BS842_DrawTextBitmap(void *buffer, unsigned char *textBitmap, bsint_u32 colour, bsint_f32 charX, bsint_f32 xPosPercent, bsint_f32 yPosPercent, bsint_s32 textSizeX, bsint_s32 textSizeY, bsint_b32 topLeftAlign = false)
{
    Text_BackBuffer *backBuffer = (Text_BackBuffer *)buffer;
    CHECK_TEXT_BACKBUFFER(backBuffer);
    
    bsint_s32 xPos = 0;
    bsint_s32 yPos = 0;
    
    if (topLeftAlign)
    {
         xPos = bs842_text_internal_RoundF32ToS32(backBuffer->width * xPosPercent);
         yPos = bs842_text_internal_RoundF32ToS32(backBuffer->height * yPosPercent);
    }
    else
    {
        xPos = bs842_text_internal_RoundF32ToS32(backBuffer->width * xPosPercent) - ((bsint_s32)charX / 2);
        yPos = bs842_text_internal_RoundF32ToS32(backBuffer->height * yPosPercent) - (textSizeY / 2);
    }
    
    bsint_u8 *row = (bsint_u8 *)backBuffer->memory + (xPos * BITMAP_BYTES_PER_PIXEL) + (yPos * backBuffer->pitch);
    for (bsint_s32 y = 0; y < textSizeY; ++y)
    {
        bsint_u32 *pixel = (bsint_u32 *)row;
        for (bsint_s32 x = 0; x < textSizeX; ++x)
        {
            if (((bsint_u8 *)pixel + BITMAP_BYTES_PER_PIXEL) <= (bsint_u8 *)backBuffer->memory + (backBuffer->pitch * backBuffer->height))
            {
                bsint_u8 pixelR = (*pixel >> 16) & 0xFF;
                bsint_u8 pixelG = (*pixel >> 8) & 0xFF;
                bsint_u8 pixelB = *pixel & 0xFF;
                
                bsint_u8 colourR = (colour >> 16) & 0xFF;
                bsint_u8 colourG = (colour >> 8) & 0xFF;
                bsint_u8 colourB = colour & 0xFF;
                
                bsint_u8 newR = bs842_internal_Lerp(pixelR, colourR, ((bsint_f32)textBitmap[y * textSizeX + x] / 255.0f));
                bsint_u8 newG = bs842_internal_Lerp(pixelG, colourG, ((bsint_f32)textBitmap[y * textSizeX + x] / 255.0f));
                bsint_u8 newB = bs842_internal_Lerp(pixelB, colourB, ((bsint_f32)textBitmap[y * textSizeX + x] / 255.0f));
                
                *pixel++ = (0xFF << 24) | (newR << 16) | (newG << 8) | newB;
            }
        }
        
        row += backBuffer->pitch;
    }
}

bsint_function void BS842_DrawTextBitmap(void *buffer, unsigned char *textBitmap, bsint_u32 colour1, bsint_u32 colour2, bsint_s32 colourChangeX, bsint_f32 charX,
                             bsint_f32 xPosPercent, bsint_f32 yPosPercent, bsint_s32 textSizeX, bsint_s32 textSizeY)
{
    Text_BackBuffer *backBuffer = (Text_BackBuffer *)buffer;
    CHECK_TEXT_BACKBUFFER(backBuffer);
    
    bsint_s32 xPos = bs842_text_internal_RoundF32ToS32(backBuffer->width * xPosPercent) - ((bsint_s32)charX / 2);
    bsint_s32 yPos = bs842_text_internal_RoundF32ToS32(backBuffer->height * yPosPercent) - (textSizeY / 2);
    
    bsint_u8 *row = (bsint_u8 *)backBuffer->memory + (xPos * BITMAP_BYTES_PER_PIXEL) + (yPos * backBuffer->pitch);
    for (bsint_s32 y = 0; y < textSizeY; ++y)
    {
        bsint_u32 *pixel = (bsint_u32 *)row;
        for (bsint_s32 x = 0; x < textSizeX; ++x)
        {
            if (((bsint_u8 *)pixel + BITMAP_BYTES_PER_PIXEL) <= (bsint_u8 *)backBuffer->memory + (backBuffer->pitch * backBuffer->height))
            {
                bsint_u32 colour = colour1;
                if (x >= colourChangeX)
                {
                    colour = colour2;
                }
                
                bsint_u8 pixelR = (*pixel >> 16) & 0xFF;
                bsint_u8 pixelG = (*pixel >> 8) & 0xFF;
                bsint_u8 pixelB = *pixel & 0xFF;
                
                bsint_u8 colourR = (colour >> 16) & 0xFF;
                bsint_u8 colourG = (colour >> 8) & 0xFF;
                bsint_u8 colourB = colour & 0xFF;
                
                bsint_u8 newR = bs842_internal_Lerp(pixelR, colourR, ((bsint_f32)textBitmap[y * textSizeX + x] / 255.0f));
                bsint_u8 newG = bs842_internal_Lerp(pixelG, colourG, ((bsint_f32)textBitmap[y * textSizeX + x] / 255.0f));
                bsint_u8 newB = bs842_internal_Lerp(pixelB, colourB, ((bsint_f32)textBitmap[y * textSizeX + x] / 255.0f));
                
                *pixel++ = (0xFF << 24) | (newR << 16) | (newG << 8) | newB;
            }
        }
        
        row += backBuffer->pitch;
    }
}

bsint_function void BS842_DrawBasicTextElement(void *buffer, stbtt_fontinfo *fontInfo, bsint_f32 sizeRatio, bsint_f32 xPosPercent, bsint_f32 yPosPercent, bsint_s32 textSizeX, bsint_f32 lineHeight, char *text, bsint_u32 colour)
{
    Text_BackBuffer *backBuffer = (Text_BackBuffer *)buffer;
    CHECK_TEXT_BACKBUFFER(backBuffer);
    
    lineHeight *= sizeRatio;
    textSizeX = (bsint_s32)(textSizeX * sizeRatio);
    bsint_s32 textSizeY = (bsint_s32)(lineHeight);
    
    unsigned char *bitmap = (unsigned char *)malloc((textSizeX * textSizeY) * BITMAP_BYTES_PER_PIXEL);
    bs842_internal_ZeroMem(bitmap, ((textSizeX * textSizeY) * BITMAP_BYTES_PER_PIXEL));
    
    bsint_f32 charX = 0;
    BS842_CreateTextBitmap(bitmap, fontInfo, text, lineHeight, textSizeX, &charX);
    BS842_DrawTextBitmap(backBuffer, bitmap, colour, charX, xPosPercent, yPosPercent, textSizeX, textSizeY);
    free(bitmap);
}

bsint_function void BS842_DrawBasicTextElement(void *buffer, stbtt_fontinfo *fontInfo, bsint_f32 sizeRatio, bsint_f32 xPosPercent, bsint_f32 yPosPercent, bsint_s32 textSizeX, bsint_f32 headLineHeight, bsint_f32 contentLineHeight, char *headText, char *contentText, bsint_u32 headColour, bsint_u32 contentColour, bsint_f32 lineGap)
{
    Text_BackBuffer *backBuffer = (Text_BackBuffer *)buffer;
    CHECK_TEXT_BACKBUFFER(backBuffer);
    
    headLineHeight *= sizeRatio;
    contentLineHeight *= sizeRatio;
    textSizeX = (bsint_s32)(textSizeX * sizeRatio);
    bsint_s32 textSizeY = (bsint_s32)(headLineHeight + contentLineHeight + (lineGap * backBuffer->height));
    
    unsigned char *headBitmap = (unsigned char *)malloc((textSizeX * textSizeY) * BITMAP_BYTES_PER_PIXEL);
    bs842_internal_ZeroMem(headBitmap, ((textSizeX * textSizeY) * BITMAP_BYTES_PER_PIXEL));
    
    unsigned char *contentBitmap = (unsigned char *)malloc((textSizeX * textSizeY) * BITMAP_BYTES_PER_PIXEL);
    bs842_internal_ZeroMem(contentBitmap, ((textSizeX * textSizeY) * BITMAP_BYTES_PER_PIXEL));
    
    bsint_f32 headCharX = 0;
    bsint_f32 contentCharX = 0;
    
    BS842_CreateTextBitmap(headBitmap, fontInfo, headText, headLineHeight, textSizeX, &headCharX);
    BS842_CreateTextBitmap(contentBitmap, fontInfo, contentText, contentLineHeight, textSizeX, &contentCharX);
    
    BS842_DrawTextBitmap(backBuffer, headBitmap, headColour, headCharX, xPosPercent, yPosPercent + lineGap, textSizeX, textSizeY);
    BS842_DrawTextBitmap(backBuffer, contentBitmap, contentColour, contentCharX, xPosPercent, yPosPercent, textSizeX, textSizeY);
    
    free(contentBitmap);
    free(headBitmap);
}

#define BS842_TEXT_H
#endif // BS842_TEXT_H