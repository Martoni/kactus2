//-----------------------------------------------------------------------------
// File: IPXactSystemVerilogParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 02.12.2014
//
// Description:
// Parser for SystemVerilog expressions within one IP-XACT component.
//-----------------------------------------------------------------------------

#ifndef IPXACTSYSTEMVERILOGPARSER_H
#define IPXACTSYSTEMVERILOGPARSER_H

#include "SystemVerilogExpressionParser.h"

#include <QSharedPointer>
#include <QString>

class Parameter;
class ParameterFinder;

//-----------------------------------------------------------------------------
// Parser for SystemVerilog expressions within one IP-XACT component.
//-----------------------------------------------------------------------------
class IPXactSystemVerilogParser : public SystemVerilogExpressionParser
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] finder   The finder for parameters available in the SystemVerilog expressions.
     */
    IPXactSystemVerilogParser(QSharedPointer<ParameterFinder> finder);
    
    //! The destructor.
    virtual ~IPXactSystemVerilogParser();
    
    /*!
     *  Parses an expression to decimal number.
     *
     *      @param [in] expression   The expression to parse.
     *
     *      @return The decimal value of the constant.
     */
    virtual QString parseExpression(QString const& expression) const;

    /*!
     *  Checks if the given expression is not valid for parsing.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is not in valid format, otherwise false.
     */
    virtual bool isValidExpression(QString const& expression) const;

    /*!
     *  Finds the common base in the expression.
     *
     *      @param [in] expression   The expression to search in.
     *
     *      @return The common base for the expression.
     */
    virtual int baseForExpression(QString const& expression) const;

private:

    // Disable copying.
    IPXactSystemVerilogParser(IPXactSystemVerilogParser const& rhs);
    IPXactSystemVerilogParser& operator=(IPXactSystemVerilogParser const& rhs);

    /*!
     *  Evaluates the values of references parameter in the given expression recursively.
     *
     *      @param [in] expression              The expression to evaluate.
     *      @param [in] availableIds            The ids available in the expressions.
     *      @param [in] recursionStep           The current depth in recursion.
     *
     *      @return The expression where the references have been replaced with the evaluated values.
     */
    QString evaluateReferencesIn(QString const& expression, QStringList const& availableIds,
        int recursionStep) const;

    /*!
     *  Checks if the recursion for solving references should terminate.
     *
     *      @param [in] recursionStep   The current recursion step (depth).
     *
     *      @return True, if the recursion should be terminated, otherwise false.
     */
    bool shouldTerminateRecursion(int recursionStep) const;

    /*!
     *  Replaces the references parameter in the given expression with their values recursively.
     *
     *      @param [in] expression              The expression to evaluate.
     *      @param [in] availableIds            The ids available in the expressions.
     *      @param [in] recursionStep           The current depth in recursion.
     *
     *      @return The expression where the references have been replaced with their values.
     */
    QString replaceReferencesIn(QString const& expression, QStringList const& availableIds,
        int recursionStep) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Maximum number of recursion steps in finding value for a parameter.
    static const int MAX_RECURSION_STEPS = 24;
    
    //! The finder for parameters available in the SystemVerilog expressions.
    QSharedPointer<ParameterFinder> finder_;
};

#endif // IPXACTSYSTEMVERILOGPARSER_H
