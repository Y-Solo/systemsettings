set( icon_mode_srcs
     IconMode.cpp
     CategoryDrawer.cpp
     CategorizedView.cpp
) 

add_library(icon_mode MODULE ${icon_mode_srcs})

target_link_libraries(icon_mode systemsettingsview
    KF5::ItemViews
    KF5::KCMUtils
    KF5::I18n
    KF5::KIOWidgets
    KF5::Service
)

install( TARGETS icon_mode DESTINATION ${KDE_INSTALL_PLUGINDIR} )
install( FILES settings-icon-view.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR} )

