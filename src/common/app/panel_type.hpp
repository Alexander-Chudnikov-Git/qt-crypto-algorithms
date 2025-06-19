#ifndef PANEL_TYPE_HPP
#define PANEL_TYPE_HPP

#include <QMetaType>

namespace APP
{
Q_NAMESPACE

enum class PanelType : quint32
{
	NONE,
	TEST_RSA,
	TEST_EL_GAMAL,
	TEST_DIFFIE_HELLMAN,
	TEST_PERFORMANCE,
	COUNT
};
Q_ENUM_NS(PanelType)

enum class QUESTION_TYPE
{
	NONE,
	SELECTOR,
	INPUT
};

} // namespace APP
Q_DECLARE_METATYPE(APP::PanelType)

#endif // PANEL_TYPE_HPP
