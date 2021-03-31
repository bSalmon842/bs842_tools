/*
Project: BS842 Tools
File: bs842_imgui.h
Author: Brock Salmon
Notice: (C) Copyright 2021 by Brock Salmon. All Rights Reserved
Dependencies:
/ bs842_text: https://github.com/bSalmon842/bs842_tools/blob/master/bs842_text.h
// stb_truetype: https://github.com/nothings/stb/blob/master/stb_truetype.h
/ bs842_2dprim: https://github.com/bSalmon842/bs842_tools/blob/master/bs842_2dprim.h
*/

#ifndef BS842_IMGUI_H

#include "bs842_text.h"
#include "bs842_2dprim.h"

//// INTERNAL ////
#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#define bsint_function static
#define bsint_global static
#define bsint_local_persist static

typedef unsigned __int8 bsint_u8;
typedef unsigned __int32 bsint_u32;
typedef __int32 bsint_s32;
typedef __int32 bsint_b32;
typedef size_t bsint_mem_index;
typedef float bsint_f32;

inline void *bs842_internal_CopyMem(void *dest, void *src, bsint_mem_index size)
{
    bsint_u8 *destP = (bsint_u8  *)dest;
    bsint_u8 *srcP = (bsint_u8  *)src;
    
    while (size--)
    {
        *destP++ = *srcP++;
    }
    
    return destP;
}

inline void bs842_internal_ConcatenateStrings(bsint_mem_index sourceACount, char *sourceA,
                                              bsint_mem_index sourceBCount, char *sourceB,
                               char *dest)
{
	for (bsint_s32 i = 0; i < sourceACount; ++i)
	{
		*dest++ = *sourceA++;
	}
    
	for (bsint_s32 i = 0; i < sourceBCount; ++i)
	{
		*dest++ = *sourceB++;
	}
    
	*dest++ = 0;
}

inline bsint_s32 bs842_internal_StringLength(char *string)
{
	bsint_s32 result = 0;
	while (*string++)
	{
		++result;
        if (*string == '\0')
        {
            break;
        }
	}
    
	return result;
}

inline bsint_b32 bs842_internal_StringCompare(char *str1, char *str2)
{
    bsint_b32 result = true;
    
    int i = 0;
    while((str1[i] != '\0') && (str2[i] != '\0'))
    {
        if(str1[i] != str2[i]) 
        {
            return false;
        }
        i++;
    }
    
    if(str1[i] == '\0' && str2[i] == '\0')
    {
         result = true;
    }
    else
    {
         result = false;
    }
    
    return result;
}

struct BSInternal_StringNode
{
    char *string;
    bsint_s32 strLength;
    
    BSInternal_StringNode *next;
    BSInternal_StringNode *prev;
};

bsint_function BSInternal_StringNode *AddStringNode(BSInternal_StringNode *stringSentinel, bsint_s32 stringSize)
{
     BSInternal_StringNode *stringNode = (BSInternal_StringNode *)malloc(sizeof(BSInternal_StringNode));
    
    stringNode->string = (char *)malloc(stringSize);
    stringNode->strLength = stringSize;
    
    stringNode->prev = stringSentinel;
    stringNode->next = stringSentinel->next;
    
    stringNode->prev->next = stringNode;
    stringNode->next->prev = stringNode;
    
    return stringNode;
}

bsint_function void RemoveStringNode(BSInternal_StringNode *removed)
{
    free(removed->string);
    
    removed->prev->next = removed->next;
    removed->next->prev = removed->prev;
    
    removed->next = 0;
    removed->prev = 0;
    
    free(removed);
}

struct BSInternal_Theme
{
    bsint_u32 elemOutline;
    bsint_u32 elemBackground;
    bsint_u32 defaultText;
    bsint_u32 menuBarBackground;
    bsint_u32 menuItemBorder;
    bsint_u32 menuItemHovered;
    bsint_u32 fileBrowseFolder;
    bsint_u32 fileBrowseFile;
    bsint_u32 fileBrowseInvert;
    bsint_u32 fileBrowseSelected;
    bsint_u32 fileBrowseSelectedBar;
};

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

inline bsint_b32 BSInternal_CheckBackBuffer(BSInternal_BackBuffer *backBuffer)
{
    bsint_b32 result = true;
    
    result &= (backBuffer->width > 0);
    result &= (backBuffer->height > 0);
    result &= (backBuffer->pitch > 0);
    result &= (backBuffer->memory != 0);
    
    return result;
}

struct BSInternal_ImguiInfo
{
    stbtt_fontinfo fontInfo;
    stbtt_fontinfo fileFontInfo;
    BSInternal_BackBuffer *backBuffer;
    BSInternal_Theme theme;
    
    bsint_b32 clicked;
    bsint_b32 dblClicked;
    bsint_b32 scrolledUp;
    bsint_b32 scrolledDown;
    bsint_s32 mouseX;
    bsint_s32 mouseY;
    
    bsint_s32 currentID;
};
bsint_global BSInternal_ImguiInfo bs842_internal_info;

enum BS842_Themes
{
    Theme_Light,
    Theme_Dark,
    Theme_TTR,
    Theme_Count,
};

bsint_function BSInternal_Theme BSInternal_LoadTheme(bsint_s32 theme)
{
    BSInternal_Theme result = {};
    
    switch (theme)
    {
        case Theme_TTR:
        {
            result.elemOutline = 0xFFFF0000;
            result.elemBackground = 0xFF4D4D4D;
            result.defaultText = 0xFFFFFFFF;
             result.menuBarBackground = 0xFF181818;
            result.menuItemBorder = 0xFFF3F3F3;
            result.menuItemHovered = 0xFFFF0000;
            result.fileBrowseFolder = 0xFFAAAAAA;
            result.fileBrowseFile = 0xFFFFFFFF;
            result.fileBrowseInvert = 0xFF000000;
            result.fileBrowseSelected = 0xFFFF0000;
            result.fileBrowseSelectedBar = 0xFFF3F3F3;
            break;
        }
        
        default:
        {
            break;
        }
    }
    
    return result;
}

#if 1
#define INTERNAL_ASSERT(expr) if (!(expr)) {*(bsint_s32 *)0 = 0;}
#else
#define INTERNAL_ASSERT(expr) 
#endif

//////////////////

enum BS842_InitCodes
{
    Init_Null,
    Init_Success,
    Init_FontFail, // Error from stbtt_InitFont()
    Init_InvalidBackBuffer, // BackBuffer does not match BSInternal_BackBuffer, or format contains invalid values
    Init_InvalidTheme, // Theme S32 does not correspond to a valid theme
};

bsint_function bsint_s32 BS842_InitImgui(bsint_u8* fontBuffer, bsint_u8* fileFontBuffer, void *backBuffer, bsint_s32 theme)
{
    bsint_s32 result = Init_Null;
    
    if (!stbtt_InitFont(&bs842_internal_info.fontInfo, fontBuffer, 0))
    {
        result = Init_FontFail;
        return result;
    }
    
    if (!stbtt_InitFont(&bs842_internal_info.fileFontInfo, fileFontBuffer, 0))
    {
        result = Init_FontFail;
        return result;
    }
    
     bs842_internal_info.backBuffer = (BSInternal_BackBuffer *)backBuffer;
    if (!BSInternal_CheckBackBuffer(bs842_internal_info.backBuffer))
    {
        result = Init_InvalidBackBuffer;
        return result;
    }
    
    if (!(theme > -1) || !(theme < Theme_Count))
    {
        result = Init_InvalidTheme;
        return result;
    }
    bs842_internal_info.theme = BSInternal_LoadTheme(theme);
    
    bs842_internal_info.clicked = false;
    bs842_internal_info.mouseX = 0;
    bs842_internal_info.mouseY = 0;
    
    bs842_internal_info.currentID = 1;
    
    result = Init_Success;
    return result;
}

inline void BS842_Imgui_RegisterMouseClick(bsint_b32 down)
{
    bs842_internal_info.clicked = down;
}

inline void BS842_Imgui_RegisterMouseMove(bsint_s32 mouseX, bsint_s32 mouseY)
{
    bs842_internal_info.mouseX = mouseX;
    bs842_internal_info.mouseY = mouseY;
}

inline void BS842_Imgui_RegisterMouseDoubleClick(bsint_b32 dbl)
{
    bs842_internal_info.dblClicked = dbl;
}

inline void BS842_Imgui_RegisterMouseScrollUp(bsint_b32 up)
{
    bs842_internal_info.scrolledUp = up;
}

inline void BS842_Imgui_RegisterMouseScrollDown(bsint_b32 down)
{
    bs842_internal_info.scrolledDown = down;
}

bsint_function void BS842_ImguiBegin() {}
bsint_function void BS842_ImguiEnd()
{
    if (bs842_internal_info.clicked)
    {
        bs842_internal_info.clicked = false;
    }
    
    if (bs842_internal_info.dblClicked)
    {
        bs842_internal_info.dblClicked = false;
    }
    
    if (bs842_internal_info.scrolledUp)
    {
        bs842_internal_info.scrolledUp = false;
    }
    
    if (bs842_internal_info.scrolledDown)
    {
        bs842_internal_info.scrolledDown = false;
    }
    
    if (bs842_internal_info.currentID != 1)
    {
        bs842_internal_info.currentID = 1;
    }
}

bsint_function BS842_Prim_SizeSpec BS842_MenuBar()
{
    BS842_Prim_SizeSpec result = BS842_FillSizeSpec(0.0f, 1.0f, 0.0f, 0.04f);
    
    BS842_Prim_SizeSpec boxSizeSpec = result;
    BS842_Prim_SizeSpec lineSizeSpec = BS842_FillSizeSpec(result.x1, result.x2, result.y2, result.y2);
    BS842_DrawSolidBox(bs842_internal_info.backBuffer, boxSizeSpec, bs842_internal_info.theme.menuBarBackground);
    BS842_DrawLine(bs842_internal_info.backBuffer, lineSizeSpec, 3.0f, bs842_internal_info.theme.elemOutline);
    
    return result;
}

bsint_function bsint_b32 BS842_MenuBarItem(BS842_Prim_SizeSpec menuBarSizeSpec, bsint_s32 menuItemOrder, char *title, bsint_b32 *hasBeenClicked, BS842_Prim_SizeSpec *childAnchor = 0)
{
    bsint_b32 result = false;
    
    bsint_f32 itemWidth = 0.1f;
    bsint_f32 itemLeft = ((bsint_f32)menuItemOrder * itemWidth) + 0.0075f;
    BS842_Prim_SizeSpec sizeSpec = BS842_FillSizeSpec(itemLeft, itemLeft + itemWidth, menuBarSizeSpec.y1, menuBarSizeSpec.y2 - 0.004f);
    BSInternal_SizeSpec int_sizeSpec = bs842_internal_ConvertSizeSpec(bs842_internal_info.backBuffer, sizeSpec);
    
    bsint_b32 hovered = (((bs842_internal_info.mouseX >= int_sizeSpec.x1) && (bs842_internal_info.mouseX <= int_sizeSpec.x2)) &&
                         ((bs842_internal_info.mouseY >= int_sizeSpec.y1) && (bs842_internal_info.mouseY <= int_sizeSpec.y2))) ? true : false;
    
    if (hovered)
    {
        BS842_DrawSolidBox(bs842_internal_info.backBuffer, sizeSpec, bs842_internal_info.theme.menuItemHovered);
        if (bs842_internal_info.clicked)
        {
            *hasBeenClicked = true;
        }
    }
    else
    {
        BS842_DrawSolidBox(bs842_internal_info.backBuffer, sizeSpec, bs842_internal_info.theme.menuBarBackground);
        *hasBeenClicked = false;
    }
    
    BS842_DrawLine(bs842_internal_info.backBuffer, BS842_FillSizeSpec(sizeSpec.x1, sizeSpec.x1, sizeSpec.y1, sizeSpec.y2), 1.0f, bs842_internal_info.theme.menuItemBorder);
    BS842_DrawLine(bs842_internal_info.backBuffer, BS842_FillSizeSpec(sizeSpec.x2, sizeSpec.x2, sizeSpec.y1, sizeSpec.y2), 1.0f, bs842_internal_info.theme.menuItemBorder);
    
    bsint_s32 textSizeY = (bsint_s32)(((sizeSpec.y2 - sizeSpec.y1) * bs842_internal_info.backBuffer->height) * 0.9f);
    unsigned char *textBitmap = (unsigned char *)malloc(bs842_internal_info.backBuffer->pitch * textSizeY);
    bs842_internal_ZeroMem(textBitmap, bs842_internal_info.backBuffer->pitch * textSizeY);
    bsint_f32 charX = 0.0f;
    BS842_CreateTextBitmap(textBitmap, &bs842_internal_info.fontInfo, title, (bsint_f32)textSizeY, bs842_internal_info.backBuffer->width, &charX);
    BS842_DrawTextBitmap(bs842_internal_info.backBuffer, textBitmap, bs842_internal_info.theme.defaultText, charX, sizeSpec.x1 + 0.0025f, sizeSpec.y1 + 0.0025f, bs842_internal_info.backBuffer->width, textSizeY, true);
    
    free(textBitmap);
    
    if (childAnchor)
    {
        *childAnchor = BS842_FillSizeSpec(sizeSpec.x1, sizeSpec.x1, sizeSpec.y2, sizeSpec.y2);
    }
    
    if (*hasBeenClicked)
    {
        result = true;
        }
        
    return result;
}

bsint_function void BS842_TextBox(BS842_Prim_SizeSpec *anchor, char *text, bsint_f32 fontLineHeight = 5.0f, bsint_f32 xPos = 0.0f, bsint_f32 yPos = 0.0f)
{
    bsint_s32 lineCount = 0;
    bsint_s32 ch = 0;
    
    char *stringStartPos = text;
    bsint_s32 currStringCharCount = 0;
    bsint_s32 longestStringCharCount = 1;
    BSInternal_StringNode stringSentinel = {};
    stringSentinel.prev = &stringSentinel;
    stringSentinel.next = &stringSentinel;
    
    bsint_s32 strLen = bs842_internal_StringLength(text);
    char *textTemp = (char *)malloc(strLen + 2);
    bs842_internal_CopyMem(textTemp, text, strLen);
    bs842_internal_ConcatenateStrings(strLen, textTemp, 1, "\n\0", textTemp);
    
    while (textTemp[ch])
    {
        ++currStringCharCount;
        
        if (textTemp[ch] == '\n')
        {
            lineCount++;
            
            BSInternal_StringNode *stringNode = AddStringNode(&stringSentinel, currStringCharCount);
            bs842_internal_CopyMem(stringNode->string, stringStartPos, currStringCharCount - 1);
            stringNode->string[currStringCharCount - 1] = '\0';
            
            if (currStringCharCount > longestStringCharCount)
            {
                longestStringCharCount = currStringCharCount;
            }
            
            stringStartPos = &textTemp[ch + 1];
            currStringCharCount = 0;
        }
        
        ++ch;
    }
    
    free(textTemp);
    
    if (anchor)
    {
        xPos = anchor->x1;
        yPos = anchor->y2;
    }
        
        bsint_f32 yCursor = yPos + 0.005f;
    bsint_s32 textSizeY = fontLineHeight; // TODO(bSalmon): Convert to scale with window size
        
        bsint_s32 xPosS = bs842_prim_internal_RoundF32ToS32(xPos * bs842_internal_info.backBuffer->width);
        xPosS = (xPosS < 0) ? 0 : ((xPosS > bs842_internal_info.backBuffer->width) ? bs842_internal_info.backBuffer->width : xPosS);
        bsint_s32 yPosS = bs842_prim_internal_RoundF32ToS32(yPos * bs842_internal_info.backBuffer->height);
        yPosS = (yPosS < 0) ? 0 : ((yPosS > bs842_internal_info.backBuffer->width) ? bs842_internal_info.backBuffer->height : yPosS);
        
        bsint_s32 boxBottom = yPosS + ((textSizeY + (bsint_s32)(0.005f * bs842_internal_info.backBuffer->height)) * lineCount) + (bsint_s32)(0.01f * bs842_internal_info.backBuffer->height);
        bsint_s32 boxRight = xPosS + ((bsint_s32)(textSizeY * 0.5f) * longestStringCharCount);
        
        BSInternal_SizeSpec sizeSpec = BS842_FillSizeSpec(xPosS, boxRight, yPosS, boxBottom);
        BS842_DrawOutlinedBox(bs842_internal_info.backBuffer, sizeSpec, 1.0f, bs842_internal_info.theme.elemBackground, bs842_internal_info.theme.elemOutline);
        
        for (BSInternal_StringNode *stringNode = stringSentinel.prev; stringNode != &stringSentinel; stringNode = stringNode->prev)
        {
            if (stringNode->string[0] != '\0')
            {
                unsigned char *textBitmap = (unsigned char *)malloc(bs842_internal_info.backBuffer->pitch * textSizeY);
                bs842_internal_ZeroMem(textBitmap, bs842_internal_info.backBuffer->pitch * textSizeY);
        bsint_f32 charX = 0.0f;
        BS842_CreateTextBitmap(textBitmap, &bs842_internal_info.fontInfo, stringNode->string, fontLineHeight, bs842_internal_info.backBuffer->width, &charX);
        BS842_DrawTextBitmap(bs842_internal_info.backBuffer, textBitmap, bs842_internal_info.theme.defaultText, charX, xPos + 0.005f, yCursor, bs842_internal_info.backBuffer->width, textSizeY, true);
        
            free(textBitmap);
            }
            
            yCursor += (fontLineHeight / bs842_internal_info.backBuffer->height) + 0.005f;
        }
        
        for (BSInternal_StringNode *stringNode = stringSentinel.next; stringNode != &stringSentinel; stringNode = stringSentinel.next)
        {
            RemoveStringNode(stringNode);
        }
}

bsint_function void BSInternal_DrawBasicWindow(void *buffer, char *title, BS842_Prim_SizeSpec sizeSpec)
{
    bsint_f32 titleBarRatio = 0.05f;
    BS842_DrawSolidBox(buffer, BS842_FillSizeSpec(sizeSpec.x1, sizeSpec.x2, sizeSpec.y1, sizeSpec.y1 + ((sizeSpec.y2 - sizeSpec.y1) * titleBarRatio)), bs842_internal_info.theme.menuBarBackground);
    BS842_DrawSolidBox(buffer, BS842_FillSizeSpec(sizeSpec.x1, sizeSpec.x2, sizeSpec.y1 + ((sizeSpec.y2 - sizeSpec.y1) * titleBarRatio), sizeSpec.y2), bs842_internal_info.theme.elemBackground);
    BS842_DrawHollowBox(buffer, sizeSpec, 2.0f, bs842_internal_info.theme.elemOutline);
    
    BS842_Prim_SizeSpec textSizeSpec = BS842_FillSizeSpec(sizeSpec.x1, sizeSpec.x2, sizeSpec.y1, sizeSpec.y1 + ((sizeSpec.y2 - sizeSpec.y1) * titleBarRatio));
    bsint_s32 textSizeY = (bsint_s32)(((textSizeSpec.y2 - textSizeSpec.y1) * bs842_internal_info.backBuffer->height) * 0.9f);
    unsigned char *textBitmap = (unsigned char *)malloc(bs842_internal_info.backBuffer->pitch * textSizeY);
    bs842_internal_ZeroMem(textBitmap, bs842_internal_info.backBuffer->pitch * textSizeY);
    bsint_f32 charX = 0.0f;
    BS842_CreateTextBitmap(textBitmap, &bs842_internal_info.fontInfo, title, (bsint_f32)textSizeY, bs842_internal_info.backBuffer->width, &charX);
    BS842_DrawTextBitmap(bs842_internal_info.backBuffer, textBitmap, bs842_internal_info.theme.defaultText, charX, sizeSpec.x1 + 0.0025f, sizeSpec.y1 + 0.0025f, bs842_internal_info.backBuffer->width, textSizeY, true);
    
    free(textBitmap);
}

bsint_function bsint_b32 BS842_Button(BS842_Prim_SizeSpec anchor, char *label)
{
    bsint_b32 result = false;
    
    BS842_Prim_SizeSpec sizeSpec = anchor;
    BSInternal_SizeSpec int_sizeSpec = bs842_internal_ConvertSizeSpec(bs842_internal_info.backBuffer, sizeSpec);
    
    bsint_b32 hovered = (((bs842_internal_info.mouseX >= int_sizeSpec.x1) && (bs842_internal_info.mouseX <= int_sizeSpec.x2)) &&
                         ((bs842_internal_info.mouseY >= int_sizeSpec.y1) && (bs842_internal_info.mouseY <= int_sizeSpec.y2))) ? true : false;
    
    if (hovered)
    {
        BS842_DrawOutlinedBox(bs842_internal_info.backBuffer, sizeSpec, 2.0f, bs842_internal_info.theme.menuItemHovered, bs842_internal_info.theme.menuItemHovered);
        if (bs842_internal_info.clicked)
        {
             result = true;
        }
    }
    else
    {
        BS842_DrawOutlinedBox(bs842_internal_info.backBuffer, sizeSpec, 2.0f, bs842_internal_info.theme.menuBarBackground, bs842_internal_info.theme.menuItemHovered);
    }
    
    bsint_s32 textSizeY = (bsint_s32)(((sizeSpec.y2 - sizeSpec.y1) * bs842_internal_info.backBuffer->height) * 0.9f);
    unsigned char *textBitmap = (unsigned char *)malloc(bs842_internal_info.backBuffer->pitch * textSizeY);
    bs842_internal_ZeroMem(textBitmap, bs842_internal_info.backBuffer->pitch * textSizeY);
    bsint_f32 charX = 0.0f;
    BS842_CreateTextBitmap(textBitmap, &bs842_internal_info.fontInfo, label, (bsint_f32)textSizeY, bs842_internal_info.backBuffer->width, &charX);
    BS842_DrawTextBitmap(bs842_internal_info.backBuffer, textBitmap, bs842_internal_info.theme.defaultText, charX, sizeSpec.x1 + 0.0025f, sizeSpec.y1 + 0.0025f, bs842_internal_info.backBuffer->width, textSizeY, true);
    
    free(textBitmap);
    
    return result;
}

struct BS842_Imgui_FileInfo
{
    char filename[MAX_PATH];
    char currFolder[MAX_PATH];
    bsint_s32 currFolderLength;
    bsint_b32 hasBeenRead;
};

enum BSInternal_FindResultType
{
    FindResult_Folder,
    FindResult_File,
};

struct BSInternal_FindResult
{
    char *file;
    BSInternal_FindResultType type;
    bsint_s32 orderInList;
    bsint_b32 selected;
    
    BSInternal_FindResult *next;
    BSInternal_FindResult *prev;
};

bsint_function BSInternal_FindResult *AddFindResult(BSInternal_FindResult *sentinel, BSInternal_FindResultType type)
{
    BSInternal_FindResult *node = (BSInternal_FindResult *)malloc(sizeof(BSInternal_FindResult));
    
    node->file = (char *)malloc(MAX_PATH);
    node->type = type;
    node->selected = false;
    
    node->prev = sentinel;
    node->next = sentinel->next;
    
    node->prev->next = node;
    node->next->prev = node;
    
    if (node->next != sentinel)
    {
        node->orderInList = (node->next->orderInList + 1);
    }
    else
    {
        node->orderInList = 0;
    }
    
    return node;
}

bsint_function void RemoveFindResult(BSInternal_FindResult *removed)
{
    free(removed->file);
    
    removed->prev->next = removed->next;
    removed->next->prev = removed->prev;
    
    removed->next = 0;
    removed->prev = 0;
    
    free(removed);
}

bsint_function bsint_b32 BS842_ScrollUp(BS842_Prim_SizeSpec sizeSpec)
{
    bsint_b32 result = false;
    
    BSInternal_SizeSpec int_sizeSpec = bs842_internal_ConvertSizeSpec(bs842_internal_info.backBuffer, sizeSpec);
    
    bsint_b32 hovered = (((bs842_internal_info.mouseX >= int_sizeSpec.x1) && (bs842_internal_info.mouseX <= int_sizeSpec.x2)) &&
                         ((bs842_internal_info.mouseY >= int_sizeSpec.y1) && (bs842_internal_info.mouseY <= int_sizeSpec.y2))) ? true : false;
    
    if (hovered)
    {
        BS842_DrawOutlinedBox(bs842_internal_info.backBuffer, sizeSpec, 1.0f, bs842_internal_info.theme.menuItemHovered, bs842_internal_info.theme.menuItemHovered);
        if (bs842_internal_info.clicked)
        {
            result = true;
        }
    }
    else
    {
        BS842_DrawOutlinedBox(bs842_internal_info.backBuffer, sizeSpec, 1.0f, bs842_internal_info.theme.menuBarBackground, bs842_internal_info.theme.menuItemHovered);
    }
    
    BS842_Prim_SizeSpec line1 = BS842_FillSizeSpec(sizeSpec.x1 + ((sizeSpec.x2 - sizeSpec.x1) * 0.2f), sizeSpec.x1 + ((sizeSpec.x2 - sizeSpec.x1) * 0.5f),
                                                   sizeSpec.y1 + ((sizeSpec.y2 - sizeSpec.y1) * 0.8f), sizeSpec.y1 + ((sizeSpec.y2 - sizeSpec.y1) * 0.2f));
    BS842_Prim_SizeSpec line2 = BS842_FillSizeSpec(sizeSpec.x1 + ((sizeSpec.x2 - sizeSpec.x1) * 0.8f), sizeSpec.x1 + ((sizeSpec.x2 - sizeSpec.x1) * 0.5f),
                                                   sizeSpec.y1 + ((sizeSpec.y2 - sizeSpec.y1) * 0.8f), sizeSpec.y1 + ((sizeSpec.y2 - sizeSpec.y1) * 0.2f));
    BS842_DrawLine(bs842_internal_info.backBuffer, line1, 1.0f, bs842_internal_info.theme.defaultText);
    BS842_DrawLine(bs842_internal_info.backBuffer, line2, 1.0f, bs842_internal_info.theme.defaultText);
    
    return result;
}

bsint_function bsint_b32 BS842_ScrollDown(BS842_Prim_SizeSpec sizeSpec)
{
    bsint_b32 result = false;
    
    BSInternal_SizeSpec int_sizeSpec = bs842_internal_ConvertSizeSpec(bs842_internal_info.backBuffer, sizeSpec);
    
    bsint_b32 hovered = (((bs842_internal_info.mouseX >= int_sizeSpec.x1) && (bs842_internal_info.mouseX <= int_sizeSpec.x2)) &&
                         ((bs842_internal_info.mouseY >= int_sizeSpec.y1) && (bs842_internal_info.mouseY <= int_sizeSpec.y2))) ? true : false;
    
    if (hovered)
    {
        BS842_DrawOutlinedBox(bs842_internal_info.backBuffer, sizeSpec, 1.0f, bs842_internal_info.theme.menuItemHovered, bs842_internal_info.theme.menuItemHovered);
        if (bs842_internal_info.clicked)
        {
            result = true;
        }
    }
    else
    {
        BS842_DrawOutlinedBox(bs842_internal_info.backBuffer, sizeSpec, 1.0f, bs842_internal_info.theme.menuBarBackground, bs842_internal_info.theme.menuItemHovered);
    }
    
    BS842_Prim_SizeSpec line1 = BS842_FillSizeSpec(sizeSpec.x1 + ((sizeSpec.x2 - sizeSpec.x1) * 0.2f), sizeSpec.x1 + ((sizeSpec.x2 - sizeSpec.x1) * 0.5f),
                                                   sizeSpec.y1 + ((sizeSpec.y2 - sizeSpec.y1) * 0.2f), sizeSpec.y1 + ((sizeSpec.y2 - sizeSpec.y1) * 0.8f));
    BS842_Prim_SizeSpec line2 = BS842_FillSizeSpec(sizeSpec.x1 + ((sizeSpec.x2 - sizeSpec.x1) * 0.8f), sizeSpec.x1 + ((sizeSpec.x2 - sizeSpec.x1) * 0.5f),
                                                   sizeSpec.y1 + ((sizeSpec.y2 - sizeSpec.y1) * 0.2f), sizeSpec.y1 + ((sizeSpec.y2 - sizeSpec.y1) * 0.8f));
    BS842_DrawLine(bs842_internal_info.backBuffer, line1, 1.0f, bs842_internal_info.theme.defaultText);
    BS842_DrawLine(bs842_internal_info.backBuffer, line2, 1.0f, bs842_internal_info.theme.defaultText);
    
    return result;
}

bsint_function void BS842_FileBrowser(char *title, char *fileTypes, BS842_Imgui_FileInfo *fileInfo, BS842_Prim_SizeSpec sizeSpec)
{
    BSInternal_DrawBasicWindow(bs842_internal_info.backBuffer, title, sizeSpec);
    BS842_Prim_SizeSpec filesSizeSpec = BS842_FillSizeSpec(sizeSpec.x1 + 0.01f, sizeSpec.x2 - 0.01f, sizeSpec.y1 + 0.1f, sizeSpec.y2 - 0.1f);
    BS842_DrawSolidBox(bs842_internal_info.backBuffer, filesSizeSpec, bs842_internal_info.theme.menuBarBackground);
    
#ifdef _WIN32
    // NOTE(bSalmon): If first time through, set folder to be exe folder
    if (fileInfo->currFolder[0] == '\0')
    {
    char fileFolder[MAX_PATH] = {};
        GetModuleFileNameA(0, fileFolder, sizeof(fileFolder));
        char *lastSlash = fileFolder;
        bsint_s32 length = 0;
        bsint_s32 savedLength = 0;
	for (char *scan = fileFolder; *scan; ++scan)
        {
            length++;
		if(*scan == '\\')
        {
                lastSlash = scan + 1;
                savedLength = length;
        }
        }
        
        *lastSlash = '\0';
        bs842_internal_CopyMem(fileInfo->currFolder, fileFolder, MAX_PATH);
        fileInfo->currFolderLength = savedLength;
    }
    
#else
    INTERNAL_ASSERT(false);
#endif
    
    // TODO(bSalmon): Separate out current directory for buttons up top
    BSInternal_StringNode stringSentinel;
    stringSentinel.string = 0;
    stringSentinel.next = &stringSentinel;
    stringSentinel.prev = &stringSentinel;
    
    bsint_s32 i = 1;
    bsint_s32 old_i = 0;
    for (char *scan = fileInfo->currFolder; *scan; ++scan, ++i)
	{
		if(*scan == '\\')
        {
            BSInternal_StringNode *node = AddStringNode(&stringSentinel, (i - old_i) + 1);
            bs842_internal_CopyMem(node->string, &fileInfo->currFolder[old_i], (i - old_i) + 1);
            node->string[i - old_i] = '\0';
            
            old_i = i;
        }
    }
    
    bsint_local_persist bsint_s32 lastSelectedOrderInList = -1;
    bsint_local_persist bsint_s32 topOfCurrList = 0;
    bsint_b32 isFirstInList = true;
        BS842_Prim_SizeSpec anchor = {};
    for (BSInternal_StringNode *stringNode = stringSentinel.prev; stringNode != &stringSentinel; stringNode = stringNode->prev)
    {
        if (isFirstInList)
        {
            anchor = BS842_FillSizeSpec(sizeSpec.x1 + 0.025f, (sizeSpec.x1 + 0.025f) + (stringNode->strLength * 0.0075f), sizeSpec.y1 + 0.055f, (sizeSpec.y1 + 0.055f) + 0.035f);
            isFirstInList = false;
        }
        else
        {
            anchor =  BS842_FillSizeSpec(anchor.x2 + 0.01f, (anchor.x2 + 0.025f) + (stringNode->strLength * 0.0075f), anchor.y1, anchor.y2);
        }
        
        if (BS842_Button(anchor, stringNode->string))
        {
            bsint_s32 currStringLength = 0;
            lastSelectedOrderInList = -1;
            topOfCurrList = 0;
            
            for (BSInternal_StringNode *subStringNode = stringSentinel.prev; subStringNode != &stringSentinel; subStringNode = subStringNode->prev)
            {
                if (subStringNode == stringSentinel.prev)
                {
                    bs842_internal_ZeroMem(fileInfo->currFolder, MAX_PATH);
                    bs842_internal_CopyMem(fileInfo->currFolder, subStringNode->string, subStringNode->strLength - 1);
                }
                else
                {
                    char currFolderCopy[MAX_PATH] = {};
                    bs842_internal_CopyMem(currFolderCopy, fileInfo->currFolder, currStringLength);
                bs842_internal_ConcatenateStrings(currStringLength, currFolderCopy, subStringNode->strLength - 1, subStringNode->string, fileInfo->currFolder);
                }
                
                currStringLength += (subStringNode->strLength - 1);
                fileInfo->currFolderLength = currStringLength;
                
                if (subStringNode->string == stringNode->string)
                {
                    break;
                }
            }
        }
    }

    for (BSInternal_StringNode *stringNode = stringSentinel.next; stringNode != &stringSentinel; stringNode = stringSentinel.next)
    {
        RemoveStringNode(stringNode);
    }
    
#ifdef _WIN32
    HANDLE findHandle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA findData = {};
    char findDir[MAX_PATH] = {};
    bs842_internal_ConcatenateStrings(fileInfo->currFolderLength, fileInfo->currFolder, 2, "*\0", findDir);
    findHandle = FindFirstFile(findDir, &findData);
    INTERNAL_ASSERT(findHandle != INVALID_HANDLE_VALUE);
    
    BSInternal_FindResult findSentinel = {};
    findSentinel.next = &findSentinel;
    findSentinel.prev = &findSentinel;
    
    do
    {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (!((findData.cFileName[0] == '.') && (findData.cFileName[1] == '\0')))
            {
            BSInternal_FindResult *findResult = AddFindResult(&findSentinel, FindResult_Folder);
                bs842_internal_CopyMem(findResult->file, findData.cFileName, MAX_PATH);
            }
        }
        else
        {
            bsint_s32 filenameLength = bs842_internal_StringLength(findData.cFileName);
            char *extension = &findData.cFileName[filenameLength - 4];
            if (bs842_internal_StringCompare(extension, fileTypes)) // NOTE(bSalmon): Won't work for multiple fileTypes
            {
                BSInternal_FindResult *findResult = AddFindResult(&findSentinel, FindResult_File);
                bs842_internal_CopyMem(findResult->file, findData.cFileName, MAX_PATH);
            }
        }
    }
    while (FindNextFile(findHandle, &findData) != 0);
    FindClose(findHandle);
    
#else
    INTERNAL_ASSERT(false);
#endif
    
    for (BSInternal_FindResult *findResult = findSentinel.prev; findResult != &findSentinel; findResult = findResult->prev, ++i)
    {
        if ((findResult->type == FindResult_Folder) && (findResult->next != &findSentinel) && (findResult->next->type != FindResult_Folder))
        {
            INTERNAL_SWAP(findResult->file, findResult->next->file);
            INTERNAL_SWAP(findResult->type, findResult->next->type);
            INTERNAL_SWAP(findResult->orderInList, findResult->next->orderInList);
        }
    }
    
    bsint_s32 maxListCount = 25;
    BS842_Prim_SizeSpec scrollUpSizeSpec = BS842_FillSizeSpec(filesSizeSpec.x2 - 0.025f, filesSizeSpec.x2, filesSizeSpec.y1, filesSizeSpec.y1 + 0.05f);
    BS842_Prim_SizeSpec scrollDownSizeSpec = BS842_FillSizeSpec(filesSizeSpec.x2 - 0.025f, filesSizeSpec.x2, filesSizeSpec.y2 - 0.05f, filesSizeSpec.y2);
    BS842_DrawLine(bs842_internal_info.backBuffer, BS842_FillSizeSpec(filesSizeSpec.x2 - 0.025f, filesSizeSpec.x2 - 0.025f, filesSizeSpec.y1 + 0.05f, filesSizeSpec.y2 - 0.05f), 1.0f, bs842_internal_info.theme.elemOutline);
    BS842_DrawLine(bs842_internal_info.backBuffer, BS842_FillSizeSpec(filesSizeSpec.x2, filesSizeSpec.x2, filesSizeSpec.y1 + 0.05f, filesSizeSpec.y2 - 0.05f), 1.0f, bs842_internal_info.theme.elemOutline);
    if ((BS842_ScrollUp(scrollUpSizeSpec) || bs842_internal_info.scrolledUp) && (topOfCurrList - maxListCount >= 0))
    {
        topOfCurrList -= maxListCount;
    }
    if ((BS842_ScrollDown(scrollDownSizeSpec) || bs842_internal_info.scrolledDown) && (topOfCurrList + maxListCount <= findSentinel.next->orderInList))
    {
        topOfCurrList += maxListCount;
    }
    
    i = 0;
    for (BSInternal_FindResult *findResult = findSentinel.prev; findResult != &findSentinel; findResult = findResult->prev)
    {
        if (findResult->orderInList >= topOfCurrList)
        {
            
            bsint_s32 textSizeY = (bsint_s32)(((filesSizeSpec.y2 - filesSizeSpec.y1) * bs842_internal_info.backBuffer->height) * (1.0f / (bsint_f32)maxListCount));
        bsint_f32 yPos = filesSizeSpec.y1 + ((((bsint_f32)textSizeY / (bsint_f32)bs842_internal_info.backBuffer->height) * i) + 0.0025f);
            
            if (findResult->orderInList >= (topOfCurrList + maxListCount))
        {
            break;
            }
            
            bsint_u32 textColour = (findResult->type == FindResult_Folder) ? bs842_internal_info.theme.fileBrowseFolder : bs842_internal_info.theme.fileBrowseFile;
            BS842_Prim_SizeSpec boundBox = BS842_FillSizeSpec(filesSizeSpec.x1, filesSizeSpec.x2 - 0.025f,
                                                              filesSizeSpec.y1 + (((bsint_f32)textSizeY / (bsint_f32)bs842_internal_info.backBuffer->height) * i) + 0.0025f, filesSizeSpec.y1 + (((bsint_f32)textSizeY / (bsint_f32)bs842_internal_info.backBuffer->height) * i) + (((bsint_f32)textSizeY / (bsint_f32)bs842_internal_info.backBuffer->height)));
            
            BSInternal_SizeSpec int_boundBox = bs842_internal_ConvertSizeSpec(bs842_internal_info.backBuffer, boundBox);
            
            bsint_b32 hovered = (((bs842_internal_info.mouseX >= int_boundBox.x1) && (bs842_internal_info.mouseX <= int_boundBox.x2)) &&
                                 ((bs842_internal_info.mouseY >= int_boundBox.y1) && (bs842_internal_info.mouseY <= int_boundBox.y2))) ? true : false;
            
            if (hovered)
            {
                BS842_DrawSolidBox(bs842_internal_info.backBuffer, boundBox, bs842_internal_info.theme.fileBrowseSelectedBar);
                if (bs842_internal_info.clicked)
                {
                    findResult->selected = true;
                    lastSelectedOrderInList = findResult->orderInList;
                }
                else if (bs842_internal_info.dblClicked)
                {
                    findResult->selected = true;
                    lastSelectedOrderInList = findResult->orderInList;
                    if (findResult->type == FindResult_Folder)
                    {
                        if (bs842_internal_StringCompare(findResult->file, ".."))
                        {
                            bsint_s32 sub_i = 1;
                            bsint_s32 sub_old_i = 0;
                            for (char *scan = fileInfo->currFolder; *scan; ++scan, ++sub_i)
                            {
                                if(*scan == '\\')
                                {
                                    if (!(*(scan + 1)))
                                    {
                                        char currFolderCopy[MAX_PATH] = {};
                                        bs842_internal_CopyMem(currFolderCopy, fileInfo->currFolder, fileInfo->currFolderLength);
                                        bs842_internal_ZeroMem(fileInfo->currFolder, MAX_PATH);
                                        bs842_internal_CopyMem(fileInfo->currFolder, currFolderCopy, sub_old_i);
                                        fileInfo->currFolderLength = sub_old_i;
                                        topOfCurrList = 0;
                                        break;
                                    }
                                    else
                                    {
                                        sub_old_i = sub_i;
                                    }
                                }
                            }
                        }
                        else
                        {
                            char currFolderCopy[MAX_PATH] = {};
                            bs842_internal_CopyMem(currFolderCopy, fileInfo->currFolder, fileInfo->currFolderLength);
                            bs842_internal_ConcatenateStrings(fileInfo->currFolderLength, currFolderCopy, bs842_internal_StringLength(findResult->file), findResult->file, fileInfo->currFolder);
                            fileInfo->currFolderLength += bs842_internal_StringLength(findResult->file);
                            
                            bs842_internal_ZeroMem(currFolderCopy, MAX_PATH);
                            bs842_internal_CopyMem(currFolderCopy, fileInfo->currFolder, fileInfo->currFolderLength);
                            bs842_internal_ConcatenateStrings(fileInfo->currFolderLength, currFolderCopy, 1, "\\", fileInfo->currFolder);
                            fileInfo->currFolderLength++;
                            topOfCurrList = 0;
                        }
                    }
                    else
                    {
                        bs842_internal_ZeroMem(fileInfo->filename, MAX_PATH);
                    bs842_internal_ConcatenateStrings(fileInfo->currFolderLength, fileInfo->currFolder, bs842_internal_StringLength(findResult->file), findResult->file, fileInfo->filename);
                        fileInfo->hasBeenRead = false;
                    }
                }
                else
                {
                textColour = bs842_internal_info.theme.fileBrowseInvert;
                }
            }
            
            if (findResult->orderInList == lastSelectedOrderInList)
            {
                textColour = bs842_internal_info.theme.fileBrowseSelected;
                BS842_DrawSolidBox(bs842_internal_info.backBuffer, boundBox, bs842_internal_info.theme.fileBrowseSelectedBar);
            }
            
        unsigned char *textBitmap = (unsigned char *)malloc(bs842_internal_info.backBuffer->pitch * textSizeY);
        bs842_internal_ZeroMem(textBitmap, bs842_internal_info.backBuffer->pitch * textSizeY);
        bsint_f32 charX = 0.0f;
        BS842_CreateTextBitmap(textBitmap, &bs842_internal_info.fileFontInfo, findResult->file, (bsint_f32)textSizeY, bs842_internal_info.backBuffer->width, &charX);
        BS842_DrawTextBitmap(bs842_internal_info.backBuffer, textBitmap, textColour, charX, filesSizeSpec.x1 + 0.0025f, yPos, bs842_internal_info.backBuffer->width, textSizeY, true);
        
            free(textBitmap);
            
            ++i;
        }
        else
        {
            findResult->selected = false;
        }
    }
    
    if (BS842_Button(BS842_FillSizeSpec(filesSizeSpec.x2 - 0.1f, filesSizeSpec.x2, filesSizeSpec.y2 + 0.025f, sizeSpec.y2 - 0.025f), "Open"))
    {
        if (lastSelectedOrderInList != -1)
        {
            for (BSInternal_FindResult *findResult = findSentinel.prev; findResult != &findSentinel; findResult = findResult->prev)
            {
                if (findResult->orderInList == lastSelectedOrderInList)
                {
                    if (findResult->type == FindResult_Folder)
                    {
                        if (bs842_internal_StringCompare(findResult->file, ".."))
                        {
                            bsint_s32 sub_i = 1;
                            bsint_s32 sub_old_i = 0;
                            for (char *scan = fileInfo->currFolder; *scan; ++scan, ++sub_i)
                            {
                                if(*scan == '\\')
                                {
                                    if (!(*(scan + 1)))
                                    {
                                        char currFolderCopy[MAX_PATH] = {};
                                        bs842_internal_CopyMem(currFolderCopy, fileInfo->currFolder, fileInfo->currFolderLength);
                                        bs842_internal_ZeroMem(fileInfo->currFolder, MAX_PATH);
                                        bs842_internal_CopyMem(fileInfo->currFolder, currFolderCopy, sub_old_i);
                                        fileInfo->currFolderLength = sub_old_i;
                                        topOfCurrList = 0;
                                        break;
                                    }
                                    else
                                    {
                                        sub_old_i = sub_i;
                                    }
                                }
                            }
                        }
                        else
                        {
                            char currFolderCopy[MAX_PATH] = {};
                            bs842_internal_CopyMem(currFolderCopy, fileInfo->currFolder, fileInfo->currFolderLength);
                            bs842_internal_ConcatenateStrings(fileInfo->currFolderLength, currFolderCopy, bs842_internal_StringLength(findResult->file), findResult->file, fileInfo->currFolder);
                            fileInfo->currFolderLength += bs842_internal_StringLength(findResult->file);
                            
                            bs842_internal_ZeroMem(currFolderCopy, MAX_PATH);
                            bs842_internal_CopyMem(currFolderCopy, fileInfo->currFolder, fileInfo->currFolderLength);
                            bs842_internal_ConcatenateStrings(fileInfo->currFolderLength, currFolderCopy, 1, "\\", fileInfo->currFolder);
                            fileInfo->currFolderLength++;
                            topOfCurrList = 0;
                        }
                    }
                    else
                    {
                        bs842_internal_ZeroMem(fileInfo->filename, MAX_PATH);
                    bs842_internal_ConcatenateStrings(fileInfo->currFolderLength, fileInfo->currFolder, bs842_internal_StringLength(findResult->file), findResult->file, fileInfo->filename);
                        fileInfo->hasBeenRead = false;
                    }
                }
            }
        }
    }
    
    for (BSInternal_FindResult *findResult = findSentinel.next; findResult != &findSentinel; findResult = findSentinel.next)
    {
        RemoveFindResult(findResult);
    }
}

#define BS842_IMGUI_H
#endif // BS842_IMGUI_H