#ifndef ALIGNED_VECTOR_HPP_
#define ALIGNED_VECTOR_HPP_

#include <cmath>
#include <functional>
#include <memory>
#include <assert.h>
#include <cstring>

#include <string>
#include <sstream>

namespace is {

/**
 * Template parameters: T class type, A alignment size in bytes.
 */
template<typename T, size_t A>
class aligned_vector {
 public:
  explicit aligned_vector() noexcept : data_(NULL), raw_data_(NULL), size_(0), capacity_(0) { }

  // explicit aligned_vector() noexcept {
  // }

  ~aligned_vector() noexcept {
    free_data_();
  }

  size_t size() const { return size_; }

  bool empty() { return (size_ == 0); }

  T* data() const { return data_; }

  T& at(size_t pos) const {
    assert(pos >= 0 && pos < this->size_);
    return data_[pos];
  }

  T& operator[](size_t pos) {
    assert(pos >= 0 && pos < this->size_);
    return data_[pos];
  }

  void clear() {
    free_data_();
  }

  size_t capacity() { return capacity_; }

  void push_back(T& t) {
    if (size_ >= capacity_) {
      reserve((capacity_ == 0) ? (1) : (capacity_ * 2));
    }
    data_[size_] = t;
    size_ += 1;
  }

  void push_back(const T& t) {
    if (size_ >= capacity_) {
      reserve((capacity_ == 0) ? (1) : (capacity_ * 2));
    }
    data_[size_] = t;
    size_ += 1;
  }

  void emplace_back(T& t) {
    push_back(t);
  }

  void resize(size_t size) {
    reserve(size);
    size_ = size;
  }

  void reserve(size_t capacity) {
    // Allocate the aligned memory and perform sanity checks.
    T* new_raw_data = NULL;
    T* new_aligned_data = alloc_aligned_(&new_raw_data, capacity, A);
    assert(new_raw_data);
    assert(new_aligned_data);

    // Move the existing data.
    size_t min_size = (size_ < capacity) ? (size_) : (capacity);
    std::memmove(new_aligned_data, data_, sizeof(T)*min_size);

    // Reset the memory info.
    if (raw_data_) {
      delete[] raw_data_;
    }
    raw_data_ = new_raw_data;
    data_ = new_aligned_data;
    capacity_ = capacity;
    size_ = min_size;
  }

#ifdef DEBUG_VERBOSE_
  std::string verbose() {
    std::stringstream ss;
    char x[100];
    sprintf (x, "%lX", (size_t) this->data());

    char raw_x[100];
    sprintf (raw_x, "%lX", (size_t) (this->raw_data_));

    ss << "size() = " << this->size() << " capacity() = " << this->capacity() << " aligned address = " << raw_x << " (% " << A << " = " << (((size_t) this->raw_data_)) % ((size_t) A) << ") ";
    ss << " hex address = " << x << " (% " << A << " = " << (((size_t) this->data())) % ((size_t) A) << ")";
    if (this->data() == this->raw_data_) { ss << "\tSAME address (no alignment was performed)"; }
    else { ss << "\tDIFFERENT address (alignment was performed)"; }

    return ss.str();
  }
#endif
  
  // back()
  // front()
  // begin()
  // end()
  // assign
  // insert

 private:
  T *data_;
  T *raw_data_;
  size_t size_;
  size_t capacity_;

  void free_data_() {
    if (raw_data_) {
      delete[] raw_data_;
      raw_data_ = NULL;
      data_ = NULL;
      size_ = 0;
      capacity_ = 0;
    }
  }

  T* alloc_aligned_(T** pt, size_t size, size_t alignment_size) {
      *pt = new T[size + alignment_size - 1];
      void* ptr = (void*) *pt;
      size_t storage_size = (size + alignment_size - 1) * sizeof(T);
      return (T*) std::align(alignment_size, size * sizeof(T), ptr, storage_size);
  }
};

}

#endif
