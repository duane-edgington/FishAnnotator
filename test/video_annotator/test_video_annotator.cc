#include <QtPlugin>

#include "test_video_annotator.h"
#include "mainwindow.h"

#ifdef _WIN32
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif

void TestVideoAnnotator::testLoadVideo() {
  fish_annotator::video_annotator::MainWindow mainwin;
  QString name = "slow_motion_drop.mp4";
  mainwin.requestLoadVideo(name);
}
void TestVideoAnnotator::testSaveAnnotationsNoFish() {
  fish_annotator::video_annotator::MainWindow mainwin;
  QString name = "slow_motion_drop.mp4";
  mainwin.requestLoadVideo(name);
  mainwin.on_saveAnnotationFile_triggered();
}

void TestVideoAnnotator::testAddRegionNoFish() {
  fish_annotator::video_annotator::MainWindow mainwin;
  QString name = "slow_motion_drop.mp4";
  mainwin.requestLoadVideo(name);
  mainwin.on_addRegion_clicked();
  mainwin.on_removeRegion_clicked();
  mainwin.on_nextAndCopy_clicked();
}

void TestVideoAnnotator::testSaveAnnotations() {
  fish_annotator::video_annotator::MainWindow mainwin;
  QString name = "slow_motion_drop.mp4";
  mainwin.requestLoadVideo(name);
  mainwin.addIndividual("flat", "aspodjhf");
  mainwin.addIndividual("round", "ahsjdah");
  mainwin.addIndividual("round", "ahsjdah");
  for(int n=0;n<4;n++) {
    mainwin.on_addRegion_clicked();
    mainwin.on_prevFish_clicked();
  }
  for(int n=0;n<5;n++) mainwin.on_nextAndCopy_clicked();
  for(int n=0;n<6;n++) mainwin.on_minusOneFrame_clicked();
  for(int n=0;n<3;n++) mainwin.addIndividual("other", "ahs");
  for(int n=0;n<5;n++) mainwin.on_nextAndCopy_clicked();
  for(int n=0;n<10;n++) mainwin.addIndividual("skate", "ahjks");
  for(int n=0;n<10;n++) {
    mainwin.on_addRegion_clicked();
    mainwin.on_nextFish_clicked();
  }
  for(int n=0;n<3;n++) mainwin.on_nextAndCopy_clicked();
  mainwin.on_saveAnnotationFile_triggered();
}

QTEST_MAIN(TestVideoAnnotator)
#include "test_video_annotator.moc"

