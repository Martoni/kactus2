/* 
 *  	Created on: 29.5.2012
 *      Author: Antti Kamppi
 * 		filename: filesetsdelegate.h
 *		Project: Kactus 2
 */

#ifndef FILESETSDELEGATE_H
#define FILESETSDELEGATE_H

#include <QStyledItemDelegate>

/*! \brief The delegate class to provide editors for file sets editor.
 *
 */
class FileSetsDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:

	//! \brief The column number for name field.
	static const int NAME_COLUMN = 0;

	//! \brief The column number for description field.
	static const int DESC_COLUMN = 1;

	//! \brief The column number for group identifiers.
	static const int GROUP_COLUMN = 2;

	//! \brief The minimum height for the custom list editor.
	static const int LIST_EDITOR_MIN_HEIGHT = 100;

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of the delegate.
	 *
	*/
	FileSetsDelegate(QObject *parent);
	
	//! \brief The destructor
	~FileSetsDelegate();

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
	FileSetsDelegate(const FileSetsDelegate& other);

	//! \brief No assignment
	FileSetsDelegate& operator=(const FileSetsDelegate& other);
};

#endif // FILESETSDELEGATE_H
