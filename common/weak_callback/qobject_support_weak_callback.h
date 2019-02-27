#ifndef __COMMON_WEAK_CALLBACK_QOBJECT_SUPPORT_WEAK_CALLBACK_H__
#define __COMMON_WEAK_CALLBACK_QOBJECT_SUPPORT_WEAK_CALLBACK_H__
#include <type_traits>
#include <QObject>
#include <QPointer>
#include "base/callback/callback.h" // for StdClosure
#include "weak_callback.h"

namespace wcb
{
	class QObjectWeakCallbackHelper;
	
	template<typename ObjectT, typename = std::enable_if_t<std::is_base_of<QObject, ObjectT>::value>>
	class QObjectSupportWeakCallback
	{
		typedef std::shared_ptr<QObjectWeakCallbackHelper> QObjectWeakCallbackHelperSharedPtr;

	public:
		QObjectSupportWeakCallback(ObjectT* obj_ptr) : callback_helper_(std::make_shared<QObjectWeakCallbackHelper>(obj_ptr)) {}

		inline QObjectWeakCallbackHelperSharedPtr CallbackHelper() const { return callback_helper_; }

	protected:
		QObjectWeakCallbackHelperSharedPtr callback_helper_;
	};

	class QObjectWeakCallbackHelper : public QObject, public SupportWeakCallback<QObjectWeakCallbackHelper>
	{
		Q_OBJECT

	public:
		QObjectWeakCallbackHelper(QObject* host);

	public:
		inline void			EmitClosureToHost(const StdClosure& closure) { emit this->SignalClosure(closure); }

	private:
		Q_SIGNAL void		SignalClosure(StdClosure);
		Q_SLOT void			SlotClosure(StdClosure);

	private:
		QPointer<QObject>	host_;
	};
}

#endif