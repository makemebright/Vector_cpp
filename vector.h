#ifndef VECTOR
#define VECTOR

#define VECTOR_MEMORY_IMPLEMENTED

#include <exception>
#include <iostream>

template <typename T>
class Vector {
 public:
  using ValueType = T;

  using Pointer = T*;

  using ConstPointer = const T*;

  using Reference = T&;

  using ConstReference = const T&;

  using SizeType = size_t;

  SizeType size_ = static_cast<SizeType>(0);

  SizeType capacity_ = static_cast<SizeType>(0);

  Pointer buffer_ = nullptr;

 private:
  void Reset() {
    if (buffer_ != nullptr) {
      for (SizeType i = 0; i < size_; ++i) {
        (buffer_ + i)->~ValueType();
      }
      operator delete(buffer_);
    }
    buffer_ = nullptr;
    size_ = 0;
    capacity_ = 0;
  }

  void SecureCopy(Pointer new_buffer, SizeType new_size) const {
    SizeType i = 0;
    try {
      while (i < new_size) {
        new (new_buffer + i) ValueType(buffer_[i]);
        ++i;
      }
    } catch (...) {
      while (i > 0) {
        (new_buffer + i - 1)->~ValueType();
        --i;
      }
      operator delete(new_buffer);
      new_buffer = nullptr;
      throw;
    }
  }

  void SecureMove(Pointer new_buffer, SizeType new_size) {
    SizeType i = 0;
    try {
      while (i < new_size) {
        new (new_buffer + i) ValueType(std::move(buffer_[i]));
        ++i;
      }
    } catch (...) {
      while (i > 0) {
        (new_buffer + i - 1)->~ValueType();
        --i;
      }
      operator delete(new_buffer);
      new_buffer = nullptr;
      throw;
    }
  }

  void SecureInizial(Pointer new_buffer, SizeType new_size) const {
    SizeType i = 0;
    try {
      while (i < new_size) {
        new (new_buffer + i) ValueType;
        ++i;
      }
    } catch (...) {
      while (i > 0) {
        (new_buffer + i - 1)->~ValueType();
        --i;
      }
      operator delete(new_buffer);
      new_buffer = nullptr;
      throw;
    }
  }

  void SecureValue(Pointer new_buffer, SizeType new_size, ValueType value) const {
    SizeType i = 0;
    try {
      while (i < new_size) {
        new (new_buffer + i) ValueType(value);
        ++i;
      }
    } catch (...) {
      while (i > 0) {
        (new_buffer + i - 1)->~ValueType();
        --i;
      }
      operator delete(new_buffer);
      new_buffer = nullptr;
      throw;
    }
  }

  void NewMemoryMove(SizeType new_capacity) {
    Pointer new_buffer = nullptr;
    SizeType new_size = std::min(new_capacity, size_);
    if (new_capacity != 0) {
      new_buffer = static_cast<Pointer>(operator new(sizeof(ValueType) * new_capacity));
      this->SecureMove(new_buffer, new_size);
    }
    this->Reset();
    buffer_ = new_buffer;
    size_ = new_size;
    capacity_ = new_capacity;
  }

 public:
  Vector() = default;

  explicit Vector(SizeType size) {
    if (size != 0) {
      auto new_buffer = static_cast<Pointer>(operator new(sizeof(ValueType) * size));
      this->SecureInizial(new_buffer, size);
      buffer_ = new_buffer;
      size_ = size;
      capacity_ = size;
    }
  }

  Vector(SizeType size, ValueType value) {
    if (size != 0) {
      auto new_buffer = static_cast<Pointer>(operator new(sizeof(ValueType) * size));
      this->SecureValue(new_buffer, size, value);
      buffer_ = new_buffer;
      size_ = size;
      capacity_ = size;
    }
  }

  template <class Iterator, class = std::enable_if_t<std::is_base_of_v<
                                std::forward_iterator_tag, typename std::iterator_traits<Iterator>::iterator_category>>>
  Vector(Iterator begin, Iterator end) : Vector() {
    if (begin - end != 0) {
      auto new_buffer = static_cast<Pointer>(operator new(sizeof(ValueType) * (end - begin)));
      SizeType i = 0;
      try {
        auto it = begin;
        while (it != end) {
          new (new_buffer + i) ValueType(*it);
          ++i;
          ++it;
        }
      } catch (...) {
        while (i > 0) {
          (new_buffer + i - 1)->~ValueType();
          --i;
        }
        operator delete(new_buffer);
        new_buffer = nullptr;
        throw;
      }
      buffer_ = new_buffer;
      capacity_ = end - begin;
      size_ = end - begin;
    }
  }

  Vector(std::initializer_list<ValueType> list) : Vector(list.begin(), list.end()) {
  }

  Vector(const Vector<ValueType>& other) {
    if (other.buffer_ != nullptr) {
      auto new_buffer = static_cast<Pointer>(operator new(sizeof(ValueType) * other.capacity_));
      other.SecureCopy(new_buffer, other.size_);
      size_ = other.size_;
      capacity_ = other.capacity_;
      buffer_ = new_buffer;
    }
  }

  Vector(Vector&& other) noexcept {
    buffer_ = other.buffer_;
    size_ = other.size_;
    capacity_ = other.capacity_;
    other.buffer_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
  }

  ~Vector() {
    this->Reset();
  }

  Vector& operator=(const Vector<ValueType>& other) {
    if (this != &other) {
      Vector<ValueType> copy(other);
      this->Swap(copy);
    }
    return *this;
  }

  Vector& operator=(Vector<ValueType>&& other) {
    this->Swap(other);
    other.Reset();
    return *this;
  }

  SizeType Size() const {
    return size_;
  }

  SizeType Capacity() const {
    return capacity_;
  }

  bool Empty() const {
    return 0 == size_;
  }

  Reference operator[](SizeType index) {
    return buffer_[index];
  }

  ConstReference operator[](SizeType index) const {
    return buffer_[index];
  }

  Reference At(SizeType index) {
    if (index >= size_) {
      throw std::out_of_range("");
    }
    return buffer_[index];
  }

  ConstReference At(SizeType index) const {
    if (index >= size_) {
      throw std::out_of_range("");
    }
    return buffer_[index];
  }

  Reference Front() {
    return *buffer_;
  }

  ConstReference Front() const {
    return *buffer_;
  }

  Reference Back() {
    return *(buffer_ + size_ - 1);
  }

  ConstReference Back() const {
    return *(buffer_ + size_ - 1);
  }

  Pointer Data() {
    return buffer_;
  }

  ConstPointer Data() const {
    return buffer_;
  }

  void Swap(Vector<ValueType>& other) {
    std::swap(buffer_, other.buffer_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
  }

  void Resize(SizeType new_size) {
    if (new_size == 0) {
      this->Reset();
      return;
    }
    if (new_size < size_) {
      while (new_size < size_) {
        (buffer_ + size_ - 1)->~ValueType();
        --size_;
      }
      return;
    }
    if (new_size == size_) {
      return;
    }
    if (new_size > size_ && new_size <= capacity_) {
      try {
        while (new_size > size_) {
          new (buffer_ + size_) ValueType;
          ++size_;
        }
      } catch (...) {
        throw;
      }
      return;
    }
    if (new_size > capacity_) {
      auto new_buffer = static_cast<Pointer>(operator new(sizeof(ValueType) * new_size));
      SizeType i = 0;
      try {
        while (i < size_) {
          new (new_buffer + i) ValueType(std::move(buffer_[i]));
          ++i;
        }
        while (i < new_size) {
          new (new_buffer + i) ValueType;
          ++i;
        }
      } catch (...) {
        while (i > 0) {
          (new_buffer + i - 1)->~ValueType();
          --i;
        }
        operator delete(new_buffer);
        new_buffer = nullptr;
        throw;
      }
      this->Reset();
      capacity_ = new_size;
      size_ = new_size;
      buffer_ = new_buffer;
    }
  }

  void Resize(SizeType new_size, ConstReference value) {
    if (new_size == 0) {
      this->Reset();
      return;
    }
    if (new_size < size_) {
      while (new_size < size_) {
        (buffer_ + size_ - 1)->~ValueType();
        --size_;
      }
      return;
    }
    if (new_size == size_) {
      return;
    }
    if (new_size > size_ && new_size <= capacity_) {
      try {
        while (new_size > size_) {
          new (buffer_ + size_) ValueType(value);
          ++size_;
        }
      } catch (...) {
        throw;
      }
      return;
    }
    if (new_size > capacity_) {
      auto new_buffer = static_cast<Pointer>(operator new(sizeof(ValueType) * new_size));
      SizeType i = 0;
      try {
        while (i < size_) {
          new (new_buffer + i) ValueType(std::move(buffer_[i]));
          ++i;
        }
        while (i < new_size) {
          new (new_buffer + i) ValueType(value);
          ++i;
        }
      } catch (...) {
        while (i > 0) {
          (new_buffer + i - 1)->~ValueType();
          --i;
        }
        operator delete(new_buffer);
        new_buffer = nullptr;
        throw;
      }
      this->Reset();
      capacity_ = new_size;
      size_ = new_size;
      buffer_ = new_buffer;
    }
  }

  void Reserve(SizeType new_capacity) {
    if (capacity_ < new_capacity) {
      this->NewMemoryMove(new_capacity);
    }
  }

  void ShrinkToFit() {
    if ((capacity_ > size_)) {
      this->NewMemoryMove(size_);
    }
  }

  void Clear() noexcept {
    while (size_ > 0) {
      --size_;
      (buffer_ + size_)->~ValueType();
    }
  }

  void PushBack(const T& value) {
    if (capacity_ == 0) {
      auto new_buffer = static_cast<Pointer>(operator new(sizeof(ValueType) * 1));
      try {
        new (new_buffer) ValueType(value);
        ++size_;
      } catch (...) {
        (new_buffer)->~ValueType();
        operator delete(new_buffer);
        throw;
      }
      buffer_ = new_buffer;
      capacity_ = 1;
      size_ = 1;
      return;
    }
    if (size_ < capacity_) {
      try {
        new (buffer_ + size_) ValueType(value);
        ++size_;
      } catch (...) {
        throw;
      }
      return;
    }
    if (size_ == capacity_) {
      SizeType counter = 0;
      auto new_buffer = static_cast<Pointer>(operator new(sizeof(ValueType) * (capacity_ * 2)));
      try {
        for (SizeType i = 0; i < size_; ++i) {
          new (new_buffer + i) ValueType(std::move(buffer_[i]));
          ++counter;
        }
        new (new_buffer + counter) ValueType(value);
        ++counter;
      } catch (...) {
        while (counter > 0) {
          (new_buffer + counter - 1)->~ValueType();
          --counter;
        }
        operator delete(new_buffer);
        throw;
      }
      for (SizeType i = 0; i < size_; ++i) {
        (buffer_ + i)->~ValueType();
      }
      operator delete(buffer_);
      capacity_ = capacity_ * 2;
      ++size_;
      buffer_ = new_buffer;
    }
  }

  void PushBack(T&& value) {
    if (capacity_ == 0) {
      auto new_buffer = static_cast<Pointer>(operator new(sizeof(ValueType) * 1));
      try {
        new (new_buffer) ValueType(std::move(value));
        ++size_;
      } catch (...) {
        (new_buffer)->~ValueType();
        operator delete(new_buffer);
        throw;
      }
      buffer_ = new_buffer;
      capacity_ = 1;
      size_ = 1;
      return;
    }
    if (size_ < capacity_) {
      try {
        new (buffer_ + size_) ValueType(std::move(value));
        ++size_;
      } catch (...) {
        throw;
      }
      return;
    }
    if (size_ == capacity_) {
      SizeType counter = 0;
      auto new_buffer = static_cast<Pointer>(operator new(sizeof(ValueType) * (capacity_ * 2)));
      try {
        for (SizeType i = 0; i < size_; ++i) {
          new (new_buffer + i) ValueType(std::move(buffer_[i]));
          ++counter;
        }
        new (new_buffer + counter) ValueType(std::move(value));
        ++counter;
      } catch (...) {
        while (counter > 0) {
          (new_buffer + counter - 1)->~ValueType();
          --counter;
        }
        operator delete(new_buffer);
        throw;
      }
      for (SizeType i = 0; i < size_; ++i) {
        (buffer_ + i)->~ValueType();
      }
      operator delete(buffer_);
      capacity_ = capacity_ * 2;
      ++size_;
      buffer_ = new_buffer;
    }
  }

  void PopBack() {
    if (size_ == 0) {
      throw std::out_of_range("");
    }
    --size_;
    (buffer_ + size_)->~ValueType();
  }

  template <typename... Args>
  void EmplaceBack(Args&&... args) {
    if (size_ >= capacity_) {
      if (capacity_ == 0) {
        this->NewMemoryMove(1);
      } else {
        this->NewMemoryMove(2 * capacity_);
      }
    }
    try {
      new (buffer_ + size_) ValueType(std::forward<Args>(args)...);
      ++size_;
    } catch (...) {
      --size_;
      throw;
    }
  }

  template <typename Type>
  class Itterator {
    const Pointer data_ = nullptr;
    SizeType index_ = 0;

   public:
    using value_type = std::remove_cv_t<Type>;                  // NOLINT
    using pointer = Pointer;                                    // NOLINT
    using reference = Reference;                                // NOLINT
    using difference_type = ptrdiff_t;                          // NOLINT
    using iterator_category = std::random_access_iterator_tag;  // NOLINT

    Itterator() = default;

    Itterator(Pointer data, SizeType idx) : data_(data), index_(idx) {
    }

    explicit operator Itterator<const Type>() const {
      return Itterator<const Type>(data_, index_);
    }

    Itterator(const Itterator& other) = default;

    Itterator& operator=(const Itterator& other) = default;

    Itterator& operator++() {
      ++index_;
      return *this;
    }

    Itterator operator++(int) {
      auto copy = *this;
      ++index_;
      return copy;
    }

    Itterator& operator--() {
      --index_;
      return *this;
    }

    Itterator operator--(int) {
      auto copy = *this;
      --index_;
      return copy;
    }

    Itterator& operator+=(difference_type value) {
      index_ += value;
      return (*this);
    }

    Itterator& operator-=(difference_type value) {
      index_ -= value;
      return *this;
    }

    Itterator operator+(difference_type value) const {
      Itterator copy = *this;
      return copy += value;
    }

    Itterator operator-(difference_type value) const {
      Itterator copy = *this;
      return copy -= value;
    }

    difference_type operator-(const Itterator& other) const {
      return (index_ - other.index_);
    }

    difference_type operator-=(const Itterator& other) const {
      return (index_ - other.index_);
    }

    bool operator==(const Itterator& other) const {
      return (index_ == other.index_);
    }

    bool operator!=(const Itterator& other) const {
      return !(*this == other);
    }

    bool operator<(const Itterator& other) const {
      return ((*this - other) < 0);
    }

    bool operator>=(const Itterator& other) const {
      return !(*this < other);
    }

    bool operator<=(const Itterator& other) const {
      return ((*this < other) || (*this == other));
    }

    bool operator>(const Itterator& other) const {
      return !(*this <= other);
    }

    Reference operator*() const {
      return *(data_ + index_);
    }

    Pointer operator->() const {
      return data_ + index_;
    }

    ~Itterator() = default;
  };

  using Iterator = Itterator<ValueType>;
  using ConstIterator = Itterator<const ValueType>;
  using ReverseIterator = std::reverse_iterator<Iterator>;
  using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

  Iterator begin() {  // NOLINT
    return Iterator(buffer_, 0);
  }

  Iterator end() {  // NOLINT
    return Iterator(buffer_, size_);
  }

  ConstIterator begin() const {  // NOLINT
    return ConstIterator(buffer_, 0);
  }

  ConstIterator end() const {  // NOLINT
    return ConstIterator(buffer_, size_);
  }

  ConstIterator cbegin() const {  // NOLINT
    return ConstIterator(buffer_, 0);
  }

  ConstIterator cend() const {  // NOLINT
    return ConstIterator(buffer_, size_);
  }

  ReverseIterator rbegin() {  // NOLINT
    return ReverseIterator({buffer_, size_});
  }

  ReverseIterator rend() {  // NOLINT
    return ReverseIterator({buffer_, 0});
  }

  ConstReverseIterator rbegin() const {  // NOLINT
    return ConstReverseIterator({buffer_, size_});
  }

  ConstReverseIterator rend() const {  // NOLINT
    return ConstReverseIterator({buffer_, 0});
  }

  ConstReverseIterator crbegin() const {  // NOLINT
    return ConstReverseIterator({buffer_, size_});
  }

  ConstReverseIterator crend() const {  // NOLINT
    return ConstReverseIterator({buffer_, 0});
  }
};

template <class T>
bool operator==(const Vector<T>& first, const Vector<T>& other) {
  for (size_t i = 0; i < std::min(first.Size(), other.Size()); ++i) {
    if (first[i] != other[i]) {
      return false;
    }
  }
  return first.Size() == other.Size();
}

template <class T>
bool operator!=(const Vector<T>& first, const Vector<T>& other) {
  return !(first == other);
}

template <class T>
bool operator<(const Vector<T>& first, const Vector<T>& other) {
  for (size_t i = 0; i < std::min(first.Size(), other.Size()); ++i) {
    if (first[i] == other[i]) {
      continue;
    }
    return first[i] < other[i];
  }
  return first.Size() < other.Size();
}

template <class T>
bool operator<=(const Vector<T>& first, const Vector<T>& other) {
  return (first < other || first == other);
}

template <class T>
bool operator>(const Vector<T>& first, const Vector<T>& other) {
  return (!(first < other) && first != other);
}

template <class T>
bool operator>=(const Vector<T>& first, const Vector<T>& other) {
  return !(first < other);
}

#endif