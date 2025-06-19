#include "user_panel_widget.hpp"

#include "diffie_hellman_widget.hpp"
#include "elgamal_widget.hpp"
#include "performance_test_widget.hpp"
#include "rsa_widget.hpp"
#include "settings_manager.hpp"
#include "spdlog_wrapper.hpp"

#include <QLabel>
#include <QRegularExpression>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

namespace APP
{
UserPanelWidget::UserPanelWidget(QWidget *parent) : QWidget(parent)
{
	this->initialize();
}

UserPanelWidget::~UserPanelWidget()
{}

void UserPanelWidget::addScreen(const ScreenInfo &screen_info)
{
	if (screen_info.type == PanelType::NONE)
	{
		SPD_ERROR_CLASS(UTILS::DEFAULTS::d_logger_user_panel, QObject::tr("pebkac_error_invalid_screen"));
		return;
	}

	auto index = this->m_main_layout->addWidget(screen_info.widget);

	this->m_screens.insert(index, screen_info);
}

void UserPanelWidget::switchScreen(PanelType type)
{
	int index = -1;
	for (auto it = this->m_screens.begin(); it != this->m_screens.end(); ++it)
	{
		if (it.value().type == type)
		{
			if (this->m_main_layout->currentIndex() == it.key())
			{
				return;
			}
			this->m_main_layout->setCurrentIndex(it.key());
			UTILS::SettingsManager::instance()->setDirectValue(UTILS::SettingsManager::Setting::LAST_OPEN_PANEL,
															   QVariant::fromValue(it.value().type));
			return;
		}
	}
	SPD_ERROR_CLASS(UTILS::DEFAULTS::d_logger_user_panel, QObject::tr("pebkac_error_invalid_screen"));
	return;
}

void UserPanelWidget::nextScreen()
{
	int index = this->m_main_layout->currentIndex();
	if (index == this->m_screens.size() - 1)
	{
		return;
	}

	this->m_main_layout->setCurrentIndex(index + 1);
	UTILS::SettingsManager::instance()->setDirectValue(UTILS::SettingsManager::Setting::LAST_OPEN_PANEL,
													   QVariant::fromValue(this->m_screens[index + 1].type));
	return;
}

void UserPanelWidget::initialize()
{
	this->setupUi();
	this->setupStyle();
	this->setupConnections();
}

void UserPanelWidget::setupUi()
{
	this->m_main_layout = new QStackedWidget();
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(this->m_main_layout);
	this->setLayout(layout);

	ScreenInfo rsa_screen			 = {PanelType::TEST_RSA, this->resolveScreenText(PanelType::TEST_RSA),
										this->resolveScreenWidget(PanelType::TEST_RSA)};
	ScreenInfo el_gamal_screen		 = {PanelType::TEST_EL_GAMAL, this->resolveScreenText(PanelType::TEST_EL_GAMAL),
										this->resolveScreenWidget(PanelType::TEST_EL_GAMAL)};
	ScreenInfo diffie_hellman_screen = {PanelType::TEST_DIFFIE_HELLMAN, this->resolveScreenText(PanelType::TEST_DIFFIE_HELLMAN),
										this->resolveScreenWidget(PanelType::TEST_DIFFIE_HELLMAN)};
	ScreenInfo performance_screen	 = {PanelType::TEST_PERFORMANCE, this->resolveScreenText(PanelType::TEST_PERFORMANCE),
										this->resolveScreenWidget(PanelType::TEST_PERFORMANCE)};

	this->addScreen(rsa_screen);
	this->addScreen(el_gamal_screen);
	this->addScreen(diffie_hellman_screen);
	this->addScreen(performance_screen);

	switchScreen(
		UTILS::SettingsManager::instance()->getValue(UTILS::SettingsManager::Setting::LAST_OPEN_PANEL).value<PanelType>());
}

void UserPanelWidget::setupConnections()
{}

void UserPanelWidget::setupStyle()
{
	setStyleSheet("UserPanelWidget { background: #181818; }"
				  "QLabel { color: #EEEEEE; }");
}

QWidget *UserPanelWidget::resolveScreenWidget(PanelType type)
{
	switch (type)
	{
		case PanelType::TEST_RSA: {
			auto widget = new RSAWidget();

			return std::move(widget);
		}
		case PanelType::TEST_EL_GAMAL: {
			auto widget = new ElGamalWidget();

			return std::move(widget);
		}
		case PanelType::TEST_DIFFIE_HELLMAN: {
			auto widget = new DiffieHellmanWidget();

			return std::move(widget);
		}
		case PanelType::TEST_PERFORMANCE: {
			auto widget = new PerformanceTestWidget();

			return std::move(widget);
		}
		default: {
			return new QWidget();
		}
	}
}

QString UserPanelWidget::resolveScreenText(PanelType type) const
{
	switch (type)
	{
		default:
			return "";
	}
}
} // namespace APP
