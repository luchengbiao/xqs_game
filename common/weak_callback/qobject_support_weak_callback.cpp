#include "qobject_support_weak_callback.h"

namespace wcb
{
	QObjectWeakCallbackHelper::QObjectWeakCallbackHelper(QObject* host)
		: host_(host)
	{
		qRegisterMetaType<StdClosure>("StdClosure");
		connect(this, SIGNAL(SignalClosure(StdClosure)), this, SLOT(SlotClosure(StdClosure)));
	}

	void QObjectWeakCallbackHelper::SlotClosure(StdClosure closure)
	{
		if (closure && host_) { closure(); }
	}
}