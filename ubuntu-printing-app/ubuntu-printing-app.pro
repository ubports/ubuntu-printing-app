TEMPLATE = aux
TARGET = ubuntu-printing-app

RESOURCES += ubuntu-printing-app.qrc

QML_FILES += $$files(*.qml,true) \
             $$files(*.js,true)

CONF_FILES +=  ubuntu-printing-app.apparmor \
               ubuntu-printing-app.png \
               ubuntu-printing-app

AP_TEST_FILES += tests/autopilot/run \
                 $$files(tests/*.py,true)

OTHER_FILES += $${CONF_FILES} \
               $${QML_FILES} \
               $${AP_TEST_FILES} \
               ubuntu-printing-app.desktop 

#specify where the qml/js files are installed to
qml_files.path = /usr/share/ubuntu-printing-app
qml_files.files += $${QML_FILES}

comp_qml_files.path = /usr/share/ubuntu-printing-app/components
comp_qml_files.files += $$files(components/*.qml,false) \
                        $$files(components/*.js,false)

#specify where the config files are installed to
config_files.path = /usr/share/ubuntu-printing-app
config_files.files += $${CONF_FILES}

#install the desktop file, a translated version is automatically created in 
#the build directory
desktop_file.path = /usr/share/applications/
desktop_file.files = $$OUT_PWD/ubuntu-printing-app.desktop 
desktop_file.CONFIG += no_check_exist 

#install content-hub in the correct place for debs
content_hub.files = ubuntu-printing-app
content_hub.path = /usr/share/content-hub/peers/

INSTALLS+=config_files qml_files desktop_file comp_qml_files content_hub
