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

#ifndef KLOCWORKPROJECTSETTINGS_H
#define KLOCWORKPROJECTSETTINGS_H

#include "ui_klocworkprojectsettings.h"

#include <QWidget>

namespace ProjectExplorer { class Project; }

namespace Ui { class KlocworkProjectSettingsPropertiesPage; }

namespace Klocwork {
namespace Internal {

/*
 * Provides the means for entering Klocwork project settings.
 */
class KlocworkProjectSettings : public QWidget
{
    Q_OBJECT
public:
    KlocworkProjectSettings(ProjectExplorer::Project *project);

signals:
    void klocworkProjectSettingsSaved();

private slots:
    void saveKlocworkProject();
    void saveKlocworkBuildSpec();
    void saveKlocworkServerHost();
    void saveKlocworkServerPort();

private:
    Ui::KlocworkProjectSettingsPropertiesPage m_ui;
    ProjectExplorer::Project *m_project;
};

} // namespace Internal
} // namespace KlocworkPlugin

#endif // KLOCWORKPROJECTSETTINGS_H
