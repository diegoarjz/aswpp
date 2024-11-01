#include <gtest/gtest.h>

#include "aswpp/script_engine.h"

TEST(ScriptModuleTest, construct_with_name_and_source) {
  aswpp::Module m("name", "source");
  EXPECT_EQ(m.GetName(), "name");
  EXPECT_EQ(m.GetSource(), "source");
}

TEST(ScriptModuleTest, attach_to_engine) {
  const std::string script = R"(
  void main() {
  }
)";

  aswpp::ModulePtr m = std::make_shared<aswpp::Module>("test", script);
  aswpp::Engine e;

  EXPECT_TRUE(e.Attach(m));
}

TEST(ScriptModuleTest, dont_allow_a_module_to_be_attached_to_multiple_engines) {
  const std::string script = R"(
  void main() {
  }
)";

  aswpp::ModulePtr m = std::make_shared<aswpp::Module>("test", script);
  aswpp::Engine e1;
  aswpp::Engine e2;

  EXPECT_TRUE(e1.Attach(m));
  EXPECT_FALSE(e1.Attach(m));
}

TEST(ScriptModuleTest, should_report_compilation_errors) {
  const std::string script = R"(
  void main( {
  }
)";

  aswpp::ModulePtr m = std::make_shared<aswpp::Module>("test", script);
  aswpp::Engine e;

  EXPECT_FALSE(e.Attach(m));
}

//
TEST(ScriptModuleTest, getting_global_var_should_return_true_if_var_exists) {
  const std::string script = R"(
  int value = 0;
)";

  aswpp::ModulePtr m = std::make_shared<aswpp::Module>("test", script);
  aswpp::Engine e;
  e.Attach(m);

  EXPECT_TRUE(m->GetGlobalVar<int>("value", nullptr));
}

TEST(ScriptModuleTest, getting_global_var_should_be_able_to_retrive_value) {
  const std::string script = R"(
  int value = 123;
)";

  aswpp::ModulePtr m = std::make_shared<aswpp::Module>("test", script);
  aswpp::Engine e;
  e.Attach(m);

  int value = 0;
  m->GetGlobalVar<int>("value", &value);
  EXPECT_EQ(value, 123);
}

TEST(ScriptModuleTest,
     getting_global_var_should_return_false_if_var_doesnt_exist) {
  const std::string script = R"(
  int value = 0;
)";

  aswpp::ModulePtr m = std::make_shared<aswpp::Module>("test", script);
  aswpp::Engine e;
  e.Attach(m);

  EXPECT_FALSE(m->GetGlobalVar<int>("values", nullptr));
}
