#ifndef AC3DMESHANALYSIS_H
#define AC3DMESHANALYSIS_H

#include <QtWidgets>
#include <QtWidgets/QDialog>
#include <qlayout.h>
#include <qlabel.h>
#include <QPushButton>
#include <QGroupBox>
#include <QTextBrowser>
#include <QString>
#include <assert.h>
#include "modeldata.h"
#include "meshoptimizer.h"

#define DEFAULT_HWVCACHE_SZ          16
#define DEFAULT_WARP_SZ              64
#define DEFAULT_PRIMITIVEBUFFER_SZ   128

#define DEFAULT_CACHE_LINE_BYTES     64
#define DEFAULT_CACHE_TOTAL_BYTES    131072 //128*1024

#define DEFAULT_ORTHOGRAHIC_X_Y_MAX  256


class ac3DMeshAnalysis : public QDialog {
    Q_OBJECT

  public:
    ac3DMeshAnalysis(QWidget *parent = nullptr);
    void Init(const QString& title, const QString& productName);
    void cleanText();
    ~ac3DMeshAnalysis();
    std::vector<CMP_Mesh> m_meshData;
    std::vector<CMP_Mesh> m_meshDataCompare;
    QString          m_fileName;
    QString          m_fileNameCompare;
  public slots:
    void onCancel();
    void onRunAnalyseMesh();
    void onAnalysisChecked();

  private:
    QPushButton      *m_PBCancel;
    QPushButton      *m_PBRunAnalyse;
    QGroupBox        *m_GBAnalyseVCache;
    QGroupBox        *m_GBAnalyseVFetch;
    QGroupBox        *m_GBAnalyseOverdraw;

    // For Analyse Vertex Cache
    QSpinBox         *m_vCachesizeSBox;
    QSpinBox         *m_warpSizeSBox;
    QSpinBox         *m_primBufferSizeSBox;

    // For Analyse Vertex Fetch
    QSpinBox         *m_vCachelinebytesSBox;
    QSpinBox         *m_vTotalCachebytesSBox;

    // For Analyse overdraw
    QSpinBox         *m_orthographicViewXYMaxSBox;

    QTextBrowser     *m_textBrowser;
    QWidget          *m_parent;

    unsigned int      m_cacheSize;  //vertex cache size- in range 16-32
    unsigned int      m_warpSize;   //# of thread within a wavefront(warp)
    unsigned int      m_bufferSize; //primitive(triangles) group size

    unsigned int      m_cacheLineSize;  //prefetch cache line size
    unsigned int      m_totalCacheSize;  //prefetch total cache size

    unsigned int      m_orthographcamXYmax;  //orthographic viewpoints X and Y max


    void createGBAnalyseVCache();
    void createGBAnalyseVFetch();
    void createGBAnalyseOverdraw();

    void runAnalysisDisplayResult(QString filename, std::vector<CMP_Mesh> meshData, bool runVCache, bool runOverdraw, bool runVFetch);
};

#endif // AC3DMESHANALYSIS_H
