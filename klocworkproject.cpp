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

#include "klocworkproject.h"
#include "klocworkpluginconstants.h"

#include <coreplugin/id.h>

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/project.h>
#include <projectexplorer/target.h>

#include <QObject>

using namespace Klocwork::Internal;
using namespace ProjectExplorer;

KlocworkProject::KlocworkProject(Project *project) : m_project(project) {
    m_klocworkProjectDir = project->activeTarget()
                               ->activeBuildConfiguration()
                               ->buildDirectory()
                               .appendPath(QObject::tr(".kwlp"))
                               .toString();
    m_klocworkSettingsDir = project->activeTarget()
                                ->activeBuildConfiguration()
                                ->buildDirectory()
                                .appendPath(QObject::tr(".kwps"))
                                .toString();
    m_klocworkReportsFile = project->activeTarget()
                                ->activeBuildConfiguration()
                                ->buildDirectory()
                                .appendPath(QObject::tr("klocworkResults.csv"))
                                .toString();

    m_klocworkServerProject =
        project->namedSettings(QObject::tr(Constants::KW_SETTINGS_PROJECT))
            .toString();
    m_klocworkBuildSpec =
        project->namedSettings(QObject::tr(Constants::KW_SETTINGS_BUILD_SPEC))
            .toString();
}

bool KlocworkProject::updateSettings() {
    bool returnValue = false;
    m_klocworkBuildSpec =
        m_project->namedSettings(QObject::tr(Constants::KW_SETTINGS_BUILD_SPEC))
            .toString();

    if (m_klocworkServerProject !=
        m_project->namedSettings(QObject::tr(Constants::KW_SETTINGS_PROJECT))
            .toString()) {
        m_klocworkServerProject =
            m_project->namedSettings(
                           QObject::tr(Constants::KW_SETTINGS_PROJECT))
                .toString();
        returnValue = true;
    }
    if (m_klocworkServerHost !=
        m_project->namedSettings(
                       QObject::tr(Constants::KW_SETTINGS_SERVER_HOST))
            .toString()) {
        m_klocworkServerHost =
            m_project->namedSettings(
                           QObject::tr(Constants::KW_SETTINGS_SERVER_HOST))
                .toString();
        returnValue = true;
    }
    if (m_klocworkServerPort !=
        m_project->namedSettings(
                       QObject::tr(Constants::KW_SETTINGS_SERVER_PORT))
            .toString()) {
        m_klocworkServerPort =
            m_project->namedSettings(
                           QObject::tr(Constants::KW_SETTINGS_SERVER_PORT))
                .toString();
        returnValue = true;
    }

    m_klocworkProjectDir = m_project->activeTarget()
                               ->activeBuildConfiguration()
                               ->buildDirectory()
                               .appendPath(QObject::tr(".kwlp"))
                               .toString();
    m_klocworkSettingsDir = m_project->activeTarget()
                                ->activeBuildConfiguration()
                                ->buildDirectory()
                                .appendPath(QObject::tr(".kwps"))
                                .toString();
    m_klocworkReportsFile = m_project->activeTarget()
                                ->activeBuildConfiguration()
                                ->buildDirectory()
                                .appendPath(QObject::tr("klocworkResults.csv"))
                                .toString();

    return returnValue;
}

Project *KlocworkProject::getProject() { return m_project; }

const Core::Id KlocworkProject::getProjectId() const { return m_project->id(); }

const QString KlocworkProject::getKlocworkServerProject() const {
    return m_klocworkServerProject;
}

const QString KlocworkProject::getKlocworkServerHost() const {
    return m_klocworkServerHost;
}

const QString KlocworkProject::getKlocworkServerPort() const {
    return m_klocworkServerPort;
}

const QString KlocworkProject::getKlocworkBuildSpec() const {
    return m_klocworkBuildSpec;
}

const QString &KlocworkProject::getKlocworkProjectDir() const {
    return m_klocworkProjectDir;
}

const QString &KlocworkProject::getKlocworkSettingsDir() const {
    return m_klocworkSettingsDir;
}

const QString &KlocworkProject::getKlocworkReportsFile() const {
    return m_klocworkReportsFile;
}
