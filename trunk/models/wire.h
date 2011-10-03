/* 
 *
 *  Created on: 5.8.2010
 *      Author: Antti Kamppi
 */

#ifndef WIRE_H_
#define WIRE_H_

#include "generaldeclarations.h"

#include <QString>
#include <QList>
#include <QDomNode>
#include <QSharedPointer>
#include <QXmlStreamWriter>

class Vector;

// TODO only default driver value is supported within spirit:driver at this
// point

/*! \brief Equals the spirit:wire element in IP-Xact specification under
 * spirit:component element.
 *
 * Describes the properties for ports that are of wire style.
 */
class Wire {

public:

	/*! \brief Equals the spirit:wireTypeDef element in IP-Xact specification.
	 *
	 * Describes the type properties for a port per view of a component or
	 * abstractor.
	 */
	struct WireTypeDef {

		/*!
		 * MANDATORY
		 * Defines the name of the type for the port
		 */
		QString typeName_;

		/*!
		 * OPTIONAL
		 * Indicates whether or not the number of bits in the type declaration
		 * is fixed or not.
		 * default = false
		 */
		bool constrained_;

		/*!
		 * OPTIONAL
		 * Contains language specific reference to where given type is actually
		 * defined.
		 */
		QList<QString> typeDefinitions_;

		/*!
		 * MANDATORY
		 * Indicates the view or views in which this type definition applies.
		 */
		QList<QString> viewNameRefs_;

		/*! \brief The constructor
		 *
		 * \param wireTypeNode A reference to a QDomNode to parse the information
		 * from.
		 *
		 * Exception guarantee: basic
		 * \exception Parse_error Occurs when a mandatory element is missing in
		 * this class or one of it's member classes.
		 */
		WireTypeDef(QDomNode &wireTypeNode);
	};

	/*! \brief The constructor
	 *
	 * \param wireNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	Wire(QDomNode &wireNode);

	/*! \brief The constructor
	 *
	 * \param direction Direction of the port
	 * \param leftBound Left bound of the port
	 * \param rightBound Right bound of the port
	 * \param defaultValue Default value of the port
	 * \param allLogicalDirections Specified is all logical directions are allowed.
	 *
	*/
	Wire(General::Direction direction, int leftBound, int rightBound,
		const QString& defaultValue, bool allLogicalDirections);

    /*!
	 * Copy constructor.
	 */
	Wire(Wire const& other);

	/*! \brief The default constructor.
	 *
	 * Constructs an empty Wire-element which is in invalid state.
	*/
	Wire();

	//! \brief Assignment operator
	Wire &operator=(const Wire &other);

	/*! \brief The destructor
	 *
	 */
	~Wire();

	/*! \brief Get the value of allLogicalDirectionsAllowed element
	 *
	 * \return Boolean value of the element.
	 */
	bool getAllLogicalDirectionsAllowed() const;

	/*! \brief Get the direction of the port
	 *
	 * \return Enum Direction that specifies the direction
	 */
	General::Direction getDirection() const;

	/*! \brief Get the vector element of this port
	 *
	 * \return A pointer to this port's vector element.
	 */
	Vector *getVector() const;

	/*! \brief Get the list of this port's wireTypeDefs
	 *
	 * \return QList containing pointers to this port's wireTypeDefs.
	 */
	const QList<QSharedPointer<WireTypeDef> >& getWireTypeDefs();

	/*! \brief Set the allLogicalDirectionsAllowed value of this port
	 *
	 * \param allLogicalDirectionsAllowed The setting
	 */
	void setAllLogicalDirectionsAllowed(bool allLogicalDirectionsAllowed);

	/*! \brief Set the direction of this port
	 *
	 * \param direction The direction to be set.
	 */
	void setDirection(General::Direction direction);

	/*! \brief Set the vector element for this port
	 *
	 * Calling this function will delete the old vector element.
	 *
	 * \param vector A pointer to the new vector element.
	 */
	void setVector(Vector *vector);

	/*! \brief Set the wireTypeDefs of this port
	 *
	 * Calling this function will delete the old wireTypeDefs
	 *
	 * \param wireTypeDefs A reference to the QList containing the pointers
	 * to the new wireTypeDefs.
	 */
	void setWireTypeDefs(const QList<QSharedPointer<WireTypeDef> > &wireTypeDefs);

	/*! \brief Get the default value for the wire
	 *
	 * \return QString containing the value.
	 */
	QString getDefaultDriverValue() const;

	/*! \brief Set the default value for the port
	 *
	 * \param defaultDriverValue QString containing the value
	 */
	void setDefaultDriverValue(const QString& defaultDriverValue);

	/*! \brief Write the contents of the class using the writer.
	 *
	 * Uses the specified writer to write the class contents into file as valid
	 * IP-Xact.
	 *
	 * \param writer A reference to a QXmlStreamWriter instance that is used to
	 * write the document into file.
	 *
	 * Exception guarantee: basic
	 * \exception Write_error Occurs if class or one of it's member classes is
	 * not valid IP-Xact in the moment of writing.
	 */
	void write(QXmlStreamWriter& writer);

	/*! \brief Check if the wire element is in valid state or not.
	 *
	 * \return True if wire is in valid state.
	*/
	bool isValid() const;

	/*! \brief Set the left bound of the vector.
	 *
	 * \param leftBound Left bound to be set.
	 *
	*/
	void setLeftBound(int leftBound);

	/*! \brief Set the right bound of the vector.
	 *
	 * \param rightBound The right bound to be set.
	 *
	*/
	void setRightBound(int rightBound);

private:

	/*!
	 * MANDATORY spirit:direction
	 * Specifies the direction of this port.
	 */
	General::Direction direction_;

	/*!
	 * OPTIONAL spirit:allLogicalDirectionsAllowed
	 * Defines whether the port may be mapped to a port in an
	 * AbstractionDefinition with a different direction
	 * default = false
	 */
	bool allLogicalDirectionsAllowed_;

	/*!
	 * OPTIONAL spirit:vector
	 * Determines if this port is a scalar port or a vectored port
	 */
	QSharedPointer<Vector> vector_;

	/*!
	 * OPTIONAL (spirit:wireTypeDefs)
	 * Describes the ports type as defined by the implementation
	 */
	QList<QSharedPointer<WireTypeDef> > wireTypeDefs_;

	/*!
	 * OPTIONAL spirit:defaultValue under spirit:driver
	 * Specifies a static logic value for this port.
	 * spirit:driver is not supported element at this point.
	 */
	QString defaultDriverValue_;

	//! \brief OPTIONAL Attributes for the defaultDriverValue element.
	QMap<QString, QString> defaultValueAttributes_;

};

#endif /* WIRE_H_ */
