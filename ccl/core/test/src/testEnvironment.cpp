#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/env/cclEnvironment.h"

TEST(UTEnvironment, Version) {
  EXPECT_FALSE(std::empty(ccl::Environment::VersionInfo()));
}

TEST(UTEnvironment, RuntimeInjection) {
  auto anotherSrc = std::make_unique<ccl::SourceManager>();
  const auto* srcAddr = anotherSrc.get();
  EXPECT_NE(&ccl::Environment::Sources(), srcAddr);

  ccl::Environment::Instance().SetSourceManager(std::move(anotherSrc));

  EXPECT_EQ(&ccl::Environment::Sources(), srcAddr);
}