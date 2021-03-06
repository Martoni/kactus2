/* 
 *
 *  Created on: 29.7.2010
 *      Author: Antti Kamppi
 */

#ifndef SUBSPACEMAP_H_
#define SUBSPACEMAP_H_

#include "memorymapitem.h"
#include "generaldeclarations.h"
#include "parameter.h"

#include "ipxactmodels_global.h"

#include <QDomNode>
#include <QList>
#include <QSharedPointer>
#include <QXmlStreamWriter>

class Choice;
class Parameter;
class VendorExtension;

/*! \brief Equals the spirit:subspaceMap element in IP-Xact specification
 *
 * SubspaceMap maps the address space of a master interface from an opaque bus
 * bridge into the memory map.
 */
class IPXACTMODELS_EXPORT SubspaceMap: public MemoryMapItem {

public:

	/*! \brief The constructor
	 *
	 * \param memoryMapNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	SubspaceMap(QDomNode &memoryMapNode);

    /*!
     *  The default constructor.
     */
    SubspaceMap();

	//! \brief Copy constructor
	SubspaceMap(const SubspaceMap &other);

	//! \brief Assignment operator
	SubspaceMap &operator=(const SubspaceMap &other);

	/*! \brief The destructor
	 *
	 */
	virtual ~SubspaceMap();

	/*! \brief Clone this subspace map and return pointer to the copy.
	 * 
	 * This is virtual function so it can be used to make a copy of classes that
	 * inherit MemoryMapItem.
	 *
	 * \return QSharedPointer<MemoryMapItem> Pointer to the cloned subspace map.
	*/
	virtual QSharedPointer<MemoryMapItem> clone() const;

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWriter instance that is used to
	* write the document into file.
	*/
	virtual void write(QXmlStreamWriter& writer);

	/*! \brief Check if the subspace map is in a valid state.
	 *
     * \param componentChoices  Choices in the containing component.
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	virtual bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
        QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the subspace map is in a valid state.
	 *
     * \param componentChoices  Choices in the containing component.
     *
	 * \return bool True if the state is valid and writing is possible.
	*/
	virtual bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const;

	/*! \brief Get the list of parameters for the subSpaceMap
	 *
	 * \return QList containing pointers to the parameters
	 */
	QList<QSharedPointer<Parameter> > const& getParameters();

private:

	/*!
	 * OPTIONAL
	 * Contains the parameters for the SubspaceMap instance.
	 */
	QList<QSharedPointer<Parameter> > parameters_;

    /*!
	 * OPTIONAL (spirit: vendorExtensions)
	 * SubspaceMap vendor extensions.
	 */
    QList<QSharedPointer<VendorExtension> > vendorExtensions_;

};

#endif /* SUBSPACEMAP_H_ */
