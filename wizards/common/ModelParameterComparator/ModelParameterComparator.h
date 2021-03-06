//-----------------------------------------------------------------------------
// File: ModelParameterComparator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 17.10.2014
//
// Description:
// Comparator for finding differences in lists of model parameters.
//-----------------------------------------------------------------------------

#ifndef MODELPARAMETERCOMPARATOR_H
#define MODELPARAMETERCOMPARATOR_H

#include <QList>
#include <QSharedPointer>

#include <wizards/common/IPXactDiff.h>
#include <wizards/common/ListComparator.h>
#include <wizards/common/ParameterComparator/ParameterComparator.h>

class ModelParameter;

//-----------------------------------------------------------------------------
//! Comparator for finding differences in lists of model parameters.
//-----------------------------------------------------------------------------
class ModelParameterComparator : public ListComparator<ModelParameter>
{
public:
    //! The constructor.
    ModelParameterComparator();

    //! The destructor.
    ~ModelParameterComparator();

    /*!
     *  Finds the differences between the two given model parameters.
     *
     *      @param [in] reference   The reference model parameter to compare to.
     *      @param [in] subject     The model parameters to compare against the reference.
     *
     *      @return Set of differences between the reference and subject.
     */
    virtual bool compare(QSharedPointer<ModelParameter> first, QSharedPointer<ModelParameter> second) const;
    
    /*!
     *  Compares the sub-elements of two model parameters.
     *
     *      @param [in] first    The first model parameters.
     *      @param [in] second   The second model parameters.
     *
     *      @return True, if the sub-elements are similar, otherwise false.
     */
    virtual bool compareFields(QSharedPointer<const ModelParameter> first, 
        QSharedPointer<const ModelParameter> second) const;

    /*!
     *  Compares the two lists of model parameters.
     *
     *      @param [in] first    The first lists.
     *      @param [in] second   The second lists.
     *
     *      @return True, if the lists are similar, otherwise false.
     */
    bool compare(QList<QSharedPointer<ModelParameter> > const first, 
        QList<QSharedPointer<ModelParameter> > const second) ;
    
    /*!
     *  Finds the differences between the sub-elements of two given model parameters.
     *
     *      @param [in] reference   The reference model parameter to compare to.
     *      @param [in] subject     The model parameter to compare against the reference.
     *
     *      @return Set of differences between the sub-elements of reference and subject.
     */
    virtual QList<QSharedPointer<IPXactDiff> > diffFields(QSharedPointer<const ModelParameter> reference,
        QSharedPointer<const ModelParameter> subject) const;
    
    /*!
     *  Returns the type for the element.
     *
     *      @return The element type.
     */
    virtual QString elementType() const;

private:

    //! Disable copying.
    ModelParameterComparator(ModelParameterComparator const& rhs);
    ModelParameterComparator& operator=(ModelParameterComparator const& rhs);

};

#endif // MODELPARAMETERCOMPARATOR_H
