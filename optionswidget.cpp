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

#include "optionswidget.h"
#include "klocworkoptions.h"
#include "ui_optionswidget.h"

#include <projectexplorer/project.h>

#include <QFileDialog>
#include <QTextStream>
#include <QTextEdit>

using namespace Klocwork::Internal;

OptionsWidget::OptionsWidget(KlocworkOptions *settings, QWidget *parent)
    : QWidget(parent), m_ui(new Ui::OptionsWidget), m_options(settings) {
    Q_ASSERT(m_options != 0);

    m_ui->setupUi(this);

    connect(m_ui->binSelectButton, SIGNAL(clicked()), SLOT(selectBinaryFile()));

    initInterface();
}

OptionsWidget::~OptionsWidget() {
    delete m_ui;
    m_options = 0;
}

void OptionsWidget::selectBinaryFile() {
    QString fileName = QFileDialog::getOpenFileName();
    if (!fileName.isEmpty()) {
        m_ui->binFileEdit->setText(fileName);
    }
}

void OptionsWidget::applySettings() {
    Q_ASSERT(m_options != 0);
    m_options->setBinaryFile(m_ui->binFileEdit->text());
    m_options->setCheckOnSave(m_ui->onSaveCheckBox->isChecked());
    m_options->setCustomParameters(m_ui->customParametersEdit->text());
    m_options->setKwLicenseHost(m_ui->kwLicenseHostEdit->text());
    m_options->setKwLicensePort(m_ui->kwLicensePortEdit->text());
    m_options->save();
}

void OptionsWidget::initInterface() {
    Q_ASSERT(m_options != 0);
    m_ui->binFileEdit->setText(m_options->binaryFile());
    m_ui->onSaveCheckBox->setChecked(m_options->checkOnSave());
    m_ui->customParametersEdit->setText(m_options->customParameters());
    m_ui->kwLicenseHostEdit->setText(m_options->kwLicenseHost());
    m_ui->kwLicensePortEdit->setText(m_options->kwLicensePort());
}
