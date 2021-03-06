/* 
 *
 *  Created on: 26.10.2010
 *      Author: Antti Kamppi
 */

#include "alternateregister.h"
#include "registerdefinition.h"
#include "GenericVendorExtension.h"
#include "parameter.h"

#include <IPXACTmodels/validators/ParameterValidator.h>

#include <QXmlStreamWriter>
#include <QDomNode>
#include <QString>
#include <QList>
#include <QSharedPointer>
#include "XmlUtils.h"

//-----------------------------------------------------------------------------
// Function: AlternateRegister::AlternateRegister()
//-----------------------------------------------------------------------------
AlternateRegister::AlternateRegister(QDomNode& registerNode):
RegisterModel(registerNode), alternateGroups_(), alternateRegisterDef_(),
vendorExtensions_()
{

	for (int i = 0; i < registerNode.childNodes().count(); ++i) {
		QDomNode tempNode = registerNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:alternateGroups")) {

			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QString groupName = tempNode.childNodes().at(j).nodeValue();
				alternateGroups_.append(XmlUtils::removeWhiteSpace(groupName));
			}
		}
        else if (tempNode.nodeName() == QString("spirit:vendorExtensions")) 
        {
            int extensionCount = tempNode.childNodes().count();
            for (int j = 0; j < extensionCount; ++j) {
                QDomNode extensionNode = tempNode.childNodes().at(j);
                vendorExtensions_.append(QSharedPointer<VendorExtension>(new GenericVendorExtension(extensionNode)));
            }
        }
	}
	alternateRegisterDef_ = QSharedPointer<RegisterDefinition>(new RegisterDefinition(registerNode));

}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::AlternateRegister()
//-----------------------------------------------------------------------------
AlternateRegister::AlternateRegister( const AlternateRegister& other ):
RegisterModel(other),
alternateGroups_(other.alternateGroups_),
alternateRegisterDef_(),
vendorExtensions_(other.vendorExtensions_)
{
	if (other.alternateRegisterDef_)
    {
		alternateRegisterDef_ = QSharedPointer<RegisterDefinition>(
			new RegisterDefinition(*other.alternateRegisterDef_.data()));
	}
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::operator=()
//-----------------------------------------------------------------------------
AlternateRegister& AlternateRegister::operator=( const AlternateRegister& other ) 
{
	if (this != &other) {
		RegisterModel::operator=(other);

		alternateGroups_ = other.alternateGroups_;
        vendorExtensions_ = other.vendorExtensions_;

		if (other.alternateRegisterDef_)
        {
			alternateRegisterDef_ = QSharedPointer<RegisterDefinition>(
				new RegisterDefinition(*other.alternateRegisterDef_.data()));
		}
		else
        {
			alternateRegisterDef_ = QSharedPointer<RegisterDefinition>();
        }
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::~AlternateRegister()
//-----------------------------------------------------------------------------
AlternateRegister::~AlternateRegister()
{
	alternateGroups_.clear();
	alternateRegisterDef_.clear();
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::clone()
//-----------------------------------------------------------------------------
QSharedPointer<RegisterModel> AlternateRegister::clone()
{
	return QSharedPointer<RegisterModel>(new AlternateRegister(*this));
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::write()
//-----------------------------------------------------------------------------
void AlternateRegister::write(QXmlStreamWriter& writer)
{
	writer.writeStartElement("spirit:alternateRegister");

	// call base class to write itself
	RegisterModel::write(writer);

	if (alternateGroups_.size() > 0)
    {
		writer.writeStartElement("spirit:alternateGroups");

		for (int i = 0; i < alternateGroups_.size(); ++i)
        {
			writer.writeTextElement("spirit:alternateGroup", alternateGroups_.at(i));
		}

		writer.writeEndElement(); // spirit:alternateGroups
	}

	// write register definition group is one exists
	if (alternateRegisterDef_)
    {
		alternateRegisterDef_->write(writer);
	}

    if (!vendorExtensions_.isEmpty())
    {
        writer.writeStartElement("spirit:vendorExtensions");
        XmlUtils::writeVendorExtensions(writer, vendorExtensions_);
        writer.writeEndElement(); // spirit:vendorExtensions
    }

	writer.writeEndElement(); // spirit:alternateRegister
}

//-----------------------------------------------------------------------------
// Function: alternateregister::isValid()
//-----------------------------------------------------------------------------
bool AlternateRegister::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList& errorList, const QString& parentIdentifier ) const {

	bool valid = true;

	if (nameGroup_.name().isEmpty())
    {
		errorList.append(QObject::tr("No name specified for alternate register"
			" within %1").arg(parentIdentifier));
		valid = false;
	}

	if (alternateGroups_.isEmpty())
    {
		errorList.append(QObject::tr("At least one alternate group must be "
			"specified for alternate register %1 within %2").arg(nameGroup_.name()).arg(
			parentIdentifier));
		valid = false;
	}

	if (alternateRegisterDef_ && !alternateRegisterDef_->isValid(componentChoices, errorList, 
        QObject::tr("alternate register %1").arg(nameGroup_.name()))) {
		valid = false;
	}

    ParameterValidator validator;
    foreach (QSharedPointer<Parameter> param, parameters_)
    {
        errorList.append(validator.findErrorsIn(param.data(), 
            QObject::tr("alternate register %1").arg(nameGroup_.name()), componentChoices));

        if (!validator.validate(param.data(), componentChoices)) 
        {
            valid = false;
        }
    }

	return valid;
}

//-----------------------------------------------------------------------------
// Function: alternateregister::isValid()
//-----------------------------------------------------------------------------
bool AlternateRegister::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const
{
	if (nameGroup_.name().isEmpty()) {
		return false;
	}

	if (alternateGroups_.isEmpty()) {
		return false;
	}

	if (alternateRegisterDef_ && !alternateRegisterDef_->isValid(componentChoices)) {
			return false;
	}

    ParameterValidator validator;
    foreach (QSharedPointer<Parameter> param, parameters_)
    {
        if (!validator.validate(param.data(), componentChoices))
        {
            return false;
        }
    }

	return true;
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::getAlternateGroups()
//-----------------------------------------------------------------------------
const QList<QString>& AlternateRegister::getAlternateGroups() const 
{
	return alternateGroups_;
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::getAlternateRegisterDef()
//-----------------------------------------------------------------------------
RegisterDefinition* AlternateRegister::getAlternateRegisterDef() const 
{
	return alternateRegisterDef_.data();
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::setAlternateGroups()
//-----------------------------------------------------------------------------
void AlternateRegister::setAlternateGroups(const QList<QString>& alternateGroups)
{
	this->alternateGroups_.clear();
	this->alternateGroups_ = alternateGroups;
}

//-----------------------------------------------------------------------------
// Function: AlternateRegister::setAlternateRegisterDef()
//-----------------------------------------------------------------------------
void AlternateRegister::setAlternateRegisterDef(RegisterDefinition* alternateRegisterDef) 
{
	this->alternateRegisterDef_ = QSharedPointer<RegisterDefinition>(alternateRegisterDef);
}
