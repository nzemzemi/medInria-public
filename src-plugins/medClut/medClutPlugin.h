
// /////////////////////////////////////////////////////////////////
// Generated by medPluginGenerator
// /////////////////////////////////////////////////////////////////

#pragma once

#include <medPlugin.h>
#include "medClutPluginExport.h"

class MEDCLUTPLUGIN_EXPORT medClutPlugin : public medPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
    medClutPlugin(QObject *parent = 0);
    virtual bool initialize(void);
    
    virtual QString name(void) const;
    virtual QString version (void) const;
    virtual QStringList types(void) const;
};
