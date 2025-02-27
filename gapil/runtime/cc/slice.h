// Copyright (C) 2018 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __GAPIL_RUNTIME_SLICE_H__
#define __GAPIL_RUNTIME_SLICE_H__

#include "pool.h"

#include <functional>

namespace gapil {

// Slice is a vector of elements of type T backed by a pool. Slice is compatible
// with the slices produced by the gapil compiler.
// Slices hold references to their pool, and several slices may share the same
// underlying data.
template <typename T>
class Slice {
 public:
  // Constructs a slice that points to nothing.
  inline Slice() : Slice(nullptr, 0, 0, 0, 0) {}

  // Constructs a slice which shares ownership over the data with other.
  inline Slice(const Slice<T>& other)
      : Slice(other.pool_, other.root_, other.base_, other.size_,
              other.count_) {}

  // Constructs a new slice and pool sized to the given number of elements.
  inline Slice(T* base, uint64_t count)
      : Slice(nullptr, reinterpret_cast<uint64_t>(base),
              reinterpret_cast<uint64_t>(base), count * sizeof(T), count) {}

  // Constructs a new slice given the full explicit parameters.
  inline Slice(Pool* pool, uint64_t root, uint64_t base, uint64_t size,
               uint64_t count, bool add_ref = true)
      : pool_(pool), root_(root), base_(base), size_(size), count_(count) {
    if (add_ref && pool != nullptr) {
      pool->reference();
    }
  }

  // Creates and returns a new slice wrapping the given pool.
  // If add_ref is true then the pool's reference count will be incremented.
  inline static Slice create(Pool* pool, bool add_ref);

  // Creates and returns a new slice and pool sized to the given number of
  // elements.
  inline static Slice create(core::Arena* arena, uint32_t pool_id,
                             uint64_t count);

  inline Slice(Slice<T>&& other)
      : Slice(other.pool_, other.root_, other.base_, other.size_,
              other.count_) {
    other.pool_ = nullptr;
  }

  inline ~Slice() {
    if (pool_ != nullptr) {
      pool_->release();
    }
  }

  // Copy assignment
  inline Slice<T>& operator=(const Slice<T>& other);

  // Equality operator.
  inline bool operator==(const Slice<T>& other) const;

  // Returns the root of the slice.
  inline uint64_t root() const { return root_; }

  // Returns the root of the slice.
  inline uint64_t base() const { return base_; }

  // Returns the number of elements in the slice.
  inline uint64_t count() const { return count_; }

  // Returns the size of the slice in bytes.
  inline uint64_t size() const { return size_; }

  // Returns true if this is a slice on the application pool (external memory).
  inline bool is_app_pool() const { return pool_ == nullptr; }

  // Returns the underlying pool identifier.
  inline uint32_t pool_id() const {
    return (pool_ != nullptr) ? pool_->id() : 0;
  }

  // Returns the underlying pool.
  inline const Pool* pool() const { return pool_; }

  // Returns true if the slice contains the specified value.
  inline bool contains(const T& value) const;

  // Returns a new subset slice from this slice.
  inline Slice<T> operator()(uint64_t start, uint64_t end) const;

  // Returns a reference to a single element in the slice.
  // Care must be taken to not mutate data in the application pool.
  inline T& operator[](uint64_t index) const;

  // Copies count elements starting at start into the dst Slice starting at
  // dstStart.
  inline void copy(const Slice<T>& dst, uint64_t start, uint64_t count,
                   uint64_t dstStart) const;

  // Casts this slice to a slice of type U.
  // The return slice length will be calculated so that the returned slice
  // length is no longer (in bytes) than this slice.
  template <typename U>
  inline Slice<U> as() const;

  // Support for range-based for looping
  inline T* begin() const;
  inline T* end() const;

 private:
  void reference() const;
  void release();

  // the underlying pool. nullptr represents the application pool.
  Pool* pool_;

  // original offset in bytes from pool base that this slice derives from.
  uint64_t root_;

  // offset in bytes from pool base of the first element.
  uint64_t base_;

  // size in bytes of the slice.
  uint64_t size_;

  // total number of elements in the slice.
  uint64_t count_;
};

}  // namespace gapil

#endif  // __GAPIL_RUNTIME_SLICE_H__
