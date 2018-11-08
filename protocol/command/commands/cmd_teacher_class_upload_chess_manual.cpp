#include "cmd_teacher_class_upload_chess_manual.h"
#include "cmd_group_with_timestamp.h"
#include "zip/user_zip.h"

PROTO_NAMESPACE_BEGIN

IMPLEMENT_COMMAND_RUNTIME_INFO(CommandType::TeacherClassUploadChessManual, CmdTeacherClassUploadChessManual, CommandCell)

void CmdTeacherClassUploadChessManual::SetChessManualFilePath(const std::wstring& file_path)
{
	local_file_path_ = file_path;
}

CommandGroupPtr CmdTeacherClassUploadChessManual::GetCommandGroup(const CommandFactory& cmd_factory)
{
	if (cmd_group_) { return cmd_group_; }

	do
	{
		if (local_file_path_.empty()) { break; }

		std::string str = zip::UnCompressGz(local_file_path_);
		if (str.empty()) { break; }

		cmd_group_ = CmdGroupWithTimestamp::Create();

		int totalLen = str.length();
		for (int i = 0; i < totalLen;)
		{
			auto cmdGroupPtr = CmdGroupWithTimestamp::Create();

			int cmdLen = cmdGroupPtr->UnpackFromString(str, i);

			if (cmdLen > 0)
			{
				cmdGroupPtr->UnpackGroup(true, &cmd_factory);
			}
			else
			{
				break;
			}

			cmdGroupPtr->Foreach([=](const CommandSharedPtr& cmd_ptr){
				cmd_group_->Add(cmd_ptr);
			});

			i += cmdLen;
		}

	} while (false);

	return cmd_group_;
}

PROTO_NAMESPACE_END