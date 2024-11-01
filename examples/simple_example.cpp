#include "aswpp/script_engine.h"

#include <iostream>

int main(int argc, char *argv[]) {
  const std::string script = R"(
    float doubleTheValue(float value) {
        return value * 2.0;
    }
)";

  aswpp::Engine engine;
  engine.CreateModule("main", script);

  float returnValue;
  float arg = 2.0f;
  engine.Run("main", "float doubleTheValue(float)", &returnValue, arg);

  std::cout << "Return Value: " << returnValue << std::endl;

  return 1;
}
