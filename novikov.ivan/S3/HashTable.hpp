#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP
#include <cstddef>
#include <utility>
#include <cmath>
#include <stdexcept>

namespace novikov
{
  template < class Key, class Value, class Hash, class Equal >
  class HashTable;

  template < class Key, class Value, class Hash, class Equal >
  class HTIter
  {
  public:
    explicit HTIter(HashTable< Key, Value, Hash, Equal >& ht, size_t pos);
    bool operator==(const HTIter&) const noexcept;
    bool operator!=(const HTIter&) const noexcept;
    HTIter& operator++();
    std::pair< Key, Value& > operator*();

  private:
    size_t pos_;
    HashTable< Key, Value, Hash, Equal >* table_;
  };

  template < class Key, class Value, class Hash, class Equal >
  class HTCIter
  {
  public:
    HTCIter(const HashTable< Key, Value, Hash, Equal >& ht, size_t pos);
    bool operator==(const HTCIter&) const noexcept;
    bool operator!=(const HTCIter&) const noexcept;
    HTCIter& operator++();
    HTCIter& operator--();
    const std::pair< Key, Value& > operator*();

  private:
    size_t pos_;
    const HashTable< Key, Value, Hash, Equal >* table_;
  };

  template < class Key, class Value, class Hash, class Equal >
  class HashTable
  {
  public:
    HashTable();
    HashTable(const HashTable& rhs);
    HashTable(HashTable&& rhs);
    ~HashTable();
    HashTable& operator=(const HashTable& rhs);
    HashTable& operator=(HashTable&& rhs);
    Value& operator[](Key key);

    HTIter< Key, Value, Hash, Equal > begin();
    HTCIter< Key, Value, Hash, Equal > begin() const;
    HTCIter< Key, Value, Hash, Equal > cbegin() const;

    HTIter< Key, Value, Hash, Equal > end();
    HTCIter< Key, Value, Hash, Equal > end() const;
    HTCIter< Key, Value, Hash, Equal > cend() const;

    bool insert(Key k, Value v);
    bool erase(Key k);
    bool contains(Key k) const;
    void rehash(size_t slots);
    bool empty() const;
    size_t slotsCount() const;
    size_t size() const;
    Value& at(Key key);
    const Value& at(Key key) const;
    void swap(HashTable< Key, Value, Hash, Equal >& rhs);

  private:
    friend class HTIter< Key, Value, Hash, Equal >;
    friend class HTCIter< Key, Value, Hash, Equal >;
    struct Slot
    {
      Key key;
      Value value;
      bool isEmpty = true;
    };
    size_t size_;
    size_t slotsCount_;
    Slot* slots_;
  };
}

template < class Key, class Value, class Hash, class Equal >
novikov::HashTable< Key, Value, Hash, Equal >::HashTable():
    size_(16),
    slotsCount_(0),
    slots_(new Slot[16])
{}

template < class Key, class Value, class Hash, class Equal >
novikov::HashTable< Key, Value, Hash, Equal >::HashTable::~HashTable()
{
  delete[] slots_;
}

template < class Key, class Value, class Hash, class Equal >
size_t novikov::HashTable< Key, Value, Hash, Equal >::size() const
{
  return size_;
}

template < class Key, class Value, class Hash, class Equal >
size_t novikov::HashTable< Key, Value, Hash, Equal >::slotsCount() const
{
  return slotsCount_;
}

template < class Key, class Value, class Hash, class Equal >
bool novikov::HashTable< Key, Value, Hash, Equal >::empty() const
{
  return !slotsCount_;
}

template < class Key, class Value, class Hash, class Equal >
bool novikov::HashTable< Key, Value, Hash, Equal >::insert(Key k, Value v)
{
  size_t limit = std::log2(size_);
  size_t pos = Hash{}(k) & (size_ - 1);
  for (size_t i = 0; i < limit; ++i)
  {
    if (slots_[pos].isEmpty)
    {
      slots_[pos] = {k, v, false};
      ++slotsCount_;
      return true;
    }
    if (slots_[pos].key == k)
    {
      return false;
    }
    pos = (pos + i) & (size_ - 1);
  }
  throw std::runtime_error("Hash table is full");
}

template < class Key, class Value, class Hash, class Equal >
bool novikov::HashTable< Key, Value, Hash, Equal >::contains(Key k) const
{
  size_t limit = std::log2(size_);
  size_t pos = Hash{}(k) & (size_ - 1);
  for (size_t i = 0; i < limit; ++i)
  {
    if (!slots_[pos].isEmpty && slots_[pos].key == k)
    {
      return true;
    }
    pos = (pos + i) & (size_ - 1);
  }
  return false;
}

template < class Key, class Value, class Hash, class Equal >
const Value& novikov::HashTable< Key, Value, Hash, Equal >::at(Key k) const
{
  size_t limit = std::log2(size_);
  size_t pos = Hash{}(k) & (size_ - 1);
  for (size_t i = 0; i < limit; ++i)
  {
    if (!slots_[pos].isEmpty && slots_[pos].key == k)
    {
      return slots_[pos].value;
    }
    pos = (pos + i) & (size_ - 1);
  }
  throw std::out_of_range("Entry not found");
}

template < class Key, class Value, class Hash, class Equal >
Value& novikov::HashTable< Key, Value, Hash, Equal >::at(Key k)
{
  const HashTable< Key, Value, Hash, Equal >* cthis = this;
  const Value& ret = cthis->at(k);
  return const_cast< Value& >(ret);
}

template < class Key, class Value, class Hash, class Equal >
Value& novikov::HashTable< Key, Value, Hash, Equal >::operator[](Key k)
{
  size_t limit = std::log2(size_);
  size_t pos = Hash{}(k) & (size_ - 1);
  for (size_t i = 0; i < limit; ++i)
  {
    if (!slots_[pos].isEmpty && slots_[pos].key == k)
    {
      return slots_[pos].value;
    }
    pos = (pos + i) & (size_ - 1);
  }

  pos = Hash{}(k) & (size_ - 1);
  for (size_t i = 0; i < limit; ++i)
  {
    if (slots_[pos].isEmpty)
    {
      slots_[pos] = {k, Value(), false};
      ++slotsCount_;
      return slots_[pos].value;
    }
    pos = (pos + i) & (size_ - 1);
  }
  throw std::runtime_error("Hash table is full");
}

#endif
