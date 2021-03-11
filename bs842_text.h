/*
Project: BS842 Text Rendering w/ stb_truetype
File: bs842_text.h
Author: Brock Salmon
Notice: (C) Copyright 2021 by Brock Salmon. All Rights Reserved
*/

#ifndef STB_TRUETYPE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#endif

//// INTERNAL FUNCTIONS ////

inline s32 bs842_internal_RoundF32ToS32(f32 value)
{
    s32 result = 0;
    
    result = (s32)(value + 0.5f);
    
    return result;
}

inline void *bs842_internal_SetMem(void *block, u8 value, mem_index size)
{
    u8 *pointer = (u8 *)block;
    while (size--)
    {
        *pointer++ = value;
    }
    
    return block;
}

#define bs842_internal_ZeroMem(block, size) bs842_internal_SetMem(block, (u8)0, size)

inline u8 bs842_internal_Lerp(u8 a, u8 b, f32 t)
{
    u8 result = (u8)((1.0f - t) * a + t * b);
    
    return result;
}

////////////////////////////

struct Text_BackBuffer
{
    s32 width;
    s32 height;
    void *memory;
    s32 pitch;
};
#define INTERNAL_ASSERT(expr) if (!(expr)) {*(s32 *)0 = 0;}
#define CHECK_TEXT_BACKBUFFER(backBuffer) \
INTERNAL_ASSERT(backBuffer->width > 0); \
INTERNAL_ASSERT(backBuffer->height > 0); \
INTERNAL_ASSERT(backBuffer->pitch > 0); \
INTERNAL_ASSERT(backBuffer->memory);

function void CreateTextBitmap(unsigned char *result, stbtt_fontinfo *fontInfo, char *text, f32 lineHeight, s32 textSizeX, f32 *charX)
{
    f32 scale = stbtt_ScaleForPixelHeight(fontInfo, lineHeight);
    
    s32 ascent, baseline;
    stbtt_GetFontVMetrics(fontInfo, &ascent, 0, 0);
    baseline = (s32)(ascent * scale);
    
    s32 ch = 0;
    while (text[ch])
    {
        s32 advanceX, leftSideBearing;
        stbtt_GetCodepointHMetrics(fontInfo, text[ch], &advanceX, &leftSideBearing);
        
        s32 chXMin, chXMax, chYMin, chYMax;
        f32 xShift = *charX - (s32)*charX;
        stbtt_GetCodepointBitmapBoxSubpixel(fontInfo, text[ch], scale, scale, xShift, 0, &chXMin, &chYMin, &chXMax, &chYMax);
        
        s32 y = baseline + chYMin;
        
        s32 byteOffset = (s32)*charX + (s32)(leftSideBearing * scale) + (y * textSizeX);
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

function void DrawTextBitmap(void *buffer, unsigned char *textBitmap, u32 colour, f32 charX, f32 xPosPercent, f32 yPosPercent, s32 textSizeX, s32 textSizeY)
{
    Text_BackBuffer *backBuffer = (Text_BackBuffer *)buffer;
    CHECK_TEXT_BACKBUFFER(backBuffer);
    
    s32 xPos = bs842_internal_RoundF32ToS32(backBuffer->width * xPosPercent) - ((s32)charX / 2);
    s32 yPos = bs842_internal_RoundF32ToS32(backBuffer->height * yPosPercent) - (textSizeY / 2);
    
    u8 *row = (u8 *)backBuffer->memory + (xPos * BITMAP_BYTES_PER_PIXEL) + (yPos * backBuffer->pitch);
    for (s32 y = 0; y < textSizeY; ++y)
    {
        u32 *pixel = (u32 *)row;
        for (s32 x = 0; x < textSizeX; ++x)
        {
            if (((u8 *)pixel + BITMAP_BYTES_PER_PIXEL) <= (u8 *)backBuffer->memory + (backBuffer->pitch * backBuffer->height))
            {
                u8 pixelR = (*pixel >> 16) & 0xFF;
                u8 pixelG = (*pixel >> 8) & 0xFF;
                u8 pixelB = *pixel & 0xFF;
                
                u8 colourR = (colour >> 16) & 0xFF;
                u8 colourG = (colour >> 8) & 0xFF;
                u8 colourB = colour & 0xFF;
                
                u8 newR = bs842_internal_Lerp(pixelR, colourR, ((f32)textBitmap[y * textSizeX + x] / 255.0f));
                u8 newG = bs842_internal_Lerp(pixelG, colourG, ((f32)textBitmap[y * textSizeX + x] / 255.0f));
                u8 newB = bs842_internal_Lerp(pixelB, colourB, ((f32)textBitmap[y * textSizeX + x] / 255.0f));
                
                *pixel++ = (0xFF << 24) | (newR << 16) | (newG << 8) | newB;
            }
        }
        
        row += backBuffer->pitch;
    }
}

function void DrawTextBitmap(void *buffer, unsigned char *textBitmap, u32 colour1, u32 colour2, s32 colourChangeX, f32 charX,
                             f32 xPosPercent, f32 yPosPercent, s32 textSizeX, s32 textSizeY)
{
    Text_BackBuffer *backBuffer = (Text_BackBuffer *)buffer;
    CHECK_TEXT_BACKBUFFER(backBuffer);
    
    s32 xPos = bs842_internal_RoundF32ToS32(backBuffer->width * xPosPercent) - ((s32)charX / 2);
    s32 yPos = bs842_internal_RoundF32ToS32(backBuffer->height * yPosPercent) - (textSizeY / 2);
    
    u8 *row = (u8 *)backBuffer->memory + (xPos * BITMAP_BYTES_PER_PIXEL) + (yPos * backBuffer->pitch);
    for (s32 y = 0; y < textSizeY; ++y)
    {
        u32 *pixel = (u32 *)row;
        for (s32 x = 0; x < textSizeX; ++x)
        {
            if (((u8 *)pixel + BITMAP_BYTES_PER_PIXEL) <= (u8 *)backBuffer->memory + (backBuffer->pitch * backBuffer->height))
            {
                u32 colour = colour1;
                if (x >= colourChangeX)
                {
                    colour = colour2;
                }
                
                u8 pixelR = (*pixel >> 16) & 0xFF;
                u8 pixelG = (*pixel >> 8) & 0xFF;
                u8 pixelB = *pixel & 0xFF;
                
                u8 colourR = (colour >> 16) & 0xFF;
                u8 colourG = (colour >> 8) & 0xFF;
                u8 colourB = colour & 0xFF;
                
                u8 newR = bs842_internal_Lerp(pixelR, colourR, ((f32)textBitmap[y * textSizeX + x] / 255.0f));
                u8 newG = bs842_internal_Lerp(pixelG, colourG, ((f32)textBitmap[y * textSizeX + x] / 255.0f));
                u8 newB = bs842_internal_Lerp(pixelB, colourB, ((f32)textBitmap[y * textSizeX + x] / 255.0f));
                
                *pixel++ = (0xFF << 24) | (newR << 16) | (newG << 8) | newB;
            }
        }
        
        row += backBuffer->pitch;
    }
}

function void DrawBasicTextElement(void *buffer, stbtt_fontinfo *fontInfo, f32 sizeRatio, f32 xPosPercent, f32 yPosPercent, s32 textSizeX, f32 lineHeight, char *text, u32 colour)
{
    Text_BackBuffer *backBuffer = (Text_BackBuffer *)buffer;
    CHECK_TEXT_BACKBUFFER(backBuffer);
    
    lineHeight *= sizeRatio;
    textSizeX = (s32)(textSizeX * sizeRatio);
    s32 textSizeY = (s32)(lineHeight);
    
    unsigned char *bitmap = (unsigned char *)malloc((textSizeX * textSizeY) * BITMAP_BYTES_PER_PIXEL);
    bs842_internal_ZeroMem(bitmap, ((textSizeX * textSizeY) * BITMAP_BYTES_PER_PIXEL));
    
    f32 charX = 0;
    CreateTextBitmap(bitmap, fontInfo, text, lineHeight, textSizeX, &charX);
    DrawTextBitmap(backBuffer, bitmap, colour, charX, xPosPercent, yPosPercent, textSizeX, textSizeY);
    free(bitmap);
}

function void DrawBasicTextElement(void *buffer, stbtt_fontinfo *fontInfo, f32 sizeRatio, f32 xPosPercent, f32 yPosPercent, s32 textSizeX, f32 headLineHeight, f32 contentLineHeight, char *headText, char *contentText, u32 headColour, u32 contentColour, f32 lineGap)
{
    Text_BackBuffer *backBuffer = (Text_BackBuffer *)buffer;
    CHECK_TEXT_BACKBUFFER(backBuffer);
    
    headLineHeight *= sizeRatio;
    contentLineHeight *= sizeRatio;
    textSizeX = (s32)(textSizeX * sizeRatio);
    s32 textSizeY = (s32)(headLineHeight + contentLineHeight + (lineGap * backBuffer->height));
    
    unsigned char *headBitmap = (unsigned char *)malloc((textSizeX * textSizeY) * BITMAP_BYTES_PER_PIXEL);
    bs842_internal_ZeroMem(headBitmap, ((textSizeX * textSizeY) * BITMAP_BYTES_PER_PIXEL));
    
    unsigned char *contentBitmap = (unsigned char *)malloc((textSizeX * textSizeY) * BITMAP_BYTES_PER_PIXEL);
    bs842_internal_ZeroMem(contentBitmap, ((textSizeX * textSizeY) * BITMAP_BYTES_PER_PIXEL));
    
    f32 headCharX = 0;
    f32 contentCharX = 0;
    
    CreateTextBitmap(headBitmap, fontInfo, headText, headLineHeight, textSizeX, &headCharX);
    CreateTextBitmap(contentBitmap, fontInfo, contentText, contentLineHeight, textSizeX, &contentCharX);
    
    DrawTextBitmap(backBuffer, headBitmap, headColour, headCharX, xPosPercent, yPosPercent + lineGap, textSizeX, textSizeY);
    DrawTextBitmap(backBuffer, contentBitmap, contentColour, contentCharX, xPosPercent, yPosPercent, textSizeX, textSizeY);
    
    free(contentBitmap);
    free(headBitmap);
}
