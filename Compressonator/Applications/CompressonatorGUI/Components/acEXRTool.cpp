#include "acEXRTool.h"

acEXRTool::acEXRTool(QWidget *parent)
    : QDialog(parent)
{
    Init("HDR Properties","Compressonator");
}

void acEXRTool::Init(const QString & title, const QString & productName)
{
    // Set the dialog title:
    setWindowTitle(title);
    resize(300,150);

    // Set window flags (minimize / maximize / close buttons):
    Qt::WindowFlags flags = windowFlags();
    flags &= ~Qt::WindowContextHelpButtonHint;
    flags |= Qt::WindowStaysOnTopHint;
    setWindowFlags(flags);
 

    // layout components:
    QHBoxLayout* pMainLayout = new QHBoxLayout;
  
    QVBoxLayout* pVerticalLayout = new QVBoxLayout;
    
    //Add exposure, defog, kneelow, kneehigh slider widget
    QHBoxLayout* exposureLayout = new QHBoxLayout;
    QLabel *exposureLabel = new QLabel(this);
    exposureLabel->setText("Exposure:");
    exrExposureBox = new QDoubleSpinBox;
    exrExposureBox->setValue(DEFAULT_EXPOSURE);
    exrExposureBox->setDecimals(3);
    exrExposureBox->setRange(-10, 10);
    exrExposureBox->setSingleStep(0.125);
    
    exposureLayout->addWidget(exposureLabel);
    exposureLayout->addWidget(exrExposureBox);

    QHBoxLayout* defogLayout = new QHBoxLayout;
    QLabel *defogLabel = new QLabel(this);
    defogLabel->setText("Defog:");
    exrDefogBox = new QDoubleSpinBox;
    exrDefogBox->setDecimals(3);
    exrDefogBox->setRange(0, 0.01);
    exrDefogBox->setValue(DEFAULT_DEFOG);
    exrDefogBox->setSingleStep(0.001);

    defogLayout->addWidget(defogLabel);
    defogLayout->addWidget(exrDefogBox);

    QHBoxLayout* klLayout = new QHBoxLayout;
    QLabel *klLabel = new QLabel(this);
    klLabel->setText("Knee Low:");
    exrKneeLowBox = new QDoubleSpinBox;
    exrKneeLowBox->setDecimals(3);
    exrKneeLowBox->setRange(-3, 3);
    exrKneeLowBox->setValue(DEFAULT_KNEELOW);
    exrKneeLowBox->setSingleStep(0.125);

    klLayout->addWidget(klLabel);
    klLayout->addWidget(exrKneeLowBox);

    QHBoxLayout* khLayout = new QHBoxLayout;
    QLabel *khLabel = new QLabel(this);
    khLabel->setText("Knee High:");
    exrKneeHighBox = new QDoubleSpinBox;
    exrKneeHighBox->setDecimals(3);
    exrKneeHighBox->setRange(3.5, 7.5);
    exrKneeHighBox->setValue(DEFAULT_KNEEHIGH);
    exrKneeHighBox->setSingleStep(0.125);

    khLayout->addWidget(khLabel);
    //exposureLayout->addWidget(exrExposure);
    khLayout->addWidget(exrKneeHighBox);

    // Buttons
    m_PBClose = new QPushButton("Close");
    //QDialogButtonBox* pCloseButtonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Vertical);

    connect(m_PBClose, SIGNAL(pressed()), this, SLOT(onClose()));
    
    // Left Vertical Layout:
    //pVerticalLayoutLeft->addStretch(1);
    pVerticalLayout->addLayout(exposureLayout);
    //pVerticalLayoutLeft->addSpacing(5);
    pVerticalLayout->addLayout(defogLayout);
    //pVerticalLayoutLeft->addStretch(1);
    pVerticalLayout->addLayout(klLayout);
    pVerticalLayout->addLayout(khLayout);
    //pVerticalLayoutLeft->addSpacing(5);
    pVerticalLayout->addWidget(m_PBClose, 0, Qt::AlignCenter | Qt::AlignBottom);
    pVerticalLayout->setMargin(10);

    // Main Horizontal Layout:
    //pMainLayout->addLayout(pVerticalLayoutRight);
    pMainLayout->addLayout(pVerticalLayout);
    pMainLayout->setMargin(0);

    // Activate:
    setLayout(pMainLayout);
}

acEXRTool::~acEXRTool()
{

}



void acEXRTool::onClose()
{
    close();
}
