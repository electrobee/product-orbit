/*
Following tables are defined:
    tableQuality
    tableISO
    tableAEMode
    tableAv
    tableEvfAFMode
    tableExpComp
    tableMeteringMode
    tableTv
    tableWB
    tablePictureStyle
    tableWB
    tablePictureStyle
    tableDriveMode
*/

//
// Size/Quality
//

const PropertyTableEntry tableQuality[] =
{
    // PTP Camera
    {0x00640f0f, "RAW"},
    {0x00640013, "RAW + Large Fine Jpeg"},
    {0x00640113, "RAW + Middle Fine Jpeg"},
    {0x00640213, "RAW + Small Fine Jpeg"},
    {0x00640012, "RAW + Large Normal Jpeg"},
    {0x00640112, "RAW + Middle Normal Jpeg"},
    {0x00640212, "RAW + Small Normal Jpeg"},

    {0x00640010, "RAW + Large Jpeg"},
    {0x00640510, "RAW + Middle1 Jpeg"},
    {0x00640610, "RAW + Middle2 Jpeg"},
    {0x00640210, "RAW + Small Jpeg"},

    {0x01640f0f, "Middle Raw(Small RAW1)"},
    {0x01640013, "Middle Raw(Small RAW1) + Large Fine Jpeg"},
    {0x01640113, "Middle Raw(Small RAW1) + Middle Fine Jpeg"},
    {0x01640213, "Middle Raw(Small RAW1) + Small Fine Jpeg"},
    {0x01640012, "Middle Raw(Small RAW1) + Large Normal Jpeg"},
    {0x01640112, "Middle Raw(Small RAW1) + Middle Normal Jpeg"},
    {0x01640212, "Middle Raw(Small RAW1) + Small Normal Jpeg"},

    {0x01640010, "Middle Raw + Large Jpeg"},
    {0x01640510, "Middle Raw + Middle1 Jpeg"},
    {0x01640610, "Middle Raw + Middle2 Jpeg"},
    {0x01640210, "Middle Raw + Small Jpeg"},

    {0x02640f0f, "Small RAW(Small RAW2)"},
    {0x02640013, "Small RAW(Small RAW2) + Large Fine Jpeg"},
    {0x02640113, "Small RAW(Small RAW2) + Middle Fine Jpeg"},
    {0x02640213, "Small RAW(Small RAW2) + Small Fine Jpeg"},
    {0x02640012, "Small RAW(Small RAW2) + Large Normal Jpeg"},
    {0x02640112, "Small RAW(Small RAW2) + Middle Normal Jpeg"},
    {0x02640212, "Small RAW(Small RAW2) + Small Normal Jpeg"},

    {0x02640010, "Small RAW + Large Jpeg"},
    {0x02640510, "Small RAW + Middle1 Jpeg"},
    {0x02640610, "Small RAW + Middle2 Jpeg"},
    {0x02640210, "Small RAW + Small Jpeg"},

    {0x00130f0f, "Large Fine Jpeg"},
    {0x00120f0f, "Large Normal Jpeg"},
    {0x01130f0f, "Middle Fine Jpeg"},
    {0x01120f0f, "Middle Normal Jpeg"},
    {0x02130f0f, "Small Fine Jpeg"},
    {0x02120f0f, "Small Normal Jpeg"},

    {0x00100f0f, "Large Jpeg"},
    {0x05100f0f, "Middle1 Jpeg"},
    {0x06100f0f, "Middle2 Jpeg"},
    {0x02100f0f, "Small Jpeg"},

    // Legacy Camera
    {0x00240000, "RAW"},
    {0x00240013, "RAW + Large Fine Jpeg"},
    {0x00240113, "RAW + Middle Fine Jpeg"},
    {0x00240213, "RAW + Small Fine Jpeg"},
    {0x00240012, "RAW + Large Normal Jpeg"},
    {0x00240112, "RAW + Middle Normal Jpeg"},
    {0x00240212, "RAW + Small Normal Jpeg"},
    {0x00130000, "Large Fine Jpeg"},
    {0x01130000, "Middle Fine Jpeg"},
    {0x02130000, "Small Fine Jpeg"},
    {0x00120000, "Large Normal Jpeg"},
    {0x01120000, "Middle Normal Jpeg"},
    {0x02120000, "Small Normal Jpeg"},

    {0x002f000f, "RAW"},
    {0x002f001f, "RAW + Large Jpeg"},
    {0x002f051f, "RAW + Middle1 Jpeg"},
    {0x002f061f, "RAW + Middle2 Jpeg"},
    {0x002f021f, "RAW + Small Jpeg"},
    {0x001f000f, "Large Jpeg"},
    {0x051f000f, "Middle1 Jpeg"},
    {0x061f000f, "Middle2 Jpeg"},
    {0x021f000f, "Small Jpeg"},

    {0,0}, // terminator
};

//
// ISO
//
const PropertyTableEntry tableISO[] =
{
    {0x00,"Auto"},
    {0x28,"6"},
    {0x30,"12"},
    {0x38,"25"},
    {0x40,"50"},
    {0x48,"100"},
    {0x4b,"125"},
    {0x4d,"160"},
    {0x50,"200"},
    {0x53,"250"},
    {0x55,"320"},
    {0x58,"400"},
    {0x5b,"500"},
    {0x5d,"640"},
    {0x60,"800"},
    {0x63,"1000"},
    {0x65,"1250"},
    {0x68,"1600"},
    {0x70,"3200"},
    {0x78,"6400"},
    {0x80,"12800"},
    {0x88,"25600"},
    {0x90,"51200"},
    {0x98,"102400"},
    {0xffffffff,"Unknown"},

    {0,0}, // terminator
};

//
// AE Mode
//
const PropertyTableEntry tableAEMode[] =
{
    {0,"Program"},
    {1,"Tv"},
    {2,"Av"},
    {3,"Manual"},
    {4,"Bulb"},
    {5,"A-DEP"},
    {6,"DEP"},
    {7,"Custom"},
    {8,"Lock"},
    {9,"Auto"},
    {10,"Night Portrait"},
    {11,"Sports"},
    {12,"Portrait"},
    {13,"Landscape"},
    {14,"Close Up"},
    {15,"No Flash"},
    {19,"Creative Auto"},
    {20,"Movie"},
    {21,"Photo in Movie"},
    {0xffffffff,"Unknown"},

    {0,0}, // terminator
};

//
// Av
//
const PropertyTableEntry tableAv[] =
{
    {0x00,"Auto"},
    {0x08,"1"},
    {0x0B,"1.1"},
    {0x0C,"1.2"},
    {0x0D,"1.2"},
    {0x10,"1.4"},
    {0x13,"1.6"},
    {0x14,"1.8"},
    {0x15,"1.8"},
    {0x18,"2"},
    {0x1B,"2.2"},
    {0x1C,"2.5"},
    {0x1D,"2.5"},
    {0x20,"2.8"},
    {0x23,"3.2"},
    {0x24,"3.5"},
    {0x25,"3.5"},
    {0x28,"4"},
    {0x2B,"4.5"},
    {0x2C,"4.5"},
    {0x2D,"5.0"},
    {0x30,"5.6"},
    {0x33,"6.3"},
    {0x34,"6.7"},
    {0x35,"7.1"},
    {0x38,"8"},
    {0x3B,"9"},
    {0x3C,"9.5"},
    {0x3D,"10"},
    {0x40,"11"},
    {0x43,"13"},
    {0x44,"13"},
    {0x45,"14"},
    {0x48,"16"},
    {0x4B,"18"},
    {0x4C,"19"},
    {0x4D,"20"},
    {0x50,"22"},
    {0x53,"25"},
    {0x54,"27"},
    {0x55,"29"},
    {0x58,"32"},
    {0x5B,"36"},
    {0x5C,"38"},
    {0x5D,"40"},
    {0x60,"45"},
    {0x63,"51"},
    {0x64,"54"},
    {0x65,"57"},
    {0x68,"64"},
    {0x6B,"72"},
    {0x6C,"76"},
    {0x6D,"80"},
    {0x70,"91"},
    {0xffffffff,"Unknown"},

    {0,0}, // terminator
};

//
// Evf AF Mode
//
const PropertyTableEntry tableEvfAFMode[] =
{
    {0x00,"Quick mode"},
    {0x01,"Live mode"},
    {0x02,"Live face detection mode"},
    {0xffffffff,"Unknown"},

    {0,0}, // terminator
};

//
// Exposure Compensation
//
const PropertyTableEntry tableExpComp[] =
{
    {0x18,"+3"},
    {0x15,"+2 2/3"},
    {0x14,"+2 1/2"},
    {0x13,"+2 1/3"},
    {0x10,"+2"},
    {0x0d,"+1 2/3"},
    {0x0c,"+1 1/2"},
    {0x0b,"+1 1/3"},
    {0x08,"+1"},
    {0x05,"+2/3"},
    {0x04,"+1/2"},
    {0x03,"+1/3"},
    {0x00,"0"},
    {0xfd,"-1/3"},
    {0xfc,"-1/2"},
    {0xfb,"-2/3"},
    {0xf8,"-1"},
    {0xf5,"-1 1/3"},
    {0xf4,"-1 1/2"},
    {0xf3,"-1 2/3"},
    {0xf0,"-2"},
    {0xed,"-2 1/3"},
    {0xec,"-2 1/2"},
    {0xeb,"-2 2/3"},
    {0xe8,"-3"},
    {0xffffffff,"Unknown"},

    {0,0}, // terminator
};

//
// Metering Mode
//
const PropertyTableEntry tableMeteringMode[] =
{
    {1,"Spot"},
    {3,"Evaluative"},
    {4,"Partial"},
    {5,"Center-Weighted Average"},
    {0xffffffff,"Unknown"},

    {0,0}, // terminator
};

//
// Tv
//
const PropertyTableEntry tableTv[] =
{
    {0x00,"Auto"},
    {0x0c,"Bulb"},
    {0x10,"30―""},
    {0x13,"25―""},
    {0x14,"20―""},
    {0x15,"20―""},     // when step in custom function set to 1/3
    {0x18,"15―""},
    {0x1B,"13―""},
    {0x1C,"10―""},
    {0x1D,"10―""},     // when step in custom function set to 1/3
    {0x20,"8―""},
    {0x23,"6―""},      // when step in custom function set to 1/3
    {0x24,"6―""},
    {0x25,"5―""},
    {0x28,"4―""},
    {0x2B,"3―"2"},
    {0x2C,"3―""},
    {0x2D,"2―"5"},
    {0x30,"2―""},
    {0x33,"1―"6"},
    {0x34,"1―"5"},
    {0x35,"1―"3"},
    {0x38,"1―""},
    {0x3B,"0―"8"},
    {0x3C,"0―"7"},
    {0x3D,"0―"6"},
    {0x40,"0―"5"},
    {0x43,"0―"4"},
    {0x44,"0―"3"},
    {0x45,"0―"3"},     // when step in custom function set to 1/3
    {0x48,"1/4"},
    {0x4B,"1/5"},
    {0x4C,"1/6"},
    {0x4D,"1/6"},      // when step in custom function set to 1/3
    {0x50,"1/8"},
    {0x53,"1/10"},     // when step in custom function set to 1/3
    {0x54,"1/10"},
    {0x55,"1/13"},
    {0x58,"1/15"},
    {0x5B,"1/20"},     // when step in custom function set to 1/3
    {0x5C,"1/20"},
    {0x5D,"1/25"},
    {0x60,"1/30"},
    {0x63,"1/40"},
    {0x64,"1/45"},
    {0x65,"1/50"},
    {0x68,"1/60"},
    {0x6B,"1/80"},
    {0x6C,"1/90"},
    {0x6D,"1/100"},
    {0x70,"1/125"},
    {0x73,"1/160"},
    {0x74,"1/180"},
    {0x75,"1/200"},
    {0x78,"1/250"},
    {0x7B,"1/320"},
    {0x7C,"1/350"},
    {0x7D,"1/400"},
    {0x80,"1/500"},
    {0x83,"1/640"},
    {0x84,"1/750"},
    {0x85,"1/800"},
    {0x88,"1/1000"},
    {0x8B,"1/1250"},
    {0x8C,"1/1500"},
    {0x8D,"1/1600"},
    {0x90,"1/2000"},
    {0x93,"1/2500"},
    {0x94,"1/3000"},
    {0x95,"1/3200"},
    {0x98,"1/4000"},
    {0x9B,"1/5000"},
    {0x9C,"1/6000"},
    {0x9D,"1/6400"},
    {0xA0,"1/8000"},
    {0xffffffff,"Unknown"},

    {0,0}, // terminator
};

//
// White Balance
//
const PropertyTableEntry tableWB[] =
{
    {0,"Auto"},
    {1,"Daylight"},
    {2,"Cloudy"},
    {3,"Tungsten"},
    {4,"Fluorescent"},
    {5,"Strobe"},
    {6,"Manual"},
    {8,"Shade"},
    {9,"Color Temperature"},
    {10,"PC-1"},
    {11,"PC-2"},
    {12,"PC-3"},
	{15,"Manual 2"},
	{16,"Manual 3"},
	{18,"Manual 4"},
	{19,"Manual 5"},
    {20,"PC-4"},
    {21,"PC-5"},
    /*
    {-1,"Click"},
    {-2,"Pasted"},
    */

    {0,0}, // terminator
};

//
// Picture Style
//
const PropertyTableEntry tablePictureStyle[] =
{
    {0x0081,"Standard"},
    {0x0082,"Portrait"},
    {0x0083,"Landscape"},
    {0x0084,"Neutral"},
    {0x0085,"Faithful"},
    {0x0086,"Monochrome"},
    {0x0021,"User1"},
    {0x0022,"User2"},
    {0x0023,"User3"},
    {0x0041,"PC1"},
    {0x0042,"PC2"},
    {0x0043,"PC3"},

    {0,0}, // terminator
};

//
// Drive Mode
//
const PropertyTableEntry tableDriveMode[] =
{
    {0x00,"Single-Frame"},
    {0x01,"Continuous"},
    {0x02,"Video"},
    {0x04,"HS Continuous"},
    {0x05,"LS Continuous"},
    {0x06,"Silent Single"},
    {0x07,"10s Self-Timer + Continuous"},
    {0x10,"10s Self-Timer"},
    {0x11,"2s Self-Timer"},

    {0,0}, // terminator
};


const char* LookupDesc(const PropertyTableEntry * propTable, EdsUInt32 val) {
    for(unsigned int i = 0; propTable[i].desc[0]; i++) {
        if(propTable[i].val == val) {
            return propTable[i].desc;
        }
    }
    return 0;
}

EdsUInt32 LookupVal(const PropertyTableEntry * propTable, const char* desc) {
    for(unsigned int i = 0; propTable[i].desc[0]; i++) {
        if(strcmp(propTable[i].desc, desc) == 0) {
            return propTable[i].val;
        }
    }
    return 0;
}

