//-----------------------------------------------------------------------------
// File: modelparametermodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 29.3.2011
//
// Description:
// This model can be used to edit and update a list of model parameters.
//-----------------------------------------------------------------------------

#include "modelparametermodel.h"

#include "ModelParameterColumns.h"
#include "ModelParameterFactoryImplementation.h"

#include <IPXACTmodels/modelparameter.h>

#include <IPXACTmodels/validators/ModelParameterValidator.h>

#include <QColor>
#include <QPersistentModelIndex>
#include <QUuid>

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::ModelParameterModel()
//-----------------------------------------------------------------------------
ModelParameterModel::ModelParameterModel(QSharedPointer<QList<QSharedPointer<ModelParameter> > > modelParameters,
    QSharedPointer<QList<QSharedPointer<Choice> > > choices,
    QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QObject *parent): 
AbstractParameterModel(choices, expressionParser, parameterFinder, expressionFormatter, parent),
modelParameters_(modelParameters), 
parameterFactory_(new ModelParameterFactoryImplementation()), 
lockedIndexes_()
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::~ModelParameterModel()
//-----------------------------------------------------------------------------
ModelParameterModel::~ModelParameterModel()
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::rowCount()
//-----------------------------------------------------------------------------
int ModelParameterModel::rowCount(QModelIndex const& parent /*= QModelIndex() */ ) const 
{
	// not hierarchical model
	if (parent.isValid())
    {
		return 0;
    }

	return modelParameters_->count();
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::columnCount()
//-----------------------------------------------------------------------------
int ModelParameterModel::columnCount(QModelIndex const& parent /*= QModelIndex() */ ) const
{
	// not hierarchical model
	if (parent.isValid())
    {
		return 0;
    }

	return ModelParameterColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::data()
//-----------------------------------------------------------------------------
QVariant ModelParameterModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
    {
		return QVariant();
    }

    QSharedPointer<ModelParameter> modelParameter = getParameterOnRow(index.row()).dynamicCast<ModelParameter>();

    if (role == Qt::DisplayRole)
    {
        if (index.column() == ModelParameterColumns::DATA_TYPE)
        {
            return modelParameter->getDataType();
        }
        else if (index.column() ==  ModelParameterColumns::USAGE_TYPE)
        {
            return modelParameter->getUsageType();
        }
        else
        {
            return AbstractParameterModel::data(index, role);
        }
    }
    else if (Qt::BackgroundRole == role)
    {
        return AbstractParameterModel::data(index, role);     
    }
    else if (Qt::ForegroundRole == role)
    {
        if (!validateIndex(index))
        {
             return QColor("red");
        }
        else if (isLocked(index) )
        {
            return QColor("gray");
        }
        else
        {
            return AbstractParameterModel::data(index, role);
        } 
    }

    return AbstractParameterModel::data(index, role);
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::headerData()
//-----------------------------------------------------------------------------
QVariant ModelParameterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) 
    {	
		if (section ==  ModelParameterColumns::DATA_TYPE)
        {
            return tr("Data\ntype");
        }
        else if (section == ModelParameterColumns::USAGE_TYPE)
        {
            return tr("OO usage");
        }
        else
        {
            return AbstractParameterModel::headerData(section, orientation, role);
        }
	}
	else 
    {
        return AbstractParameterModel::headerData(section, orientation, role);
	}
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::setData()
//-----------------------------------------------------------------------------
bool ModelParameterModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
    {
		return false;
    }

    if (isLocked(index))
    {
        return false;
    }

	if (role == Qt::EditRole)
    {
        QSharedPointer<ModelParameter> modelParameter = 
            getParameterOnRow(index.row()).dynamicCast<ModelParameter>();

        if (index.column() == ModelParameterColumns::DATA_TYPE)
        {
            modelParameter->setDataType(value.toString());
            emit dataChanged(index, index);
            return true;
        }
        else if (index.column() == ModelParameterColumns::USAGE_TYPE)
        {
            modelParameter->setUsageType(value.toString());
            emit dataChanged(index, index);
            return true;
        }
        else
        {
            return AbstractParameterModel::setData(index, value, role);
        }
	}
	else
    {
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ModelParameterModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    if (isLocked(index))
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

    return AbstractParameterModel::flags(index);
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::onAddItem()
//-----------------------------------------------------------------------------
void ModelParameterModel::onAddItem(QModelIndex const& index)
{
    int row = rowCount();

    // if the index is valid then add the item to the correct position
    if (index.isValid())
    {
        row = index.row();
    }

    beginInsertRows(QModelIndex(), row, row);
    modelParameters_->insert(row, QSharedPointer<ModelParameter>(parameterFactory_->make()));
    endInsertRows();

    // tell also parent widget that contents have been changed
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ModelParameterModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
    {
		return;
	}

    if (isLocked(QAbstractTableModel::index(index.row(), 0, QModelIndex())))
    {
        return;
    }

    if (canRemoveRow(index.row()))
    {
        // remove the specified item
        beginRemoveRows(QModelIndex(), index.row(), index.row());
        modelParameters_->removeAt(index.row());
        endRemoveRows();
        
        // tell also parent widget that contents have been changed
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::getParameterOnRow()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ModelParameterModel::getParameterOnRow(int row) const
{
    return modelParameters_->at(row);
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::nameColumn()
//-----------------------------------------------------------------------------
int ModelParameterModel::nameColumn() const
{
    return ModelParameterColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::displayNameColumn()
//-----------------------------------------------------------------------------
int ModelParameterModel::displayNameColumn() const
{
    return ModelParameterColumns::DISPLAY_NAME;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::formatColumn()
//-----------------------------------------------------------------------------
int ModelParameterModel::typeColumn() const
{
    return ModelParameterColumns::TYPE;
}

//-----------------------------------------------------------------------------
// Function: modelparametermodel::bitWidthLeftColumn()
//-----------------------------------------------------------------------------
int ModelParameterModel::bitWidthLeftColumn() const
{
    return ModelParameterColumns::BITWIDTH_LEFT;
}

//-----------------------------------------------------------------------------
// Function: modelparametermodel::bitWidthRightColumn()
//-----------------------------------------------------------------------------
int ModelParameterModel::bitWidthRightColumn() const
{
    return ModelParameterColumns::BITWIDTH_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::minimumColumn()
//-----------------------------------------------------------------------------
int ModelParameterModel::minimumColumn() const
{
    return ModelParameterColumns::MINIMUM;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::maximumColumn()
//-----------------------------------------------------------------------------
int ModelParameterModel::maximumColumn() const
{
    return ModelParameterColumns::MAXIMUM;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::choiceColumn()
//-----------------------------------------------------------------------------
int ModelParameterModel::choiceColumn() const
{
    return ModelParameterColumns::CHOICE;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::valueColumn()
//-----------------------------------------------------------------------------
int ModelParameterModel::valueColumn() const
{
    return ModelParameterColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::resolveColumn()
//-----------------------------------------------------------------------------
int ModelParameterModel::resolveColumn() const
{
    return ModelParameterColumns::RESOLVE;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::arrayLeftColumn()
//-----------------------------------------------------------------------------
int ModelParameterModel::arrayLeftColumn() const
{
    return ModelParameterColumns::ARRAY_LEFT;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::arrayRightColumn()
//-----------------------------------------------------------------------------
int ModelParameterModel::arrayRightColumn() const
{
    return ModelParameterColumns::ARRAY_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::descriptionColumn()
//-----------------------------------------------------------------------------
int ModelParameterModel::descriptionColumn() const
{
    return ModelParameterColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: modelparametermodel::idColumn()
//-----------------------------------------------------------------------------
int ModelParameterModel::idColumn() const
{
    return ModelParameterColumns::ID;
}

//-----------------------------------------------------------------------------
// Function: modelparametermodel::usageCountColumn()
//-----------------------------------------------------------------------------
int ModelParameterModel::usageCountColumn() const
{
    return ModelParameterColumns::USAGE_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::getParameter()
//-----------------------------------------------------------------------------
QSharedPointer<ModelParameter> ModelParameterModel::getParameter(QModelIndex const& index) const
{
	Q_ASSERT(index.isValid());
    return  modelParameters_->at(index.row());
}   

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::setAndLockModelParameters()
//-----------------------------------------------------------------------------
void ModelParameterModel::setAndLockModelParameters(
    QSharedPointer<QList<QSharedPointer<ModelParameter> > > modelParameters)
{
    beginResetModel();
    lockedIndexes_.clear();
    modelParameters_ = modelParameters;
    endResetModel();

    foreach(QSharedPointer<ModelParameter> modelParameter, *modelParameters_)
    {
        lockModelParameter(modelParameter);
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::setParameterFactory()
//-----------------------------------------------------------------------------
void ModelParameterModel::setParameterFactory(QSharedPointer<ModelParameterFactory> factory)
{
    parameterFactory_ = factory;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::validateIndex()
//-----------------------------------------------------------------------------
bool ModelParameterModel::validateIndex(QModelIndex const& index) const
{
    QSharedPointer <Parameter> parameter = getParameterOnRow(index.row());

    if (index.column() == ModelParameterColumns::USAGE_TYPE)
    {
        ModelParameterValidator validator;
        return validator.hasValidUsageType(parameter.dynamicCast<ModelParameter>().data());
    }
    else
    {
        return AbstractParameterModel::validateIndex(index);
    }
}

//-----------------------------------------------------------------------------
// Function: lockModelParameter()
//-----------------------------------------------------------------------------
void ModelParameterModel::lockModelParameter(QSharedPointer<ModelParameter> modelParam)
{
    QModelIndex nameIndex = indexFor(modelParam);
    QModelIndex typeIndex = nameIndex.sibling(nameIndex.row(), ModelParameterColumns::DATA_TYPE);
    QModelIndex usageIndex = nameIndex.sibling(nameIndex.row(), ModelParameterColumns::USAGE_TYPE);
    if (nameIndex.isValid() && typeIndex.isValid() && usageIndex.isValid())
    {     
        lockIndex(nameIndex);  
        lockIndex(typeIndex);
        lockIndex(usageIndex);
    }
}

//-----------------------------------------------------------------------------
// Function: ModelParameterModel::indexFor()
//-----------------------------------------------------------------------------
QModelIndex ModelParameterModel::indexFor(QSharedPointer<ModelParameter> modelParam) const
{
    // find the correct row
    int row =  modelParameters_->indexOf(modelParam);

    if (row < 0)
    {
        return QModelIndex();
    }

    // The base class creates the index for the row.
    return QAbstractTableModel::index(row, 0, QModelIndex());
}

//-----------------------------------------------------------------------------
// Function: lockIndex()
//-----------------------------------------------------------------------------
void ModelParameterModel::lockIndex(QModelIndex const& index)
{
    lockedIndexes_.append(QPersistentModelIndex(index));
}

//-----------------------------------------------------------------------------
// Function: isLocked()
//-----------------------------------------------------------------------------
bool ModelParameterModel::isLocked(QModelIndex const& index) const
{
    return lockedIndexes_.contains(index);
}
