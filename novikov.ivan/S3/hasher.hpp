#ifndef HASHER_HPP
#define HASHER_HPP
#include <boost/hash2/get_integral_result.hpp>
#include <boost/hash2/hash_append.hpp>
#include <boost/hash2/siphash.hpp>
#include <random>
#include <array>

namespace novikov
{
  template< class T >
  class SipHash
  {
  public:
    SipHash();
    size_t operator()(const T& v) const;

  private:
    boost::hash2::siphash_64 h_;
  };
}

template< class T >
novikov::SipHash< T >::SipHash()
{
  std::random_device rd;
  std::array< unsigned char, 16 > random_seed;
  
  for (size_t i = 0; i < random_seed.size(); ++i)
  {
    random_seed[i] = static_cast< unsigned char >(rd());
  }
  
  h_ = boost::hash2::siphash_64(random_seed);
}

template< class T >
size_t novikov::SipHash< T >::operator()(const T& v) const
{
  boost::hash2::siphash_64 h(h_);
  boost::hash2::hash_append(h, boost::hash2::default_flavor{}, v);
  return boost::hash2::get_integral_result< size_t >(h);
}

#endif
