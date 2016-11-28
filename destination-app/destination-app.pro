TEMPLATE = aux
TARGET = destination-app

RESOURCES += destination-app.qrc

QML_FILES += $$files(*.qml,true) \
             $$files(*.js,true)

CONF_FILES +=  destination-app.apparmor \
               destination-app.png \
               content.json

AP_TEST_FILES += tests/autopilot/run \
                 $$files(tests/*.py,true)

OTHER_FILES += $${CONF_FILES} \
               $${QML_FILES} \
               $${AP_TEST_FILES} \
               destination-app.desktop 

#specify where the qml/js files are installed to
qml_files.path = /destination-app
qml_files.files += $${QML_FILES}

comp_qml_files.path = /destination-app/components
comp_qml_files.files += $$files(components/*.qml,false) \
                        $$files(components/*.js,false)

#specify where the config files are installed to
config_files.path = /destination-app
config_files.files += $${CONF_FILES}

#install the desktop file, a translated version is automatically created in 
#the build directory
desktop_file.path = /destination-app
desktop_file.files = $$OUT_PWD/destination-app.desktop 
desktop_file.CONFIG += no_check_exist 

INSTALLS+=config_files qml_files desktop_file comp_qml_files
