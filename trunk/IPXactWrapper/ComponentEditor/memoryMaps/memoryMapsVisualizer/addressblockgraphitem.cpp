/* 
 *  	Created on: 18.4.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockgraphitem.cpp
 *		Project: Kactus 2
 */

#include "addressblockgraphitem.h"
#include "registergraphitem.h"
#include <models/register.h>
#include <common/utils.h>
#include <IPXactWrapper/ComponentEditor/visualization/memoryvisualizationitem.h>
#include <IPXactWrapper/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacevisualizationitem.h>
#include <common/KactusColors.h>

#include <QBrush>
#include <QColor>

AddressBlockGraphItem::AddressBlockGraphItem( QSharedPointer<AddressBlock> addrBlock, 
											 QGraphicsItem *parent ):
MemoryVisualizationItem(parent),
addrBlock_(addrBlock) {

	Q_ASSERT(addrBlock_);
	QBrush brush(KactusColors::ADDR_BLOCK_COLOR);
	setDefaultBrush(brush);

    setOverlappingTop(addrBlock_->getBaseAddress().toLongLong());
    setOverlappingBottom(addrBlock_->getLastAddress());
}

AddressBlockGraphItem::~AddressBlockGraphItem() {
}

void AddressBlockGraphItem::refresh() {
    setName(addrBlock_->getName());
    setOverlappingTop(addrBlock_->getBaseAddress().toLongLong());
    setOverlappingBottom(addrBlock_->getLastAddress());

    // Set tooltip to show addresses in hexadecimals.
    setToolTip("<b>Name: </b>" + addrBlock_->getName() + "<br>" +
        "<b>Offset: </b>" + AddressSpaceVisualizationItem::addr2Str(getOffset(), getBitWidth()) + "<br>" +
        "<b>Last address: </b>" + AddressSpaceVisualizationItem::addr2Str(addrBlock_->getLastAddress(), getBitWidth()));

	// set the positions for the children
	MemoryVisualizationItem::reorganizeChildren();

	MemoryVisualizationItem* parentGraphItem = static_cast<MemoryVisualizationItem*>(parentItem());
	Q_ASSERT(parentGraphItem);
	parentGraphItem->refresh();
}

quint64 AddressBlockGraphItem::getOffset() const {
	QString offset = addrBlock_->getBaseAddress();
	return Utils::str2Uint(offset);
}

int AddressBlockGraphItem::getBitWidth() const {
	return addrBlock_->getWidth();
}

unsigned int AddressBlockGraphItem::getAddressUnitSize() const {
	MemoryVisualizationItem* memMap = static_cast<MemoryVisualizationItem*>(parentItem());
	Q_ASSERT(memMap);
	return memMap->getAddressUnitSize();
}

quint64 AddressBlockGraphItem::getLastAddress() const {
	return addrBlock_->getLastAddress();
}

qreal AddressBlockGraphItem::itemTotalWidth() const {

    return MemoryVisualizationItem::itemTotalWidth();
/*	qreal width = VisualizerItem::DEFAULT_WIDTH;

	// if there are children
	QList<QGraphicsItem*> children = childItems();
	foreach (QGraphicsItem* child, children) {

		// The larger width
		VisualizerItem* childItem = dynamic_cast<VisualizerItem*>(child);
		if (childItem) {
			width = qMax(width, childItem->itemTotalWidth());
		}
	}
	return width;*/
}

void AddressBlockGraphItem::setWidth( qreal width ) {
	//setRect(0, 0, width + VisualizerItem::DEFAULT_HEIGHT, VisualizerItem::DEFAULT_HEIGHT);
	//ExpandableItem::reorganizeChildren();
    MemoryVisualizationItem::setWidth(width);
}