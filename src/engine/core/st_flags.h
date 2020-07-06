#pragma once

#include <cstdint>
#include <limits>

template<typename Bits, typename T = uint32_t>
class st_flags
{
public:
	constexpr st_flags() : _mask(0) {}
	st_flags(Bits bits) : _mask(static_cast<T>(bits)) {}
	st_flags(st_flags<T, Bits>& rhs) : _mask(rhs._mask) {}
	explicit st_flags(T value) : _mask(value) {}

	st_flags<Bits> & operator=(st_flags<Bits> const& rhs)
	{
		_mask = rhs._mask;
		return *this;
	}

	st_flags<Bits> & operator|=(st_flags<Bits> const& rhs)
	{
		_mask |= rhs._mask;
		return *this;
	}

	st_flags<Bits> & operator&=(st_flags<Bits> const& rhs)
	{
		_mask &= rhs._mask;
		return *this;
	}

	st_flags<Bits> & operator^=(st_flags<Bits> const& rhs)
	{
		_mask ^= rhs._mask;
		return *this;
	}

	st_flags<Bits> operator|(st_flags<Bits> const& rhs) const
	{
		st_flags<Bits> result(*this);
		result |= rhs;
		return result;
	}

	st_flags<Bits> operator&(st_flags<Bits> const& rhs) const
	{
		st_flags<Bits> result(*this);
		result &= rhs;
		return result;
	}

	st_flags<Bits> operator^(st_flags<Bits> const& rhs) const
	{
		st_flags<Bits> result(*this);
		result ^= rhs;
		return result;
	}

	bool operator!() const
	{
		return !_mask;
	}

	st_flags<Bits> operator~() const
	{
		st_flags<Bits> result(*this);
		result._mask = std::numeric_limits<T>::max();
		return result;
	}

	bool operator==(st_flags<Bits> const& rhs) const
	{
		return _mask == rhs._mask;
	}

	bool operator!=(st_flags<Bits> const& rhs) const
	{
		return _mask != rhs._mask;
	}

	explicit operator bool() const
	{
		return !!_mask;
	}

	explicit operator T() const
	{
		return _mask;
	}
	
private:
	T _mask;
};
