#ifndef _CANON_POWERSHOT_PROPERTIES_H_
#define _CANON_POWERSHOT_PROPERTIES_H_

#include "prType.h"

#ifdef _MSC_VER
#	if	900 <= _MSC_VER
	//#	pragma message( "push, _PR_TYPE_H_PACK_, 1" )
	#	pragma pack( push, _PR_TYPE_H_PACK_, 1 )
	#	pragma warning( disable : 4250 4355 4244 4005)
#	endif	/* 900 <= _MSC_VER */
#elif defined __WATCOMC__
	#pragma pack(__push,1);
#endif

#ifdef macintosh
 #if PRAGMA_STRUCT_ALIGN
  #pragma options align=mac68k
 #endif
#endif

// GetSet values
#define GETSET_READONLY 0x00
#define GETSET_RW       0x01

// FormFlag values
#define FORM_NONE       0x00
#define FORM_RANGE      0x01
#define FORM_ENUM       0x02

typedef struct {
    prptpDevicePropCode DevicePropertyCode;
    prUInt16    DataType;
    prUInt8     GetSet;
    // Factory Default Value (data type indicated)
    // Current Value (data type indicated)
    // Form Flag (prUInt8) - format of the next field
    // Form (variable length, type indicated)
} DescHeader;

typedef struct {
    prptpDevicePropCode DevicePropertyCode;
    prUInt16    DataType;                       // prUInt8
    prUInt8     GetSet;                         // Get/Set
    prUInt8     DefaultValue;
    prUInt8     CurrentValue;
    prUInt8     FormFlag;                       // Enum
    prUInt16    NumElements;
    prUInt8     SupportedValue[128];            // list of size NumElements
} DescEnumUInt8;

typedef struct {
    prptpDevicePropCode DevicePropertyCode;
    prUInt16    DataType;                       // prUInt16
    prUInt8     GetSet;                         // Get/Set
    prUInt16    DefaultValue;
    prUInt16    CurrentValue;
    prUInt8     FormFlag;                       // Enum
    prUInt16    NumElements;
    prUInt16    SupportedValue[128];            // list of size NumElements
} DescEnumUInt16;

// "Large Fine JPEG" etc

//
// Quality
//
const PSPropertyTableEntry tableQuality[] =
{
    {0x00,"Undefined"},
    {0x01,"Economy"},
    {0x02,"Normal"},
    {0x03,"Fine"},
    {0x04,"Lossless"},
    {0x05,"SuperFine"},

    {0,0}, // terminator
};

//
// File Format
//
const PSPropertyTableEntry tableFileFormat[] =
{
    {0x00,"Undefined"},
    {0x01,"JPEG"},
    {0x02,"RAW"},

    {0,0}, // terminator
};

//
// Image Size
//
const PSPropertyTableEntry tableImageSize[] =
{
    {0x00,"Large"},
    {0x01,"Medium 1"},
    {0x02,"Small"},
    {0x03,"Medium 2"},
    {0x07,"Medium 3"},

    {0,0}, // terminator
};

//
// Combined Quality (size, format, quality)
//
const PSPropertyTableEntry tableCombinedQuality[] =
{
    {0x000200,"Large RAW"},
    {0x010200,"Medium 1 RAW"},
    {0x020200,"Small RAW"},
    {0x030200,"Medium 2 RAW"},
    {0x070200,"Medium 3 RAW"},

    {0x000101,"Large Economy JPEG"},
    {0x010101,"Medium 1 Economy JPEG"},
    {0x020101,"Small Economy JPEG"},
    {0x030101,"Medium 2 Economy JPEG"},
    {0x070101,"Medium 3 Economy JPEG"},

    {0x000102,"Large Normal JPEG"},
    {0x010102,"Medium 1 Normal JPEG"},
    {0x020102,"Small Normal JPEG"},
    {0x030102,"Medium 2 Normal JPEG"},
    {0x070102,"Medium 3 Normal JPEG"},

    {0x000103,"Large Fine JPEG"},
    {0x010103,"Medium 1 Fine JPEG"},
    {0x020103,"Small Fine JPEG"},
    {0x030103,"Medium 2 Fine JPEG"},
    {0x070103,"Medium 3 Fine JPEG"},

    {0x000104,"Large Lossless JPEG"},
    {0x010104,"Medium 1 Lossless JPEG"},
    {0x020104,"Small Lossless JPEG"},
    {0x030104,"Medium 2 Lossless JPEG"},
    {0x070104,"Medium 3 Lossless JPEG"},

    {0x000105,"Large SuperFine JPEG"},
    {0x010105,"Medium 1 SuperFine JPEG"},
    {0x020105,"Small SuperFine JPEG"},
    {0x030105,"Medium 2 SuperFine JPEG"},
    {0x070105,"Medium 3 SuperFine JPEG"},

    {0,0}, // terminator
};

//
// Exposure Mode
//
const PSPropertyTableEntry tableMode[] =
{
    {0x00,"Auto"},
    {0x01,"P"},
    {0x02,"Tv"},
    {0x03,"Av"},
    {0x04,"M"},
    {0x05,"A_DEP"},
    {0x06,"M_DEP"},
    {0x07,"Bulb"},
    {0x80,"CAMERAM"},
    {0x81,"MYCOLOR"},
    {0x82,"PORTRAIT"},
    {0x83,"LANDSCAPE"},
    {0x84,"NIGHTSCENE"},
    {0x85,"FOREST"},
    {0x86,"SNOW"},
    {0x87,"BEACH"},
    {0x88,"FIREWORKS"},
    {0x89,"PARTY"},
    {0x8A,"NIGHTSNAP"},
    {0x8B,"STITCH"},
    {0x8C,"MOVIE"},
    {0x8D,"CUSTOM"},
    {0x8E,"INTERVAL"},
    {0x8F,"DIGITALMACRO"},
    {0x90,"LONGSHUTTER"},
    {0x91,"UNDERWATER"},
    {0x92,"KIDSANDPETS"},
    {0x93,"FASTSHUTTER"},
    {0x94,"SLOWSHUTTER"},
    {0x95,"CUSTOM1"},
    {0x96,"CUSTOM2"},
    {0x97,"NEUTRAL"},
    {0x98,"GRAY"},
    {0x99,"SEPIA"},
    {0x9A,"VIVID"},
    {0x9B,"SPORTS"},
    {0x9C,"MACRO"},
    {0x9D,"SUPERMACRO"},
    {0x9E,"PANFOCUS"},
    {0x9F,"BW"},
    {0xA0,"FLASHINHIBIT"},

    {0,0}, // terminator
};

//
// ISO
//
const PSPropertyTableEntry tableISO[] =
{
    {0x00,"Auto"},
    {0x28,"6"},
    {0x30,"12"},
    {0x38,"25"},
    {0x40,"50"},
    {0x43,"64"},
    {0x45,"Auto"},
    {0x48,"100"},
    {0x50,"200"},
    {0x58,"400"},
    {0x60,"800"},
    {0x68,"1600"},
    {0x70,"3200"},
    {0x78,"6400"},

    {0,0}, // terminator
};

//
// Av
//
const PSPropertyTableEntry tableAv[] =
{
    {0x08,"1.0"},
    {0x0b,"1.1"},
    {0x0c,"1.2"},
    {0x0d,"1.2 (1/3)"},
    {0x10,"1.4"},
    {0x13,"1.6"},
    {0x14,"1.8"},
    {0x15,"1.8 (1/3)"},
    {0x18,"2.0"},
    {0x1b,"2.2"},
    {0x1c,"2.5"},
    {0x1d,"2.5 (1/3)"},
    {0x20,"2.8"},
    {0x23,"3.2"},
    {0x24,"3.5"},
    {0x25,"3.5 (1/3)"},
    {0x28,"4.0"},
    {0x2b,"4.5 (1/3)"},
    {0x2c,"4.5"},
    {0x2d,"5.6 (1/3)"},
    {0x30,"5.6"},
    {0x33,"6.3"},
    {0x34,"6.7"},
    {0x35,"7.1"},
    {0x38,"8.0"},
    {0x3b,"9.0"},
    {0x3c,"9.5"},
    {0x3d,"10"},
    {0x40,"11"},
    {0x43,"13 (1/3)"},
    {0x44,"13"},
    {0x45,"14"},
    {0x48,"16"},
    {0x4b,"18"},
    {0x4c,"19"},
    {0x4d,"20"},
    {0x50,"22"},
    {0x53,"25"},
    {0x54,"27"},
    {0x55,"29"},
    {0x58,"32"},
    {0x5b,"36"},
    {0x5c,"38"},
    {0x5d,"40"},
    {0x60,"45"},
    {0x63,"51"},
    {0x64,"54"},
    {0x65,"57"},
    {0x68,"64"},
    {0x6b,"72"},
    {0x6c,"76"},
    {0x6d,"81"},
    {0x70,"91"},

    {0,0}, // terminator
};


//
// Tv
//
const PSPropertyTableEntry tableTv[] =
{
    {0x04,"Bulb"},
    {0x10,"30\""},
    {0x13,"25\""},
    {0x14,"20\""},
    {0x15,"20\" (1/3)"},
    {0x18,"15\""},
    {0x1b,"13\""},
    {0x1c,"10\""},
    {0x1d,"10\" (1/3)"},
    {0x20,"8\""},
    {0x23,"6\" (1/3)"},
    {0x24,"6\""},
    {0x25,"5\""},
    {0x28,"4\""},
    {0x2b,"3\"2"},
    {0x2c,"3\""},
    {0x2d,"2\"5"},
    {0x30,"2\""},
    {0x33,"1\"6"},
    {0x34,"1\"5"},
    {0x35,"1\"3"},
    {0x38,"1\""},
    {0x3b,"0\"8"},
    {0x3c,"0\"7"},
    {0x3d,"0\"6"},
    {0x40,"0\"5"},
    {0x43,"0\"4"},
    {0x44,"0\"3"},
    {0x45,"0\"3 (1/3)"},
    {0x48,"1/4"},
    {0x4b,"1/5"},
    {0x4c,"1/6"},
    {0x4d,"1/6 (1/3)"},
    {0x50,"1/8"},
    {0x53,"1/10 (1/3)"},
    {0x54,"1/10"},
    {0x55,"1/13"},
    {0x58,"1/15"},
    {0x5b,"1/20 (1/3)"},
    {0x5c,"1/20"},
    {0x5d,"1/25"},
    {0x60,"1/30"},
    {0x63,"1/40"},
    {0x64,"1/45"},
    {0x65,"1/50"},
    {0x68,"1/60"},
    {0x6b,"1/80"},
    {0x6c,"1/90"},
    {0x6d,"1/100"},
    {0x70,"1/125"},
    {0x73,"1/160"},
    {0x74,"1/180"},
    {0x75,"1/200"},
    {0x78,"1/250"},
    {0x7b,"1/320"},
    {0x7c,"1/350"},
    {0x7d,"1/400"},
    {0x80,"1/500"},
    {0x83,"1/640"},
    {0x84,"1/750"},
    {0x85,"1/800"},
    {0x88,"1/1000"},
    {0x8b,"1/1250"},
    {0x8c,"1/1500"},
    {0x8d,"1/1600"},
    {0x90,"1/2000"},
    {0x93,"1/2500"},
    {0x94,"1/3000"},
    {0x95,"1/3200"},
    {0x98,"1/4000"},

    {0,0}, // terminator
};

//
// White Balance
//
const PSPropertyTableEntry tableWB[] =
{
    {0x00,"+3"},
    {0x03,"+2 (2/3)"},
    {0x04,"+2 (1/2)"},
    {0x05,"+2 (1/3)"},
    {0x08,"+2"},
    {0x0b,"+1 (2/3)"},
    {0x0c,"+1 (1/2)"},
    {0x0d,"+1 (1/3)"},
    {0x10,"+1"},
    {0x13,"+2/3"},
    {0x14,"+1/2"},
    {0x15,"+1/3"},
    {0x18,"0"},
    {0x1b,"-1/3"},
    {0x1c,"-1/2"},
    {0x1d,"-2/3"},
    {0x20,"-1"},
    {0x23,"-1 (1/3)"},
    {0x24,"-1 (1/2)"},
    {0x25,"-1 (2/3)"},
    {0x28,"-2"},
    {0x2b,"-2 (1/3)"},
    {0x2c,"-2 (1/2)"},
    {0x2d,"-2 (2/3)"},
    {0x30,"-3"},

    {0,0}, // terminator
};


const char* PSLookupDesc(const PSPropertyTableEntry * propTable, unsigned long val) {
    for(unsigned int i = 0; propTable[i].desc[0]; i++) {
        if(propTable[i].val == val) {
            return propTable[i].desc;
        }
    }
    return 0;
}

unsigned long PSLookupVal(const PSPropertyTableEntry * propTable, const char* desc) {
    for(unsigned int i = 0; propTable[i].desc[0]; i++) {
        if(strcmp(propTable[i].desc, desc) == 0) {
            return propTable[i].val;
        }
    }
    return 0;
}

#ifdef macintosh
 #if PRAGMA_STRUCT_ALIGN
  #pragma options align=reset
 #endif
#endif


#ifdef _MSC_VER
#	if	900 <= _MSC_VER
	//#	pragma message( "pop, _PR_TYPE_H_PACK_" )
	#	pragma warning( default : 4250 4355 4244 4005)
	#	pragma pack( pop, _PR_TYPE_H_PACK_ )
#	endif	/* 900 <= _MSC_VER */
#elif defined __WATCOMC__
	#pragma pack(__pop);
#endif


#endif
