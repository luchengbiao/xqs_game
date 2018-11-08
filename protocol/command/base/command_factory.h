#ifndef __PROTO_BASE_COMMAND_FACTORY_H__
#define __PROTO_BASE_COMMAND_FACTORY_H__
#include <vector>
#include "command.h"

PROTO_NAMESPACE_BEGIN

class CommandFactory
{
public:
	virtual ~CommandFactory() = default;

	virtual CommandUniquePtr				CreateCommandWithType(CommandType) const = 0;

	virtual std::vector<CommandSharedPtr>	CreateCommandsFromString(const std::string& str_raw) const;
};

PROTO_NAMESPACE_END

#endif