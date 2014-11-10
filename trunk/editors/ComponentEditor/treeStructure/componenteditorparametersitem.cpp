/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorparametersitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorparametersitem.h"

#include <editors/ComponentEditor/parameters/parameterseditor.h>

#include <IPXACTmodels/choice.h>

ComponentEditorParametersItem::ComponentEditorParametersItem(ComponentEditorTreeModel* model,
															 LibraryInterface* libHandler,
															 QSharedPointer<Component> component,
															 ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
parameters_(component->getParameters())
{

}

ComponentEditorParametersItem::~ComponentEditorParametersItem() 
{
}

QFont ComponentEditorParametersItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());    
    font.setBold(!parameters_.isEmpty());    
    return font;
}

QString ComponentEditorParametersItem::text() const
{
	return tr("Parameters");
}

bool ComponentEditorParametersItem::isValid() const
{
    foreach (QSharedPointer<Parameter> param, parameters_)
    {
        if (!param->isValid())
        {
            return false;
        }

        if (!param->getChoiceRef().isEmpty())
        {
            QSharedPointer<Choice> referencedChoice = component_->getChoice(param->getChoiceRef());
            if (referencedChoice.isNull())
            {
                return false;
            }
            else if(!referencedChoice->hasEnumeration(param->getValue()))
            {
                return false;
            }
        }
    }

	return true;
}

ItemEditor* ComponentEditorParametersItem::editor()
{
	if (!editor_) {
		editor_ = new ParametersEditor(component_, libHandler_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()),
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

QString ComponentEditorParametersItem::getTooltip() const
{
	return tr("Contains the configurable elements of the component");
}
