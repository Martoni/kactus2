//-----------------------------------------------------------------------------
// File: DiagramChangeCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 5.8.2011
//
// Description:
// Undo change commands for the block diagram.
//-----------------------------------------------------------------------------

#include "DiagramChangeCommands.h"

#include <models/component.h>

#include <ConfigurationEditor/activeviewmodel.h>

#include "diagraminterconnection.h"
#include "diagramport.h"
#include "diagramcomponent.h"
#include "diagraminterface.h"
#include "columnview/DiagramColumn.h"
#include "columnview/DiagramColumnLayout.h"

#include <models/businterface.h>

//-----------------------------------------------------------------------------
// Function: ColumnChangeCommand()
//-----------------------------------------------------------------------------
ColumnChangeCommand::ColumnChangeCommand(DiagramColumn* column, QString const& newName,
                                         ColumnContentType newContentType,
                                         unsigned int newAllowedItems,
                                         QUndoCommand* parent) : QUndoCommand(parent),
                                                                 column_(column),
                                                                 oldName_(column->getName()),
                                                                 oldContentType_(column->getContentType()),
                                                                 oldAllowedItems_(column->getAllowedItems()),
                                                                 newName_(newName),
                                                                 newContentType_(newContentType),
                                                                 newAllowedItems_(newAllowedItems)

{
}

//-----------------------------------------------------------------------------
// Function: ~ColumnChangeCommand()
//-----------------------------------------------------------------------------
ColumnChangeCommand::~ColumnChangeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ColumnChangeCommand::undo()
{
    column_->setName(oldName_);
    column_->setContentType(oldContentType_, oldAllowedItems_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ColumnChangeCommand::redo()
{
    column_->setName(newName_);
    column_->setContentType(newContentType_, newAllowedItems_);
}

//-----------------------------------------------------------------------------
// Function: ComponentChangeNameCommand()
//-----------------------------------------------------------------------------
ComponentChangeNameCommand::ComponentChangeNameCommand(DiagramComponent* component,
                                               QString const& newName,
                                               QUndoCommand* parent) : QUndoCommand(parent),
                                                                       component_(component),
                                                                       oldName_(component->name()),
                                                                       newName_(newName)
{
}

//-----------------------------------------------------------------------------
// Function: ~ComponentChangeNameCommand()
//-----------------------------------------------------------------------------
ComponentChangeNameCommand::~ComponentChangeNameCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ComponentChangeNameCommand::undo()
{
    component_->setName(oldName_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ComponentChangeNameCommand::redo()
{
    component_->setName(newName_);
}

ComponentChangeDisplayNameCommand::ComponentChangeDisplayNameCommand(DiagramComponent* component,
													   QString const& newDisplayName,
													   QUndoCommand* parent):
QUndoCommand(parent),
component_(component),
oldDisplayName_(component->displayName()),
newDisplayName_(newDisplayName) {
}

ComponentChangeDisplayNameCommand::~ComponentChangeDisplayNameCommand() {
}

void ComponentChangeDisplayNameCommand::undo() {
	component_->setDisplayName(oldDisplayName_);
}

void ComponentChangeDisplayNameCommand::redo() {
	component_->setDisplayName(newDisplayName_);
}

ComponentChangeDescriptionNameCommand::ComponentChangeDescriptionNameCommand(DiagramComponent* component,
																			 QString const& newDescription, 
																			 QUndoCommand* parent /*= 0*/ ):
QUndoCommand(parent),
component_(component),
oldDescription_(component->description()),
newDescription_(newDescription) {
}

ComponentChangeDescriptionNameCommand::~ComponentChangeDescriptionNameCommand() {
}

void ComponentChangeDescriptionNameCommand::undo() {
	component_->setDescription(oldDescription_);
}

void ComponentChangeDescriptionNameCommand::redo() {
	component_->setDescription(newDescription_);
}

ComponentActiveViewChangeCommand::ComponentActiveViewChangeCommand( 
	const QString& instanceName, 
	QString const& oldActiveView, 
	QString const& newActiveView,
	ActiveViewModel* activeViewModel,
	QUndoCommand* parent /*= 0*/ ):
QUndoCommand(parent),
instanceName_(instanceName),
newViewName_(newActiveView),
oldViewName_(oldActiveView),
activeViewModel_(activeViewModel) {
}

ComponentActiveViewChangeCommand::~ComponentActiveViewChangeCommand() {

}

void ComponentActiveViewChangeCommand::undo() {
	activeViewModel_->setActiveView(instanceName_, oldViewName_);
}

void ComponentActiveViewChangeCommand::redo() {
	activeViewModel_->setActiveView(instanceName_, newViewName_);
}

//-----------------------------------------------------------------------------
// Function: ComponentPacketizeCommand()
//-----------------------------------------------------------------------------
ComponentPacketizeCommand::ComponentPacketizeCommand(DiagramComponent* component,
                                                     VLNV const& vlnv,
                                                     QUndoCommand* parent) : QUndoCommand(parent),
                                                     component_(component),
                                                     vlnv_(vlnv)
{
}

//-----------------------------------------------------------------------------
// Function: ~ComponentPacketizeCommand()
//-----------------------------------------------------------------------------
ComponentPacketizeCommand::~ComponentPacketizeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ComponentPacketizeCommand::undo()
{
    // Set an empty VLNV.
    component_->componentModel()->setVlnv(VLNV());
    component_->updateComponent();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ComponentPacketizeCommand::redo()
{
    component_->componentModel()->setVlnv(vlnv_);
    component_->updateComponent();
}

//-----------------------------------------------------------------------------
// Function: EndPointChangeCommand()
//-----------------------------------------------------------------------------
EndPointChangeCommand::EndPointChangeCommand(DiagramConnectionEndPoint* endPoint, QString const& newName,
                                             General::InterfaceMode newMode, QUndoCommand* parent)
    : QUndoCommand(parent), endPoint_(endPoint),
      oldName_(endPoint->name()), oldMode_(endPoint->getBusInterface()->getInterfaceMode()),
      newName_(newName), newMode_(newMode)
{
}

//-----------------------------------------------------------------------------
// Function: ~EndPointChangeCommand()
//-----------------------------------------------------------------------------
EndPointChangeCommand::~EndPointChangeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndPointChangeCommand::undo()
{
    endPoint_->getBusInterface()->setName(oldName_);
    endPoint_->getBusInterface()->setInterfaceMode(oldMode_);
    endPoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndPointChangeCommand::redo()
{
    endPoint_->getBusInterface()->setName(newName_);
    endPoint_->getBusInterface()->setInterfaceMode(newMode_);
    endPoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: EndPointTypesCommand()
//-----------------------------------------------------------------------------
EndPointTypesCommand::EndPointTypesCommand(DiagramConnectionEndPoint* endPoint,
                                           VLNV const& oldBusType, VLNV const& oldAbsType,
                                           General::InterfaceMode oldMode,
                                           QString const& oldName, QUndoCommand* parent)
    : QUndoCommand(parent), endPoint_(endPoint), oldBusType_(oldBusType), oldAbsType_(oldAbsType),
      oldMode_(oldMode), oldName_(oldName), newBusType_(), newAbsType_(),
      newMode_(General::MASTER), newName_(""), connModes_()
{
    if (endPoint_->getBusInterface() != 0)
    {
        newBusType_ = endPoint_->getBusInterface()->getBusType();
        newAbsType_ = endPoint_->getBusInterface()->getAbstractionType();
        newMode_ = endPoint_->getBusInterface()->getInterfaceMode();
        newName_ = endPoint_->getBusInterface()->getName();
    }

    // Save the interface modes for each connection.
    foreach (DiagramInterconnection* conn, endPoint_->getInterconnections())
    {
        DiagramConnectionEndPoint* endPoint = conn->endPoint1();

        if (conn->endPoint1() == endPoint_)
        {
            endPoint = conn->endPoint2();
        }
        
        QSharedPointer<BusInterface> busIf = endPoint->getBusInterface();

        if (busIf != 0 && busIf->getBusType().isValid())
        {
            connModes_.insert(endPoint, busIf->getInterfaceMode());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ~EndPointTypesCommand()
//-----------------------------------------------------------------------------
EndPointTypesCommand::~EndPointTypesCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndPointTypesCommand::undo()
{
    endPoint_->setTypes(oldBusType_, oldAbsType_, oldMode_);

    if (endPoint_->getBusInterface() != 0 && oldName_ != newName_)
    {
        endPoint_->getBusInterface()->setName(oldName_);
    }

    endPoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndPointTypesCommand::redo()
{
    endPoint_->setTypes(newBusType_, newAbsType_, newMode_);

    if (oldName_ != newName_)
    {
        endPoint_->getBusInterface()->setName(newName_);
    }

    endPoint_->updateInterface();

    // Set interface modes for the other end points.
    QMap<DiagramConnectionEndPoint*, General::InterfaceMode>::iterator cur = connModes_.begin();

    while (cur != connModes_.end())
    {
        cur.key()->getBusInterface()->setInterfaceMode(cur.value());
        cur.key()->updateInterface();
        ++cur;
    }
}

//-----------------------------------------------------------------------------
// Function: EndPointPortMapCommand()
//-----------------------------------------------------------------------------
EndPointPortMapCommand::EndPointPortMapCommand(DiagramConnectionEndPoint* endPoint,
                                               QList< QSharedPointer<General::PortMap> > newPortMaps,
                                               QUndoCommand* parent)
    : QUndoCommand(parent), endPoint_(endPoint),
      oldPortMaps_(endPoint->getBusInterface()->getPortMaps()),
      newPortMaps_(newPortMaps)
{
}

//-----------------------------------------------------------------------------
// Function: ~EndPointPortMapCommand()
//-----------------------------------------------------------------------------
EndPointPortMapCommand::~EndPointPortMapCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndPointPortMapCommand::undo()
{
    if (endPoint_->isHierarchical())
    {
        endPoint_->getBusInterface()->setPortMaps(oldPortMaps_);
        endPoint_->updateInterface();
    }
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndPointPortMapCommand::redo()
{
    if (endPoint_->isHierarchical())
    {
        endPoint_->getBusInterface()->setPortMaps(newPortMaps_);
        endPoint_->updateInterface();
    }
}

ComponentConfElementChangeCommand::ComponentConfElementChangeCommand( 
	DiagramComponent* component, 
	const QMap<QString, QString>& newConfElements, 
	QUndoCommand* parent /*= 0*/ ):
QUndoCommand(parent),
component_(component),
oldConfElements_(component->getConfigurableElements()),
newConfElements_(newConfElements) {
}

ComponentConfElementChangeCommand::~ComponentConfElementChangeCommand() {
}

void ComponentConfElementChangeCommand::undo() {
	component_->setConfigurableElements(oldConfElements_);
}

void ComponentConfElementChangeCommand::redo() {
	component_->setConfigurableElements(newConfElements_);
}
