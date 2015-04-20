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

#ifndef KLOCWORKPLUGINCONSTANTS_H
#define KLOCWORKPLUGINCONSTANTS_H

namespace Klocwork {
namespace Constants {

const char OPTIONS_PAGE_ID[] = "KlocworkPlugin.OptionsPage";
const char OPTIONS_CATEGORY_ID[] = "T.Analyzer"; // from valgrind
const char OPTIONS_CATEGORY_ICON[] =
    ":/images/analyzer_category.png"; // from valgrind

const char KW_OPTIONS_GROUP[] = "KlocworkPlugin.Setting";
const char KW_OPTIONS_BINARY_FILE[] = "KlocworkPlugin.Setting.BinaryFile";
const char KW_OPTIONS_CHECK_ON_SAVE[] = "KlocworkPlugin.Setting.CheckOnSave";
const char KW_OPTIONS_CUSTOM_PARAMS[] = "KlocworkPlugin.Setting.CustomParams";
const char KW_OPTIONS_SHOW_OUTPUT[] = "KlocworkPlugin.Setting.ShowOutput";

const char KW_OPTIONS_LICENSE_HOST[] = "KlocworkPlugin.Setting.KwLicenseHost";
const char KW_OPTIONS_LICENSE_PORT[] = "KlocworkPlugin.Setting.KwLicensePort";

const char KW_SETTINGS_PROJECT[] = "KlocworkPlugin.Setting.Project";
const char KW_SETTINGS_BUILD_SPEC[] = "KlocworkPlugin.Setting.BuildSpec";
const char KW_SETTINGS_SERVER_HOST[] = "KlocworkPlugin.Setting.KwServerHost";
const char KW_SETTINGS_SERVER_PORT[] = "KlocworkPlugin.Setting.KwServerPort";

const char TASK_CATEGORY_ID[] = "KlocworkPlugin.TaskCategory";
const char TASK_CATEGORY_NAME[] = "Klocwork";

const char OUTPUT_PANE_KW_ANALYSIS[] = "kwOutputPane";

const char ACTION_ID_CLEANPROJECT[] = "KlocworkPlugin.Action.CleanProject";
const char ACTION_ID_ANALYSESELECTION[] =
    "KlocworkPlugin.Action.AnalyseSelection";
const char MENU_ID[] = "KlocworkPlugin.Menu";

const char KW_STATUS_LIST[] = "Analyze,Ignore,Not a Problem,Fix,Fix in Next "
                              "Release,Fix in Later Release,Defer,Filter";

namespace KlocworkIssueColumn {
enum {
    Icon = 0,
    Message,
    File,
    LineNo,
    Code,
    Origin,
    Status,
    Severity,
    Id,
    NumFields
};
}

namespace KlocworkNodeType {
enum { ProjectNode = 0, IssueNode };
}

} // namespace KlocworkPlugin
} // namespace Constants

#endif // KLOCWORKPLUGINCONSTANTS_H
