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

#ifndef KLOCWORKPLUGIN_H
#define KLOCWORKPLUGIN_H

#include "klocworkplugin_global.h"

#include <extensionsystem/iplugin.h>
#include <QModelIndex>
#include <QStringList>
#include <QPointer>

namespace ProjectExplorer {
    class Project;
    class ProjectPanelFactory;
}

namespace Klocwork {
namespace Internal {

class KlocworkRunner;
class KlocworkOptions;
class IssueOutputPane;

class KlocworkPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "Klocwork.json")

public:
    KlocworkPlugin();
    ~KlocworkPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private slots:
    void analyseSelection();
    void projectOpenened(ProjectExplorer::Project*);
    void projectClosed(ProjectExplorer::Project*);
    void cleanKlocworkProject();
    void updateSettings();

private:


    IssueOutputPane *m_issueOutputPane;
    // AnalysisOutputPane *m_analysisOutputPane;
    KlocworkOptions* m_options;
    KlocworkRunner *m_runner;

    ProjectExplorer::ProjectPanelFactory *m_klocworkSettingsPanelFactory;
};

} // namespace Internal
} // namespace KlocworkPlugin

#endif // KLOCWORKPLUGIN_H

