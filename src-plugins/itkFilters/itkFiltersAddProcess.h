/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkFiltersProcessBase.h>

class itkFiltersAddProcessPrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersAddProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    static const double defaultAddValue;

    itkFiltersAddProcess(itkFiltersAddProcess * parent = 0);
    itkFiltersAddProcess(const itkFiltersAddProcess& other);
    virtual ~itkFiltersAddProcess(void);
    static bool registered ( void );

    virtual QString description(void) const;
    
public slots:

    void setParameter ( double  data, int channel );
    int tryUpdate();

protected:
    template <class PixelType> int updateProcess();

private:
    itkFiltersAddProcessPrivate *d;
};

dtkAbstractProcess * createitkFiltersAddProcess(void);

