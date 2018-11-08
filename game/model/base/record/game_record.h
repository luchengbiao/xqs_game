#ifndef __GAME_MODEL_BASE_RECORD_GAME_RECORD_H__
#define __GAME_MODEL_BASE_RECORD_GAME_RECORD_H__
#include <memory>
#include <vector>
#include "game_action_index_with_state.h"

GAME_NAMESPACE_BEGIN

class ChessInfoMatrix;
typedef std::shared_ptr<ChessInfoMatrix> ChessMatrixPtr;

class GameAction;
typedef std::shared_ptr<GameAction> GameActionPtr;

class GameActions : public std::vector<GameActionPtr>
{
public:
	GameActions() = default;
};
typedef std::shared_ptr<GameActions> GameActionsPtr;

typedef std::shared_ptr<GameActionIndices> GameActionIndicesPtr;

class Notations;
typedef std::shared_ptr<Notations> NotationsPtr;

class CustomRecord;
typedef std::shared_ptr<CustomRecord> CustomRecordPtr;

class GameRecord
{
public:
	GameRecord() = default;
	GameRecord(const ChessMatrixPtr& chesses, const GameActionIndicesPtr& actions, const NotationsPtr& notations, const CustomRecordPtr& custom_record)
		: chesses_(chesses), action_indices_(actions), notations_(notations), custom_record_(custom_record)
	{}

	ChessMatrixPtr			GetChesses() const { return chesses_; }
	GameActionIndicesPtr	GetActionIndices() const { return action_indices_; }
	NotationsPtr			GetNotations() const { return notations_; }

	void					SetCustomRecord(const CustomRecordPtr& custom_record) { custom_record_ = custom_record; }
	CustomRecordPtr			GetCustomRecord() const { return custom_record_; }
	template<typename CustomRecordT>
	std::shared_ptr<CustomRecordT> GetCustomRecord() const { return std::dynamic_pointer_cast<CustomRecordT>(custom_record_); }

protected:
	ChessMatrixPtr			chesses_;
	GameActionIndicesPtr	action_indices_;
	NotationsPtr			notations_;
	CustomRecordPtr			custom_record_;

	friend class GameBoardBase;
};

typedef std::shared_ptr<GameRecord> GameRecordPtr;

class GameRecords : public std::vector<GameRecordPtr>
{
public:
	GameRecords() = default;
};

typedef std::shared_ptr<GameRecords> GameRecordsPtr;

GAME_NAMESPACE_END

#endif