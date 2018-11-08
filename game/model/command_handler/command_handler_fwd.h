#ifndef __GAME_MODEL_COMMAND_HANDLER_COMMAND_HANDLER_FWD_H__
#define __GAME_MODEL_COMMAND_HANDLER_COMMAND_HANDLER_FWD_H__
#include <memory>
#include "game/macros.h"

GAME_NAMESPACE_BEGIN

class CommandHandler;

typedef std::shared_ptr<CommandHandler> CommandHandlerPtr;

GAME_NAMESPACE_END

#endif