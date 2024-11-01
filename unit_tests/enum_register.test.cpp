#include <gtest/gtest.h>

#include "aswpp/script_engine.h"

using namespace aswpp;

enum class TestEnum { value1, value2 };

TEST(EnumRegisterTest, should_be_able_to_register_an_enum) {
  Engine e;
  EXPECT_TRUE(e.RegisterEnum<TestEnum>("TestEnum"));
}

TEST(EnumRegisterTest,
     should_not_be_able_to_register_an_enum_with_duplicate_name) {
  enum class TestOtherEnum { value1, value2 };
  Engine e;
  EXPECT_TRUE(e.RegisterEnum<TestEnum>("TestEnum"));
  EXPECT_FALSE(e.RegisterEnum<TestOtherEnum>("TestEnum"));
}

TEST(EnumRegisterTest, should_be_able_to_use_enum) {
  Engine e;
  e.RegisterEnum<TestEnum>("TestEnum");

  const TestEnum val = TestEnum::value1;
  TestEnum ret = TestEnum::value1;
  e.CreateModule("test", R"(
  TestEnum main(TestEnum value) {
    if (value == TestEnum::value1) {
      return TestEnum::value2;
    }
    return TestEnum::value1;
  }
  )");
  e.Run<TestEnum>("test", "TestEnum main(TestEnum)", &ret, val);
  EXPECT_EQ(ret, TestEnum::value2);
}
