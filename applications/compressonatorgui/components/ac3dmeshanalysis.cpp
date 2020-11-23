#include "ac3dmeshanalysis.h"
#include "cpprojectview.h"


ac3DMeshAnalysis::ac3DMeshAnalysis(QWidget *parent)
    : QDialog(parent) {
    m_parent          = parent;
    m_meshData.clear();
    m_meshDataCompare.clear();
    m_cacheSize       = DEFAULT_HWVCACHE_SZ;
    m_warpSize        = DEFAULT_WARP_SZ;
    m_bufferSize      = DEFAULT_PRIMITIVEBUFFER_SZ;

    m_cacheLineSize  = DEFAULT_CACHE_LINE_BYTES;
    m_totalCacheSize = DEFAULT_CACHE_TOTAL_BYTES;

    m_orthographcamXYmax = DEFAULT_ORTHOGRAHIC_X_Y_MAX;

    Init("3D Mesh Analysis","Compressonator");
    setMinimumWidth(400);
}

void ac3DMeshAnalysis::Init(const QString & title, const QString & productName) {
    Q_UNUSED(productName)

    // Set the dialog title:
    setWindowTitle(title);

    // create UI for each analysis function
    createGBAnalyseVCache();
    createGBAnalyseVFetch();
    createGBAnalyseOverdraw();

    connect(m_GBAnalyseVCache, SIGNAL(clicked()), this, SLOT(onAnalysisChecked()));
    connect(m_GBAnalyseVFetch, SIGNAL(clicked()), this, SLOT(onAnalysisChecked()));
    connect(m_GBAnalyseOverdraw, SIGNAL(clicked()), this, SLOT(onAnalysisChecked()));

    // Set window flags (minimize / maximize / close buttons):
    Qt::WindowFlags flags = windowFlags();
    flags &= ~Qt::WindowContextHelpButtonHint;
    flags |= Qt::WindowStaysOnTopHint;
    setWindowFlags(flags);

    // layout components:
    QVBoxLayout* pMainLayout = new QVBoxLayout;

    m_PBCancel = new QPushButton("Cancel");
    connect(m_PBCancel, SIGNAL(pressed()), this, SLOT(onCancel()));

    m_PBRunAnalyse = new QPushButton("Run Mesh Analysis");
    connect(m_PBRunAnalyse, SIGNAL(pressed()), this, SLOT(onRunAnalyseMesh()));

    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(m_PBRunAnalyse);
    buttonsLayout->addWidget(m_PBCancel);

    m_textBrowser = new QTextBrowser(this);
    m_textBrowser->setReadOnly(true);
    m_textBrowser->setAcceptRichText(true);
    m_textBrowser->setMinimumHeight(32);

    // Main Vertical Layout:
    pMainLayout->addWidget(m_GBAnalyseVCache);
    pMainLayout->addWidget(m_GBAnalyseVFetch);
    pMainLayout->addWidget(m_GBAnalyseOverdraw);
    pMainLayout->addWidget(m_textBrowser);
    pMainLayout->addLayout(buttonsLayout);
    pMainLayout->setMargin(10);

    setLayout(pMainLayout);
}

void ac3DMeshAnalysis::cleanText() {
    m_textBrowser->clear();
}

ac3DMeshAnalysis::~ac3DMeshAnalysis() {

}

void ac3DMeshAnalysis::createGBAnalyseVCache() {
    m_GBAnalyseVCache = new QGroupBox(tr("&Analyse Vertex Cache"));
    m_GBAnalyseVCache->setWhatsThis("Generate ACMR(average cache miss ratio = #transformed vertex/#triangles, range from 0.5-3.0) and ATVR(average transformed vertices ratio =#transformed vertices/#vertices, range from 1.0-6.0).");
    m_GBAnalyseVCache->setCheckable(true);
    m_GBAnalyseVCache->setChecked(true);


    QHBoxLayout *vchLayout = new QHBoxLayout;
    QLabel *vcsizeLabel = new QLabel(tr("Vertex Cache Size: "));
    vcsizeLabel->setWhatsThis("Hardware GPU vertex cache size. Usually range between 16-32.");
    m_vCachesizeSBox = new QSpinBox;
    m_vCachesizeSBox->setMinimum(10);
    m_vCachesizeSBox->setSingleStep(1);
    m_vCachesizeSBox->setMaximum(100);
    m_vCachesizeSBox->setValue(m_cacheSize);
    vchLayout->addWidget(vcsizeLabel);
    vchLayout->addWidget(m_vCachesizeSBox);

    QHBoxLayout *warpLayout = new QHBoxLayout;
    QLabel *warpsizeLabel = new QLabel(tr("Warp Size (# of threads): "));
    warpsizeLabel->setWhatsThis("# of thread per wavefront group in the GPU scheduling. (usually 64)");
    m_warpSizeSBox = new QSpinBox;
    m_warpSizeSBox->setMinimum(0);
    m_warpSizeSBox->setSingleStep(16);
    m_warpSizeSBox->setMaximum(512);
    m_warpSizeSBox->setValue(m_warpSize);
    warpLayout->addWidget(warpsizeLabel);
    warpLayout->addWidget(m_warpSizeSBox);

    QHBoxLayout *primszLayout = new QHBoxLayout;
    QLabel *primsizeLabel = new QLabel(tr("Primitive(Triangle) Buffer Size: "));
    primsizeLabel->setWhatsThis("Triangles group size. Default is 128");
    m_primBufferSizeSBox = new QSpinBox;
    m_primBufferSizeSBox->setMinimum(0);
    m_primBufferSizeSBox->setSingleStep(3);
    m_primBufferSizeSBox->setMaximum(1026);
    m_primBufferSizeSBox->setValue(m_bufferSize);
    primszLayout->addWidget(primsizeLabel);
    primszLayout->addWidget(m_primBufferSizeSBox);

    QVBoxLayout* vcacheLayout = new QVBoxLayout;
    vcacheLayout->addLayout(vchLayout);
    vcacheLayout->addLayout(warpLayout);
    vcacheLayout->addLayout(primszLayout);

    m_GBAnalyseVCache->setLayout(vcacheLayout);

}

void ac3DMeshAnalysis::createGBAnalyseVFetch() {
    m_GBAnalyseVFetch = new QGroupBox(tr("&Analyse Vertex Fetch"));
    m_GBAnalyseVFetch->setWhatsThis("Generate vertex fetch statistic: bytes read from vertex buffer/total vertex buffer bytes. Best case is 1.0, all bytes fetched once.");
    m_GBAnalyseVFetch->setCheckable(true);
    m_GBAnalyseVFetch->setChecked(true);

    QHBoxLayout *clineLayout = new QHBoxLayout;
    QLabel *clinesizeLabel = new QLabel(tr("Cache Line Buffer Size(bytes): "));
    clinesizeLabel->setWhatsThis("Cache line buffer size, can be retrieved from CL info. Default is 64.");
    m_vCachelinebytesSBox = new QSpinBox;
    m_vCachelinebytesSBox->setMinimum(8);
    m_vCachelinebytesSBox->setSingleStep(1);
    m_vCachelinebytesSBox->setMaximum(1024);
    m_vCachelinebytesSBox->setValue(m_cacheLineSize);
    clineLayout->addWidget(clinesizeLabel);
    clineLayout->addWidget(m_vCachelinebytesSBox);

    QHBoxLayout *ctotalLayout = new QHBoxLayout;
    QLabel *ctotalbufferLabel = new QLabel(tr("Total Cache Buffer Size(bytes): "));
    ctotalbufferLabel->setWhatsThis("Total cache buffer size, can be retrieved from CL info. Default is 128*1024.");
    m_vTotalCachebytesSBox = new QSpinBox;
    m_vTotalCachebytesSBox->setMinimum(65536);
    m_vTotalCachebytesSBox->setSingleStep(8);
    m_vTotalCachebytesSBox->setMaximum(1048576);
    m_vTotalCachebytesSBox->setValue(m_totalCacheSize);
    ctotalLayout->addWidget(ctotalbufferLabel);
    ctotalLayout->addWidget(m_vTotalCachebytesSBox);

    QVBoxLayout* vfetchLayout = new QVBoxLayout;
    vfetchLayout->addLayout(clineLayout);
    vfetchLayout->addLayout(ctotalLayout);

    m_GBAnalyseVFetch->setLayout(vfetchLayout);
}

void ac3DMeshAnalysis::createGBAnalyseOverdraw() {
    m_GBAnalyseOverdraw = new QGroupBox(tr("Analyse Overdraw"));
    m_GBAnalyseOverdraw->setWhatsThis("Generate overdraw statistic: pixels rendered/pixels covered. Best case is 1.0, all pixels drawn/shaded once.");
    m_GBAnalyseOverdraw->setCheckable(true);
    m_GBAnalyseOverdraw->setChecked(true);

    QHBoxLayout *maxXYviewLayout = new QHBoxLayout;
    QLabel *xyMaxLabel = new QLabel(tr("Orthographic X and Y View Max Value: "));
    xyMaxLabel->setWhatsThis("Overdraw calculated from different orthographic camera view. This set the max of X and Y viewports. Default is 256.");
    m_orthographicViewXYMaxSBox = new QSpinBox;
    m_orthographicViewXYMaxSBox->setMinimum(1);
    m_orthographicViewXYMaxSBox->setSingleStep(5);
    m_orthographicViewXYMaxSBox->setMaximum(300);
    m_orthographicViewXYMaxSBox->setValue(m_orthographcamXYmax);
    maxXYviewLayout->addWidget(xyMaxLabel);
    maxXYviewLayout->addWidget(m_orthographicViewXYMaxSBox);

    QVBoxLayout* overdrawLayout = new QVBoxLayout;
    overdrawLayout->addLayout(maxXYviewLayout);

    m_GBAnalyseOverdraw->setLayout(overdrawLayout);
}

void ac3DMeshAnalysis::onRunAnalyseMesh() {
    if (m_meshData.size() == 0) {
        QMessageBox msgBox;
        msgBox.setText("Mesh data is empty!");
        msgBox.exec();
        return;
    }

    if (m_textBrowser) {
        m_textBrowser->clear();
    } else {
        m_textBrowser = new QTextBrowser;
    }
    //Run then display result on the textbrowser.
    bool runVcache = m_GBAnalyseVCache->isChecked();
    bool runVfetch = m_GBAnalyseVFetch->isChecked();
    bool runOverdraw = m_GBAnalyseOverdraw->isChecked();

    if (m_fileName != "" && m_meshData.size()>0) {
        runAnalysisDisplayResult(m_fileName, m_meshData, runVcache, runOverdraw, runVfetch);
    }

    if (m_fileNameCompare != "" && m_meshDataCompare.size()>0) {
        runAnalysisDisplayResult(m_fileNameCompare, m_meshDataCompare, runVcache, runOverdraw, runVfetch);
    }
}

void ac3DMeshAnalysis::onAnalysisChecked() {
    bool runVcache = m_GBAnalyseVCache->isChecked();
    bool runVfetch = m_GBAnalyseVFetch->isChecked();
    bool runOverdraw = m_GBAnalyseOverdraw->isChecked();

    if ((!runVcache) && (!runVfetch) && (!runOverdraw)) {
        m_PBRunAnalyse->setEnabled(false);
    } else {
        m_PBRunAnalyse->setEnabled(true);
    }
}

void ac3DMeshAnalysis::runAnalysisDisplayResult(QString filename, std::vector<CMP_Mesh> meshData, bool runVCache, bool runOverdraw, bool runVFetch) {

    QFileInfo filenameinfo(filename); //todo: should check for file exist or not
    m_textBrowser->append("==== <b>Analysis Result for " + (filenameinfo.fileName() + " ====").toHtmlEscaped());

    for (int i = 0; i < meshData.size(); i++) {
        if (runVCache) {
            // parse user input from the UI spinboxes
            m_cacheSize = m_vCachesizeSBox->value();
            m_warpSize = m_warpSizeSBox->value();
            m_bufferSize = m_primBufferSizeSBox->value();

            //optional user input of vertex cache size, warp and buffer size, by default is 16, 64,128
            meshopt_VertexCacheStatistics vCacheStat = meshopt_analyzeVertexCache(&(meshData[i].indices[0]), meshData[i].indices.size(), meshData[i].vertices.size(), m_cacheSize, m_warpSize, m_bufferSize);
            m_textBrowser->append("");
            m_textBrowser->append("==== Vertex Cache Analysis for Mesh # "+ QString::number(i)+" ====");
            m_textBrowser->append("# Transformed vertices: " + QString::number(vCacheStat.vertices_transformed) + "\n" +
                                  "# Total vertices: " + QString::number(meshData[i].vertices.size()));
            m_textBrowser->append("<b>ACMR</b> (Average Cache Miss Ratio) : " + QString::number(vCacheStat.acmr).toHtmlEscaped() + "\n");
            m_textBrowser->append("<b>ATVR</b> (Average Transformed Vertices Ratio): " + QString::number(vCacheStat.atvr).toHtmlEscaped());
            m_textBrowser->append("==== Vertex Cache Analysis Done ====");
        }

        if (runVFetch) {
            // parse user input from the UI spinboxes
            m_cacheLineSize = m_vCachelinebytesSBox->value();
            m_totalCacheSize = m_vTotalCachebytesSBox->value();

            //optional user input of cache line bytes and total cache size, default is 64 and 128*1024 respectively
            meshopt_VertexFetchStatistics vFetchStat = meshopt_analyzeVertexFetch(&(meshData[i].indices[0]), meshData[i].indices.size(), meshData[i].vertices.size(), sizeof(Vertex), m_cacheLineSize, m_totalCacheSize);
            m_textBrowser->append("");
            m_textBrowser->append("==== Vertex Fetch Analysis for Mesh # " + QString::number(i) + " ====");
            m_textBrowser->append("Fetched vertices in Bytes: " + QString::number(vFetchStat.bytes_fetched));
            m_textBrowser->append("<b>Overfetch</b>(fetched bytes/vertex buffer size): " + QString::number(vFetchStat.overfetch).toHtmlEscaped());
            m_textBrowser->append("==== Vertex Fetch Analysis Done ====");
        }

        if (runOverdraw) {
            // parse user input from the UI spinboxes
            m_orthographcamXYmax = m_orthographicViewXYMaxSBox->value();

            //optional user input of viewports, default is testing with a range of viewpoints with x(0-255), y(0-255), z(0-1)
            meshopt_OverdrawStatistics overdrawStat = meshopt_analyzeOverdraw(&(meshData[i].indices[0]), meshData[i].indices.size(), &meshData[i].vertices[0].px, m_meshData[i].vertices.size(), sizeof(Vertex), m_orthographcamXYmax);
            m_textBrowser->append("");
            m_textBrowser->append("==== Overdraw Analysis for Mesh # " + QString::number(i) + " ====");
            m_textBrowser->append("#Pixels rendered: " + QString::number(overdrawStat.pixels_shaded) + "\n" +
                                  "#Pixels covered: " + QString::number(overdrawStat.pixels_covered));
            m_textBrowser->append("<b>Overdraw</b> (pixels rendered/pixels covered): " + QString::number(overdrawStat.overdraw).toHtmlEscaped());
            m_textBrowser->append("==== Overdraw Analysis Done ====");
        }
    }
    m_textBrowser->append("");
    m_textBrowser->append("==== <b>Analysis Complete</b> ====\n");
    m_textBrowser->append("");
}

void ac3DMeshAnalysis::onCancel() {
    close();
}
