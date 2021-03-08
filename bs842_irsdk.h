/*
Project: BS842 Tools
File: bs842_irsdk.h
Author: Brock Salmon
Notice: (C) Copyright 2020 by Brock Salmon. All Rights Reserved
*/

/*
BASED ON THE IRACING SDK PROVIDED BY IRACING.COM
Copyright (c) 2013, iRacing.com Motorsport Simulations, LLC.
All rights reserved.
*/

#ifdef BS842_IRSDK_IMPLEMENTATION

#ifndef INT_MAX
#define INT_MAX 2147483647
#endif // INT_MAX

typedef unsigned __int8 bs_u8;
typedef unsigned __int16 bs_u16;
typedef unsigned __int32 bs_u32;
typedef unsigned __int64 bs_u64;
typedef __int8 bs_s8;
typedef __int16 bs_s16;
typedef __int32 bs_s32;
typedef __int64 bs_s64;
typedef __int32 bs_b32;
typedef bool bs_b8;
typedef float bs_f32;
typedef double bs_f64;

#ifndef BS842_ATOI
#include <stdlib.h>
#define BS842_ATOI(string) atoi(string)
#endif // BS842_ATOI

#ifndef BS842_ATOF
#include <stdlib.h>
#define BS842_ATOF(string) atof(string)
#endif // BS842_ATOF

#ifndef BS842_STRNCMP
#include <string.h>
#define BS842_STRNCMP(string1, string2, len) strncmp(string1, string2, len)
#endif // BS842_STRNCMP

#ifndef BS842_TIME
#include <time.h>
#define BS842_TIME_T time_t
#define BS842_TIME(val) time(val)
#endif // BS842_TIME

#ifndef BS842_MEMMAP
#include <winbase.h>
#include <memoryapi.h>
#define BS842_OPENFILEMAPPING(access, inheritHandle, name) OpenFileMapping(access, inheritHandle, name)
#define BS842_MAPVIEWOFFILE(mapObject, access, offsetHigh, offsetLow, bytesToMap) MapViewOfFile(mapObject, access, offsetHigh, offsetLow, bytesToMap)
#endif // BS842_MEMMAP

#ifndef BS842_SYNCHAPI
#include <synchapi.h>
#define BS842_OPENEVENT(access, inheritHandle, name) OpenEvent(access, inheritHandle, name)
#define BS842_WAITFORSINGLEOBJECT(handle, milliseconds) WaitForSingleObject(handle, milliseconds)
#define BS842_SLEEP(milliseconds) Sleep(milliseconds)
#endif // BS842_SYNCHAPI

#ifndef IRSDK_MAX_BUFS
#define IRSDK_MAX_BUFS 4
#endif // IRSDK_MAX_BUFS

#ifndef IRSDK_MAX_STRING
#define IRSDK_MAX_STRING 32
#endif // IRSDK_MAX_STRING

#ifndef IRSDK_MAX_DESC
#define IRSDK_MAX_DESC 64
#endif // IRSDK_MAX_DESC

#ifndef IRSDK_STATUS_CONNECTED
#define IRSDK_STATUS_CONNECTED 1
#endif // IRSDK_STATUS_CONNECTED

#ifndef IRSDK_DATAVALIDEVENTNAME
#define IRSDK_DATAVALIDEVENTNAME _T("Local\\IRSDKDataValidEvent")
#endif // IRSDK_DATAVALIDEVENTNAME

#ifndef IRSDK_MEMMAPFILENAME
#define IRSDK_MEMMAPFILENAME _T("Local\\IRSDKMemMapFileName")
#endif // IRSDK_MEMMAPFILENAME

#ifndef IRSDK_UNLIMITED
#define IRSDK_UNLIMITED_LAPS 32767
#define IRSDK_UNLIMITED_TIME 604800.0f
#endif // IRSDK_UNLIMITED

#ifndef IRSDK_TIMEOUT
#define IRSDK_TIMEOUT_MS 30
#endif // IRSDK_TIMEOUT

#endif // BS842_IRSDK_IMPLEMENTATION

#ifndef BS842_IRSDK_H

#ifdef BS_STATIC
#define BS_FUNC_DEF static
#else
#define BS_FUNC_DEF extern
#endif // BS842_STATIC

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
    
    typedef struct
    {
        int tickCount;		// used to detect changes in data
        int bufOffset;		// offset from header
        int pad[2];			// (16 byte align)
    } BS842_Internal_IRSDKVarBuf;
    
    typedef struct
    {
        int type;			// irsdk_VarType
        int offset;			// offset fron start of buffer row
        int count;			// number of entrys (array)
        // so length in bytes would be irsdk_VarTypeBytes[type] * count
        bool countAsTime;
        char pad[3];		// (16 byte align)
        
        char name[IRSDK_MAX_STRING];
        char desc[IRSDK_MAX_DESC];
        char unit[IRSDK_MAX_STRING];	// something like "kg/m^2"
        
    } BS842_Internal_IRSDK_VarHeader;
    
    typedef struct
    {
        int ver;				// this api header version, see IRSDK_VER
        int status;				// bitfield using irsdk_StatusField
        int tickRate;			// ticks per second (60 or 360 etc)
        
        // session information, updated periodicaly
        int sessionInfoUpdate;	// Incremented when session info changes
        int sessionInfoLen;		// Length in bytes of session info string
        int sessionInfoOffset;	// Session info, encoded in YAML format
        
        // State data, output at tickRate
        
        int numVars;			// length of arra pointed to by varHeaderOffset
        int varHeaderOffset;	// offset to irsdk_varHeader[numVars] array, Describes the variables received in varBuf
        
        int numBuf;				// <= IRSDK_MAX_BUFS (3 for now)
        int bufLen;				// length in bytes for one line
        int pad1[2];			// (16 byte align)
         BS842_Internal_IRSDKVarBuf varBuf[IRSDK_MAX_BUFS]; // buffers of data being written to
    } BS842_Internal_IRSDKHeader;
    
    struct BS842_iRacingInfo
    {
        bool isIRSDKInitialised;
        int lastTickCount;
        
        void *dataValidEvent;
        void *memMapFile;
        
        char *sharedMem;
        BS842_Internal_IRSDKHeader *header;
        
        double timeout;
        BS842_TIME_T lastValidTime;
    };
    
    enum { // YAML State
        BS842_YAML_SPACE   = 0,
        BS842_YAML_KEY     = 1,
        BS842_YAML_KEYSEP  = 2,
        BS842_YAML_VALUE   = 3,
        BS842_YAML_NEWLINE = 4,
    };
    
    enum IRSDK_EngineWarnings 
    {
        irsdk_waterTempWarning	= 0x01,
        irsdk_fuelPressureWarning = 0x02,
        irsdk_oilPressureWarning  = 0x04,
        irsdk_engineStalled	   = 0x08,
        irsdk_pitSpeedLimiter	 = 0x10,
        irsdk_revLimiterActive	= 0x20,
    };
    
    enum IRSDK_TrkLoc
    {
        irsdk_NotInWorld = -1,
        irsdk_OffTrack,
        irsdk_InPitStall,
        irsdk_AproachingPits,
        irsdk_OnTrack
    };
    
    enum IRSDK_PitSvFlags
    {
        IRSDK_LFTireChange	  = 0x0001,
        IRSDK_RFTireChange	  = 0x0002,
        IRSDK_LRTireChange	  = 0x0004,
        IRSDK_RRTireChange	  = 0x0008,
        
        IRSDK_FuelFill		  = 0x0010,
        IRSDK_WindshieldTearoff = 0x0020,
        IRSDK_FastRepair		= 0x0040
    };
    
    BS_FUNC_DEF BS842_iRacingInfo InitiRacingInfo();
    BS_FUNC_DEF char *IRSDK_GetSessionInfoString(BS842_iRacingInfo *irInfo);
    BS_FUNC_DEF bool IRSDK_GetFromYAML(int *result, char *fmt, ...);
    BS_FUNC_DEF bool IRSDK_GetFromYAML(float *result, char *fmt, ...);
    BS_FUNC_DEF bool IRSDK_GetFromYAML(char *result, char *fmt, ...);
    BS_FUNC_DEF bool IRSDK_Startup(BS842_iRacingInfo *irInfo);
    BS_FUNC_DEF bool IRSDK_WaitForData(int timeout, char *data);
    BS_FUNC_DEF int IRSDK_VarNameToOffset(char *varName);
    
#ifdef __cplusplus
}
#endif // __cplusplus

#define BS842_IRSDK_H
#endif // BS842_IRSDK_H

#ifdef BS842_IRSDK_IMPLEMENTATION

// Reformatted version of iRacing.com's provided YAML parser
static bool bs842_internal_ParseYaml(char *data, char* path, char **val, bs_s32 *len)
{
    if (data && path && val && len)
    {
        *val = 0;
        *len = 0;
        
        bs_s32 depth = 0;
        bs_s32 yamlState = BS842_YAML_SPACE;
        
        char *keyStr = 0;
        bs_s32 keyLen = 0;
        
        char *valStr = 0;
        bs_s32 valLen = 0;
        
        char *pathPtr = path;
        bs_s32 pathDepth = 0;
        
        while (*data)
        {
            switch (*data)
            {
                case ' ':
                {
                    if (yamlState == BS842_YAML_NEWLINE)
                    {
                        yamlState = BS842_YAML_SPACE;
                    }
                    
                    if (yamlState == BS842_YAML_SPACE)
                    {
                        depth++;
                    }
                    else if (yamlState == BS842_YAML_KEY)
                    {
                        keyLen++;
                    }
                    else if (yamlState == BS842_YAML_VALUE)
                    {
                        valLen++;
                    }
                    
                    break;
                }
                
                case '-':
                {
                    if (yamlState == BS842_YAML_NEWLINE)
                    {
                        yamlState = BS842_YAML_SPACE;
                    }
                    
                    if (yamlState == BS842_YAML_SPACE)
                    {
                        depth++;
                    }
                    else if (yamlState == BS842_YAML_KEY)
                    {
                        keyLen++;
                    }
                    else if (yamlState == BS842_YAML_VALUE)
                    {
                        valLen++;
                    }
                    else if (yamlState == BS842_YAML_KEYSEP)
                    {
                        yamlState = value;
                        valStr = data;
                        valLen = 1;
                    }
                    
                    break;
                }
                
                case ':':
                {
                    if(yamlState == BS842_YAML_KEY)
                    {
                        yamlState = BS842_YAML_KEYSEP;
                        keyLen++;
                    }
                    else if(yamlState == BS842_YAML_KEYSEP)
                    {
                        yamlState = BS842_YAML_VALUE;
                        valStr = data;
                    }
                    else if(yamlState == BS842_YAML_VALUE)
                    {
                        valLen++;
                    }
                    
                    break;
                }
                
                case '\n':
                case '\r':
                {
                    if (yamlState != BS842_YAML_NEWLINE)
                    {
                        if (depth < pathDepth)
                        {
                            return false;
                        }
                        else if (keyLen && (BS842_STRNCMP(keystr, pathptr, keylen) == 0))
                        {
                            bs_b32 found = true;
                            //do we need to test the value?
                            if (*(pathPtr + keyLen) == '{')
                            {
                                //search for closing brace
                                 bs_s32 pathValLen = keyLen + 1; 
                                while (*(pathPtr + pathValLen) && (*(pathPtr+pathValLen) != '}'))
                                {
                                    pathValLen++;
                                }
                                
                                if ((valLen == (pathValLen - (keyLen + 1))) && (BS842_STRNCMP(valuestr, (pathPtr + keyLen + 1), valuelen) == 0))
                                {
                                    pathPtr += valLen + 2;
                                }
                                else
                                {
                                    found = false;
                                }
                            }
                            
                            if (found)
                            {
                                pathPtr += keyLen;
                                pathDepth = depth;
                                
                                if (*pathPtr == '\0')
                                {
                                    *val = valStr;
                                    *len = valLen;
                                    return true;
                                }
                            }
                        }
                        
                        depth = 0;
                        keyLen = 0;
                        valLen = 0;
                    }
                    yamlState = BS842_YAML_NEWLINE;
                    
                    break;
                }
            }
            
            data++;
        }
    }
    
    return false;
}

BS_FUNC_DEF char *IRSDK_GetSessionInfoString(BS842_iRacingInfo irInfo)
{
    if (irInfo.isIRSDKInitialised)
	{
		return irInfo.sharedMem + irInfo.header->sessionInfoOffset;
	}
	return 0;
}

static bool bs842_internal_GetS32FromYAML(int *result, char *sessionInfoString, BS842_iRacingInfo irInfo)
{
     char *valueString;
    bs_s32 valueLen;
    
    if (bs842_internal_ParseYaml(IRSDK_GetSessionInfoString(irInfo), sessionInfoString, &valueString, &valueLen))
    {
        *result = BS842_ATOI(valueString);
        return true;
    }
    else
    {
        return false;
    }
}

static bool bs842_internal_GetF32FromYAML(float *result, char *sessionInfoString, BS842_iRacingInfo irInfo)
{
     char *valueString;
    bs_s32 valueLen;
    
    if (bs842_internal_ParseYaml(IRSDK_GetSessionInfoString(irInfo), sessionInfoString, &valueString, &valueLen))
    {
        *result = BS842_ATOF(valueString);
        return true;
    }
    else
    {
        return false;
    }
}

static bool bs842_internal_GetStringFromYAML(char *result, char *sessionInfoString, BS842_iRacingInfo irInfo)
{
    char *valueString;
    bs_s32 valueLen;
    
    if (bs842_internal_ParseYaml(IRSDK_GetSessionInfoString(irInfo), sessionInfoString, &valueString, &valueLen))
    {
        for (s32 i = 0; i < valueLen; ++i)
        {
            result[i] = valueString[i];
        }
        
        result[valueLen] = '\0';
        
        return true;
    }
    else
    {
        return false;
    }
}

BS_FUNC_DEF bool IRSDK_GetFromYAML(BS842_iRacingInfo irInfo, int *result, char *fmt, ...)
{
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    bool boolResult = bs842_internal_GetS32FromYAML(irInfo, result, buf);
    va_end(args);
    
    return boolResult;
}

BS_FUNC_DEF bool IRSDK_GetFromYAML(BS842_iRacingInfo irInfo, float *result, char *fmt, ...)
{
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    bool boolResult = bs842_internal_GetF32FromYAML(irInfo, result, buf);
    va_end(args);
    
    return boolResult;
}

BS_FUNC_DEF bool IRSDK_GetFromYAML(BS842_iRacingInfo irInfo, char *result, char *fmt, ...)
{
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    bool boolResult = bs842_internal_GetStringFromYAML(irInfo, result, buf);
    va_end(args);
    
    return boolResult;
}

BS_FUNC_DEF bool IRSDK_Startup(BS842_iRacingInfo *irInfo)
{
    if (!irInfo->memMapFile)
	{
		irInfo->memMapFile = BS842_OPENFILEMAPPING(FILE_MAP_READ, FALSE, IRSDK_MEMMAPFILENAME);
		irInfo->lastTickCount = INT_MAX;
	}
    
	if (irInfo->memMapFile)
	{
		if (!irInfo->sharedMem)
		{
			irInfo->sharedMem = (char *)BS842_MAPVIEWOFFILE(irInfo->memMapFile, FILE_MAP_READ, 0, 0, 0);
			irInfo->header = (BS842_Internal_IRSDKHeader *)irInfo->sharedMem;
			irInfo->lastTickCount = INT_MAX;
		}
        
		if (irInfo->sharedMem)
		{
			if (!irInfo->dataValidEvent)
			{
				irInfo->dataValidEvent = BS842_OPENEVENT(SYNCHRONIZE, false, IRSDK_DATAVALIDEVENTNAME);
				irInfo->lastTickCount = INT_MAX;
			}
            
			if(irInfo->dataValidEvent)
			{
				irInfo->isIRSDKInitialised = true;
				return irInfo->isIRSDKInitialised;
			}
		}
	}
    
	irInfo->isIRSDKInitialised = false;
    return irInfo->isIRSDKInitialised;
}

 static void *bs842_internal_CopyMem(void *dest, void *src, int size)
{
    bs_u8 *destP = (bs_u8  *)dest;
    bs_u8 *srcP = (bs_u8  *)src;
    
    while (size--)
    {
        destP = srcP++;
        destP++;
    }
    
    return destP;
}

static bool bs842_internal_IRSDK_GetNewData(BS842_iRacingInfo *irInfo, char *data)
{
	if(irInfo->isIRSDKInitialised || IRSDK_Startup(irInfo))
	{
#ifdef _MSC_VER
		_ASSERTE(irInfo->header);
#endif
		if (!(irInfo->header->status & IRSDK_STATUS_CONNECTED))
		{
			irInfo->lastTickCount = INT_MAX;
			return false;
		}
        
		 bs_s32 latest = 0;
		for (bs_s32 i = 1; i < irInfo->header->numBuf; ++i)
        {
			if (irInfo->header->varBuf[latest].tickCount < irInfo->header->varBuf[i].tickCount)
            {
                latest = i;
            }
        }
        
		// if newer than last recieved, than report new data
		if (irInfo->lastTickCount < irInfo->header->varBuf[latest].tickCount)
		{
			// if asked to retrieve the data
			if (data)
			{
				// try twice to get the data out
				for (bs_s32 count = 0; count < 2; ++count)
				{
					 bs_s32 currTickCount = irInfo->header->varBuf[latest].tickCount;
					bs842_internal_CopyMem(data, irInfo->sharedMem + irInfo->header->varBuf[latest].bufOffset, irInfo->header->bufLen);
					if (currTickCount == irInfo->header->varBuf[latest].tickCount)
					{
						irInfo->lastTickCount = currTickCount;
						irInfo->lastValidTime = BS842_TIME(0);
						return true;
					}
				}
				// if here, the data changed out from under us.
				return false;
			}
			else
			{
				irInfo->lastTickCount = irInfo->header->varBuf[latest].tickCount;
				irInfo->lastValidTime = BS842_TIME(0);
				return true;
			}
		}
		else if (irInfo->lastTickCount >  pHeader->varBuf[latest].tickCount)
		{
			irInfo->lastTickCount = irInfo->header->varBuf[latest].tickCount;
			return false;
		}
	}
    
	return false;
}

BS_FUNC_DEF bool IRSDK_WaitForData(int timeout, char *data, BS842_iRacingInfo *irInfo)
{
#ifdef _MSC_VER
	_ASSERTE(timeOut >= 0);
#endif
    
	if (irInfo->isIRSDKInitialised || IRSDK_Startup(irInfo))
	{
		if (bs842_internal_IRSDK_GetNewData(data, irInfo))
        {
			return true;
        }
        
		BS842_WAITFORSINGLEOBJECT(irInfo->dataValidEvent, timeOut);
        
		if (bs842_internal_IRSDK_GetNewData(data, irInfo))
        {
			return true;
        }
		else
        {
			return false;
        }
	}
    
	if (timeOut > 0)
    {
		BS842_SLEEP(timeOut);
    }
    
	return false;
}

BS_FUNC_DEF int IRSDK_VarNameToOffset(char *varName, BS842_iRacingInfo *irInfo)
{
    BS842_Internal_IRSDK_VarHeader *varHeader;
    
	if (varName)
	{
		for (bs_s32 index = 0; index < irInfo->header->numVars; ++index)
		{
            if(index >= 0 && index < irInfo->header->numVars)
            {
                varHeader = &((BS842_Internal_IRSDK_VarHeader *)(irInfo->sharedMem + irInfo->header->varHeaderOffset))[index];
            }
            
			if (varHeader && (BS842_STRNCMP(varName, varHeader->name, IRSDK_MAX_STRING) == 0))
			{
				return varName->offset;
			}
		}
	}
    
	return -1;
}

BS_FUNC_DEF BS842_iRacingInfo InitiRacingInfo()
{
    BS842_iRacingInfo result = {};
    result.lastTickCount = INT_MAX;
    result.timeout = IRSDK_TIMEOUT_MS;
    
    return result;
}

#endif // BS842_IRSDK_IMPLEMENTATION