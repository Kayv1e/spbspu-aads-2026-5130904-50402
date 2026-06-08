#ifndef HASHER_HPP
#define HASHER_HPP
#include <array>
#include <cstddef>
#include <random>
#include <boost/hash2/siphash.hpp>
#include <boost/hash2/hash_append.hpp>
#include <boost/hash2/get_integral_result.hpp>

namespace novikov
{
  template <class Key>
  class SipHash
  {
    boost::hash2::siphash_64 h_;
    static std::array<unsigned char, 16> make_seed_16()
    {
      std::array<unsigned char, 16> s{};
      std::random_device rd;

      for (auto& b : s)
        b = static_cast<unsigned char>(rd());

      return s;
    }

  public:
    SipHash()
    {
      auto seed = make_seed_16();
      h_ = boost::hash2::siphash_64(seed.data(), seed.size());
    }

    explicit SipHash(std::array<unsigned char, 16> const& seed)
        : h_(seed.data(), seed.size())
    {
    }

    SipHash(unsigned char const *p, std::size_t n) : h_(p, n) {}

    std::size_t operator()(Key const &key) const
    {
      auto h = h_;
      boost::hash2::hash_append(h, {}, key);
      return boost::hash2::get_integral_result<std::size_t>(h);
    }
  };
}

#endif
