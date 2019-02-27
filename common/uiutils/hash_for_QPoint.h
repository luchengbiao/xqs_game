#ifndef __UI_UTILS__HASH_FOR_QPOINT_H__
#define __UI_UTILS__HASH_FOR_QPOINT_H__
#include <functional> // for std::hash
#include <QPoint>

namespace std
{
	template<>
	struct hash<QPoint>
	{
		typedef QPoint argument_type;
		typedef size_t result_type;

		result_type operator()(const argument_type& rh) const
		{
			return hash<int>()(rh.x()) ^ (hash<int>()(rh.y()) << 1);
		}
	};
}

#endif