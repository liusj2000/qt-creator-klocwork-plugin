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

#include "optionspage.h"
#include "optionswidget.h"
#include "klocworkpluginconstants.h"

#include <utils/qtcassert.h>

#include <QPointer>

using namespace Klocwork::Internal;

KlocworkOptionsPage::KlocworkOptionsPage(KlocworkOptions *options,
                                         QObject *parent)
    : IOptionsPage(parent), m_options(options) {
    QTC_ASSERT(m_options != 0, return );
    setId(Constants::OPTIONS_PAGE_ID);
    setDisplayName(tr("Klocwork"));
    setCategory(Constants::OPTIONS_CATEGORY_ID);
    setCategoryIcon(QLatin1String(Constants::OPTIONS_CATEGORY_ICON));

    m_keyWords << QLatin1String("klocwork");
}

KlocworkOptionsPage::~KlocworkOptionsPage() { m_options = 0; }

bool KlocworkOptionsPage::matches(const QString &searchKeyWord) const {
    return m_keyWords.contains(searchKeyWord);
}

QWidget *KlocworkOptionsPage::widget() { return createPage(0); }

QWidget *KlocworkOptionsPage::createPage(QWidget *parent) {
    QTC_ASSERT(m_options != 0, return 0);
    m_widget = new OptionsWidget(m_options, parent);
    Q_CHECK_PTR(m_widget.data());
    return m_widget.data();
}

void KlocworkOptionsPage::apply() {
    QTC_ASSERT(!m_widget.isNull(), return );
    m_widget->applySettings();
    emit settingsChanged();
}

void KlocworkOptionsPage::finish() {}
