#pragma once

#define VER     "1.2.0"

#ifdef __WXDEBUG__
#define VERSION VER " DEBUG"
#else
#define VERSION VER
#endif
