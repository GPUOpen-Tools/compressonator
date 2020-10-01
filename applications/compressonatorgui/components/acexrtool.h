#ifndef ACEXRTOOL_H
#define ACEXRTOOL_H

#include <QtWidgets>
#include <QtWidgets/QDialog>
#include <QDoubleSpinBox>
//#include <qslider.h>
#include <qdialogbuttonbox.h>
#include <qimagereader.h>
#include <qlayout.h>
#include <qlabel.h>
#include <QIcon>
#include <assert.h>

#define DEFAULT_EXPOSURE 0
#define DEFAULT_DEFOG    0
#define DEFAULT_KNEELOW  0
#define DEFAULT_KNEEHIGH 5
#define DEFAULT_GAMMA    2.2

class acEXRTool : public QDialog {
    Q_OBJECT

  public:
    acEXRTool(QWidget *parent = 0);
    void Init(const QString& title, const QString& productName);
    ~acEXRTool();

    QDoubleSpinBox *exrExposureBox;
    QDoubleSpinBox *exrDefogBox;
    QDoubleSpinBox *exrKneeLowBox;
    QDoubleSpinBox *exrKneeHighBox;
    QDoubleSpinBox *exrGammaBox;

  public slots:
    void onClose();


  private:
    QPushButton      *m_PBClose;
};

#endif // ACEXRTOOL_H
