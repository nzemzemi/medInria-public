/*=========================================================================
 medInria
 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.
=========================================================================*/

#pragma once

#include <itkMorphologicalFiltersProcessBase.h>

class ITKFILTERSPLUGIN_EXPORT itkFiltersBinaryCloseProcess : public itkMorphologicalFiltersProcessBase
{
    Q_OBJECT

public:
    itkFiltersBinaryCloseProcess(itkFiltersBinaryCloseProcess * parent = 0);

    static bool registered ( void );

    virtual QString description(void) const;
};

dtkAbstractProcess * createitkFiltersBinaryCloseProcess(void);
