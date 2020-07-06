#pragma once

#include <cstdint>
#include <limits>

#define ST_ENUM_FLAG_OPS(Enum, Type)				\
	inline Type operator|(Enum bit0, Enum bit1 ) { return Type(bit0) | bit1; }

template<typename Bits, typename T = uint32_t>
class st_flags
{
public:
	constexpr st_flags() : _mask(0) {}
	st_flags(Bits bits) : _mask(static_cast<T>(bits)) {}
	st_flags(const st_flags<Bits, T>& rhs) : _mask(rhs._mask) {}
	explicit st_flags(T value) : _mask(value) {}

	st_flags<Bits, T> & operator=(st_flags<Bits, T> const& rhs)
	{
		_mask = rhs._mask;
		return *this;
	}

	st_flags<Bits, T> & operator|=(st_flags<Bits, T> const& rhs)
	{
		_mask |= rhs._mask;
		return *this;
	}

	st_flags<Bits, T> & operator&=(st_flags<Bits, T> const& rhs)
	{
		_mask &= rhs._mask;
		return *this;
	}

	st_flags<Bits, T> & operator^=(st_flags<Bits, T> const& rhs)
	{
		_mask ^= rhs._mask;
		return *this;
	}

	st_flags<Bits, T> operator|(st_flags<Bits, T> const& rhs) const
	{
		st_flags<Bits, T> result(*this);
		result |= rhs;
		return result;
	}

	st_flags<Bits, T> operator&(st_flags<Bits, T> const& rhs) const
	{
		st_flags<Bits> result(*this);
		result &= rhs;
		return result;
	}

	st_flags<Bits, T> operator^(st_flags<Bits, T> const& rhs) const
	{
		st_flags<Bits, T> result(*this);
		result ^= rhs;
		return result;
	}

	bool operator!() const
	{
		return !_mask;
	}

	st_flags<Bits, T> operator~() const
	{
		st_flags<Bits, T> result(*this);
		result._mask = std::numeric_limits<T>::max();
		return result;
	}

	bool operator==(st_flags<Bits, T> const& rhs) const
	{
		return _mask == rhs._mask;
	}

	bool operator!=(st_flags<Bits, T> const& rhs) const
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
