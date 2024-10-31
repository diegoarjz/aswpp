#include <gtest/gtest.h>

#include "aswpp/script_engine.h"

using namespace aswpp;

class TestClass2 {
public:
  float value() const { return m_value; }

  float m_value{221.0};
};

class TestClass {
public:
  TestClass() {
    ++ctorCount;
  }
  TestClass(float value) : m_value{value} {}

  void setValue(float val) { m_value = val; }
  float value() const { return m_value; }

  TestClass2 getObject() const {
    return TestClass2{};
  }

  TestClass2 getObjectWithValue(float value) const {
    TestClass2 t2;
    t2.m_value = value;
    return t2;
  }

  TestClass2 getObjectWithMultipleValues(float value1, float value2) {
    TestClass2 t2;
    t2.m_value = value1 + value2;
    return t2;
  }

  float setValueWithObject(TestClass2 t) {
    return t.m_value;
  }

  float setValueWithReferenceObject(TestClass2& t) {
    return t.m_value;
  }

  float m_value{123.0f};

  static int ctorCount;
};
int TestClass::ctorCount = 0;

class ClassRegisterTest : public ::testing::Test {
public:
  void SetUp() override {
    e.RegisterClass<TestClass2>("TestClass2")
      .RegisterObjectMethod("float value() const", &TestClass2::value)
      .RegisterObjectProperty("float m_value", &TestClass2::m_value);
    e.RegisterClass<TestClass>("TestClass")
      .RegisterObjectMethod("float value() const", &TestClass::value)
      .RegisterObjectMethod("void setValue(float)", &TestClass::setValue)
      .RegisterValueObjectMethod("TestClass2@ getObject()", &TestClass::getObject)
      .RegisterValueObjectMethod("TestClass2@ getObjectWithValue(float) const", &TestClass::getObjectWithValue)
      .RegisterValueObjectMethod("TestClass2@ getObjectWithMultipleValues(float, float)", &TestClass::getObjectWithMultipleValues)
      .RegisterValueObjectMethod("float setValueWithObject(const TestClass2@)", &TestClass::setValueWithObject)
      //.RegisterValueObjectMethod("float setValueWithReferenceObject(const TestClass2@)", &TestClass::setValueWithReferenceObject)
      .RegisterObjectProperty("float m_value", &TestClass::m_value)
      ;
    e.CreateModule("test", R"(
    void main() {
      TestClass t;
    }

    float call_method() {
      TestClass t;
      return t.value();
    }

    float get_value() {
      TestClass t;
      return t.m_value;
    }

    float set_value() {
      TestClass t;
      t.setValue(321);
      return t.value();
    }

    float call_method_return_object() {
      TestClass t;
      return t.getObject().value();
    }

    float call_method_return_object_with_arg() {
      TestClass t;
      return t.getObjectWithValue(456).value();
    }

    float call_method_return_object_with_multiple_args() {
      TestClass t;
      return t.getObjectWithMultipleValues(100, 205).value();
    }

    float set_value_with_object() {
      TestClass t;
      TestClass2 t2;
      t2.m_value = 546;
      t.setValueWithObject(t2);
      return t.value();
    }
    )");
  }

  Engine e;
};

TEST_F(ClassRegisterTest, should_be_able_to_register) {
  TestClass::ctorCount = 0;
  e.Run("test", "void main(void)");
  EXPECT_EQ(TestClass::ctorCount, 1);
}

TEST_F(ClassRegisterTest, should_be_able_to_call_methods) {
  float value = 0.0f;
  e.Run("test", "float call_method(void)", &value);
  EXPECT_EQ(value, 123.0f);
}

TEST_F(ClassRegisterTest, should_be_able_to_call_methods_with_args) {
  float value = 0.0f;
  e.Run("test", "float set_value()", &value);
  EXPECT_EQ(value, 321.0f);
}

TEST_F(ClassRegisterTest, should_be_able_to_call_methods_that_return_objects) {
  float value = 0.0f;
  e.Run("test", "float call_method_return_object()", &value);
  EXPECT_EQ(value, 221.0f);
}

TEST_F(ClassRegisterTest, should_be_able_to_call_methods_with_args_that_return_objects) {
  float value = 0.0f;
  e.Run("test", "float call_method_return_object_with_arg()", &value);
  EXPECT_EQ(value, 456.0f);
}

TEST_F(ClassRegisterTest, should_be_able_to_call_methods_with_multiple_args_that_return_objects) {
  float value = 0.0f;
  e.Run("test", "float call_method_return_object_with_multiple_args()", &value);
  EXPECT_EQ(value, 305);
}

TEST_F(ClassRegisterTest, should_be_able_to_get_data_members) {
  float value = 0.0f;
  e.Run("test", "float get_value(void)", &value);
  EXPECT_EQ(value, 123.0f);
}

TEST_F(ClassRegisterTest, should_be_able_to_call_methods_with_objects) {
  float value = 0.0f;
  e.Run("test", "float set_value_with_object(void)", &value);
  EXPECT_EQ(value, 123.0f);
}
