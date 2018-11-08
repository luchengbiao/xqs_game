#ifndef __GAME_MODEL_MESSAGE_PROCESSOR_MESSAGE_PROCESSOR_H__
#define __GAME_MODEL_MESSAGE_PROCESSOR_MESSAGE_PROCESSOR_H__
#include <stdint.h>
#include <memory>
#include <string>
#include <QObject>
#include "base/framework/observer_list.h"
#include "game/macros.h"
#include "channel_manager/channel_manager_global.h"
#include "protocol/command/base/command_fwd.h"

GAME_NAMESPACE_BEGIN

class MessageProcessor : public QObject
{
	Q_OBJECT

	using CommandFactoryPtr = std::shared_ptr<proto::CommandFactory>;

public:
	class CommandObserver
	{
	public:
		virtual ~CommandObserver() = default;

		virtual void OnCommand(const CommandSharedPtr& cmd_ptr, bool sender_is_self) = 0;
	};
	using CommandObserverList = ObserverList<CommandObserver>;

public:
	MessageProcessor(const std::string& msg_channel_account, NetworkServiceType msg_channel_service, const CommandFactoryPtr& cmd_factory);
	~MessageProcessor();

	MessageProcessor(const MessageProcessor&) = delete;
	MessageProcessor& operator=(const MessageProcessor&) = delete;

	void				AddCommandObserver(CommandObserver*);
	void				RemoveCommandObserver(CommandObserver*);

private:
	bool				ChannelMessageIsEncoded() const { return msg_channel_service_ != SWITCH_NETWORK_NETEASE; }
	void				DecodeMessageIfEncoded(std::string& msg);

	void				HandleCommandGroup(const proto::CommandGroup& cmd_group, bool sender_is_self);
	
private	slots:
	void				SlotAgoraMessageChannel(std::string channel_id, std::string account, uint32_t uid, std::string msg);

private:
	std::string			msg_channel_account_;
	NetworkServiceType	msg_channel_service_{ SWITCH_NETWORK_AGORA };
	CommandFactoryPtr	cmd_factory_;
	CommandObserverList cmd_observers_;
};

GAME_NAMESPACE_END

#endif