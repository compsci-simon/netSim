#include "hosts/router.h"
#include "hosts/switch.h"

int main() {
  Router* r = new Router();
  Switch s {r};
  return 0;
}