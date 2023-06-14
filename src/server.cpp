#include "router.h"

int main() {
  Router router;
  router.accept_connections();
  router.handleConnection();
  return 0;
}