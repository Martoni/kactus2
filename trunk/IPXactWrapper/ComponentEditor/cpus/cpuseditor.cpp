/* 
 *  	Created on: 14.6.2012
 *      Author: Antti Kamppi
 * 		filename: cpuseditor.cpp
 *		Project: Kactus 2
 */

#include "cpuseditor.h"
#include "cpusdelegate.h"
#include <common/widgets/summaryLabel/summarylabel.h>

CpusEditor::CpusEditor( QSharedPointer<Component> component, 
					   QWidget* parent /*= 0*/ ):
ItemEditor(component, parent),
view_(this),
proxy_(this),
model_(component, this) {

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("CPUs"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);
	view_.setModel(&proxy_);

	// items can not be dragged
	view_.setItemsDraggable(false);

	view_.setItemDelegate(new CpusDelegate(component, this));
	// 	view_.setColumnWidth(0, FileSetsEditor::NAME_COLUMN_WIDTH);
	// 	view_.setColumnWidth(1, FileSetsEditor::DESC_COLUMN_WIDTH);
	// 	

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(cpuAdded(int)),
		this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(cpuRemoved(int)),
		this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

CpusEditor::~CpusEditor() {
}

bool CpusEditor::isValid() const {
	return model_.isValid();
}

void CpusEditor::makeChanges() {
	// TODO remove this in final
}

void CpusEditor::refresh() {
	view_.update();
}
