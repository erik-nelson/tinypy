#pragma once

#include <functional>
#include <optional>
#include <queue>
#include <utility>
#include <vector>

// A simple unidirectional stream that allows consumers to pull from producers.
// NOTE(erik): This is currently synchronous.
//
// Example usage:
//
//    // User defines a callback that generates new values on request.
//    Stream<Foo>::FillCallback = [](std::vector<Foo>* fill_me) -> bool { ... };
//
//    // Create a stream and a reader.
//    Stream<Foo> stream(fill_callback, /*min_buffer_size=*/10);
//    StreamReader<Foo> reader = stream.MakeReader();
//
//    // Users can now request values from the stream as long as they are able
//    // to be produced. This internally calls the FillCallback until the
//    // producer is depleted.
//    std::optional<Foo> next = reader.Read();
//
//    // Users can also simply read all values from the stream, requesting that
//    // the producer continue producing them until it runs out.
//    std::vector<Foo> values = reader.ReadAll();
//
template <typename T>
class Stream;

template <typename T>
class StreamReader {
 public:
  StreamReader(Stream<T>* stream) : stream_(stream) {}

  // Peek at the next element in the stream without consuming it.
  // Returns nullopt if the stream was depleted.
  std::optional<const T*> Peek() {
    stream_->Fill();
    if (!Empty()) return &stream_->stream_.front();
    return std::nullopt;
  }

  // Read the next element from the stream, consuming it.
  // Returns nullopt if the stream was depleted.
  std::optional<T> Read() {
    stream_->Fill();
    if (!Empty()) {
      T value = std::move(stream_->stream_.front());
      stream_->stream_.pop();
      return value;
    }
    return std::nullopt;
  }

  // Consume the next element in the stream (without reading it).
  // Returns false if the stream was depleted.
  bool Advance() {
    stream_->Fill();
    if (!Empty()) {
      stream_->stream_.pop();
      return true;
    }
    return false;
  }

  // Helper that reads all remaining values from the stream into a vector.
  std::vector<T> ReadAll() {
    std::vector<T> values;
    while (auto value = Read()) values.emplace_back(std::move(*value));
    return values;
  }

  // Is the producer finished?
  bool Finished() const { return stream_->Finished(); }
  // Is the remaining stream buffer empty?
  bool Empty() const { return stream_->Empty(); }
  // Is the stream depleted? i.e. the producer is finished and the buffer is
  // empty.
  bool Depleted() const { return stream_->Depleted(); }

 protected:
  Stream<T>* stream_;
};

#include <stdio.h>
// A simple synchronous pull stream.
template <typename T>
class Stream {
 public:
  // A function that when called fills up a container of more values to put into
  // the stream.
  using FillCallback = std::function<bool(std::vector<T>*)>;

  // Initialize a stream with a fill callback and min buffer size. The read
  // callback fills the stream with values, when requested by a reader.
  Stream(FillCallback callback, size_t min_buffer_size = 10)
      : callback_(callback), min_buffer_size_(min_buffer_size) {}

  // Make a new reader.
  StreamReader<T> MakeReader() { return {this}; }

  // Is the producer finished?
  bool Finished() const { return finished_; }
  // Is the remaining stream buffer empty?
  bool Empty() const { return stream_.empty(); }
  // Is the stream depleted? i.e. the producer is finished and the buffer is
  // empty.
  bool Depleted() const { return Finished() && Empty(); }

  // Clear the stream.
  void Clear() {
    stream_ = {};
    finished_ = false;
  }

 private:
  friend class StreamReader<T>;

  // Called by readers. Requests new values to be inserted into the queue by
  // the producer callback.
  void Fill() {
    std::vector<T> values;
    while (stream_.size() < min_buffer_size_ && !finished_) {
      values.clear();
      finished_ = !callback_(&values);
      for (T& value : values) stream_.push(std::move(value));
    }
  }

  // Callback to use to fill stream buffer.
  FillCallback callback_;

  // When the stream is read from, refill it to at least this many elements.
  size_t min_buffer_size_;

  // The stream buffer.
  std::queue<T> stream_;

  // Whether the producer is finished. It is possible for the producer to be
  // finished producing values while `stream_` is still non-empty.
  bool finished_ = false;
};