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

#ifndef OPTIONSWIDGET_H
#define OPTIONSWIDGET_H

#include <QWidget>
#include <QProcess>

namespace Ui {
class OptionsWidget;
}

namespace Klocwork {
namespace Internal {
class KlocworkOptions;

/*
 * Widget in Options page to edit plugin's settings.
 * Keeps actual settings in settings_. Updates them in applySettings().
 * Does not have ownership on settings_ (options widget begin destroyed before
 * settings).
 * Reads settings and inits self interface in initInterface() on creation.
 */
class OptionsWidget : public QWidget {
    Q_OBJECT

  public:
    explicit OptionsWidget(KlocworkOptions *settings, QWidget *parent = 0);
    ~OptionsWidget();

    void applySettings();

  private slots:
    void selectBinaryFile();

  private:
    void initInterface();

  private:
    Ui::OptionsWidget *m_ui;
    KlocworkOptions *m_options;
    QProcess m_process;
    QStringList m_processArguments;
};

} // namespace Internal
} // namespace KlocworkPlugin

#endif // OPTIONSWIDGET_H
