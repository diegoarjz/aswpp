#include <gtest/gtest.h>

#include <aswpp/script_engine.h>

using namespace aswpp;

namespace {
float test_free_function(float val) { return val * 2; }
std::string test_free_function_string_arg(std::string val) { return val; }
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


TEST(FunctionRegisterTeset, should_be_able_to_register_function_string_arg) {
  Engine e;
  EXPECT_TRUE(
      e.Register("string test_free_function_string_arg(string)", test_free_function_string_arg));
}

TEST(FunctionRegisterTeset, should_be_able_to_call_registerd_function_string_arg) {
  Engine e;
  e.Register("string test_free_function_string_arg(string)", test_free_function_string_arg);

  std::string ret;
  e.CreateModule("test", R"(
  string main() {
    return test_free_function_string_arg("abcd");
  }
  )");
  e.Run("test", "string main()", &ret);

  EXPECT_EQ(ret, "abcd");
}
