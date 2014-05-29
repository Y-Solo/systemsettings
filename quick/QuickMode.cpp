/**************************************************************************
 * Copyright (C) 2009 Ben Cooksley <bcooksley@kde.org>                    *
 * Copyright (C) 2008 Mathias Soeken <msoeken@informatik.uni-bremen.de>   *
 * Copyright 2014 Sebastian Kügler <sebas@kde.org>                        *
 *                                                                        *
 * This program is free software; you can redistribute it and/or          *
 * modify it under the terms of the GNU General Public License            *
 * as published by the Free Software Foundation; either version 2         *
 * of the License, or (at your option) any later version.                 *
 *                                                                        *
 * This program is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 * GNU General Public License for more details.                           *
 *                                                                        *
 * You should have received a copy of the GNU General Public License      *
 * along with this program; if not, write to the Free Software            *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA          *
 * 02110-1301, USA.                                                       *
***************************************************************************/

#include "QuickMode.h"
#include "ui_configClassic.h"

#include <QDebug>
#include <QLayout>
#include <QSplitter>
#include <QTreeView>
#include <QModelIndex>
#include <QStackedWidget>
#include <QAbstractItemModel>

#include <Plasma/Package>
#include <Plasma/PackageStructure>
#include <Plasma/PluginLoader>

#include <KAboutData>
#include <KCModuleInfo>
#include <KConfigGroup>
#include <KConfigDialog>
#include <KGlobalSettings>

#include "MenuItem.h"
#include "MenuModel.h"
#include "ModuleView.h"
#include "CategoryList.h"
#include "MenuProxyModel.h"

K_PLUGIN_FACTORY(QuickModeFactory, registerPlugin<QuickMode>();)

class QuickMode::Private {
public:
    Private() : moduleView( 0 ) {}
    virtual ~Private() {
        delete aboutClassic;
    }

    QSplitter * classicWidget;
    QTreeView * classicTree;
    Ui::ConfigClassic classicConfig;
    CategoryList * classicCategory;
    QStackedWidget * stackedWidget;
    ModuleView * moduleView;
    QModelIndex currentItem;

    MenuProxyModel * proxyModel;
    MenuModel * model;
    KAboutData * aboutClassic;
    Plasma::Package package;
};

QuickMode::QuickMode( QObject * parent, const QVariantList& )
    : BaseMode( parent ), d( new Private() )
{
    d->aboutClassic = new KAboutData(QStringLiteral("BreezeView"),
                                     i18n("Breeze View"),
                                     QStringLiteral("1.0"),
                                     i18n("Provides a Breeze-styled interface to settings modules."),
                                     KAboutLicense::GPL,
                                     i18n("Copyright 2014, Sebastian Kügler"));
    d->aboutClassic->addAuthor(i18n("Sebastian Kügler"), i18n("Author"), QStringLiteral("sebas@kde.org"));
    d->aboutClassic->addAuthor(i18n("Ben Cooksley"), i18n("Author"), QStringLiteral("bcooksley@kde.org"));
    d->aboutClassic->addAuthor(i18n("Mathias Soeken"), i18n("Developer"), QStringLiteral("msoeken@informatik.uni-bremen.de"));
    d->aboutClassic->setProgramIconName("view-list-tree");


    const QString packageRoot = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "plasma/packages/org.kde.systemsettings.breeze", QStandardPaths::LocateDirectory);
    qDebug() << "QSP" << packageRoot;

    Plasma::PackageStructure *ps = new Plasma::PackageStructure(this);
    d->package = Plasma::Package(ps);
    d->package.addFileDefinition("Categories", "ui/Categories.qml", i18n("Sidebar Script File"));
    d->package.setRequired("Categories", true);
    d->package.addFileDefinition("Modules", "ui/Modules.qml", i18n("Modules List Script File"));
    d->package.setRequired("Modules", true);

    d->package.setPath(packageRoot);

    qDebug() << "valid / defroot?: " << d->package.isValid() << d->package.defaultPackageRoot();
    qDebug() << "categories: " << d->package.filePath("Categories");
    qDebug() << "modules: " << d->package.filePath("Modules");
}

QuickMode::~QuickMode()
{
    if( !d->classicTree ) {
        delete d->classicWidget;
    }
    delete d;
}

void QuickMode::initEvent()
{
    // Create the model
    d->model = new MenuModel( rootItem(), this );
    
    // Move items that are the sole child of a category up....
    moveUp( rootItem() );

    // Create the proxy model
    d->proxyModel = new MenuProxyModel( this );
    d->proxyModel->setSourceModel( d->model );
    d->proxyModel->sort( 0 );
    d->classicWidget = new QSplitter( Qt::Horizontal, 0 );
    d->classicWidget->setChildrenCollapsible( false );
    d->moduleView = new ModuleView( d->classicWidget );
    d->classicTree = 0;
}

QWidget * QuickMode::mainWidget()
{
    if( !d->classicTree ) {
        initWidget();
    }
    return d->classicWidget;
}

KAboutData * QuickMode::aboutData()
{
    return d->aboutClassic;
}

ModuleView * QuickMode::moduleView() const
{
    return d->moduleView;
}

QList<QAbstractItemView*> QuickMode::views() const
{
    QList<QAbstractItemView*> theViews;
    theViews << d->classicTree;
    return theViews;
}

void QuickMode::saveState()
{
    config().writeEntry( "viewLayout", d->classicWidget->sizes() );
    config().sync();
}

void QuickMode::expandColumns()
{
    d->classicTree->resizeColumnToContents(0);
}

void QuickMode::searchChanged( const QString& text )
{
    d->proxyModel->setFilterRegExp(text);
    if( d->classicTree ) {
        d->classicCategory->changeModule( d->classicTree->currentIndex() );
    }
}

void QuickMode::selectModule( const QModelIndex& selectedModule )
{
    d->classicTree->setCurrentIndex( selectedModule );
    if( d->proxyModel->rowCount(selectedModule) > 0 ) {
        d->classicTree->setExpanded(selectedModule, true);
    }
    changeModule( selectedModule );
}

void QuickMode::changeModule( const QModelIndex& activeModule )
{
    if( activeModule == d->currentItem ) {
        return;
    }
    if( !d->moduleView->resolveChanges() ) {
        return;
    }
    d->moduleView->closeModules();
    d->currentItem = activeModule;
    if( d->proxyModel->rowCount(activeModule) > 0 ) {
        d->stackedWidget->setCurrentWidget( d->classicCategory );
        d->classicCategory->changeModule(activeModule);
        emit viewChanged( false );
    } else {
        d->moduleView->loadModule( activeModule );
    }
}

void QuickMode::moduleLoaded()
{
    d->stackedWidget->setCurrentWidget( d->moduleView );
}

void QuickMode::initWidget()
{
    // Create the widget
    d->classicTree = new QTreeView( d->classicWidget );
    d->classicCategory = new CategoryList( d->classicWidget, d->proxyModel );

    d->stackedWidget = new QStackedWidget( d->classicWidget );
    d->stackedWidget->layout()->setMargin(0);
    d->stackedWidget->addWidget( d->classicCategory );
    d->stackedWidget->addWidget( d->moduleView );

    d->classicWidget->addWidget( d->classicTree );
    d->classicWidget->addWidget( d->stackedWidget );

    d->classicTree->setModel( d->proxyModel );
    d->classicTree->setHeaderHidden( true );
    d->classicTree->setIconSize( QSize( 24, 24 ) );
    d->classicTree->setSortingEnabled( true );
    d->classicTree->setMouseTracking( true );
    d->classicTree->setMinimumWidth( 200 );
    d->classicTree->setSelectionMode( QAbstractItemView::SingleSelection );
    d->classicTree->sortByColumn( 0, Qt::AscendingOrder );

    d->classicCategory->changeModule( d->classicTree->rootIndex() );

    connect( d->classicCategory, SIGNAL(moduleSelected(QModelIndex)), this, SLOT(selectModule(QModelIndex)) );
    connect( d->classicTree, SIGNAL(activated(QModelIndex)), this, SLOT(changeModule(QModelIndex)) );
    connect( d->classicTree, SIGNAL(collapsed(QModelIndex)), this, SLOT(expandColumns()) );
    connect( d->classicTree, SIGNAL(expanded(QModelIndex)), this, SLOT(expandColumns()) );
    connect( d->moduleView, SIGNAL(moduleChanged(bool)), this, SLOT(moduleLoaded()) );

    if( !KGlobalSettings::singleClick() ) {
        // Needed because otherwise activated() is not fired with single click, which is apparently expected for tree views
        connect( d->classicTree, SIGNAL(clicked(QModelIndex)), this, SLOT(changeModule(QModelIndex)) );
    }

    if( config().readEntry( "autoExpandOneLevel", false ) ) {
        for( int processed = 0; d->proxyModel->rowCount() > processed; processed++ ) {
            d->classicTree->setExpanded( d->proxyModel->index( processed, 0 ), true );
        }
    }

    expandColumns();
    QList<int> defaultSizes;
    defaultSizes << 250 << 500;
    d->classicWidget->setSizes( config().readEntry( "viewLayout", defaultSizes ) );
}

void QuickMode::leaveModuleView()
{
    d->moduleView->closeModules();
    d->stackedWidget->setCurrentWidget( d->classicCategory );
}

void QuickMode::giveFocus()
{
    d->classicTree->setFocus();
}

void QuickMode::addConfiguration( KConfigDialog * config )
{
    QWidget * configWidget = new QWidget( config );
    d->classicConfig.setupUi( configWidget );
    config->addPage( configWidget, i18n("Tree View"), aboutData()->programIconName() );
}

void QuickMode::loadConfiguration()
{
    d->classicConfig.CbExpand->setChecked( config().readEntry( "autoExpandOneLevel", false ) );
}

void QuickMode::saveConfiguration()
{
    config().writeEntry("autoExpandOneLevel", d->classicConfig.CbExpand->isChecked());
}

void QuickMode::moveUp( MenuItem * item )
{
    foreach( MenuItem * child, item->children() ) {
        if( child->children().count() == 1 ) {
            d->model->addException( child );
        }
        moveUp( child );
    }
}

#include "QuickMode.moc"
