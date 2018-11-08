#ifndef __PROTO_COMMANDS_CMD_TEACHER_CLASS_UPLOAD_CHESS_MANUAL_H__
#define __PROTO_COMMANDS_CMD_TEACHER_CLASS_UPLOAD_CHESS_MANUAL_H__
#include "../base/command_cell.h"

PROTO_NAMESPACE_BEGIN

class CommandGroup;
typedef std::shared_ptr<CommandGroup> CommandGroupPtr;

class CommandFactory;

class CmdTeacherClassUploadChessManual : public CommandCell
{
	DECLARE_COMMAND_RUNTIME_INFO(CmdTeacherClassUploadChessManual)

public:
	void			SetChessManualFilePath(const std::wstring& file_path);

	CommandGroupPtr	GetCommandGroup(const CommandFactory&);

private:
	std::wstring	local_file_path_;
	CommandGroupPtr	cmd_group_;
};

PROTO_NAMESPACE_END

#endif