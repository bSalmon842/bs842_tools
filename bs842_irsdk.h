/*
File: bs842_irsdk.h
Author: Brock Salmon

The iRacing.com Simulator and the iRacing Software Development Kit (IRSDK) are properties of iRacing.com Motorsport Simulations, LLC.
Copyright (c) 2013, iRacing.com Motorsport Simulations, LLC.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of iRacing.com Motorsport Simulations nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


/* USAGE
To include the file:
#define BS842_IRSDK_IMPLEMENTATION
#include "bs842_irsdk.h"

Optional Defines:
These defines should be placed before the IMPLEMENTATION define
- #define BS842_ASSERT(check) to use your own assert function or define it as empty to disable failed initialisation asserts
- #define BS842_PARSEYAML(path, returnVal, returnLen) to use your own yaml parser, must return a bool or int, parameters must be a
char * containing the string to be searching for, a char ** containing the result of the search, and an int * containing the length
of the result string.
*/


#ifndef BS842_INCLUDE_IRSDK_H

#ifdef __cplusplus
extern "C"
{
#endif
    
#ifndef _WINDOWS_
#pragma warning(disable : 4042)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
    
#ifndef CLOCKS_PER_SEC
#include <time.h>
#endif
    
#ifndef va_start
#include <stdarg.h>
#endif
    
    // NOTE(bSalmon): Check for any particular macro in <stdio.h>
#ifndef FOPEN_MAX
#include <stdio.h>
#endif
    
#ifndef BSDEF
#ifdef BS842_IRSDK_STATIC
#define BSDEF static
#else
#define BSDEF extern
#endif
#endif
    
#ifndef BS842_ASSERT
#define BS842_ASSERT(check) if(!(check)) {*(int *)0 = 0;}
#endif
    
#define IRSDK_DATAVALIDEVENTNAME "Local\\IRSDKDataValidEvent"
#define IRSDK_MEMMAPFILENAME "Local\\IRSDKMemMapFileName"
#define IRSDK_BROADCASTMSGNAME "IRSDK_BROADCASTMSG"
    
#define IRSDK_MAX_BUFFERS 4
#define IRSDK_MAX_STRING 32
#define IRSDK_MAX_DESC 64
    
#define IRSDK_UNLIMITED_LAPS 32767
#define IRSDK_UNLIMITED_TIME 604800.0f
    
#define IRSDK_TIMEOUT 30
    
#define IRSDK_VERSION 2
    
    enum IRSDK_EngineWarnings 
    {
        IR_Engine_WaterTempWarning    = 0x01,
        IR_Engine_FuelPressureWarning = 0x02,
        IR_Engine_OilPressureWarning  = 0x04,
        IR_Engine_EngineStalled       = 0x08,
        IR_Engine_PitSpeedLimiter     = 0x10,
        IR_Engine_RevLimiterActive    = 0x20,
        IR_Engine_OilTempWarning      = 0x40,
    };
    
    enum IRSDK_Flags
    {
        // global flags
        IR_Flags_Checkered     = 0x00000001,
        IR_Flags_White         = 0x00000002,
        IR_Flags_Green         = 0x00000004,
        IR_Flags_Yellow        = 0x00000008,
        IR_Flags_Red           = 0x00000010,
        IR_Flags_Blue          = 0x00000020,
        IR_Flags_Debris        = 0x00000040,
        IR_Flags_Crossed       = 0x00000080,
        IR_Flags_YellowWaving  = 0x00000100,
        IR_Flags_OneLapToGreen = 0x00000200,
        IR_Flags_GreenHeld     = 0x00000400,
        IR_Flags_TenToGo       = 0x00000800,
        IR_Flags_FiveToGo      = 0x00001000,
        IR_Flags_RandomWaving  = 0x00002000,
        IR_Flags_Caution       = 0x00004000,
        IR_Flags_CautionWaving = 0x00008000,
        
        // drivers black flags
        IR_Flags_Black         = 0x00010000,
        IR_Flags_DSQ           = 0x00020000,
        IR_Flags_Servicible    = 0x00040000, // car is allowed service (not a flag)
        IR_Flags_Furled        = 0x00080000,
        IR_Flags_Repair        = 0x00100000,
        
        // start lights
        IR_Flags_StartHidden   = 0x10000000,
        IR_Flags_StartReady    = 0x20000000,
        IR_Flags_StartSet      = 0x40000000,
        IR_Flags_StartGo       = 0x80000000,
    };
    
    enum IRSDK_TrackLocation
    {
        IR_TrkLoc_NotInWorld = -1,
        IR_TrkLoc_OffTrack,
        IR_TrkLoc_InPitStall,
        IR_TrkLoc_ApproachingPits,
        IR_TrkLoc_OnTrack
    };
    
    enum IRSDK_TrackSurface
    {
        IR_TrkSurf_SurfaceNotInWorld = -1,
        IR_TrkSurf_UndefinedMaterial = 0,
        
        IR_TrkSurf_Asphalt1Material,
        IR_TrkSurf_Asphalt2Material,
        IR_TrkSurf_Asphalt3Material,
        IR_TrkSurf_Asphalt4Material,
        IR_TrkSurf_Concrete1Material,
        IR_TrkSurf_Concrete2Material,
        IR_TrkSurf_RacingDirt1Material,
        IR_TrkSurf_RacingDirt2Material,
        IR_TrkSurf_Paint1Material,
        IR_TrkSurf_Paint2Material,
        IR_TrkSurf_Rumble1Material,
        IR_TrkSurf_Rumble2Material,
        IR_TrkSurf_Rumble3Material,
        IR_TrkSurf_Rumble4Material,
        
        IR_TrkSurf_Grass1Material,
        IR_TrkSurf_Grass2Material,
        IR_TrkSurf_Grass3Material,
        IR_TrkSurf_Grass4Material,
        IR_TrkSurf_Dirt1Material,
        IR_TrkSurf_Dirt2Material,
        IR_TrkSurf_Dirt3Material,
        IR_TrkSurf_Dirt4Material,
        IR_TrkSurf_SandMaterial,
        IR_TrkSurf_Gravel1Material,
        IR_TrkSurf_Gravel2Material,
        IR_TrkSurf_GrasscreteMaterial,
        IR_TrkSurf_AstroturfMaterial,
    };
    
    enum IRSDK_SessionState
    {
        IR_SessionState_Invalid,
        IR_SessionState_GetInCar,
        IR_SessionState_Warmup,
        IR_SessionState_ParadeLaps,
        IR_SessionState_Racing,
        IR_SessionState_Checkered,
        IR_SessionState_CoolDown
    };
    
    enum IRSDK_CarLeftRight
    {
        IR_LR_Off,
        IR_LR_Clear,        // no cars around us.
        IR_LR_CarLeft,      // there is a car to our left.
        IR_LR_CarRight,     // there is a car to our right.
        IR_LR_CarLeftRight, // there are cars on each side.
        IR_LR_2CarsLeft,    // there are two cars to our left.
        IR_LR_2CarsRight    // there are two cars to our right.
    };
    
    enum IRSDK_CameraState
    {
        IR_CamState_IsSessionScreen       = 0x0001, // the camera tool can only be activated if viewing the session screen (out of car)
        IR_CamState_IsScenicActive        = 0x0002, // the scenic camera is active (no focus car)
        
        //these can be changed with a broadcast message
        IR_CamState_CamToolActive         = 0x0004,
        IR_CamState_UIHidden              = 0x0008,
        IR_CamState_UseAutoShotSelection  = 0x0010,
        IR_CamState_UseTemporaryEdits     = 0x0020,
        IR_CamState_UseKeyAcceleration    = 0x0040,
        IR_CamState_UseKey10xAcceleration = 0x0080,
        IR_CamState_UseMouseAimMode       = 0x0100
    };
    
    enum IRSDK_PitSvFlags
    {
        IR_PitFlag_LFTireChange      = 0x0001,
        IR_PitFlag_RFTireChange      = 0x0002,
        IR_PitFlag_LRTireChange      = 0x0004,
        IR_PitFlag_RRTireChange      = 0x0008,
        
        IR_PitFlag_FuelFill          = 0x0010,
        IR_PitFlag_WindshieldTearoff = 0x0020,
        IR_PitFlag_FastRepair        = 0x0040
    };
    
    enum IRSDK_PitSvStatus
    {
        // status
        IR_PitStatus_None = 0,
        IR_PitStatus_InProgress,
        IR_PitStatus_Complete,
        
        // errors
        IR_PitStatus_TooFarLeft = 100,
        IR_PitStatus_TooFarRight,
        IR_PitStatus_TooFarForward,
        IR_PitStatus_TooFarBack,
        IR_PitStatus_BadAngle,
        IR_PitStatus_CantFixThat,
    };
    
    enum IRSDK_PaceMode
    {
        IR_PaceMode_SingleFileStart = 0,
        IR_PaceMode_DoubleFileStart,
        IR_PaceMode_SingleFileRestart,
        IR_PaceMode_DoubleFileRestart,
        IR_PaceMode_NotPacing,
    };
    
    enum irsdk_PaceFlags
    {
        irsdk_PaceFlagsEndOfLine = 0x01,
        irsdk_PaceFlagsFreePass = 0x02,
        irsdk_PaceFlagsWavedAround = 0x04,
    };
    
    enum YAML_State
    {
        YAML_Space,
        YAML_Key,
        YAML_KeySep,
        YAML_Value,
        YAML_NewLine,
    };
    
    typedef struct
    {
        int type;
        int offset;
        int count;
        bool countAsTime;
        char padding[3];
        
        char name[IRSDK_MAX_STRING];
        char desc[IRSDK_MAX_DESC];
        char unit[IRSDK_MAX_STRING];
    } IRSDK_VarHeader;
    
    typedef struct
    {
        int tickCount;
        int bufferOffset;
        int padding[2];
    } IRSDK_VarBuffer;
    
    typedef struct
    {
        int version;
        int status;
        int tickRate;
        
        int sessionInfoUpdate;
        int sessionInfoLength;
        int sessionInfoOffset;
        
        int numVars;
        int varHeaderOffset;
        
        int numBuffers;
        int bufferLength;
        int padding[2];
        IRSDK_VarBuffer varBuffer[IRSDK_MAX_BUFFERS];
    } IRSDK_Header;
    
    typedef struct
    {
        time_t sessionStartDate;
        double sessionStartTime;
        double sessionEndTime;
        int sessionLapCount;
        int sessionRecordCount;
    } IRSDK_DiskSubHeader;
    
    typedef struct
    {
        bool initialised;
        HANDLE dataValidEvent;
        HANDLE memMapFile;
        char *sharedMem;
        IRSDK_Header *irsdkHeader;
        int lastTickCount;
        time_t lastValidTime;
    } BS842_IRSDK_Internal_Info;
    
    static BS842_IRSDK_Internal_Info bs842_IRSDK_internal_info = {};
    
    BSDEF bool BS842_IRSDK_Init();
    BSDEF bool BS842_IRSDK_GetNewData(char *data);
    BSDEF bool BS842_IRSDK_CheckForNewData(char *data);
    
    BSDEF IRSDK_VarHeader *BS842_IRSDK_GetVarHeaderEntry(int index);
    BSDEF int BS842_IRSDK_VarNameToOffset(char *varName);
    
    BSDEF bool BS842_IRSDK_ParseYAML(char *data, char *path, char **returnVal);
    BSDEF char *BS842_IRSDK_GetSessionInfoString();
    BSDEF bool BS842_IRSDK_GetIntFromYAML(int *result, char *fmt, ...);
    BSDEF bool BS842_IRSDK_GetFloatFromYAML(float *result, char *fmt, ...);
    BSDEF bool BS842_IRSDK_GetStringFromYAML(char *result, char *fmt, ...);
    
    BSDEF bool BS842_IRSDK_IsInitialised();
    BSDEF IRSDK_Header *BS842_IRSDK_GetHeader();
    
#ifndef BS842_PARSEYAML
#define BS842_PARSEYAML(path, returnVal, returnLen) BS842_IRSDK_ParseYAML(path, returnVal, returnLen) 
#endif
    
#define BS842_IRSDK_GetInt(data, offset) *((s32 *)(data + offset));
#define BS842_IRSDK_GetFloat(data, offset) *((f32 *)(data + offset));
#define BS842_IRSDK_GetDouble(data, offset) *((f64 *)(data + offset));
#define BS842_IRSDK_GetBool(data, offset) *((b8 *)(data + offset));
#define BS842_IRSDK_GetFlags(data, offset) *((IRSDK_Flags *)(data + offset));
#define BS842_IRSDK_GetEngineWarnings(data, offset) *((IRSDK_EngineWarnings *)(data + offset));
    
#ifdef __cplusplus
}
#endif

#define BS842_INCLUDE_IRSDK_H
#endif // BS842_INCLUDE_IRSDK_H

#ifdef BS842_IRSDK_IMPLEMENTATION

BSDEF bool BS842_IRSDK_Init()
{
    bool result = false;
    
    if (!bs842_IRSDK_internal_info.memMapFile)
    {
        bs842_IRSDK_internal_info.memMapFile = OpenFileMapping(FILE_MAP_READ, FALSE, IRSDK_MEMMAPFILENAME);
        bs842_IRSDK_internal_info.lastTickCount = INT_MAX;
    }
    
    if (bs842_IRSDK_internal_info.memMapFile)
    {
        if (!bs842_IRSDK_internal_info.sharedMem)
        {
            bs842_IRSDK_internal_info.sharedMem = (char *)MapViewOfFile(bs842_IRSDK_internal_info.memMapFile, FILE_MAP_READ, 0, 0, 0);
            bs842_IRSDK_internal_info.irsdkHeader = (IRSDK_Header *)bs842_IRSDK_internal_info.sharedMem;
            bs842_IRSDK_internal_info.lastTickCount = INT_MAX;
        }
        
        if (bs842_IRSDK_internal_info.sharedMem)
        {
            if (!bs842_IRSDK_internal_info.dataValidEvent)
            {
                bs842_IRSDK_internal_info.dataValidEvent = OpenEvent(SYNCHRONIZE, false, IRSDK_DATAVALIDEVENTNAME);
                bs842_IRSDK_internal_info.lastTickCount = INT_MAX;
            }
            
            if (bs842_IRSDK_internal_info.dataValidEvent)
            {
                bs842_IRSDK_internal_info.initialised = true;
            }
        }
    }
    
    return result;
}

BSDEF bool BS842_IRSDK_GetNewData(char *data)
{
    bool result = false;
    
    ASSERT(bs842_IRSDK_internal_info.initialised);
    
    if (!(bs842_IRSDK_internal_info.irsdkHeader->status & 0x00000001))
    {
        bs842_IRSDK_internal_info.lastTickCount = INT_MAX;
    }
    else
    {
        int latest = 0;
        for (int i = 1; i < bs842_IRSDK_internal_info.irsdkHeader->numBuffers; ++i)
        {
            if (bs842_IRSDK_internal_info.irsdkHeader->varBuffer[latest].tickCount <
                bs842_IRSDK_internal_info.irsdkHeader->varBuffer[i].tickCount)
            {
                latest = i;
            }
        }
        
        if (bs842_IRSDK_internal_info.lastTickCount < bs842_IRSDK_internal_info.irsdkHeader->varBuffer[latest].tickCount)
        {
            if (data)
            {
                for (int count = 0; count < 2; ++count)
                {
                    int currTickCount = bs842_IRSDK_internal_info.irsdkHeader->varBuffer[latest].tickCount;
                    memcpy(data,
                           &bs842_IRSDK_internal_info.sharedMem[bs842_IRSDK_internal_info.irsdkHeader->varBuffer[latest].bufferOffset],
                           bs842_IRSDK_internal_info.irsdkHeader->bufferLength);
                    if (currTickCount == bs842_IRSDK_internal_info.irsdkHeader->varBuffer[latest].tickCount)
                    {
                        bs842_IRSDK_internal_info.lastTickCount = currTickCount;
                        bs842_IRSDK_internal_info.lastValidTime = time(0);
                        result = true;
                    }
                }
            }
            else
            {
                bs842_IRSDK_internal_info.lastTickCount = bs842_IRSDK_internal_info.irsdkHeader->varBuffer[latest].tickCount;
                bs842_IRSDK_internal_info.lastValidTime = time(0);
                result = true;
            }
        }
        else if (bs842_IRSDK_internal_info.lastTickCount > bs842_IRSDK_internal_info.irsdkHeader->varBuffer[latest].tickCount)
        {
            bs842_IRSDK_internal_info.lastTickCount = bs842_IRSDK_internal_info.irsdkHeader->varBuffer[latest].tickCount;
        }
    }
    
    return result;
}

BSDEF bool BS842_IRSDK_CheckForNewData(char *data)
{
    bool result = false;
    
    ASSERT(bs842_IRSDK_internal_info.initialised);
    
    if (BS842_IRSDK_GetNewData(data))
    {
        result = true;
    }
    else
    {
        WaitForSingleObject(bs842_IRSDK_internal_info.dataValidEvent, IRSDK_TIMEOUT);
        
        if (BS842_IRSDK_GetNewData(data))
        {
            result = true;
        }
    }
    
    if (!result)
    {
        Sleep(IRSDK_TIMEOUT);
    }
    
    return result;
}

BSDEF IRSDK_VarHeader *BS842_IRSDK_GetVarHeaderEntry(int index)
{
    IRSDK_VarHeader *result = 0;
    
    ASSERT(bs842_IRSDK_internal_info.initialised);
    
    if (index >= 0 && index < bs842_IRSDK_internal_info.irsdkHeader->numVars)
    {
        result = &((IRSDK_VarHeader *)(&bs842_IRSDK_internal_info.sharedMem[bs842_IRSDK_internal_info.irsdkHeader->varHeaderOffset]))[index];
    }
    
    return result;
}

BSDEF int BS842_IRSDK_VarNameToOffset(char *varName)
{
    int result = -1;
    
    IRSDK_VarHeader *varHeader;
    
    if (varName)
    {
        for (int index = 0; index < bs842_IRSDK_internal_info.irsdkHeader->numVars; ++index)
        {
            varHeader = BS842_IRSDK_GetVarHeaderEntry(index);
            if (varHeader && strncmp(varName, varHeader->name, IRSDK_MAX_STRING) == 0)
            {
                result = varHeader->offset;
            }
        }
    }
    
    return result;
}

BSDEF bool BS842_IRSDK_ParseYAML(char *path, char **returnVal, int *returnLen)
{
    char *data = BS842_IRSDK_GetSessionInfoString();
    
    if(path && returnVal)
	{
		// make sure we set this to something
		*returnVal = NULL;
        *returnLen = 0;
        
		int depth = 0;
		YAML_State state = YAML_Space;
        
        char *keystr = NULL;
		int keylen = 0;
        
        char *valuestr = NULL;
		int valuelen = 0;
        
        char *pathptr = path;
		int pathdepth = 0;
        
		while(*data)
		{
			switch(*data)
			{
                case ' ':
                {
                    
                    if(state == YAML_NewLine)
                        state = YAML_Space;
                    if(state == YAML_Space)
                        depth++;
                    else if(state == YAML_Key)
                        keylen++;
                    else if(state == YAML_Value)
                        valuelen++;
                } break;
                
                case '-':
                {
                    if(state == YAML_NewLine)
                        state = YAML_Space;
                    if(state == YAML_Space)
                        depth++;
                    else if(state == YAML_Key)
                        keylen++;
                    else if(state == YAML_Value)
                        valuelen++;
                    else if(state == YAML_KeySep)
                    {
                        state = YAML_Value;
                        valuestr = data;
                        valuelen = 1;
                    }
                } break;
                
                case ':':
                {
                    if(state == YAML_Key)
                    {
                        state = YAML_KeySep;
                        keylen++;
                    }
                    else if(state == YAML_KeySep)
                    {
                        state = YAML_Value;
                        valuestr = data;
                    }
                    else if(state == YAML_Value)
                        valuelen++;
                } break;
                
                case '\n':
                case '\r':
                {
                    if(state != YAML_NewLine)
                    {
                        if(depth < pathdepth)
                        {
                            return false;
                        }
                        else if(keylen && 0 == strncmp(keystr, pathptr, keylen))
                        {
                            bool found = true;
                            //do we need to test the value?
                            if(*(pathptr+keylen) == '{')
                            {
                                //search for closing brace
                                int pathvaluelen = keylen + 1; 
                                while(*(pathptr+pathvaluelen) && *(pathptr+pathvaluelen) != '}')
                                    pathvaluelen++; 
                                
                                if(valuelen == pathvaluelen - (keylen+1) && 0 == strncmp(valuestr, (pathptr+keylen+1), valuelen))
                                    pathptr += valuelen + 2;
                                else
                                    found = false;
                            }
                            
                            if(found)
                            {
                                pathptr += keylen;
                                pathdepth = depth;
                                
                                if(*pathptr == '\0')
                                {
                                    *returnVal = valuestr;
                                    *returnLen = valuelen;
                                    return true;
                                }
                            }
                        }
                        
                        depth = 0;
                        keylen = 0;
                        valuelen = 0;
                    }
                    state = YAML_NewLine;
                } break;
                
                default:
                {
                    if(state == YAML_Space || state == YAML_NewLine)
                    {
                        state = YAML_Key;
                        keystr = data;
                        keylen = 0; //redundant?
                    }
                    else if(state == YAML_KeySep)
                    {
                        state = YAML_Value;
                        valuestr = data;
                        valuelen = 0; //redundant?
                    }
                    if(state == YAML_Key)
                        keylen++;
                    if(state == YAML_Value)
                        valuelen++;
                } break;
			}
            
			// important, increment our pointer
			data++;
		}
        
	}
    return false;
}

BSDEF char *BS842_IRSDK_GetSessionInfoString()
{
    char *result = 0;
    
    ASSERT(bs842_IRSDK_internal_info.initialised);
    
    result = bs842_IRSDK_internal_info.sharedMem + bs842_IRSDK_internal_info.irsdkHeader->sessionInfoOffset;
    
    return result;
}

BSDEF bool BS842_IRSDK_GetIntFromYAML(int *result, char *fmt, ...)
{
    bool boolResult = false;
    
    char *valueString;
    int valueLen;
    
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    if (BS842_PARSEYAML(buf, &valueString, &valueLen))
    {
        *result = atoi(valueString);
        boolResult = true;
    }
    
    va_end(args);
    return boolResult;
}

BSDEF bool BS842_IRSDK_GetFloatFromYAML(float *result, char *fmt, ...)
{
    bool boolResult = false;
    
    char *valueString;
    s32 valueLen;
    
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    if (BS842_PARSEYAML(buf, &valueString, &valueLen))
    {
        *result = (float)atof(valueString);
        boolResult = true;
    }
    
    va_end(args);
    return boolResult;
}

BSDEF bool BS842_IRSDK_GetStringFromYAML(char *result, char *fmt, ...)
{
    bool boolResult = false;
    
    char *valueString;
    int valueLen;
    
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    if (BS842_PARSEYAML(buf, &valueString, &valueLen))
    {
        for (int i = 0; i < valueLen; ++i)
        {
            result[i] = valueString[i];
        }
        
        result[valueLen] = '\0';
        boolResult = true;
    }
    
    va_end(args);
    return boolResult;
}

BSDEF bool BS842_IRSDK_IsInitialised()
{
    return bs842_IRSDK_internal_info.initialised;
}

BSDEF IRSDK_Header *BS842_IRSDK_GetHeader()
{
    ASSERT(bs842_IRSDK_internal_info.initialised);
    return bs842_IRSDK_internal_info.irsdkHeader;
}

#endif // BS842_IRSDK_IMPLEMENTATION
