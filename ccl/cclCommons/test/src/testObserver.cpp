#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "FakeObserver.hpp"

namespace ct = ccl::types;

class UTObserver : public ::testing::Test {
public:
  FakeObserver obs{};
  FakeObservable object{};
};

TEST_F(UTObserver, BasicMod) {
  auto basic = ct::BasicMsg{ 42U };
  EXPECT_EQ(basic.Type(), 42U);
}

TEST_F(UTObserver, NoObservers) {
  EXPECT_NO_THROW(object.Notify(42U));
  EXPECT_NO_THROW(object.Notify(ct::BasicMsg{ 42U }));
}

TEST_F(UTObserver, AddObserver) {
  object.AddObserver(obs);
  object.Notify(42U);
  EXPECT_EQ(ssize(obs.events), 1);
}

TEST_F(UTObserver, ImportObservers) {
  object.AddObserver(obs);
  FakeObservable anotherObj{};
  anotherObj.ImportObserversFrom(object);

  object.Notify(42U);
  EXPECT_EQ(ssize(obs.events), 1);

  anotherObj.Notify(42U);
  EXPECT_EQ(ssize(obs.events), 2);
}

TEST_F(UTObserver, AddDoubleObserver) {
  object.AddObserver(obs);
  object.AddObserver(obs);
  object.Notify(42U);
  EXPECT_EQ(ssize(obs.events), 2);
}

TEST_F(UTObserver, RemoveObserver) {
  object.AddObserver(obs);
  object.RemoveObserver(obs);
  object.Notify(42U);
  EXPECT_EQ(ssize(obs.events), 0);
}

TEST_F(UTObserver, RemoveDoubleObserver) {
  object.AddObserver(obs);
  object.AddObserver(obs);
  object.RemoveObserver(obs);
  object.Notify(42U);
  EXPECT_EQ(ssize(obs.events), 0);
}

TEST_F(UTObserver, Notify) {
  object.AddObserver(obs);
  object.Notify(42U);
  EXPECT_EQ(ssize(obs.events), 1);
  EXPECT_EQ(obs.events[0], 42U);
}

TEST_F(UTObserver, DndStatus) {
  EXPECT_FALSE(obs.DndStatus());
  auto guard = obs.DndGuard();
  EXPECT_TRUE(obs.DndStatus());
}

TEST_F(UTObserver, DndBlockObserve) {
  object.AddObserver(obs);
  auto guard = obs.DndGuard();
  object.Notify(42U);
  EXPECT_EQ(ssize(obs.events), 0);
}