#pragma once

#define VER     "1.2.1"

#ifdef __WXDEBUG__
#define VERSION VER " DEBUG"
#else
#define VERSION VER
#endif
