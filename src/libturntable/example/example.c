#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <turntable.h>

void help() {
  printf(
  "Usage:\n\texample [command] [command]...\n\n"
  "Commands: \n"
  "\t--get-position\t\tGet current position\n"
  "\t--get-target\t\tGet current target\n"
  "\t--set-position=1234\tSet current position\n"
  "\t--set-target=1234\tSet target position\n"
  "\t--rel-target=1234\tRotate relatively to current position\n"
  "\t--set-v0=123\t\tSet initial speed (0..255)\n"
  "\t--set-vmax=123\t\tSet maximum speed (0..255)\n"
  "\t--stop=123\t\tStop immediately\n"
  "\t--sleep=1234\t\tSleep (ms)\n\n"
  "Example:\n"
  "\texample --get-position --rel-target=10 --get-position\n"
  );
}

int main(int argc, char ** argv) {
  int i;

  if(argc <= 1) {
    help();
  }

  if(argc >= 2) {
    ttOpen();
    for(i = 1; i < argc; i++) {
      if(strstr(argv[i], "set") && strchr(argv[i], '=') == 0) {
        printf("Invalid argument: %s\n", argv[i]);
        continue; // argument must contain '='
      }
      if(strstr(argv[i], "get-position")) {
        int16_t position = ttGetPosition();
        if(ttError() == TT_OK) {
          printf("Position: %i\n", position);
        }
      } else if(strstr(argv[i], "get-target")) {
        int16_t target = ttGetTarget();
        if(ttError() == TT_OK) {
          printf("Target: %i\n", target);
        }
      } else if(strstr(argv[i], "set-position")) {
        int16_t value = atoi(strchr(argv[i], '=') + 1);
        printf("Set Position: %i\n", value);
        ttSetPosition(value);
      } else if(strstr(argv[i], "set-target")) {
        int16_t value = atoi(strchr(argv[i], '=') + 1);
        printf("Set Target: %i\n", value);
        ttSetTarget(value);
      } else if(strstr(argv[i], "rel-target")) {
        int16_t value = atoi(strchr(argv[i], '=') + 1);
        int16_t position = ttGetPosition();
        if(ttError() == TT_OK) {
          printf("Set Target: %i + (%i) = %i\n", position, value, position + value);
          ttSetTarget(position + value);
        }
      } else if(strstr(argv[i], "set-v0")) {
        uint8_t value = atoi(strchr(argv[i], '=') + 1);
        printf("Set V0: %i\n", value);
        ttSetV0(value);
      } else if(strstr(argv[i], "set-vmax")) {
        uint8_t value = atoi(strchr(argv[i], '=') + 1);
        printf("Set Vmax: %i\n", value);
        ttSetVmax(value);
      } else if(strstr(argv[i], "stop")) {
        printf("Stop\n");
        ttStop();
      } else if(strstr(argv[i], "sleep")) {
        int value = atoi(strchr(argv[i], '=') + 1);
        usleep(1000 * value);
      } else {
        printf("Unknown argument: %s\n", argv[i]);
      }
    }

    if(ttError() == TT_NOT_CONNECTED) {
      printf("Turntable not connected.\n");
    } else if(ttError() == TT_NOT_RESPONDING) {
      printf("Turntable not responding.\n");
    } else if(ttError() != TT_OK) {
      printf("Unknown error\n");
    }
    ttClearError();
  }

  ttClose();
  return 0;
}

