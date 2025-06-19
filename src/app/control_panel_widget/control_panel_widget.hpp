#ifndef CONTROL_PANEL_WIDGET_HPP
#define CONTROL_PANEL_WIDGET_HPP

#include "panel_type.hpp"

#include <QMap>
#include <QWidget>

class QVBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;

namespace APP
{
class ControlPanelWidget : public QWidget
{
	Q_OBJECT
public:
	struct ButtonInfo
	{
		PanelType type = PanelType::NONE;
		QString	  text;
		QIcon	  icon;
	};

public:
	explicit ControlPanelWidget(QWidget *parent = nullptr);
	~ControlPanelWidget();

	void addButton(const ButtonInfo &button_info);

signals:
	void buttonClicked(PanelType type);

private:
	void initialize();
	void setupUi();
	void setupStyle();
	void setupConnections();

	QIcon	resolveButtonIcon(PanelType type) const;
	QString resolveButtonText(PanelType type) const;

private:
	QVBoxLayout *m_main_layout;

	QMap<QPushButton *, ButtonInfo> m_buttons;
};
} // namespace APP
#endif // CONTROL_PANEL_WIDGET_HPP
