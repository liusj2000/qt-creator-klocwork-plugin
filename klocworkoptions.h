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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

namespace Klocwork {
namespace Internal {

/*
 * Klocwork Plugin's settings.
 * Keeps actual settings.
 * Saves settings in Core::ICore::settings().
 */
class KlocworkOptions {
  public:
    KlocworkOptions();

    void save();
    void load();

    QString binaryFile() const;
    void setBinaryFile(const QString &binaryFile);

    bool checkOnSave() const;
    void setCheckOnSave(bool checkOnSave);

    QString customParameters() const;
    void setCustomParameters(const QString &customParameters);

    QString kwServerProject() const;
    void setKwServerProject(const QString &kwServerProject);

    QString kwBuildSpec() const;
    void setKwBuildSpec(const QString &kwBuildSpec);

    QString kwLicenseHost() const;
    void setKwLicenseHost(const QString &kwLicenseHost);

    QString kwLicensePort() const;
    void setKwLicensePort(const QString &kwLicensePort);

  private:
    QString m_binaryFile;

    bool m_checkOnSave;

    QString m_customParameters;

    QString m_kwLicenseHost;
    QString m_kwLicensePort;
};

} // namespace Internal
} // namespace KlocworkPlugin

#endif // SETTINGS_H
