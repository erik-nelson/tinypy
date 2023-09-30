#include "gtest/gtest.h"
#include "stream.h"

TEST(Stream, TestStream) {
  // The producer counts to 5, and is then finished.
  int value = 0;
  Stream<int>::FillCallback fill_callback = [&](std::vector<int>* buffer) -> bool {
    if (value >= 5) return false;
    buffer->push_back(value++);
    return true;
  };

  // Create a single element stream.
  Stream<int> stream(fill_callback, /*min_buffer_size=*/1);
  StreamReader<int> reader = stream.MakeReader();

  // Sanity checks.
  EXPECT_FALSE(reader.Finished());
  EXPECT_FALSE(reader.Depleted());
  EXPECT_TRUE(reader.Empty());

  // Peek at the first value. Peeking arbitrarily many times only reads once.
  ASSERT_EQ(value, 0);
  for (size_t i = 0; i < 10; ++i) {
    auto peeked = reader.Peek();
    ASSERT_TRUE(peeked.has_value());
    EXPECT_EQ(peeked.value(), 0);
  }
  ASSERT_EQ(value, 1);
  EXPECT_FALSE(reader.Finished());
  EXPECT_FALSE(reader.Depleted());
  EXPECT_FALSE(reader.Empty());

  // Read the first value, consuming it.
  std::optional<int> read = reader.Read();
  EXPECT_TRUE(read.has_value());
  EXPECT_EQ(read.value(), 0);
  ASSERT_EQ(value, 1);
  EXPECT_FALSE(reader.Finished());
  EXPECT_FALSE(reader.Depleted());
  EXPECT_TRUE(reader.Empty());
  
  // Advance past the second value, consuming it.
  EXPECT_TRUE(reader.Advance());
  EXPECT_TRUE(reader.Empty());

  // Check the third value.
  auto peeked = reader.Peek();
  ASSERT_TRUE(peeked.has_value());
  EXPECT_EQ(peeked.value(), 2);
  EXPECT_EQ(value, 3);
  EXPECT_FALSE(reader.Finished());
  EXPECT_FALSE(reader.Depleted());
  EXPECT_FALSE(reader.Empty());

  // Read the next couple values until the stream is depleted.
  for (int expected = 2; expected < 5; ++expected) {
    read = reader.Read();
    ASSERT_TRUE(read.has_value());
    EXPECT_EQ(read.value(), expected);
    EXPECT_FALSE(reader.Finished());
    EXPECT_FALSE(reader.Depleted());
    EXPECT_TRUE(reader.Empty());
    EXPECT_EQ(value, expected + 1);
  }

  // On next read, the producer will be finished, and the stream depleted.
  read = reader.Read();
  ASSERT_FALSE(read.has_value());
  EXPECT_TRUE(reader.Finished());
  EXPECT_TRUE(reader.Depleted());
  EXPECT_TRUE(reader.Empty());

  // No longer able to peek, read, or advance.
  EXPECT_FALSE(reader.Peek().has_value());
  EXPECT_FALSE(reader.Read().has_value());
  EXPECT_FALSE(reader.Advance());
  
  // Still finished/depleted/empty after attempting redundant reads.
  EXPECT_TRUE(reader.Finished());
  EXPECT_TRUE(reader.Depleted());
  EXPECT_TRUE(reader.Empty());
}