#ifndef __PROTO_BASE_COMMAND_CELL_H__
#define __PROTO_BASE_COMMAND_CELL_H__
#include <vector>
#include "command.h"

PROTO_NAMESPACE_BEGIN

class CommandCell : public Command
{
	DECLARE_COMMAND_RUNTIME_INFO(CommandCell)

public:
	virtual int				UnpackFromString(const std::string& str, int startIndex) override;
	virtual std::string		PackToString() const override;

	virtual int				GetVariantCount() const override;
	virtual Variant::Type	TypeOfVariantAt(size_t index) const;

	virtual int				IntOfVariantAt(size_t index) const override;
	virtual long long		LongLongOfVariantAt(size_t index) const override;
	virtual float			FloatOfVariantAt(size_t index) const override;
	virtual std::string		StringOfVariantAt(size_t index) const override;

	virtual void			PushIntVariant(int i) override;
	virtual void			PushLongLongVariant(long long ll) override;
	virtual void			PushFloatVariant(float f) override;
	virtual void			PushStringVariant(std::string str) override;

	virtual void			ForeachVariant(const std::function<void(const Variant&)>& callback) const override;

	virtual bool			EqualTo(const Command& rh) const override;

protected:
	virtual std::string		_StringOfLog(const std::string& strPrefix) const override;

protected:
	std::vector<Variant>	variants_;
};

PROTO_NAMESPACE_END

#endif