#ifndef ACDIFFIMAGE_H
#define ACDIFFIMAGE_H

#include <QtWidgets>
#include <QtWidgets/QDialog>
#include <qlayout.h>
#include <qlabel.h>
#include <qfiledialog.h>
#include <QPushButton>
#include <QComboBox>
#include <QString>
#include <assert.h>

class acDiffImage : public QDialog {
    Q_OBJECT

  public:
    acDiffImage(QWidget *parent = 0);
    void Init(const QString& title, const QString& productName);
    ~acDiffImage();

    QString           m_RecentImageDir;
    QString           m_SupportedImageFormat;

    QComboBox        *m_file1Name;
    QComboBox        *m_file2Name;

  public slots:
    void onClose();
    void onOkDiff();
    void onOpenFile1();
    void onOpenFile2();

  private:
    QPushButton      *m_PBOk;
    QPushButton      *m_PBClose;
    QPushButton      *m_PBFile1;
    QPushButton      *m_PBFile2;
    QWidget          *m_parent;
};

#endif // ACDIFFIMAGE_H
