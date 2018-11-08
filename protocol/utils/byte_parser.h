#ifndef __PROTO_UTILS_BYTE_PARSER_H__
#define __PROTO_UTILS_BYTE_PARSER_H__
#include <stddef.h> // for size_t
#include <stdint.h> // for uint8_t
#include <string>
#include "protocol/macros.h"

PROTO_NAMESPACE_BEGIN

namespace byte_parser
{
	class EndianChecker
	{
	public:
		static bool IsLittleEndian()
		{
			static EndianChecker checker;

			return checker.u_.c == 1;
		}

	private:
		EndianChecker()
		{
			u_.i = 1;
		}

		union
		{
			int		i;
			char	c;
		}u_;
	};

#define IS_LITTLE_ENDIAN (EndianChecker::IsLittleEndian())

	// Read an integer (signed or unsigned) from |buf|
	template<typename T>
	inline void ReadFromBytes(const char buf[], T* out, bool read_as_lb_order = true)
	{
		if (IS_LITTLE_ENDIAN == read_as_lb_order)
		{
			*out = buf[0];
			for (size_t i = 1; i < sizeof(T); ++i)
			{
				*out <<= 8;
				// Must cast to uint8_t to avoid clobbering by sign extension.
				*out |= static_cast<uint8_t>(buf[i]);
			}
		}
		else
		{
			*out = buf[sizeof(T) - 1];
			for (int i = sizeof(T) - 2; i >= 0; --i)
			{
				*out <<= 8;
				*out |= static_cast<uint8_t>(buf[i]);
			}
		}
	}

	template<typename T>
	inline T ReadFromBytes(const char buf[], bool read_as_lb_order = true)
	{
		T t;

		ReadFromBytes(buf, &t, read_as_lb_order);

		return t;
	}

	// Write an integer (signed or unsigned) |val| to |buf|
	template<typename T>
	inline void WriteToBytes(char buf[], T val, bool write_as_lb_order = true)
	{
		if (IS_LITTLE_ENDIAN == write_as_lb_order)
		{
			for (size_t i = 0; i < sizeof(T); ++i)
			{
				buf[sizeof(T)-i - 1] = static_cast<char>(val & 0xFF);
				val >>= 8;
			}
		}
		else
		{
			for (size_t i = 0; i < sizeof(T); ++i)
			{
				buf[i] = static_cast<char>(val & 0xFF);
				val >>= 8;
			}
		}
	}

	inline int Bytes2Int(const char bytes[], bool read_as_lb_order = true)
	{
		return ReadFromBytes<int>(bytes, read_as_lb_order);
	}

	inline void Int2Bytes(std::string& ss, int i, bool write_as_lb_order = true)
	{
		ss.resize(4);
		
		WriteToBytes(&ss[0], i, write_as_lb_order);
	}

	inline long long Bytes2Longlong(const char *bytes, bool read_as_lb_order = true)
	{
		return ReadFromBytes<long long>(bytes, read_as_lb_order);
	}

	inline void Longlong2Bytes(std::string& ss, long long ll, bool write_as_lb_order = true)
	{
		ss.resize(8);

		WriteToBytes(&ss[0], ll, write_as_lb_order);
	}

	union IntUFloat 
	{
		float	f;
		int		i;
	};

	inline float Bytes2Float(const char *bytes, bool read_as_lb_order = true)
	{
		IntUFloat iuf;

		iuf.i = Bytes2Int(bytes, read_as_lb_order);

		return iuf.f;
	}

	inline void Float2Bytes(std::string& ss, float f, bool write_as_lb_order = true)
	{
		IntUFloat iuf;
		iuf.f = f;

		Int2Bytes(ss, iuf.i, write_as_lb_order);
	}
}

PROTO_NAMESPACE_END

#endif