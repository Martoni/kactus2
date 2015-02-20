//-----------------------------------------------------------------------------
// File: VerilogParameterParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 26.09.2014
//
// Description:
// Parser for Verilog parameters.
//-----------------------------------------------------------------------------

#include "VerilogParameterParser.h"
#include "VerilogSyntax.h"

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/port.h>
#include <IPXACTmodels/model.h>

#include <Plugins/PluginSystem/ImportPlugin/ImportColors.h>

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <QString>
#include <QRegularExpression>
#include <QDebug>

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::VerilogParameterParser()
//-----------------------------------------------------------------------------
VerilogParameterParser::VerilogParameterParser() : highlighter_(0)
{
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::~VerilogParameterParser()
//-----------------------------------------------------------------------------
VerilogParameterParser::~VerilogParameterParser()
{
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::setHighlighter()
//-----------------------------------------------------------------------------
void VerilogParameterParser::setHighlighter(Highlighter* highlighter)
{
    highlighter_ = highlighter;
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::import()
//-----------------------------------------------------------------------------
void VerilogParameterParser::import(QString const& input, QSharedPointer<Component> targetComponent)
{
    // Find parameter declarations. Try both formats, as we cannot know which one is used.
    QStringList declarations;
    declarations.append(findANSIDeclarations(input));
    declarations.append(findOldDeclarations(input));

    QList<QSharedPointer<ModelParameter> > parsedParameters;

    foreach (QString declaration, declarations)
    {
        parsedParameters.append(parseParameters(declaration));
    }

    copyIdsFromOldModelParameters(parsedParameters, targetComponent);

    targetComponent->getModelParameters()->clear();
    targetComponent->getModelParameters()->append(parsedParameters);

    replaceNamesWithIdsInModelParameterValues(targetComponent);
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::findANSIDeclarations()
//-----------------------------------------------------------------------------
QStringList VerilogParameterParser::findANSIDeclarations(QString const &input)
{
    // In the ANSI style parameter declarations the parameters are contained WITHIN the module header.
    int endIndex = VerilogSyntax::MODULE_BEGIN.match(input).capturedEnd();

    // And that is why only the module header is inspected in the parsing.
    QString inspect = input.mid( 0, endIndex );

    // We shall further crop until the start of the ports.
    QRegularExpression portsBegin("([)](\\s*|(\\s*" + VerilogSyntax::COMMENT + "\\s*))[(])", 
        QRegularExpression::CaseInsensitiveOption);

    QRegularExpression beginEnd("([)])");

    int portLoc = inspect.lastIndexOf(portsBegin);
    int loc = portLoc;

    if ( loc == -1 )
    {
        // If not encountered, crop until the end of the module begin.
        int endLoc = inspect.lastIndexOf(beginEnd);
        loc = endLoc;
    }
    else
    {
        // If encountered, rip off the port declarations.
        QRegularExpression portsProper("([(].*[)])", QRegularExpression::DotMatchesEverythingOption);
        loc = inspect.indexOf(portsProper,loc);
        inspect = inspect.left(loc);
        loc = inspect.lastIndexOf(beginEnd);
    }

    // If the last location is contained within a comment, rip off the commend and find the another last.
    QRegularExpression lastComment(VerilogSyntax::COMMENT, QRegularExpression::CaseInsensitiveOption);
    int commentLoc = inspect.lastIndexOf(lastComment);
    int matchedLenght = lastComment.match(inspect, commentLoc).capturedLength();

    if (commentLoc != -1 && commentLoc < loc && loc < commentLoc + matchedLenght )
    {
        inspect = inspect.left(commentLoc);

        loc = inspect.lastIndexOf(beginEnd);
    }

    inspect = inspect.left(loc);

    // Cull the stray comments to avoid distractions to parsing.
    inspect = cullStrayComments(inspect);

    QRegularExpression declarRule("\\bparameter\\s+", QRegularExpression::CaseInsensitiveOption);

    return findDeclarations(declarRule, inspect);
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::findOldDeclarations()
//-----------------------------------------------------------------------------
QStringList VerilogParameterParser::findOldDeclarations(QString const& input)
{
    // In the OLD style parameter declarations the parameters are between module header and footer.
    int startIndex = VerilogSyntax::MODULE_BEGIN.match(input).capturedEnd();
    int length = input.indexOf(VerilogSyntax::MODULE_END) - startIndex;

    // And that is why the inspected the region between the header and footer are included to the parsing.
    QString inspect = input.mid(startIndex, length);
    inspect = cullStrayComments(inspect);

    QRegularExpression declarRule("\\bparameter\\s+", QRegularExpression::CaseInsensitiveOption);

    return findDeclarations(declarRule, inspect);
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::parseParameters()
//-----------------------------------------------------------------------------
QList<QSharedPointer<ModelParameter> > VerilogParameterParser::parseParameters(QString const &input)
{
    QList<QSharedPointer<ModelParameter> > parameters;

    // Find the type and the declaration. Only one per declaration is supported.
    QString type = parseType(input);
    QString description = parseDescription(input);

    // Must have name-value pair, possibly multiple times separated by comma. May have comments between.
    QRegularExpression parameterRule("(" + VerilogSyntax::NAME_VALUE + "(\\s*,\\s*(" + VerilogSyntax::COMMENT +
        ")?\\s*" + VerilogSyntax::NAME_VALUE + ")*)", QRegularExpression::CaseInsensitiveOption);

    QString parametersString = parameterRule.match(input).captured();

    // We know for sure that each name value pair is separated by comma, and as such we get a list of them.
    foreach (QString parameter, parametersString.split(","))
    {
        // After acquiring a name value pair, we separate the name and the value from each other.
        QRegularExpression splitRule("(\\w+)\\s*=((\\s*(" + VerilogSyntax::OPERATION_OR_ALPHANUMERIC + "))+)", 
            QRegularExpression::CaseInsensitiveOption);

        QString name = splitRule.match(parameter).captured(1).trimmed();
        QString value = splitRule.match(parameter).captured(2);

        QRegularExpression cullRule("//", QRegularExpression::CaseInsensitiveOption);
        int cullIndex = value.indexOf(cullRule);
        value = value.left(cullIndex).trimmed();

        // Each name value pair produces a new model parameter, but the type and the description is recycled.
        QSharedPointer<ModelParameter> modelParameter =  QSharedPointer<ModelParameter>(new ModelParameter());
           
        modelParameter->setName(name);
        modelParameter->setDataType(type);
        modelParameter->setValue(value);
        modelParameter->setUsageType("nontyped");
        modelParameter->setDescription(description);

        parameters.append(modelParameter);
    }

    return parameters;
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::findDeclarations()
//-----------------------------------------------------------------------------
QStringList VerilogParameterParser::findDeclarations(QRegularExpression const& declarationRule, 
    QString const& inspect)
{
    QStringList declarations;

    int previousStart = 0;
    int declarationStart = inspect.indexOf(declarationRule);

    // Repeat the parsing until no more matches are found.
    while (declarationStart != -1)  
    {
        int declarationLength = declarationRule.match(inspect, declarationStart).capturedLength();

        // Seek for the next match beginning from the end of the previous match.
        previousStart = declarationStart;
        declarationStart = inspect.indexOf(declarationRule, declarationStart  + declarationLength + 1);

        // Take the matching part and append to the list.
        QString declaration = inspect.mid(previousStart, declarationStart - previousStart);
        declaration = declaration.trimmed();

        // Highlight the selection if applicable.
        if (highlighter_)
        {
            highlighter_->applyHighlight(declaration, ImportColors::MODELPARAMETER);
        }

        declarations.append(declaration);
    }

    return declarations;
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::cullStrayComments()
//-----------------------------------------------------------------------------
QString VerilogParameterParser::cullStrayComments(QString const& inspect)
{
    QString inspectWithoutComments = inspect;
    inspectWithoutComments.remove(VerilogSyntax::MULTILINE_COMMENT);
    inspectWithoutComments.remove(VerilogSyntax::COMMENTLINE);

    return inspectWithoutComments;
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::parseType()
//-----------------------------------------------------------------------------
QString VerilogParameterParser::parseType(QString const& input)
{
    // The type is assumed to be the first word in the declaration.
    QRegularExpression typeRule("(\\w+)\\s+(?:" + VerilogSyntax::RANGE + "\\s*){0,2}" +  VerilogSyntax::NAME_VALUE, 
        QRegularExpression::CaseInsensitiveOption);
    QString type = typeRule.match(input).captured(1);

    if (type == "parameter")
    {
        type.clear();
    }

    return type;
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::parseDescription()
//-----------------------------------------------------------------------------
QString VerilogParameterParser::parseDescription(QString const& input)
{
    QString description;

    // If exist, the description is the last comment in the declaration.
    QRegularExpression commentRule(VerilogSyntax::COMMENT, QRegularExpression::CaseInsensitiveOption);
    int lastCommentIndex = input.lastIndexOf(commentRule);

    if (lastCommentIndex != -1)
    {
        QRegularExpressionMatch commmentMatch = commentRule.match(input, lastCommentIndex);

        // Found the index. The description is starting index + length.
        description = input.mid(commmentMatch.capturedStart(), commmentMatch.capturedLength());

        // Some times the expression leaves the comment tag to the description.
        if (description.startsWith("//"))
        {
            description = description.remove(0, 2);
        }

        // No need for extra white spaces.
        description = description.trimmed();
    }

    return description;
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::copyIdsFromOldModelParameters()
//-----------------------------------------------------------------------------
void VerilogParameterParser::copyIdsFromOldModelParameters(QList<QSharedPointer<ModelParameter> > parsedParameters,
    QSharedPointer<Component> targetComponent)
{
    foreach (QSharedPointer<ModelParameter> parameter, parsedParameters)
    {
        QSharedPointer<ModelParameter> existingParameter = 
            targetComponent->getModel()->getModelParameter(parameter->getName());
        if (!existingParameter.isNull())
        {
            parameter->setValueId(existingParameter->getValueId());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::replaceReferenceNamesWithIds()
//-----------------------------------------------------------------------------
void VerilogParameterParser::replaceNamesWithIdsInModelParameterValues(QSharedPointer<Component> targetComponent)
{
    foreach (QSharedPointer<ModelParameter> parameter, *targetComponent->getModelParameters())
    {
        foreach (QSharedPointer<ModelParameter> referenced, *targetComponent->getModelParameters())
        {
            QString parameterValue = parameter->getValue();

            QRegularExpression nameReference("\\b" + referenced->getName() + "\\b");
            if (nameReference.match(parameterValue).hasMatch())
            {
                for (int i = 0; i < parameterValue.count(nameReference); ++i)
                {
                    referenced->increaseUsageCount();
                }

                parameter->setValue(parameterValue.replace(nameReference, referenced->getValueId()));
            }
        }
    }
}
