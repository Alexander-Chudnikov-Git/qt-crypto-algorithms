#ifndef RSA_WIDGET_HPP
#define RSA_WIDGET_HPP

#include "rsa.hpp"

#include <QVBoxLayout>
#include <QWidget>
#include <memory>

class QPushButton;
class QTextEdit;

namespace APP
{
class RSAWidget : public QWidget
{
	Q_OBJECT
public:
	explicit RSAWidget(QWidget *parent = nullptr);
	~RSAWidget() = default;

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

	std::unique_ptr<CRYPTO::RSA> m_rsa;
};
} // namespace APP

#endif // RSA_WIDGET_HPP
