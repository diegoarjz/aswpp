#include <gtest/gtest.h>

#include <aswpp/script_engine.h>

using namespace aswpp;

namespace {
float test_free_function(float val) { return val * 2; }
} // namespace

TEST(FunctionRegisterTeset, should_be_able_to_register_function) {
  Engine e;
  EXPECT_TRUE(
      e.Register("float test_free_function(float)", test_free_function));
}

TEST(FunctionRegisterTeset, should_be_able_to_call_registerd_function) {
  Engine e;
  e.Register("float test_free_function(float)", test_free_function);

  const float v = 2;
  float ret = 0;
  e.CreateModule("test", R"(
  float main(float value) {
    return test_free_function(value);
  }
  )");
  e.Run("test", "float main(float)", &ret, v);

  EXPECT_EQ(ret, 4);
}
