#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QMainWindow>
#include <QMoveEvent>
#include <QResizeEvent>

class QGridLayout;
class QSplitter;
class QTimer;

namespace APP
{
class ControlPanelWidget;
class UserPanelWidget;
} // namespace APP

namespace APP
{
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private:
	void initialize();
	void setupUi();
	void setupStyle();
	void setupConnections();

	void saveWindowRect();

private:
	void moveEvent(QMoveEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;

private:
	QGridLayout *m_main_layout;
	QSplitter	*m_splitter;

	ControlPanelWidget *m_control_panel;
	UserPanelWidget	   *m_user_panel;

	QTimer *m_move_resize_timer;
};
} // namespace APP

#endif // MAIN_WINDOW_HPP
