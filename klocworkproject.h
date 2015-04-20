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

#ifndef KLOCWORKPROJECT_H
#define KLOCWORKPROJECT_H

#include <QString>

namespace Core {
    class Id;
}

namespace ProjectExplorer {
    class Project;
}

namespace Klocwork {
namespace Internal {

/*
 * Stores Klocwork settings for each project open
 */
class KlocworkProject
{
public:
    KlocworkProject(ProjectExplorer::Project *project);

    bool updateSettings();

    ProjectExplorer::Project *getProject();
    const Core::Id getProjectId() const;

    const QString getKlocworkServerProject() const;
    const QString getKlocworkServerHost() const;
    const QString getKlocworkServerPort() const;

    const QString getKlocworkBuildSpec() const;

    const QString &getKlocworkProjectDir() const;

    const QString &getKlocworkSettingsDir() const;

    const QString &getKlocworkReportsFile() const;

private:
    ProjectExplorer::Project *m_project;
    QString m_klocworkServerProject;
    QString m_klocworkServerHost;
    QString m_klocworkServerPort;
    QString m_klocworkBuildSpec;
    QString m_klocworkProjectDir;
    QString m_klocworkSettingsDir;
    QString m_klocworkReportsFile;
};


} // namespace Internal
} // namespace KlocworkPlugin
#endif // KLOCWORKPROJECT_H
