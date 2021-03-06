/* 
 *
 *  Created on: 26.10.2010
 *      Author: Antti Kamppi
 */

#ifndef REGISTER_H_
#define REGISTER_H_

#include "registermodel.h"
#include "field.h"
#include "registerdefinition.h"
#include "ipxactmodels_global.h"

#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>

#include <QDomNode>
#include <QXmlStreamWriter>
#include <QString>
#include <QList>
#include <QSharedPointer>

class AlternateRegister;
class RegisterDefinition;
class VendorExtension;

/*! \brief Equals the spirit:register element in IP-Xact specification.
 *
 * This class is inherited from RegisterModel base class.
 */
class IPXACTMODELS_EXPORT Register: public RegisterModel {

public:

	/*! \brief The constructor
	 *
	 * \param registerNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	Register(QDomNode& registerNode);

	//! \brief The default constructor.
	Register();

	/*! \brief The constructor.
	 *
	 * \param volatileValue The volatile value to set.
	 * \param access The access type to set.
	 *
	*/
	Register(General::BooleanValue volatileValue, General::Access access);

	//! \brief Copy constructor
	Register(const Register& other);

	//! \brief Assignment operator
	Register& operator=(const Register& other);

	//! \brief The destructor.
	virtual ~Register();

	/*! \brief Clone the register and return pointer to the copy.
	 * 
	 * This is virtual function so it can be used to make a copy of classes that
	 * inherit register model.
	 *
	 * \return QSharedPointer<RegisterModel> Pointer to the cloned register.
	*/
	virtual QSharedPointer<RegisterModel> clone();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWriter instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Check if the register is in a valid state.
	 *
     * \param componentChoices  Choices in the containing component.
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
    virtual bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices, QStringList& errorList, 
		QString const& parentIdentifier) const;

	/*! \brief Check if the register is in a valid state.
	 *
     * \param componentChoices  Choices in the containing component.
     *
	 * \return bool True if the state is valid and writing is possible.
	*/
    virtual bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const;

	/*! \brief Get the addressOffset of the register
	 *
	 * \return QString containing the address offset.
	 */
    QString getAddressOffset() const;

	/*! \brief Get the offset of the register in numerical form.
	 *
	 * \return The offset from the start of the containing address block.
	*/
	quint64 getOffset() const;

    /*! \brief Get the alternate registers of this register.
     *
     * \return QList containing pointers to the alternate registers.
     */
    const QList<QSharedPointer<AlternateRegister> >& getAlternateRegisters() const;

    /*! \brief Get the dimension of the register.
     *
     * \return The value of the dim-element.
     */
    int getDim() const;

    /*!
     *  Set the expression for dimension.
     *
     *      @param [in] expression   [Description].
     */
    void setDimensionExpression(QString const& expression);

    /*!
     *  Get the expression for dimension.
     *
     *      @return     The dimension expression.
     */
    QString getDimensionExpression();

    /*!
     *  Remove the dimension expression.
     */
    void removeDimensionExpression();

    /*!
     *  Check if the dimension has an expression.
     *
     *      @return     True, if expression for dimension is found, otherwise false.
     */
    bool hasDimensionExpression();

    /*!
     *  Set the size expression.
     *
     *      @param [in] expression  The expression for size.
     */
    void setSizeExpression(QString const& expression);

    /*!
     *  Get the expression for size.
     *
     *      @return The expression for size.
     */
    QString getSizeExpression();

    /*!
     *  Remove the size expression.
     */
    void removeSizeExpression();

    /*!
     *  Set the expression for register presence.
     *
     *      @param [in] expression  The expression for register presence.
     */
    void setIsPresentExpression(QString const& expression);

    /*!
     *  Get the expression for register presence.
     *
     *      @return The expression for register presence.
     */
    QString getIsPresentExpression();

    /*!
     *  Removes the expression for register presence.
     */
    void removeIsPresentExpression();

    /*!
     *  Check if the register has an expression for presence.
     *
     *      @return True, if there is an expression, otherwise false.
     */
    bool hasIsPresentExpression();

    /*! \brief Set the addressOffset for this register.
     *
     * \param addressOffset QString containing the addressOffset.
     */
    void setAddressOffset(QString const& addressOffset);

    /*! \brief Set the alternate registers for this register.
     *
     * Calling this function will erase previous alternateRegisters.
     *
     * \param alternateRegisters QList containing pointers to the new
     * alternateRegisters.
     */
    void setAlternateRegisters(
		const QList<QSharedPointer<AlternateRegister> >& alternateRegisters);

    /*! \brief Set the dimension of the register.
     *
     * \param dim Value of the dim-element.
     */
    void setDim(int dim);

	/*! \brief Get list of the fields.
     *
     * \return QList containing pointers to the fields.
     */
    const QList<QSharedPointer<Field> >& getFields() const;

	/*! \brief Get list of the fields.
	*
	* \return QList containing pointers to the fields.
	*/
	QList<QSharedPointer<Field> >& getFields();

	/*! \brief Get the width of the register.
	 *
	 * \return The number of bits this register contains.
	*/
	unsigned int getWidth() const;

	/*! \brief Get the most significant bit of the register.
	 * 
	 * \return The number of the most significant bit.
	*/
	unsigned int getMSB() const;

	/*! \brief Get the type identifier of the register.
	 *
	 * \return QString containing the type identifier.
	*/
	QString getTypeIdentifier() const;

	/*! \brief Set the type identifier for the register.
	 *
	 * \param typeIdentifier The type identifier to set.
	 *
	*/
	void setTypeIdentifier(QString const& typeIdentifier);

	/*! \brief Get the width of the register.
	 *
	 * \return The width of the register in bits.
	*/
	unsigned int getSize() const;

	/*! \brief Set the width of the register.
	 *
	 * \param size The width of the register in bits.
	 *
	*/
	void setSize(unsigned int size);

	/*! \brief Get the volatile state of the register.
	 *
	 * \return The volatile state.
	*/
	General::BooleanValue getVolatile() const;

	/*! \brief Set the volatile state for the register.
	 *
	 * \param volatileValue The value to set.
	 *
	*/
	void setVolatile(General::BooleanValue volatileValue);

	/*! \brief Get the access type of the register.
	 *
	 * \return The access type.
	*/
	General::Access getAccess() const;

	/*! \brief Set the access type for the register.
	 *
	 * \param access The access type to set.
	 *
	*/
	void setAccess(General::Access access);

	/*! \brief Get the reset value of the register.
	 *
	 * \return QString containing the register reset value.
	*/
	QString getRegisterValue() const;

	/*! \brief Set the reset value for the register.
	 *
	 * \param registerValue The reset value to set.
	 *
	*/
	void setRegisterValue(QString const& registerValue);

	//! \brief Clear the register reset value and mask.
	void clearReset();

	/*! \brief Get the register's reset mask.
	 *
	 * \return QString containing the register's reset mask.
	*/
	QString getRegisterMask() const;

	/*! \brief Set the register's reset mask.
	 *
	 * \param registerMask The register's reset mask to set.
	 *
	*/
	void setRegisterMask(QString const& registerMask);

private:

    /*!
     *  Parses the vendor extensions from a DOM node.
     *
     *      @param [in] extensionsNode  The DOM node containing all vendor extensions.
     */
    void parseVendorExtensions(QDomNode const& extensionsNode);

    /*!
     *  Creates a vendor extension for expression.
     *
     *      @param [in] expression  The initial expression to set.
     */
    void createDimensionExpressionExtension(QString const& expression);


    void createIsPresentExpressionExtension(QString const& expression);

    /*!
     *  Copies vendor extensions from another register.
     *
     *      @param [in] other   The register to copy extensions from.
     */
    void copyVendorExtensions(const Register & other);

	/*! \brief Assigns an unbounded dimension to the register
	 * OPTIONAL spirit:dim
	 */
	int dim_;

	/*! \brief Offset from the base address.
	 * MANDATORY spirit:addressOffset.
	 */
	QString addressOffset_;

	/*! \brief Contains the alternateRegisters for this register
	 * OPTIONAL spirit:alternateRegisters
	 */
	QList<QSharedPointer<AlternateRegister> > alternateRegisters_;

	/*! \brief Contains the registerDefinitionGroup element.
	 * MANDATORY spirit:registerDefinitionGroup
	 */
	RegisterDefinition registerDefinition_;

    /*!
	 * OPTIONAL (spirit: vendorExtensions)
	 * Register vendor extensions.
	 */
    QList<QSharedPointer<VendorExtension> > vendorExtensions_;

    //! Expression used for register dimension.
    QSharedPointer<Kactus2Value> dimensionExpression_;

    //! The vendor extension for is present expression.
    QSharedPointer<Kactus2Value> isPresentExpression_;
};

#endif /* REGISTER_H_ */
