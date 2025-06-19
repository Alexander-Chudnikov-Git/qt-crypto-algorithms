#include "elgamal_widget.hpp"

#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

namespace APP
{
ElGamalWidget::ElGamalWidget(QWidget *parent) : QWidget(parent), m_elgamal(std::move(new CRYPTO::ElGamal(2048)))
{
	this->initialize();
}

void ElGamalWidget::initialize()
{
	this->setupUi();
	this->setupStyle();
	this->setupConnections();
}

void ElGamalWidget::setupUi()
{
	this->m_main_layout = new QVBoxLayout();

	auto rsa_label = new QLabel(tr("el_gamal_label"));

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

void ElGamalWidget::setupStyle()
{}

void ElGamalWidget::setupConnections()
{
	connect(this->m_encrypt_button, &QPushButton::pressed, this, &ElGamalWidget::onTextEncrypt);
	connect(this->m_decrypt_button, &QPushButton::pressed, this, &ElGamalWidget::onTextDecrypt);
}

void ElGamalWidget::onTextEncrypt()
{
	auto chiphertext = this->m_elgamal->encrypt(this->m_input_field->toPlainText());
	this->m_output_field->setText(chiphertext);
}

void ElGamalWidget::onTextDecrypt()
{
	auto chiphertext = this->m_elgamal->decrypt(this->m_input_field->toPlainText());
	this->m_output_field->setText(chiphertext);
}
} // namespace APP
