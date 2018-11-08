#ifndef __PROTO_COMMANDS_CMD_STUDENT_DRAW_END_H__
#define __PROTO_COMMANDS_CMD_STUDENT_DRAW_END_H__
#include "../base/command_cell_with_draw.h"

PROTO_NAMESPACE_BEGIN

class CmdStudentDrawEnd : public CommandCellWithDraw
{
	DECLARE_COMMAND_RUNTIME_INFO(CmdStudentDrawEnd)

public:
	virtual DrawOpInfo GetDrawInfo() const override;
};

PROTO_NAMESPACE_END

#endif