#include "exchanger.hpp"

#include "settings_defaults.hpp"
#include "spdlog_wrapper.hpp"

#include <QObject>

namespace CRYPTO
{
Exchanger::Exchanger()
{}

void Exchanger::init(std::shared_ptr<Protocol> protocol_a, std::shared_ptr<Protocol> protocol_b)
{
	SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_exchanger, QObject::tr("style_major_line"));
	SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_exchanger, QObject::tr("state_machine_init_start"));
	SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_exchanger, QObject::tr("style_minor_line"));

	m_protocol_a = std::move(protocol_a);
	m_protocol_b = std::move(protocol_b);

	this->steps();

	this->reset();

	SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_exchanger, QObject::tr("style_minor_line"));
	SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_exchanger, QObject::tr("state_machine_init_done"));
	SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_exchanger, QObject::tr("style_major_line"));
}

/*
void Exchanger::steps()
{
	this->addStep("IdentifySubjectA", std::make_unique<IdentifySubjectA>());
	this->addStep("RequestCertificateA", std::make_unique<RequestCertificateA>());
	this->addStep("SubjectCertificateA", std::make_unique<SubjectCertificateA>());

	this->addStep("IdentifySubjectB", std::make_unique<IdentifySubjectB>());
	this->addStep("SubjectAuthenticateA", std::make_unique<SubjectAuthenticateA>());
	this->addStep("SubjectAuthenticateB", std::make_unique<SubjectAuthenticateB>());
}
*/

std::tuple<bool, std::shared_ptr<Protocol>, std::shared_ptr<Protocol>> Exchanger::perform()
{
	bool status = true;

	SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_exchanger, QObject::tr("state_machine_start"));
	while (this->m_current_step)
	{
		status = this->m_current_step->enter(*m_protocol_a.get(), *m_protocol_b.get());

		if (!status)
		{
			SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_exchanger, QObject::tr("style_minor_line"));
			SPD_ERROR_CLASS(UTILS::DEFAULTS::d_logger_exchanger, QObject::tr("state_machine_error_enter"));
			break;
		}

		status = this->m_current_step->execute(*m_protocol_a.get(), *m_protocol_b.get());

		if (!status)
		{
			SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_exchanger, QObject::tr("style_minor_line"));
			SPD_ERROR_CLASS(UTILS::DEFAULTS::d_logger_exchanger, QObject::tr("state_machine_error_execute"));
			break;
		}

		status = this->m_current_step->exit(*m_protocol_a.get(), *m_protocol_b.get());

		if (!status)
		{
			SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_exchanger, QObject::tr("style_minor_line"));
			SPD_ERROR_CLASS(UTILS::DEFAULTS::d_logger_exchanger, QObject::tr("state_machine_error_exit"));
			break;
		}

		this->nextStep();
	}

	this->reset();

	return std::make_tuple(status, m_protocol_a, m_protocol_b);
}

void Exchanger::reset()
{
	this->m_current_index = 0;

	if (!m_step_order.empty())
	{
		this->m_current_step = (this->m_steps[this->m_step_order[this->m_current_index]]);
	}
	else
	{
		this->m_current_step = nullptr;
	}
}

void Exchanger::addStep(const std::string& name, std::shared_ptr<ExchangerStep> step)
{
	this->m_steps[name] = (step);
	this->m_step_order.push_back(name);

	if (this->m_step_order.size() == 1)
	{
		this->m_current_step = (this->m_steps[name]);
	}
	SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_exchanger,
				   QObject::tr("state_machine_step_added %1 %2").arg(this->m_step_order.size()).arg(name));
}

void Exchanger::changeStep(const std::string& name)
{
	if (this->m_steps.count(name))
	{
		this->m_current_step = (this->m_steps[name]);

		for (size_t i = 0; i < this->m_step_order.size(); ++i)
		{
			if (this->m_step_order[i] == name)
			{
				this->m_current_index = i;
				break;
			}
		}
		SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_exchanger, QObject::tr("state_machine_step_switched %1").arg(name));
	}
	else
	{
		this->m_current_step = nullptr;
		SPD_WARN_CLASS(UTILS::DEFAULTS::d_logger_exchanger, QObject::tr("state_machine_step_missing %1").arg(name));
	}
}

void Exchanger::nextStep()
{
	if (this->m_current_index < this->m_step_order.size() - 1)
	{
		this->m_current_index++;
		this->m_current_step = (this->m_steps[this->m_step_order[this->m_current_index]]);
		SPD_INFO_CLASS(UTILS::DEFAULTS::d_logger_exchanger,
					   QObject::tr("state_machine_step_switched %1").arg(this->m_step_order[this->m_current_index]));
	}
	else
	{
		this->m_current_step = nullptr;
		SPD_WARN_CLASS(UTILS::DEFAULTS::d_logger_exchanger, QObject::tr("state_machine_step_final"));
	}
}
} // namespace CRYPTO
