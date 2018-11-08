#include "cmd_student_draw_end.h"

PROTO_NAMESPACE_BEGIN

IMPLEMENT_COMMAND_RUNTIME_INFO(CommandType::StudentDrawEnd, CmdStudentDrawEnd, CommandCellWithDraw)

DrawOpInfo CmdStudentDrawEnd::GetDrawInfo() const
{
	DrawOpInfo draw;

	draw.draw_op_type_ = DrawOpType::DrawOpEnd;
	draw.x_ = this->FloatOfVariantAt(0);
	draw.y_ = this->FloatOfVariantAt(1);

	return draw;
}


PROTO_NAMESPACE_END