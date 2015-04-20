/****************************************************************************
**
** Copyright (C) 2015 Emenda Nordic AB
** Author: Jacob Lärfors (jacob.larfors@emenda.eu)
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

#include "klocworkoptions.h"
#include "klocworkpluginconstants.h"

#include <coreplugin/icore.h>

#include <QString>

using namespace Klocwork::Constants;
using namespace Klocwork::Internal;

KlocworkOptions::KlocworkOptions() { load(); }

void KlocworkOptions::save() {
    Q_ASSERT(Core::ICore::settings() != 0);
    QSettings &settings = *(Core::ICore::settings());
    settings.beginGroup(QLatin1String(KW_OPTIONS_GROUP));
    settings.setValue(QLatin1String(KW_OPTIONS_BINARY_FILE), m_binaryFile);
    settings.setValue(QLatin1String(KW_OPTIONS_CHECK_ON_SAVE), m_checkOnSave);
    settings.setValue(QLatin1String(KW_OPTIONS_CUSTOM_PARAMS),
                      m_customParameters);

    settings.setValue(QLatin1String(KW_OPTIONS_LICENSE_HOST), m_kwLicenseHost);
    settings.setValue(QLatin1String(KW_OPTIONS_LICENSE_PORT), m_kwLicensePort);
    settings.endGroup();
}

void KlocworkOptions::load() {
    Q_ASSERT(Core::ICore::settings() != 0);
    QSettings &settings = *(Core::ICore::settings());
    settings.beginGroup(QLatin1String(KW_OPTIONS_GROUP));
    m_binaryFile = settings.value(QLatin1String(KW_OPTIONS_BINARY_FILE),
                                  QString()).toString();
    m_customParameters = settings.value(QLatin1String(KW_OPTIONS_CUSTOM_PARAMS),
                                        QString()).toString();
    m_checkOnSave = settings.value(QLatin1String(KW_OPTIONS_CHECK_ON_SAVE),
                                   QString()).toBool();
    m_kwLicenseHost = settings.value(QLatin1String(KW_OPTIONS_LICENSE_HOST),
                                     QString()).toString();
    m_kwLicensePort = settings.value(QLatin1String(KW_OPTIONS_LICENSE_PORT),
                                     QString()).toString();
    settings.endGroup();
}

QString KlocworkOptions::binaryFile() const { return m_binaryFile; }

void KlocworkOptions::setBinaryFile(const QString &binaryFile) {
    m_binaryFile = binaryFile;
}

QString KlocworkOptions::customParameters() const { return m_customParameters; }

void KlocworkOptions::setCustomParameters(const QString &customParameters) {
    m_customParameters = customParameters;
}

bool KlocworkOptions::checkOnSave() const { return m_checkOnSave; }

void KlocworkOptions::setCheckOnSave(bool checkOnSave) {
    m_checkOnSave = checkOnSave;
}

QString KlocworkOptions::kwLicenseHost() const { return m_kwLicenseHost; }

void KlocworkOptions::setKwLicenseHost(const QString &kwLicenseHost) {
    m_kwLicenseHost = kwLicenseHost;
}

QString KlocworkOptions::kwLicensePort() const { return m_kwLicensePort; }

void KlocworkOptions::setKwLicensePort(const QString &kwLicensePort) {
    m_kwLicensePort = kwLicensePort;
}
