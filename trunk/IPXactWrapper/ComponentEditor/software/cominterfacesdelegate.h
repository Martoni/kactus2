/* 
 *  	Created on: 28.6.2012
 *      Author: Antti Kamppi
 * 		filename: cominterfacesdelegate.h
 *		Project: Kactus 2
 */

#ifndef COMINTERFACESDELEGATE_H
#define COMINTERFACESDELEGATE_H

#include <QStyledItemDelegate>

class ComInterfacesDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:

	//! \brief The user roles that can be used.
	enum UserRole {
		TRANSFER_TYPE_OPTIONS = Qt::UserRole	// Returns QStringList that contains the possible transfer types.
	};

	//! \brief The column numbers for columns.
	enum Column {
		NAME_COLUMN = 0,
		COM_DEF_COLUMN,
		TRANSFER_TYPE_COLUMN,
		DIRECTION_COLUMN,
		DESCRIPTION_COLUMN,
		COLUMN_COUNT
	};

	/*! \brief The constructor.
	 *
	 * \param parent Pointer to the owner of this delegate.
	 *
	*/
	ComInterfacesDelegate(QObject *parent);
	
	//! \brief The destructor
	virtual ~ComInterfacesDelegate();

	/*! \brief Create a new editor for the given item
	 *
	 * \param parent Owner for the editor.
	 * \param option Contains options for the editor.
	 * \param index Model index identifying the item.
	 *
	 * \return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, 
		const QStyleOptionViewItem& option, 
		const QModelIndex& index) const;

	/*! \brief Set the data for the editor.
	 *
	 * \param editor Pointer to the editor where the data is to be set.
	 * \param index Model index identifying the item that's data is to be set.
	 *
	*/
	virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;

	/*! \brief Save the data from the editor to the model.
	 *
	 * \param editor Pointer to the editor that contains the data to store.
	 * \param model Model that contains the data structure where data is to be saved to.
	 * \param index Model index identifying the item that's data is to be saved.
	 *
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
		const QModelIndex& index) const;

private slots:

	/*! \brief Commit the data from the sending editor and close the editor.
	 *
	*/
	void commitAndCloseEditor();

private:
	
	//! \brief No copying
	ComInterfacesDelegate(const ComInterfacesDelegate& other);

	//! \brief No assignment
	ComInterfacesDelegate& operator=(const ComInterfacesDelegate& other);
};

#endif // COMINTERFACESDELEGATE_H
