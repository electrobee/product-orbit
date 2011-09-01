#pragma once

#define VER     "1.1.4b"

#ifdef __WXDEBUG__
#define VERSION VER " DEBUG"
#else
#define VERSION VER
#endif
