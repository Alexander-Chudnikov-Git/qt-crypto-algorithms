#ifndef DIFFIE_HELLMAN_WIDGET_HPP
#define DIFFIE_HELLMAN_WIDGET_HPP

#include "diffie_hellman.hpp"
#include "diffie_hellman_exchanger.hpp"

#include <QWidget>
#include <memory>

// Прямые объявления для уменьшения зависимостей в заголовке
class QVBoxLayout;
class QPushButton;
class QTextEdit;

namespace APP
{

/**
 * @brief Виджет для демонстрации и тестирования протокола обмена ключами Диффи-Хеллмана.
 * Симулирует взаимодействие двух сторон: Алисы и Боба.
 */
class DiffieHellmanWidget : public QWidget
{
	Q_OBJECT

public:
	explicit DiffieHellmanWidget(QWidget *parent = nullptr);
	~DiffieHellmanWidget() = default;

private:
	/**
	 * @brief Инициализирует виджет, вызывая настройку UI и соединений.
	 */
	void initialize();
	/**
	 * @brief Создает и размещает все элементы пользовательского интерфейса.
	 */
	void setupUi();
	/**
	 * @brief Настраивает соединения сигналов и слотов.
	 */
	void setupConnections();

private slots:
	/**
	 * @brief Слот, вызываемый при нажатии на кнопку "Выполнить обмен ключами".
	 * Запускает процесс установления общего секрета между Алисой и Бобом.
	 */
	void onPerformExchange();

	/**
	 * @brief Слот для симуляции отправки сообщения от Алисы к Бобу.
	 */
	void onAliceEncrypt();

	/**
	 * @brief Слот для симуляции отправки сообщения от Боба к Алисе.
	 */
	void onBobEncrypt();

private:
	// --- Основные компоненты протокола ---
	std::shared_ptr<CRYPTO::DiffieHellman>			m_alice;
	std::shared_ptr<CRYPTO::DiffieHellman>			m_bob;
	std::unique_ptr<CRYPTO::DiffieHellmanExchanger> m_exchanger;
	bool											m_keys_exchanged = false;

	QVBoxLayout *m_main_layout;

	QPushButton *m_exchange_button;
	QTextEdit	*m_log_output; 

	QTextEdit	*m_alice_input;
	QPushButton *m_alice_encrypt_button;

	QTextEdit	*m_bob_input;
	QPushButton *m_bob_encrypt_button;
};

} // namespace APP

#endif // DIFFIE_HELLMAN_WIDGET_HPP
