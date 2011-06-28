// /////////////////////////////////////////////////////////////////
// Generated by dtkPluginGenerator
// /////////////////////////////////////////////////////////////////

#ifndef ITKDATATENSORIMAGEDOUBLE3_H
#define ITKDATATENSORIMAGEDOUBLE3_H

#include <medData/medAbstractDataTypedImage.h>

#include "itkDataTensorImagePluginExport.h"

class itkDataTensorImageDouble3Private;

class ITKDATATENSORIMAGEPLUGIN_EXPORT itkDataTensorImageDouble3 : public medAbstractDataTypedImage<3,double>
{
    Q_OBJECT

public:
             itkDataTensorImageDouble3(void);
    virtual ~itkDataTensorImageDouble3(void);

    virtual QString description(void) const;
    static bool registered(void);

    
public slots:
    // derived from dtkAbstractData
    void *output(void);
    void *data(void);
    void setData(void* data);
    
public:
    // derived from medAbstractDataImage
    int xDimension(void);
    int yDimension(void);
    int zDimension(void);

    /** TODO the image which is being returned right now is JUST A STUB to allow drag and drop */
    virtual QImage        &thumbnail  (void) const;
    virtual QList<QImage> &thumbnails (void) const;

private:
    itkDataTensorImageDouble3Private *d;
};

dtkAbstractData *createItkDataTensorImageDouble3(void);

#endif
