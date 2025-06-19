#ifndef ELGAMAL_WIDGET_HPP
#define ELGAMAL_WIDGET_HPP

#include "elgamal.hpp"

#include <QVBoxLayout>
#include <QWidget>
#include <memory>

class QPushButton;
class QTextEdit;

namespace APP
{
class ElGamalWidget : public QWidget
{
	Q_OBJECT
public:
	explicit ElGamalWidget(QWidget *parent = nullptr);
	~ElGamalWidget() = default;

private:
	void initialize();
	void setupUi();
	void setupStyle();
	void setupConnections();

private slots:
	void onTextEncrypt();
	void onTextDecrypt();

private:
	QVBoxLayout *m_main_layout;

	QTextEdit *m_input_field;
	QTextEdit *m_output_field;

	QPushButton *m_encrypt_button;
	QPushButton *m_decrypt_button;

	std::unique_ptr<CRYPTO::ElGamal> m_elgamal;
};
} // namespace APP

#endif // ELGAMAL_WIDGET_HPP
