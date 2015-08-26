/*

    Copyright (c) 2015 Oliver Lau <ola@ct.de>, Heise Medien GmbH & Co. KG

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <QDebug>
#include <QList>
#include <QSslCipher>
#include <QSslKey>
#include <QFormLayout>
#include <QBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "servercertificatewidget.h"
#include "ui_servercertificatewidget.h"

ServerCertificateWidget::ServerCertificateWidget(QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::ServerCertificateWidget)
{
  ui->setupUi(this);
  QObject::connect(ui->acceptPushButton, SIGNAL(pressed()), SLOT(accept()));
  QObject::connect(ui->rejectPushButton, SIGNAL(pressed()), SLOT(reject()));
}


ServerCertificateWidget::~ServerCertificateWidget()
{
  delete ui;
}


void ServerCertificateWidget::setServerSocket(const QSslSocket &sslSocket)
{
  auto fingerprintify = [](const QByteArray &ba) {
    const QByteArray &baHex = ba.toHex();
    QByteArray result = baHex;
    for (int i = baHex.size() - 2; i > 0; i -= 2)
      result.insert(i, QChar(':'));
    return result;
  };

  const QSslCipher &cipher = sslSocket.sessionCipher();

  QFormLayout *formLayout = new QFormLayout;
  formLayout->addRow(tr("Encryption"), new QLabel(cipher.name()));
  formLayout->addRow(tr("Protocol"), new QLabel(cipher.protocolString()));
  formLayout->addRow(tr("Supported bits"), new QLabel(QString("%1").arg(cipher.supportedBits())));
  formLayout->addRow(tr("Used bits"), new QLabel(QString("%1").arg(cipher.usedBits())));

  QGroupBox *groupBox = new QGroupBox(tr("SSL parameters"));
  groupBox->setLayout(formLayout);

  QTreeWidget *treeWidget = new QTreeWidget;
  treeWidget->setColumnCount(2);
  treeWidget->setHeaderLabels(QStringList({tr("Serial Number"), QString()}));
  QTreeWidgetItem *firstItem = nullptr;
  QTreeWidgetItem *lastItem = nullptr;
  foreach (QSslCertificate cert, sslSocket.peerCertificateChain()) {
    QTreeWidgetItem *rootItem = new QTreeWidgetItem;
    if (firstItem == nullptr)
      firstItem = rootItem;
    treeWidget->addTopLevelItem(rootItem);
    rootItem->setText(0, QString(cert.serialNumber()));
    rootItem->setText(1, QString());
    QList<QTreeWidgetItem*> items;
    items.append(new QTreeWidgetItem(
                   (QTreeWidget*)nullptr,
                   QStringList({tr("Fingerprint (SHA1)"),
                                fingerprintify(cert.digest(QCryptographicHash::Sha1))})));
    items.append(new QTreeWidgetItem(
                   (QTreeWidget*)nullptr,
                   QStringList({tr("Fingerprint (MD5)"),
                                fingerprintify(cert.digest(QCryptographicHash::Md5))})));
    items.append(new QTreeWidgetItem(
                   (QTreeWidget*)nullptr,
                   QStringList({tr("Effective date"),
                                cert.effectiveDate().toString()})));
    items.append(new QTreeWidgetItem(
                   (QTreeWidget*)nullptr,
                   QStringList({tr("Expiry date"),
                                cert.expiryDate().toString()})));
    items.append(new QTreeWidgetItem(
                   (QTreeWidget*)nullptr,
                   QStringList({
                                 tr("Issuer"),
                                 QString("/C=%1/ST=%2/L=%3/O=%4/OU=%5/CN=%6/emailAddress=%7")
                                 .arg(cert.issuerInfo(QSslCertificate::CountryName).join(", "))
                                 .arg(cert.issuerInfo(QSslCertificate::StateOrProvinceName).join(", "))
                                 .arg(cert.issuerInfo(QSslCertificate::LocalityName).join(", "))
                                 .arg(cert.issuerInfo(QSslCertificate::Organization).join(", "))
                                 .arg(cert.issuerInfo(QSslCertificate::OrganizationalUnitName).join(", "))
                                 .arg(cert.issuerInfo(QSslCertificate::CommonName).join(", "))
                                 .arg(cert.issuerInfo(QSslCertificate::EmailAddress).join(", "))
                               }
                               )));
    items.append(new QTreeWidgetItem(
                   (QTreeWidget*)nullptr,
                   QStringList({
                                 tr("Subject"),
                                 QString("/C=%1/ST=%2/L=%3/O=%4/OU=%5/CN=%6/emailAddress=%7")
                                 .arg(cert.subjectInfo(QSslCertificate::CountryName).join(", "))
                                 .arg(cert.subjectInfo(QSslCertificate::StateOrProvinceName).join(", "))
                                 .arg(cert.subjectInfo(QSslCertificate::LocalityName).join(", "))
                                 .arg(cert.subjectInfo(QSslCertificate::Organization).join(", "))
                                 .arg(cert.subjectInfo(QSslCertificate::OrganizationalUnitName).join(", "))
                                 .arg(cert.subjectInfo(QSslCertificate::CommonName).join(", "))
                                 .arg(cert.subjectInfo(QSslCertificate::EmailAddress).join(", "))
                               }
                               )));
    items.append(new QTreeWidgetItem(
                   (QTreeWidget*)nullptr,
                   QStringList({tr("Version"), QString(cert.version())})));
    rootItem->addChildren(items);
    lastItem = rootItem;
  }
  if (firstItem != nullptr) {
    treeWidget->expandItem(firstItem);
  }
  if (lastItem != nullptr) {
    treeWidget->expandItem(lastItem);
    lastItem->setSelected(true);
  }
  treeWidget->resizeColumnToContents(0);
  treeWidget->resizeColumnToContents(1);

  QBoxLayout *vLayout = new QBoxLayout(QBoxLayout::TopToBottom);
  vLayout->addWidget(groupBox);
  vLayout->addWidget(treeWidget);

  if (ui->scrollArea->layout() != nullptr)
    delete ui->scrollArea->layout();
  ui->scrollArea->setLayout(vLayout);
}
