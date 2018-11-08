#ifndef __PROTO_BASE_COMMAND_H__
#define __PROTO_BASE_COMMAND_H__
#include <stddef.h> // for size_t
#include <string>
#include <functional>
#include "base/memory/objectcounter.h"
#include "command_variant.h"
#include "command_class_runtime_info.h"
#include "command_fwd.h"
#include "../command_types.h"

PROTO_NAMESPACE_BEGIN

class Command
#ifdef PROTO_TEST_MEMORY_LEAK
	: public nbase::ObjectCounter<Command>
#endif
{
public:
	virtual ~Command() = default;
	virtual const CommandClassRuntimeInfo&	GetClassRuntimeInfo() const = 0;

	CommandType								Type() const;
	std::string								ClassName() const;

	virtual int								UnpackFromString(const std::string& str, int startIndex) = 0;
	virtual std::string						PackToString() const = 0;

	virtual int								GetVariantCount() const;
	virtual Variant::Type					TypeOfVariantAt(size_t index) const;

	virtual int								IntOfVariantAt(size_t index) const;
	virtual long long						LongLongOfVariantAt(size_t index) const;
	virtual float							FloatOfVariantAt(size_t index) const;
	virtual std::string						StringOfVariantAt(size_t index) const;

	virtual void							PushIntVariant(int i);
	virtual void							PushLongLongVariant(long long ll);
	virtual void							PushFloatVariant(float f);
	virtual void							PushStringVariant(std::string str);

	virtual void							ForeachVariant(const std::function<void(const Variant&)>& callback) const;
	
	virtual bool							EqualTo(const Command& rh) const = 0;

	template<typename OStream = std::cout>
	void									Log(OStream& stream) { stream << this->StringOfLog(""); }
	std::string								StringOfLog(const std::string& strPrefix) const;

protected:
	Command();
	Command(const Command&) = delete;
	Command& operator=(const Command&) = delete;

	virtual std::string						_StringOfLog(const std::string& strPrefix) const;
};

PROTO_NAMESPACE_END

#endif