#include "performance_test_widget.hpp"

#include "diffie_hellman.hpp"
#include "diffie_hellman_exchanger.hpp"
#include "elgamal.hpp"
#include "rsa.hpp"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QSplitter>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QtCharts/QLogValueAxis>
#include <QtCharts/QValueAxis>

namespace APP
{

PerformanceTestWidget::PerformanceTestWidget(QWidget* parent) : QWidget(parent), m_currentTestIndex(0)
{
	setupUi();
	setupConnections();
}

void PerformanceTestWidget::setupUi()
{
	m_mainLayout	= new QVBoxLayout(this);
	auto* topLayout = new QHBoxLayout();

	m_runButton = new QPushButton(tr("Запустить тесты производительности"));
	m_runButton->setStyleSheet("font-weight: bold; padding: 8px;");

	topLayout->addWidget(m_runButton);
	topLayout->addStretch();

	m_progressBar = new QProgressBar();
	m_progressBar->setVisible(false);
	m_progressBar->setTextVisible(true);
	m_progressBar->setFormat("%v / %m - %p%");

	// Создаем график
	m_chart		= new QChart();
	m_chartView = new QChartView(m_chart);
	m_chartView->setRenderHint(QPainter::Antialiasing);
	setupChart();

	// Создаем лог
	m_logOutput = new QTextEdit();
	m_logOutput->setReadOnly(true);
	m_logOutput->setFont(QFont("Monospace", 9));
	m_logOutput->setPlaceholderText(tr("Детальные результаты тестов будут отображены здесь."));

	// Разделитель для изменения размеров графика и лога
	QSplitter* splitter = new QSplitter(Qt::Vertical);
	splitter->addWidget(m_chartView);
	splitter->addWidget(m_logOutput);
	splitter->setStretchFactor(0, 3); // График занимает больше места
	splitter->setStretchFactor(1, 1);

	m_mainLayout->addLayout(topLayout);
	m_mainLayout->addWidget(m_progressBar);
	m_mainLayout->addWidget(splitter, 1);
}

void PerformanceTestWidget::setupChart()
{
	m_chart->setTitle("Производительность крипто-алгоритмов");
	m_chart->setAnimationOptions(QChart::SeriesAnimations);

	// Ось X (размер ключа) - логарифмическая
	auto* axisX = new QLogValueAxis();
	axisX->setBase(2); // Основание 2 идеально для нашего шага
	axisX->setTitleText("Размер ключа, бит");
	axisX->setLabelFormat("%d");
	m_chart->addAxis(axisX, Qt::AlignBottom);

	// Ось Y (время) - линейная
	auto* axisY = new QValueAxis();
	axisY->setTitleText("Время, мс");
	axisY->setLabelFormat("%d");
	axisY->setMinorTickCount(4);
	m_chart->addAxis(axisY, Qt::AlignLeft);

	m_chart->legend()->setVisible(true);
	m_chart->legend()->setAlignment(Qt::AlignRight);
}

void PerformanceTestWidget::setupConnections()
{
	connect(m_runButton, &QPushButton::clicked, this, &PerformanceTestWidget::onRunTestsClicked);
}

void PerformanceTestWidget::logMessage(const QString& message, bool isError)
{
	QString color = isError ? "red" : "black";
	m_logOutput->append(QString("<font color='%1'>%2</font>").arg(color).arg(message));
	QCoreApplication::processEvents(); // Обновляем UI
}

void PerformanceTestWidget::resetTests()
{
	logMessage("<h3>Сброс предыдущих результатов...</h3>");
	m_logOutput->clear();
	m_chart->removeAllSeries();
	for (auto* series : m_seriesMap.values())
	{
		delete series;
	}
	m_seriesMap.clear();

	// Отсоединяем оси от старых серий
	for (auto* axis : m_chart->axes())
	{
		m_chart->removeAxis(axis);
	}
	setupChart(); // Пересоздаем оси
}

void PerformanceTestWidget::onRunTestsClicked()
{
	resetTests();
	m_runButton->setEnabled(false);
	m_progressBar->setVisible(true);

	m_keySizesToTest.clear();
	for (int bits = 8; bits <= 4096; bits *= 2)
	{
		m_keySizesToTest.append(bits);
	}

	int totalTests = m_keySizesToTest.size() * 3; // 3 протокола
	m_progressBar->setRange(0, totalTests);
	m_currentTestIndex = 0;

	logMessage(QString("<h1>Запуск тестов для размеров ключей: 8 - 4096 бит</h1>"));

	// Запускаем первый тест асинхронно
	QTimer::singleShot(0, this, &PerformanceTestWidget::runNextTest);
}

void PerformanceTestWidget::runNextTest()
{
	if (m_currentTestIndex >= m_keySizesToTest.size())
	{
		logMessage("<h2>Все тесты завершены.</h2>");
		m_progressBar->setFormat(tr("Готово!"));
		m_runButton->setEnabled(true);
		return;
	}

	int keyBits = m_keySizesToTest[m_currentTestIndex];
	logMessage(QString("<h2>--- Тестирование с ключом %1 бит ---</h2>").arg(keyBits));

	testAllForKeySize(keyBits);

	m_currentTestIndex++;
	// Запускаем следующий тест после короткой паузы, чтобы UI успел обновиться
	QTimer::singleShot(10, this, &PerformanceTestWidget::runNextTest);
}

void PerformanceTestWidget::testAllForKeySize(int keySize)
{
	// Тест RSA
	m_progressBar->setValue(m_progressBar->value() + 1);
	testRSA(keySize);

	// Тест ElGamal
	m_progressBar->setValue(m_progressBar->value() + 1);
	testElGamal(keySize);

	// Тест Diffie-Hellman
	m_progressBar->setValue(m_progressBar->value() + 1);
	testDiffieHellman(keySize);
}

QLineSeries* PerformanceTestWidget::getSeries(const QString& name)
{
	if (!m_seriesMap.contains(name))
	{
		auto* series = new QLineSeries();
		series->setName(name);
		m_chart->addSeries(series);

		m_seriesMap.insert(name, series);

		series->attachAxis(m_chart->axes(Qt::Horizontal).first());
		series->attachAxis(m_chart->axes(Qt::Vertical).first());
	}
	return m_seriesMap.value(name);
}

void PerformanceTestWidget::addDataPoint(const QString& seriesName, int keyBits, qint64 timeMs)
{
	QLineSeries* series = getSeries(seriesName);
	series->append(keyBits, timeMs);
	logMessage(QString("    %1: <b>%2</b> мс").arg(seriesName).arg(timeMs));
}

void PerformanceTestWidget::testRSA(int keyBits)
{
	logMessage("<b>Протокол: RSA</b>");
	const QString testMessage = "Test message";
	QElapsedTimer timer;

	if (keyBits < 16)
	{
		logMessage("    <i>Пропущено: размер ключа слишком мал для RSA</i>", true);
		return;
	}

	try
	{
		// 1. Генерация ключей
		timer.start();
		CRYPTO::RSA rsa(keyBits);
		addDataPoint("RSA - Генерация", keyBits, timer.elapsed());

		// 2. Шифрование
		timer.restart();
		QString ciphertext = rsa.encrypt(testMessage);
		addDataPoint("RSA - Шифрование", keyBits, timer.elapsed());

		// 3. Расшифрование
		timer.restart();
		QString decryptedText = rsa.decrypt(ciphertext);
		addDataPoint("RSA - Расшифрование", keyBits, timer.elapsed());

		if (decryptedText != testMessage)
		{
			logMessage("    Корректность: ОШИБКА", true);
		}
	}
	catch (const std::exception& e)
	{
		logMessage(QString("    Исключение: %1").arg(e.what()), true);
	}
}

void PerformanceTestWidget::testElGamal(int keyBits)
{
	logMessage("<b>Протокол: ElGamal</b>");
	const QString testMessage = "Test message";
	QElapsedTimer timer;

	if (keyBits < 16)
	{
		logMessage("    <i>Пропущено: размер ключа слишком мал для ElGamal</i>", true);
		return;
	}

	try
	{
		// 1. Генерация ключей
		timer.start();
		CRYPTO::ElGamal elgamal(keyBits);
		addDataPoint("ElGamal - Генерация", keyBits, timer.elapsed());

		// 2. Шифрование
		timer.restart();
		QString ciphertext = elgamal.encrypt(testMessage);
		addDataPoint("ElGamal - Шифрование", keyBits, timer.elapsed());

		// 3. Расшифрование
		timer.restart();
		QString decryptedText = elgamal.decrypt(ciphertext);
		addDataPoint("ElGamal - Расшифрование", keyBits, timer.elapsed());

		if (decryptedText != testMessage)
		{
			logMessage("    Корректность: ОШИБКА", true);
		}
	}
	catch (const std::exception& e)
	{
		logMessage(QString("    Исключение: %1").arg(e.what()), true);
	}
}

void PerformanceTestWidget::testDiffieHellman(int keyBits)
{
	logMessage("<b>Протокол: Diffie-Hellman</b>");
	QElapsedTimer timer;

	if (keyBits < 8)
	{
		logMessage("    <i>Пропущено: размер ключа слишком мал</i>", true);
		return;
	}

	try
	{
		timer.start();
		auto alice = std::make_shared<CRYPTO::DiffieHellman>(keyBits);
		auto bob   = std::make_shared<CRYPTO::DiffieHellman>(keyBits);
		alice->init();
		bob->init();

		CRYPTO::DiffieHellmanExchanger exchanger;
		exchanger.init(alice, bob);
		auto [status, final_alice, final_bob] = exchanger.perform();

		if (status)
		{
			addDataPoint("DH - Обмен ключами", keyBits, timer.elapsed());
		}
		else
		{
			logMessage("    Обмен ключами: ОШИБКА", true);
		}
	}
	catch (const std::exception& e)
	{
		logMessage(QString("    Исключение: %1").arg(e.what()), true);
	}
}

} // namespace APP
