#include <gtest/gtest.h>

#include "aswpp/script_engine.h"

using namespace aswpp;

TEST(ScriptEngineTest, is_valid_on_construction) {
  Engine e;
  EXPECT_TRUE(e.IsValid());
}

TEST(ScriptEngineTest, run_function_no_args_no_return) {
  const std::string script = R"(
  int value = 0;
  void main() {
    value = 1;
  }
)";

  aswpp::ModulePtr m = std::make_shared<aswpp::Module>("test", script);
  aswpp::Engine e;

  e.Attach(m);

  EXPECT_TRUE(e.Run("test", "void main()"));

  int value = 0;
  m->GetGlobalVar("value", &value);
  EXPECT_EQ(value, 1);
}

TEST(ScriptEngineTest, run_function_args_no_return) {
  const std::string script = R"(
  int value = 0;
  void main(int v) {
    value = v;
  }
)";

  aswpp::ModulePtr m = std::make_shared<aswpp::Module>("test", script);
  aswpp::Engine e;

  e.Attach(m);

  int v = 1;
  EXPECT_TRUE(e.Run("test", "void main(int)", v));

  int value = 0;
  m->GetGlobalVar("value", &value);
  EXPECT_EQ(value, 1);
}

class ScriptEngineSetArgsTest : public testing::Test {
public:
  void SetUp() override {
    m_module = std::make_shared<aswpp::Module>("test", script);
    m_engine.Attach(m_module);
  }

  const std::string script = R"(
  int64 int64Value = 0;
  void main(int64 v) {
    int64Value = v;
  }

  uint64 uint64Value = 0;
  void main(uint64 v) {
    uint64Value = v;
  }

  int intValue = 0;
  void main(int v) {
    intValue = v;
  }

  uint uintValue = 0;
  void main(uint v) {
    uintValue = v;
  }

  int16 shortValue = 0;
  void main(int16 v) {
    shortValue = v;
  }

  uint16 ushortValue = 0;
  void main(uint16 v) {
    ushortValue = v;
  }

  int8 byteValue = 0;
  void main(int8 v) {
    byteValue = v;
  }

  uint8 ubyteValue = 0;
  void main(uint8 v) {
    ubyteValue = v;
  }

  float floatValue = 0;
  void main(float v) {
    floatValue = v;
  }

  double doubleValue = 0;
  void main(double v) {
    doubleValue = v;
  }

  bool boolValue = false;
  void main(bool v) {
    boolValue = v;
  }

)";

  aswpp::Engine m_engine;
  aswpp::ModulePtr m_module;
};

template <typename T>
void testSettingFunctionArg(aswpp::Engine &engine, aswpp::ModulePtr module,
                            const std::string &func, const std::string &value) {
  T v = 1;
  EXPECT_TRUE(engine.Run("test", func, v));
  T outValue = 0;
  module->GetGlobalVar(value, &outValue);
  EXPECT_EQ(outValue, 1);
}

TEST_F(ScriptEngineSetArgsTest, set_long_arg) {
  testSettingFunctionArg<int64_t>(m_engine, m_module, "void main(int64)",
                                  "int64Value");
}

TEST_F(ScriptEngineSetArgsTest, set_ulong_arg) {
  testSettingFunctionArg<uint64_t>(m_engine, m_module, "void main(uint64)",
                                   "uint64Value");
}

TEST_F(ScriptEngineSetArgsTest, set_int_arg) {
  testSettingFunctionArg<int32_t>(m_engine, m_module, "void main(int)",
                                  "intValue");
}

TEST_F(ScriptEngineSetArgsTest, set_uint_arg) {
  testSettingFunctionArg<uint32_t>(m_engine, m_module, "void main(uint)",
                                   "uintValue");
}

TEST_F(ScriptEngineSetArgsTest, set_short_arg) {
  testSettingFunctionArg<int16_t>(m_engine, m_module, "void main(int16)",
                                  "shortValue");
}

TEST_F(ScriptEngineSetArgsTest, set_ushort_arg) {
  testSettingFunctionArg<uint16_t>(m_engine, m_module, "void main(uint16)",
                                   "ushortValue");
}

TEST_F(ScriptEngineSetArgsTest, set_byte_arg) {
  testSettingFunctionArg<int8_t>(m_engine, m_module, "void main(int8)",
                                 "byteValue");
}

TEST_F(ScriptEngineSetArgsTest, set_ubyte_arg) {
  testSettingFunctionArg<uint8_t>(m_engine, m_module, "void main(uint8)",
                                  "ubyteValue");
}

TEST_F(ScriptEngineSetArgsTest, set_float_arg) {
  testSettingFunctionArg<float>(m_engine, m_module, "void main(float)",
                                "floatValue");
}

TEST_F(ScriptEngineSetArgsTest, set_double_arg) {
  testSettingFunctionArg<double>(m_engine, m_module, "void main(double)",
                                 "doubleValue");
}

TEST_F(ScriptEngineSetArgsTest, set_bool_arg) {
  bool v = false;
  bool newVal = true;
  EXPECT_TRUE(m_engine.Run("test", "void main(bool)", newVal));
  bool outValue = 0;
  m_module->GetGlobalVar("boolValue", &outValue);
  EXPECT_EQ(outValue, 1);
}

//----------------------------------------

class ScriptEngineGetReturnValueTest : public testing::Test {
public:
  void SetUp() override {
    m_module = std::make_shared<aswpp::Module>("test", script);
    m_engine.Attach(m_module);
  }

  const std::string script = R"(
  int64 returnInt64() { return 1; }
  uint64 returnUInt64() { return 2; }
  int returnInt() { return 3; }
  uint returnUInt() { return 4; }
  int16 returnInt16() { return 5; }
  uint16 returnUInt16() { return 6; }
  int8 returnInt8() { return 7; }
  uint8 returnUInt8() { return 8; }
  float returnFloat() { return 9.1; }
  double returnDouble() { return 10.1; }
  bool returnBool() { return true; }
)";

  aswpp::Engine m_engine;
  aswpp::ModulePtr m_module;
};

template <typename T>
void testGettingReturnValue(aswpp::Engine &engine, aswpp::ModulePtr module,
                            const std::string &func, T v) {
  T value{};
  EXPECT_TRUE(engine.Run("test", func, &value));
  EXPECT_EQ(value, v);
}

TEST_F(ScriptEngineGetReturnValueTest, test_get_int) {
  testGettingReturnValue<int32_t>(m_engine, m_module, "int returnInt()", 3);
}

TEST_F(ScriptEngineGetReturnValueTest, test_get_uint) {
  testGettingReturnValue<uint32_t>(m_engine, m_module, "uint returnUInt()", 4);
}

TEST_F(ScriptEngineGetReturnValueTest, test_get_int16) {
  testGettingReturnValue<int16_t>(m_engine, m_module, "int16 returnInt16()", 5);
}

TEST_F(ScriptEngineGetReturnValueTest, test_get_uint16) {
  testGettingReturnValue<uint16_t>(m_engine, m_module, "uint16 returnUInt16()",
                                   6);
}

TEST_F(ScriptEngineGetReturnValueTest, test_get_int8) {
  testGettingReturnValue<int8_t>(m_engine, m_module, "int8 returnInt8()", 7);
}

TEST_F(ScriptEngineGetReturnValueTest, test_get_uint8) {
  testGettingReturnValue<uint8_t>(m_engine, m_module, "uint8 returnUInt8()", 8);
}

TEST_F(ScriptEngineGetReturnValueTest, test_get_int64) {
  testGettingReturnValue<int8_t>(m_engine, m_module, "int64 returnInt64()", 1);
}

TEST_F(ScriptEngineGetReturnValueTest, test_get_uint64) {
  testGettingReturnValue<uint8_t>(m_engine, m_module, "uint64 returnUInt64()",
                                  2);
}

TEST_F(ScriptEngineGetReturnValueTest, test_get_float) {
  testGettingReturnValue<float>(m_engine, m_module, "float returnFloat()", 9.1);
}

TEST_F(ScriptEngineGetReturnValueTest, test_get_double) {
  testGettingReturnValue<double>(m_engine, m_module, "double returnDouble()",
                                 10.1);
}
TEST_F(ScriptEngineGetReturnValueTest, test_get_bool) {
  testGettingReturnValue<bool>(m_engine, m_module, "bool returnBool()",
                               true);
}

//----------------------------------------

class ScriptEngineTestEnumValues : public testing::Test {
public:
  void SetUp() override {
    m_engine.RegisterEnum<TestEnum>("TestEnum");
    m_module = std::make_shared<aswpp::Module>("test", script);
    m_engine.Attach(m_module);
  }

  const std::string script = R"(
  TestEnum returnValue1() { return TestEnum::Value1; }
  TestEnum returnValue2() { return TestEnum::Value2; }
)";

  aswpp::Engine m_engine;
  aswpp::ModulePtr m_module;

  enum class TestEnum { Value1, Value2 };
};

TEST_F(ScriptEngineTestEnumValues, test_get_enum_value1) {
  testGettingReturnValue<TestEnum>(m_engine, m_module,
                                   "TestEnum returnValue1()", TestEnum::Value1);
}

TEST_F(ScriptEngineTestEnumValues, test_get_enum_value) {
  testGettingReturnValue<TestEnum>(m_engine, m_module,
                                   "TestEnum returnValue2()", TestEnum::Value2);
}
