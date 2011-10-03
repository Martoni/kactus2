/* 
 *
 *  Created on: 30.7.2010
 *      Author: Antti Kamppi
 */

#ifndef REMAPSTATE_H_
#define REMAPSTATE_H_

#include "remapport.h"

#include <QString>
#include <QList>
#include <QXmlStreamWriter>

/*! \brief Equals to the spirit:remapState element in the IP-Xact specification
 *
 * Defines a conditional remap state where each state is conditioned by a remap
 * port specified with a remapPort instance.
 */
class RemapState {

public:

	/*! \brief The constructor
	 *
	 * \param remapNode A reference to the QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	RemapState(QDomNode &remapNode);

	//! \brief Copy constructor
	RemapState(const RemapState &other);

	//! \brief Assignment operator
	RemapState &operator=(const RemapState &other);

	/*! \brief The destructor
	 *
	 */
	~RemapState();

	/*! \brief Get the name of the remap state
	 *
	 * \return QString containing the name
	 */
	QString getName() const;

	/*! \brief Get list of the remapPorts for this remap state
	 *
	 * \return QList containing pointers to the remapPort instaces
	 */
	const QList<QSharedPointer<RemapPort> >& getRemapPorts();

	/*! \brief Set the name for this remapState
	 *
	 * \param name QString containing the name
	 */
	void setName(const QString &name);

	/*! \brief Set the remap ports for this remap state
	 *
	 * Calling this function will delete the old remapPorts.
	 *
	 * \param remapPorts QList containing pointers to the remapPorts to set
	 */
	void setRemapPorts(const QList<QSharedPointer<RemapPort> > &remapPorts);

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

private:

	/*!
	 * MANDATORY
	 * Identifies the remapState
	 */
	QString name_;

	/*!
	 * OPTIONAL
	 * List of the remapPorts for this remap state.
	 */
	QList<QSharedPointer<RemapPort> > remapPorts_;
};

#endif /* REMAPSTATE_H_ */
