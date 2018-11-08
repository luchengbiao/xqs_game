#ifndef __PROTO_BASE_COMMAND_FWD_H__
#define __PROTO_BASE_COMMAND_FWD_H__
#include <memory>
#include "protocol/macros.h"

PROTO_NAMESPACE_BEGIN

class Command;
class CommandGroup;
class CommandFactory;

PROTO_NAMESPACE_END

using CommandSharedPtr = std::shared_ptr<proto::Command>;
using CommandUniquePtr = std::unique_ptr<proto::Command>;
using CommandWeakPtr = std::weak_ptr<proto::Command>;

#endif