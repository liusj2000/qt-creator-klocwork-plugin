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

#ifndef OPTIONSPAGE_H
#define OPTIONSPAGE_H

#include <coreplugin/dialogs/ioptionspage.h>
#include <QPointer>

namespace Klocwork {
namespace Internal {

class KlocworkOptions;
class OptionsWidget;

/*
 * Klocwork Options page.
 * Creates edition widget. Emits settingsChanged().
 */
class KlocworkOptionsPage : public Core::IOptionsPage {
    Q_OBJECT
  public:
    explicit KlocworkOptionsPage(KlocworkOptions *settings, QObject *parent = 0);
    ~KlocworkOptionsPage();

    bool matches(const QString &searchKeyWord) const;
    QWidget *widget();
    QWidget *createPage(QWidget *parent);
    void apply();
    void finish();

signals:
    void settingsChanged();

  private:
    QPointer<OptionsWidget> m_widget;
    KlocworkOptions *m_options;
    QStringList m_keyWords;
};

} // namespace Internal
} // namespace KlocworkPlugin

#endif // OPTIONSPAGE_H
