#include "acdiffimage.h"
#include "cpprojectview.h"
acDiffImage::acDiffImage(QWidget *parent)
    : QDialog(parent) {
    m_parent = parent;
    Init("Image Diff","Compressonator");
}

void acDiffImage::Init(const QString & title, const QString & productName) {
    Q_UNUSED(productName)

    // Set the dialog title:
    setWindowTitle(title);
    resize(400,120);

    // Set window flags (minimize / maximize / close buttons):
    Qt::WindowFlags flags = windowFlags();
    flags &= ~Qt::WindowContextHelpButtonHint;
    flags |= Qt::WindowStaysOnTopHint;
    setWindowFlags(flags);

    m_RecentImageDir="";
    m_SupportedImageFormat = "";

    // layout components:
    QHBoxLayout* pMainLayout = new QHBoxLayout;

    QVBoxLayout* pVerticalLayout = new QVBoxLayout;

    // Buttons
    m_PBFile1 = new QPushButton("...");
    connect(m_PBFile1, SIGNAL(pressed()), this, SLOT(onOpenFile1()));

    m_PBFile2 = new QPushButton("...");
    connect(m_PBFile2, SIGNAL(pressed()), this, SLOT(onOpenFile2()));

    m_PBOk = new QPushButton("OK");
    connect(m_PBOk, SIGNAL(pressed()), this, SLOT(onOkDiff()));

    m_PBClose = new QPushButton("Close");
    connect(m_PBClose, SIGNAL(pressed()), this, SLOT(onClose()));

    //Add file1 and file2 ui component for diff
    QHBoxLayout* file1Layout = new QHBoxLayout;
    QLabel *file1Label = new QLabel(this);
    file1Label->setText("Image File #1: ");

    m_file1Name = new QComboBox;
    m_file1Name->setEditable(true);
    m_file1Name->setMinimumSize(300, 12);
    file1Layout->addWidget(file1Label);
    file1Layout->addWidget(m_file1Name);
    file1Layout->addWidget(m_PBFile1);

    QHBoxLayout* file2Layout = new QHBoxLayout;
    QLabel *file2Label = new QLabel(this);
    file2Label->setText("Image File #2: ");

    m_file2Name = new QComboBox;
    m_file2Name->setEditable(true);
    m_file2Name->setMinimumSize(300, 12);
    file2Layout->addWidget(file2Label);
    file2Layout->addWidget(m_file2Name);
    file2Layout->addWidget(m_PBFile2);

    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(m_PBOk);
    btnLayout->addWidget(m_PBClose);

    pVerticalLayout->addLayout(file1Layout);
    pVerticalLayout->addLayout(file2Layout);
    pVerticalLayout->addLayout(btnLayout);
    pVerticalLayout->setMargin(10);

    // Main Horizontal Layout:
    pMainLayout->addLayout(pVerticalLayout);
    pMainLayout->setMargin(0);

    // Activate:
    setLayout(pMainLayout);
}

acDiffImage::~acDiffImage() {

}

void acDiffImage::onOpenFile1() {
    //QFileDialog to open image file #1
    QString file1 = QFileDialog::getOpenFileName(this, tr("Open image file #1 for Diff"), m_RecentImageDir, m_SupportedImageFormat);
    if (!file1.isEmpty()) {
        m_file1Name->setEditText(file1);
    }
}

void acDiffImage::onOpenFile2() {
    //QFileDialog to open image file #2
    QString file2 = QFileDialog::getOpenFileName(this, tr("Open image file #2 for Diff"), m_RecentImageDir, m_SupportedImageFormat);
    if (!file2.isEmpty()) {
        m_file2Name->setEditText(file2);
    }
}

void acDiffImage::onOkDiff() {
    // Create a new view image diff
    if ((m_file1Name == NULL) || (m_file2Name == NULL)) {
        this->hide();
        return;
    }

    if (((m_file1Name->currentText()).trimmed().isEmpty()) || ((m_file2Name->currentText()).trimmed().isEmpty())) {
        this->hide();
        return;
    }

    if (m_parent != NULL) {
        this->hide();
        QString fileName1 = m_file1Name->currentText();
        QString fileName2 = m_file2Name->currentText();
        emit ((ProjectView*)m_parent)->ViewImageFileDiff(NULL, fileName1, fileName2);
    }

    this->hide();
}

void acDiffImage::onClose() {
    close();
}
