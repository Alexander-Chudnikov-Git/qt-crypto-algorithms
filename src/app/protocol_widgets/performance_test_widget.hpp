#ifndef PERFORMANCE_TEST_WIDGET_HPP
#define PERFORMANCE_TEST_WIDGET_HPP

#include <QList>
#include <QMap>
#include <QWidget>

// Подключаем заголовки QtCharts
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

// Прямые объявления для остальных виджетов
class QPushButton;
class QTextEdit;
class QProgressBar;
class QVBoxLayout;

namespace APP
{
/**
 * @brief Виджет для проведения и визуализации тестов производительности
 *        криптографических протоколов на графике.
 */
class PerformanceTestWidget : public QWidget
{
	Q_OBJECT

public:
	explicit PerformanceTestWidget(QWidget* parent = nullptr);
	~PerformanceTestWidget() = default;

private:
	void setupUi();
	void setupConnections();
	void setupChart();

	// Методы для асинхронного тестирования
	void runNextTest();
	void testAllForKeySize(int keySize);

	// Методы для тестирования каждого протокола
	void testRSA(int keyBits);
	void testElGamal(int keyBits);
	void testDiffieHellman(int keyBits);

	// Вспомогательные методы
	void		 logMessage(const QString& message, bool isError = false);
	QLineSeries* getSeries(const QString& name);
	void		 addDataPoint(const QString& seriesName, int keyBits, qint64 timeMs);
	void		 resetTests();

private slots:
	void onRunTestsClicked();

private:
	QVBoxLayout*  m_mainLayout;
	QPushButton*  m_runButton;
	QProgressBar* m_progressBar;
	QTextEdit*	  m_logOutput;

	// Компоненты для графика
	QChartView*					m_chartView;
	QChart*						m_chart;
	QMap<QString, QLineSeries*> m_seriesMap;

	// Состояние асинхронного тестирования
	QList<int> m_keySizesToTest;
	int		   m_currentTestIndex;
};

} // namespace APP

#endif // PERFORMANCE_TEST_WIDGET_HPP
