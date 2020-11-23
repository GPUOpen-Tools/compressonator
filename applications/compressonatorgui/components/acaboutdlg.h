#ifndef CPABOUTDLG_H
#define CPABOUTDLG_H

#include <QtWidgets/QDialog>
#include <qdialogbuttonbox.h>
#include <qimagereader.h>
#include <qlayout.h>
#include <qlabel.h>
#include <QIcon>
#include <assert.h>

class CHelpAboutDialog : public QDialog {
    Q_OBJECT

  public:
    CHelpAboutDialog(QWidget *parent = 0);
    void Init(const QString& title, const QString& productName);
    ~CHelpAboutDialog();

};

#endif // CPABOUTDLG_H
