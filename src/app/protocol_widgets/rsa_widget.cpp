#include "rsa_widget.hpp"

#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <qpushbutton.h>

namespace APP
{
RSAWidget::RSAWidget(QWidget *parent) : QWidget(parent), m_rsa(std::move(new CRYPTO::RSA(2048)))
{
	this->initialize();
}

void RSAWidget::initialize()
{
	this->setupUi();
	this->setupStyle();
	this->setupConnections();
}

void RSAWidget::setupUi()
{
	this->m_main_layout = new QVBoxLayout();

	auto rsa_label = new QLabel(tr("rsa_label"));

	this->m_input_field	 = new QTextEdit();
	this->m_output_field = new QTextEdit();
	// this->m_output_field->setDisabled(true);

	auto button_layout	   = new QHBoxLayout();
	this->m_encrypt_button = new QPushButton(tr("encrypt"));
	this->m_decrypt_button = new QPushButton(tr("decrypt"));

	button_layout->addWidget(this->m_encrypt_button);
	button_layout->addWidget(this->m_decrypt_button);

	rsa_label->setAlignment(Qt::AlignHCenter);

	this->m_main_layout->addWidget(rsa_label, Qt::AlignHCenter);
	this->m_main_layout->addWidget(this->m_input_field);
	this->m_main_layout->addWidget(this->m_output_field);
	this->m_main_layout->addLayout(button_layout);
	this->m_main_layout->addStretch();

	this->setLayout(this->m_main_layout);
}

void RSAWidget::setupStyle()
{}

void RSAWidget::setupConnections()
{
	connect(this->m_encrypt_button, &QPushButton::pressed, this, &RSAWidget::onTextEncrypt);
	connect(this->m_decrypt_button, &QPushButton::pressed, this, &RSAWidget::onTextDecrypt);
}

void RSAWidget::onTextEncrypt()
{
	auto chiphertext = this->m_rsa->encrypt(this->m_input_field->toPlainText());
	this->m_output_field->setText(chiphertext);
}

void RSAWidget::onTextDecrypt()
{
	auto chiphertext = this->m_rsa->decrypt(this->m_input_field->toPlainText());
	this->m_output_field->setText(chiphertext);
}
} // namespace APP
