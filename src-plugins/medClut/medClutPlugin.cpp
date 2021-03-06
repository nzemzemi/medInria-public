// /////////////////////////////////////////////////////////////////
// Generated by medPluginGenerator
// /////////////////////////////////////////////////////////////////

#include "medClutPlugin.h"
#include <medClutToolBox.h>

medClutPlugin::medClutPlugin(QObject *parent) : medPlugin(parent)
{
}

bool medClutPlugin::initialize(void)
{
    if (!medClutToolBox::registered()) { dtkWarn() << "Unable to register medClut toolbox"; }
    
    return true;
}

QString medClutPlugin::name(void) const
{
    return "Clut";
}

QString medClutPlugin::version(void) const
{
    return MEDCLUTPLUGIN_VERSION;
}

QStringList medClutPlugin::types(void) const
{
    return QStringList() << "medClut";
}

Q_EXPORT_PLUGIN2(medClutPlugin, medClutPlugin)
