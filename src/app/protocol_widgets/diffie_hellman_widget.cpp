#include "diffie_hellman_widget.hpp"

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>

namespace APP
{

DiffieHellmanWidget::DiffieHellmanWidget(QWidget *parent) :
	QWidget(parent),
	m_exchanger(std::make_unique<CRYPTO::DiffieHellmanExchanger>())
{
	this->initialize();
}

void DiffieHellmanWidget::initialize()
{
	this->setupUi();
	this->setupConnections();
}

void DiffieHellmanWidget::setupUi()
{
	m_main_layout = new QVBoxLayout(this);

	auto dh_label = new QLabel(tr("dh_label"));

	m_log_output = new QTextEdit();
	m_log_output->setReadOnly(true);
	m_log_output->setPlaceholderText(tr("placeholder_log_output"));

	m_exchange_button = new QPushButton(tr("dh_perform_key_exchange"));
	m_exchange_button->setStyleSheet("font-weight: bold; padding: 10px;");

	auto participants_layout = new QHBoxLayout();

	auto alice_group  = new QGroupBox(tr("alice"));
	auto alice_layout = new QVBoxLayout();
	m_alice_input	  = new QTextEdit();
	m_alice_input->setPlaceholderText(tr("message_from_alice"));
	m_alice_encrypt_button = new QPushButton(tr("alice_encrypt_and_send"));
	alice_layout->addWidget(m_alice_input);
	alice_layout->addWidget(m_alice_encrypt_button);
	alice_group->setLayout(alice_layout);

	auto bob_group	= new QGroupBox(tr("bob"));
	auto bob_layout = new QVBoxLayout();
	m_bob_input		= new QTextEdit();
	m_bob_input->setPlaceholderText(tr("message_from_bob"));
	m_bob_encrypt_button = new QPushButton(tr("bob_encrypt_and_send"));
	bob_layout->addWidget(m_bob_input);
	bob_layout->addWidget(m_bob_encrypt_button);
	bob_group->setLayout(bob_layout);

	participants_layout->addWidget(alice_group);
	participants_layout->addWidget(bob_group);

	dh_label->setAlignment(Qt::AlignHCenter);

	m_main_layout->addWidget(dh_label, Qt::AlignHCenter);
	m_main_layout->addWidget(m_exchange_button);
	m_main_layout->addWidget(new QLabel(tr("exchange_log_label")));
	m_main_layout->addWidget(m_log_output, 1);
	m_main_layout->addLayout(participants_layout);

	m_alice_encrypt_button->setEnabled(false);
	m_bob_encrypt_button->setEnabled(false);
}

void DiffieHellmanWidget::setupConnections()
{
	connect(m_exchange_button, &QPushButton::clicked, this, &DiffieHellmanWidget::onPerformExchange);
	connect(m_alice_encrypt_button, &QPushButton::clicked, this, &DiffieHellmanWidget::onAliceEncrypt);
	connect(m_bob_encrypt_button, &QPushButton::clicked, this, &DiffieHellmanWidget::onBobEncrypt);
}

void DiffieHellmanWidget::onPerformExchange()
{
	m_log_output->clear();
	m_log_output->append(tr("dh_key_exchange_start"));

	m_alice = std::make_shared<CRYPTO::DiffieHellman>(1);
	m_bob	= std::make_shared<CRYPTO::DiffieHellman>(1);

	m_alice->init();
	m_bob->init();

	m_log_output->append(tr("alice_and_bob_generated_keys"));

	m_exchanger->init(m_alice, m_bob);
	auto [status, final_alice, final_bob] = m_exchanger->perform();

	if (status)
	{
		m_log_output->append(tr("dh_key_exchange_success"));
		m_log_output->append(tr("dh_alice_and_bob_has_shared_key"));

		m_keys_exchanged = true;
		m_alice_encrypt_button->setEnabled(true);
		m_bob_encrypt_button->setEnabled(true);
		m_exchange_button->setEnabled(false);
	}
	else
	{
		m_log_output->append(tr("dh_key_exchange_failure"));
	}
}

void DiffieHellmanWidget::onAliceEncrypt()
{
	if (!m_keys_exchanged)
		return;

	QString plaintext = m_alice_input->toPlainText();
	if (plaintext.isEmpty())
		return;

	m_log_output->append("\n" + QString(20, '='));
	m_log_output->append(tr("alice_sent %1").arg(plaintext));

	QString ciphertext	   = m_alice->encrypt(plaintext);
	QString decrypted_text = m_bob->decrypt(ciphertext);
	m_log_output->append(tr("bob_decrypted %1").arg(decrypted_text));

	if (plaintext != decrypted_text)
	{
		m_log_output->append(tr("plaintext_ciphertext_mismatch"));
	}
	else
	{
		m_log_output->append(tr("plaintext_ciphertext_match"));
	}

	m_alice_input->clear();
}

void DiffieHellmanWidget::onBobEncrypt()
{
	if (!m_keys_exchanged)
		return;

	QString plaintext = m_bob_input->toPlainText();
	if (plaintext.isEmpty())
		return;

	m_log_output->append("\n" + QString(20, '='));
	m_log_output->append(tr("bob_sent %1").arg(plaintext));

	QString ciphertext	   = m_bob->encrypt(plaintext);
	QString decrypted_text = m_alice->decrypt(ciphertext);
	m_log_output->append(tr("alice_decrypted %1").arg(decrypted_text));

	if (plaintext != decrypted_text)
	{
		m_log_output->append(tr("plaintext_ciphertext_mismatch"));
	}
	else
	{
		m_log_output->append(tr("plaintext_ciphertext_match"));
	}

	m_bob_input->clear();
}
} // namespace APP
