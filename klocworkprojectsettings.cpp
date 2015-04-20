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

#include "klocworkprojectsettings.h"
#include "klocworkpluginconstants.h"

#include <projectexplorer/project.h>

using namespace ProjectExplorer;

using namespace Klocwork::Constants;
using namespace Klocwork::Internal;

KlocworkProjectSettings::KlocworkProjectSettings(Project *project)
    : QWidget(), m_project(project) {

    m_ui.setupUi(this);

    m_ui.klocworkProjectEdit->setText(
        m_project->namedSettings(tr(KW_SETTINGS_PROJECT)).toString());
    m_ui.klocworkBuildSpecEdit->setText(
        m_project->namedSettings(tr(KW_SETTINGS_BUILD_SPEC)).toString());
    m_ui.klocworkServerHostEdit->setText(
        m_project->namedSettings(tr(KW_SETTINGS_SERVER_HOST)).toString());
    m_ui.klocworkServerPortEdit->setText(
        m_project->namedSettings(tr(KW_SETTINGS_SERVER_PORT)).toString());

    connect(m_ui.klocworkProjectEdit, SIGNAL(editingFinished()), this,
            SLOT(saveKlocworkProject()));

    connect(m_ui.klocworkBuildSpecEdit, SIGNAL(editingFinished()), this,
            SLOT(saveKlocworkBuildSpec()));

    connect(m_ui.klocworkServerHostEdit, SIGNAL(editingFinished()), this,
            SLOT(saveKlocworkServerHost()));

    connect(m_ui.klocworkServerPortEdit, SIGNAL(editingFinished()), this,
            SLOT(saveKlocworkServerPort()));
}

void KlocworkProjectSettings::saveKlocworkProject() {
    m_project->setNamedSettings(tr(KW_SETTINGS_PROJECT),
                                QVariant(m_ui.klocworkProjectEdit->text()));
}

void KlocworkProjectSettings::saveKlocworkBuildSpec() {
    m_project->setNamedSettings(tr(KW_SETTINGS_BUILD_SPEC),
                                QVariant(m_ui.klocworkBuildSpecEdit->text()));
}

void KlocworkProjectSettings::saveKlocworkServerHost() {
    m_project->setNamedSettings(tr(KW_SETTINGS_SERVER_HOST),
                                QVariant(m_ui.klocworkServerHostEdit->text()));
}

void KlocworkProjectSettings::saveKlocworkServerPort() {
    m_project->setNamedSettings(tr(KW_SETTINGS_SERVER_PORT),
                                QVariant(m_ui.klocworkServerPortEdit->text()));
}
