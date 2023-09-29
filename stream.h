#pragma once

#include <functional>
#include <optional>
#include <queue>
#include <utility>

// A simple unidirectional stream that allows consumers to pull from producers.
// NOTE(erik): This is currently synchronous.
//
// Example usage:
//
//    // User defines a callback that generates new values on request by a reader.
//    Stream<Foo>::FillCallback = []() -> std::optional<Foo> { ... };
//
//    // Create a stream and a reader.
//    Stream<Foo> stream(fill_callback, /*capacity=*/10);
//    StreamReader<Foo> reader = stream.MakeReader();
//
//    // Users can now request values from the stream as long as they are able
//    // to be produced. This internally calls the FillCallback until the producer
//    // is depleted.
//    std::optional<Foo> next = reader.Read();
//

template <typename T>
class Stream;

template <typename T>
class StreamReader {
 public:
  StreamReader(Stream<T>* stream) : stream_(stream) {}

  // Peek at the next element in the stream without consuming it.
  // Returns nullopt if the stream was depleted.
  std::optional<std::reference_wrapper<T>> Peek() {
    stream_->Fill();
    if (!Empty()) return std::ref(stream_->stream_.front());
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

  // Is the producer finished?
  bool Finished() const { return stream_->finished_; }
  // Is the remaining stream buffer empty?
  bool Empty() const { return stream_->stream_.empty(); }
  // Is the stream depleted? i.e. the producer is finished and the buffer is empty.
  bool Depleted() const { return Finished() && Empty(); }

 protected:
  Stream<T>* stream_;
};

// A simple synchronous pull stream.
template <typename T>
class Stream {
 public:
  using FillCallback = std::function<std::optional<T>()>;

  // Initialize a stream with a fill callback and buffer capacity. The read callback
  // fills the stream with values, when requested by a reader.
  Stream(FillCallback callback, size_t buffer_capacity = 10) 
    : callback_(std::move(callback)), buffer_capacity_(buffer_capacity) {}

  // Make a new reader.
  StreamReader<T> MakeReader() { return {this}; }

 private:
  friend class StreamReader<T>;

  // Called by readers. Requests new values to be inserted into the queue by 
  // the producer callback. Returns whether the producer is finished.
  bool Fill() {
    if (finished_) return false;
    
    while (stream_.size() < buffer_capacity_) {
      if (std::optional<T> next = callback_(); next.has_value()) { 
        stream_.emplace(std::move(*next));
      } else { 
        finished_ = true; 
        return false;
      }
    }

    return true;
  }

  // Callback to use to fill stream buffer.
  FillCallback callback_;

  // Capacity of stream buffer.
  size_t buffer_capacity_;
  
  // The stream buffer.
  std::queue<T> stream_;
  
  // Whether the producer is finished. It is possible for the producer to be
  // finished producing values while `stream_` is still non-empty.
  bool finished_ = false;
};