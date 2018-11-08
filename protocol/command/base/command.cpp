#include "command.h"
#include <sstream>

PROTO_NAMESPACE_BEGIN

Command::Command() = default;

CommandType Command::Type() const
{
	return this->GetClassRuntimeInfo().Type();
}

std::string Command::ClassName() const
{
	return this->GetClassRuntimeInfo().ClassName();
}

int Command::GetVariantCount() const 
{ 
	return 0; 
}

Variant::Type Command::TypeOfVariantAt(size_t index) const 
{ 
	return Variant::Type::None; 
}

int Command::IntOfVariantAt(size_t index) const 
{ 
	return 0; 
}

long long Command::LongLongOfVariantAt(size_t index) const 
{ 
	return 0; 
}

float Command::FloatOfVariantAt(size_t index) const 
{ 
	return 0.0f;
}

std::string Command::StringOfVariantAt(size_t index) const 
{ 
	return std::string(); 
}

void Command::PushIntVariant(int i) 
{}

void Command::PushLongLongVariant(long long ll) 
{}

void Command::PushFloatVariant(float f) 
{}

void Command::PushStringVariant(std::string str) 
{}

void Command::ForeachVariant(const std::function<void(const Variant&)>& callback) const 
{}

std::string Command::StringOfLog(const std::string& strPrefix) const
{
	std::stringstream ss;
	ss << strPrefix;
	ss << this->ClassName() << "(" << static_cast<int>(Type()) << ")" << " ( " << this->_StringOfLog("") << " )";

	return ss.str();
}

std::string Command::_StringOfLog(const std::string& strPrefix) const
{
	return strPrefix;
}

PROTO_NAMESPACE_END