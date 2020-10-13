/***************************************************************************
 *   Copyright (C) 2009 Ben Cooksley <bcooksley@kde.org>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA          *
 ***************************************************************************/

#include "BaseMode.h"

#include <QList>
#include <QAction>
#include <QAbstractItemView>

#include <KConfigGroup>
#include <KConfigDialog>

#include "MenuItem.h"
#include "BaseData.h"
#include "ModuleView.h"

class BaseMode::Private {
public:
    Private() {}

    QList<QAction*> actionsList;
    KPluginMetaData metaData;
    MenuItem *rootItem = nullptr;
    MenuItem *homeItem = nullptr;
    QString startupModule;
    QStringList startupModuleArgs;
    KConfigGroup config;
    bool showToolTips = true;
    BaseMode::ApplicationMode applicationMode = BaseMode::SystemSettings;
};

BaseMode::BaseMode( QObject* parent, const QVariantList &args )
    : QObject( parent )
    , d( new Private() )
{
    if (args.count() >= 1 && args.first().canConvert<ApplicationMode>()) {
        d->applicationMode = args.first().value<ApplicationMode>();
    }
    if (args.count() >= 2 && args[1].canConvert<QString>()) {
        d->startupModule = args[1].toString();
    }
    if (args.count() >= 3 && args[2].canConvert<QStringList>()) {
        d->startupModuleArgs = args[2].toStringList();
    }
}

BaseMode::~BaseMode()
{
    delete d;
}

void BaseMode::init( const KPluginMetaData metaData )
{
    d->rootItem = BaseData::instance()->menuItem();
    d->homeItem = BaseData::instance()->homeItem();
    d->metaData = metaData;
    d->config = BaseData::instance()->configGroup( metaData.pluginId() );
    initEvent();
    connect( moduleView(), &ModuleView::moduleChanged, this, &BaseMode::viewChanged );
}

void BaseMode::initEvent()
{
}

QWidget * BaseMode::mainWidget()
{
    return nullptr;
}

KAboutData * BaseMode::aboutData()
{
    return nullptr;
}

BaseMode::ApplicationMode BaseMode::applicationMode() const
{
    return d->applicationMode;
}

ModuleView * BaseMode::moduleView() const
{
    return nullptr;
}

QList<QAction*>& BaseMode::actionsList() const
{
    return d->actionsList;
}

const KPluginMetaData &BaseMode::metaData() const
{
    return d->metaData;
}

void BaseMode::setShowToolTips( bool show)
{
    d->showToolTips = show;
}

bool BaseMode::showToolTips() const
{
    return d->showToolTips;
}

void BaseMode::setStartupModule(const QString &startupModule)
{
    d->startupModule = startupModule;
}

QString BaseMode::startupModule() const
{
    return d->startupModule;
}

void BaseMode::setStartupModuleArgs(const QStringList &startupModuleArgs)
{
    d->startupModuleArgs = startupModuleArgs;
}

QStringList BaseMode::startupModuleArgs() const
{
    return d->startupModuleArgs;
}

void BaseMode::searchChanged( const QString& text )
{
    Q_UNUSED( text );
}

void BaseMode::saveState()
{
}

void BaseMode::leaveModuleView()
{
}

void BaseMode::giveFocus()
{
}

void BaseMode::addConfiguration( KConfigDialog * config )
{
    Q_UNUSED( config );
}

void BaseMode::loadConfiguration()
{
}

void BaseMode::saveConfiguration()
{
}

MenuItem * BaseMode::rootItem() const
{
    return d->rootItem;
}

MenuItem * BaseMode::homeItem() const
{
    return d->homeItem;
}

KConfigGroup& BaseMode::config() const
{
    return d->config;
}

QList<QAbstractItemView*> BaseMode::views() const
{
    return QList<QAbstractItemView*>();
}


