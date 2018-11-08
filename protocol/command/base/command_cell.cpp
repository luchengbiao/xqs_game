#include "command_cell.h"
#include <algorithm>
#include <assert.h>
#include <sstream>
#include "protocol/utils/byte_parser.h"

PROTO_NAMESPACE_BEGIN

IMPLEMENT_COMMAND_RUNTIME_INFO(CommandType::CommandCell, CommandCell, Command)

int CommandCell::UnpackFromString(const std::string& str, int startIndex)
{
	int totalLen = str.length();

	do
	{
		int index = startIndex + 5;
		if (index >= totalLen) { break; }

		int varCount = str.at(index);

		index += 1;

		for (int i = 0; i < varCount; i++)
		{
			if (index >= totalLen) { break; }

			auto varType = (Variant::Type)str.at(index);
			index += 1;
			if (index >= totalLen) { break; }

			if (varType == Variant::Type::String)
			{
				int strLen = proto::byte_parser::Bytes2Int(&str.at(index));
				if (strLen < 0) { return 0; }

				index += 4;
				//if (index >= totalLen) { break; } // string can be empty

				if (index + strLen > totalLen) { break; } // too long
				std::string ss = str.substr(index, strLen);
				index += strLen;

				variants_.emplace_back(std::move(ss));
			}
			else if (varType == Variant::Type::LongLong)
			{
				if (index + 8 > totalLen) { break; }
				long long ll = proto::byte_parser::Bytes2Longlong(&str.at(index));
				index += 8;

				variants_.emplace_back(ll);
			}
			else if (varType == Variant::Type::Int)
			{
				if (index + 4 > totalLen) { break; }
				int j = proto::byte_parser::Bytes2Int(&str.at(index));
				index += 4;

				variants_.emplace_back(j);
			}
			else if (varType == Variant::Type::Float)
			{
				if (index + 4 > totalLen) { break; }
				float f = proto::byte_parser::Bytes2Float(&str.at(index));
				index += 4;

				variants_.emplace_back(f);
			}
			else
			{
#ifdef _DEBUG
				assert(false);
#endif
			}
		}

		return 1;

	} while (false);

	return 0;
}

std::string CommandCell::PackToString() const
{
	return std::string();
}

int CommandCell::GetVariantCount() const
{
	return variants_.size();
}

Variant::Type CommandCell::TypeOfVariantAt(size_t index) const
{
	return (index < variants_.size()) ? variants_.at(index).GetType() : Variant::Type::None;
}

int CommandCell::IntOfVariantAt(size_t index) const
{
	return (index < variants_.size()) ? variants_.at(index).ToInt() : 0;
}

long long CommandCell::LongLongOfVariantAt(size_t index) const
{
	return (index < variants_.size()) ? variants_.at(index).ToLongLong() : 0;
}

float CommandCell::FloatOfVariantAt(size_t index) const
{
	return (index < variants_.size()) ? variants_.at(index).ToFloat() : 0;
}

std::string CommandCell::StringOfVariantAt(size_t index) const
{
	return ((index >= 0) && (index < variants_.size())) ? variants_.at(index).ToString() : std::string();
}

void CommandCell::PushIntVariant(int i)
{
	variants_.emplace_back(i);
}

void CommandCell::PushLongLongVariant(long long ll)
{
	variants_.emplace_back(ll);
}

void CommandCell::PushFloatVariant(float f)
{
	variants_.emplace_back(f);
}

void CommandCell::PushStringVariant(std::string str)
{
	variants_.emplace_back(std::move(str));
}

void CommandCell::ForeachVariant(const std::function<void(const Variant&)>& callback) const
{
	std::for_each(variants_.cbegin(), variants_.cend(), callback);
}

bool CommandCell::EqualTo(const Command& rh) const
{
	auto cmd_cell_ptr = dynamic_cast<const CommandCell*>(&rh);
	if (!cmd_cell_ptr) { return false; }

	if (this == cmd_cell_ptr) { return true; }

	const CommandCell& cmd_cell = *cmd_cell_ptr;

	bool not_equal = true;

	if (Type() != cmd_cell.Type()) { return false; }

	const int var_count = GetVariantCount();
	if (var_count != cmd_cell.GetVariantCount()) { return false; }

	if (var_count > 0)
	{
		for (int i = 0; i < var_count; ++i)
		{
			const auto var_type = TypeOfVariantAt(i);
			if (var_type != cmd_cell.TypeOfVariantAt(i)) { return false; }

			if (var_type == Variant::Type::Int)
			{
				if (IntOfVariantAt(i) != cmd_cell.IntOfVariantAt(i)) { return false; }
			}
			else if (var_type == Variant::Type::LongLong)
			{
				if (LongLongOfVariantAt(i) != cmd_cell.LongLongOfVariantAt(i)) { return false; }
			}
			else if (var_type == Variant::Type::Float)
			{
				if (FloatOfVariantAt(i) != cmd_cell.FloatOfVariantAt(i)) { return false; }
			}
			else if (var_type == Variant::Type::String)
			{
				if (StringOfVariantAt(i) != cmd_cell.StringOfVariantAt(i)) { return false; }
			}
		}
	}

	return true;
}

std::string CommandCell::_StringOfLog(const std::string& strPrefix) const
{
	std::stringstream ss;
	ss << strPrefix;

	for (const auto& var : variants_)
	{
		ss << var.Info() << " ";
	}

	return ss.str();
}

PROTO_NAMESPACE_END