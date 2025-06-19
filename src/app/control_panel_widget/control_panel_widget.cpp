#include "control_panel_widget.hpp"

#include "panel_type.hpp"
#include "settings_defaults.hpp"
#include "spdlog_wrapper.hpp"

#include <QPushButton>
#include <QVBoxLayout>

namespace APP
{
ControlPanelWidget::ControlPanelWidget(QWidget* parent) : QWidget(parent)
{
	this->initialize();
}

ControlPanelWidget::~ControlPanelWidget()
{}

void ControlPanelWidget::addButton(const ButtonInfo& button_info)
{
	if (button_info.type >= PanelType::COUNT || button_info.type == PanelType::NONE)
	{
		SPD_ERROR_CLASS(UTILS::DEFAULTS::d_logger_control_panel, QObject::tr("pebkac_error_invalid_button"));
		return;
	}

	QPushButton* button = new QPushButton(button_info.text, this);
	button->setIcon(button_info.icon);
	button->setFixedSize(QSize(48, 48));
	button->setIconSize(QSize(32, 32));

	this->m_buttons.insert(button, button_info);

	connect(button, &QPushButton::clicked, [this, button_info] {
		emit buttonClicked(button_info.type);
	});
	this->m_main_layout->addWidget(button);
}

void ControlPanelWidget::initialize()
{
	this->setupUi();
	this->setupStyle();
	this->setupConnections();
}

void ControlPanelWidget::setupUi()
{
	this->m_main_layout = new QVBoxLayout();
	this->m_main_layout->setDirection(QBoxLayout::BottomToTop);
	this->m_main_layout->insertStretch(0, 1);
	this->setLayout(this->m_main_layout);

	ButtonInfo rsa_button			 = {PanelType::TEST_RSA, this->resolveButtonText(PanelType::TEST_RSA),
										this->resolveButtonIcon(PanelType::TEST_RSA)};
	ButtonInfo el_gamal_button		 = {PanelType::TEST_EL_GAMAL, this->resolveButtonText(PanelType::TEST_EL_GAMAL),
										this->resolveButtonIcon(PanelType::TEST_EL_GAMAL)};
	ButtonInfo diffie_hellman_button = {PanelType::TEST_DIFFIE_HELLMAN, this->resolveButtonText(PanelType::TEST_DIFFIE_HELLMAN),
										this->resolveButtonIcon(PanelType::TEST_DIFFIE_HELLMAN)};
	ButtonInfo performance_button	 = {PanelType::TEST_PERFORMANCE, this->resolveButtonText(PanelType::TEST_PERFORMANCE),
										this->resolveButtonIcon(PanelType::TEST_PERFORMANCE)};

	this->addButton(performance_button);
	this->addButton(el_gamal_button);
	this->addButton(diffie_hellman_button);
	this->addButton(rsa_button);
}

void ControlPanelWidget::setupConnections()
{}

void ControlPanelWidget::setupStyle()
{
	setStyleSheet("ControlPanelWidget { background: #1d2021; }"
				  "QLabel { color: #ebdbb2; }"
				  "QLineEdit { background-color: #282828; color: #ebdbb2; border: 2px solid #ebdbb2; }"
				  "QPushButton { background-color: #3c3836; color: #ebdbb2; border: 2px solid #ebdbb2; padding: 5px; "
				  "border-radius: 8px; font-weight: bold; }"
				  "QPushButton:hover { background-color: #504945; }"
				  "QPushButton:focus { outline: none; }"
				  "QGroupBox { color: #ebdbb2; border: 2px solid #ebdbb2; margin-top: 10px; }"
				  "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 0 5px; }");
}

QIcon ControlPanelWidget::resolveButtonIcon(PanelType type) const
{
	switch (type)
	{
		/*case PanelType::TEST_RSA:
			return QIcon(":/gui/app/icons/controls/viewer-menu-button.svg");
		case PanelType::TEST_EL_GAMAL:
			return QIcon(":/gui/app/icons/controls/settings-menu-button.svg");
		case PanelType::TEST_DIFFIE_HELLMAN:
			return QIcon(":/icons/stop.png");*/
		default:
			return QIcon();
	}
}

QString ControlPanelWidget::resolveButtonText(PanelType type) const
{
	switch (type)
	{
		case PanelType::TEST_RSA:
			return "RSA";
		case PanelType::TEST_EL_GAMAL:
			return "EG";
		case PanelType::TEST_DIFFIE_HELLMAN:
			return "DH";
		case PanelType::TEST_PERFORMANCE:
			return "PR";
		default:
			return "";
	}
}
} // namespace APP
