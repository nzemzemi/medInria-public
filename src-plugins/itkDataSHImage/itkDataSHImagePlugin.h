/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medPlugin.h>
#include <itkDataSHImagePluginExport.h>

class ITKDATASHIMAGEPLUGIN_EXPORT itkDataSHImagePluginPrivate;

class ITKDATASHIMAGEPLUGIN_EXPORT itkDataSHImagePlugin : public medPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
     itkDataSHImagePlugin(QObject *parent = 0);
    ~itkDataSHImagePlugin();

    virtual bool initialize();

    virtual QString name() const;
    virtual QString description() const;

    virtual QStringList tags() const;
    virtual QStringList types() const;

    virtual QString version() const;
    virtual QStringList authors() const;
    virtual QStringList contributors() const;

private:
     itkDataSHImagePluginPrivate *d;
};
