#pragma once

#define VER     "1.1.4"

#ifdef __WXDEBUG__
#define VERSION VER " DEBUG"
#else
#define VERSION VER
#endif
