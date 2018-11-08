#ifndef __PROTO_BASE_COMMAND_VARIANT_H__
#define __PROTO_BASE_COMMAND_VARIANT_H__
#include <string>
#include <memory.h>
#include <utility>
#include <sstream>
#include "protocol/macros.h"

PROTO_NAMESPACE_BEGIN

struct Variant
{
	enum class Type
	{
		None		= -1,
		Int			= 0,
		LongLong	= 1,
		Float		= 2,
		String		= 3,
	};

	Variant() : _type(Type::None) { this->ZeroData(); }
	Variant(int i) : _type(Type::Int) { _d.i = i; }
	Variant(long long ll) : _type(Type::LongLong) { _d.ll = ll; }
	Variant(float f) : _type(Type::Float) { _d.f = f; }
	Variant(std::string str) : _type(Type::String) { _d.str = new std::string(std::move(str)); }
	~Variant() { if (Type::String == _type) { delete _d.str; } }
	Variant(const Variant& var) : _type(Type::None)  { this->CopyFrom(var); }
	Variant(Variant&& var) : _type(Type::None) { this->MoveFrom(std::move(var)); }
	Variant& operator=(const Variant& var) { this->CopyFrom(var);  return *this; }
	Variant& operator=(Variant&& var) { this->MoveFrom(std::move(var));  return *this; }

	std::string Info() const
	{
		std::stringstream ss;

		if (Type::Int == _type)
		{
			ss << "Int(" << ToInt();
		}
		else if (Type::LongLong == _type)
		{
			ss << "LongLong(" << ToLongLong();
		}
		else if (Type::Float == _type)
		{
			ss << "Float(" << ToFloat();
		}
		else if (Type::String == _type)
		{
			ss << "String(" << ToString();
		}

		ss << ")";

		return ss.str();
	}

	inline Type			GetType() const { return _type; }

	inline int			ToInt() const { return (Type::Int == _type) ? _d.i : static_cast<int>(_d.ll); }
	inline long	long	ToLongLong() const { return (Type::LongLong == _type) ? _d.ll : _d.i; }
	inline float		ToFloat() const { return (Type::Float == _type) ? _d.f : (float)_d.i; }
	inline std::string	ToString() const { return (Type::String == _type) ? (*_d.str) : std::string(); }
	
private:
	void CopyFrom(const Variant& var)
	{
		if (this != &var)
		{
			this->DeleteStringIf();

			_type = var._type;

			if (Type::String == _type)
			{
				_d.str = new std::string(*var._d.str);
			}
			else
			{
				memcpy(&_d, &(var._d), sizeof(D));
			}
		}
	}

	void MoveFrom(Variant&& var)
	{
		if (this != &var)
		{
			this->DeleteStringIf();

			_type = var._type;

			if (Type::String == _type)
			{
				_d.str = var._d.str;
			}
			else
			{
				memcpy(&_d, &(var._d), sizeof(D));
			}

			var._type = Type::None;
			var.ZeroData();
		}
	}

	inline void DeleteStringIf()
	{
		if (Type::String == _type)
		{
			delete _d.str;
			_d.str = nullptr;
		}
	}

	inline void ZeroData()
	{
		memset(&_d, 0, sizeof(D));
	}

private:
	Type _type;
	union D
	{
		int				i;
		long long		ll;
		float			f;
		std::string*	str;
		void*			ptr;
	}_d;
};

PROTO_NAMESPACE_END

#endif