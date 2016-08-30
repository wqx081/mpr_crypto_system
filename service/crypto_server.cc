#include "service/crypto_server.h"

int main(int argc, char** argv) {
  (void) argc;
  (void) argv;

  crypto::CryptoServer server("localhost", 50052);
  server.Run();
  return 0;
}
