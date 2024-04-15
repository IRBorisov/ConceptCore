#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/rslang/ErrorLogger.h"

class UTLogger : public ::testing::Test {
protected:
  using ErrorStatus = ccl::rslang::ErrorStatus;
  using Error = ccl::rslang::Error;

  ccl::rslang::ErrorLogger log{};
};

TEST_F(UTLogger, ResolveErrorType) {
  EXPECT_EQ(ccl::rslang::ResolveErrorType(static_cast<uint32_t>(ErrorStatus::WARNING)), ErrorStatus::WARNING);
  EXPECT_EQ(ccl::rslang::ResolveErrorType(static_cast<uint32_t>(ErrorStatus::WARNING) + 1), ErrorStatus::WARNING);
  EXPECT_EQ(ccl::rslang::ResolveErrorType(static_cast<uint32_t>(ErrorStatus::CRITICAL)), ErrorStatus::CRITICAL);
  EXPECT_EQ(ccl::rslang::ResolveErrorType(static_cast<uint32_t>(ErrorStatus::CRITICAL) + 1), ErrorStatus::CRITICAL);
  EXPECT_EQ(ccl::rslang::ResolveErrorType(static_cast<uint32_t>(ErrorStatus::CRITICAL) - 1), ErrorStatus::WARNING);

  EXPECT_TRUE(Error(static_cast<uint32_t>(ErrorStatus::WARNING), 0).IsWarning());
  EXPECT_FALSE(Error(static_cast<uint32_t>(ErrorStatus::WARNING), 0).IsCritical());
  EXPECT_TRUE(Error(static_cast<uint32_t>(ErrorStatus::CRITICAL), 0).IsCritical());
  EXPECT_FALSE(Error(static_cast<uint32_t>(ErrorStatus::CRITICAL), 0).IsWarning());
}

TEST_F(UTLogger, LogError) {
  const Error testErr{ 0x2001, 1 };
  log.LogError(testErr);
  EXPECT_EQ(log.FirstErrorPos(), 1);
  EXPECT_EQ(log.Count(ErrorStatus::WARNING), 1);
  EXPECT_EQ(ssize(log.All()), 1);

  log.LogError(testErr);
  EXPECT_EQ(log.FirstErrorPos(), 1);
  EXPECT_EQ(log.Count(ErrorStatus::WARNING), 2);
  EXPECT_EQ(ssize(log.All()), 2);
}

TEST_F(UTLogger, FirstErrorPos) {
  EXPECT_TRUE(std::empty(log.All()));
  EXPECT_EQ(log.FirstErrorPos(), 0);
  log.LogError(Error{ 0, 2 });
  log.LogError(Error{ 0, 0 });
  EXPECT_EQ(log.FirstErrorPos(), 0);
  log.LogError(Error{ 0, 1 });
  EXPECT_EQ(log.FirstErrorPos(), 0);
  log.LogError(Error{ 0, -1 });
  EXPECT_EQ(log.FirstErrorPos(), -1);
}

TEST_F(UTLogger, HasCriticals) {
  EXPECT_FALSE(log.HasCriticalErrors());
  log.LogError(Error{ 0, 0 });
  EXPECT_FALSE(log.HasCriticalErrors());
  log.LogError(Error{ 0x8001, 0 });
  EXPECT_TRUE(log.HasCriticalErrors());
  log.LogError(Error{ 0x8001, 0 });
  EXPECT_TRUE(log.HasCriticalErrors());
}

TEST_F(UTLogger, Count) {
  EXPECT_EQ(log.Count(ErrorStatus::WARNING), 0);
  EXPECT_EQ(log.Count(ErrorStatus::CRITICAL), 0);
  log.LogError(Error{ 0, 0 });
  EXPECT_EQ(log.Count(ErrorStatus::WARNING), 1);
  EXPECT_EQ(log.Count(ErrorStatus::CRITICAL), 0);
  log.LogError(Error{ 0x8001, 0 });
  log.LogError(Error{ 0x8001, 0 });
  EXPECT_EQ(log.Count(ErrorStatus::WARNING), 1);
  EXPECT_EQ(log.Count(ErrorStatus::CRITICAL), 2);
}