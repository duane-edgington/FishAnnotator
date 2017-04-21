#include <algorithm>

#include <QFileDialog>
#include <QMessageBox>

#include "fish_annotator/common/species_dialog.h"
#include "fish_annotator/image_annotator/mainwindow.h"
#include "ui_mainwindow.h"

namespace fish_annotator { namespace image_annotator {

namespace fs = boost::filesystem;

namespace { //anonymous

static const std::vector<std::string> kDirExtensions = {
  ".jpg", ".png", ".bmp", ".tif", ".jpeg",
  ".JPG", ".PNG", ".BMP", ".TIF", ".JPEG"};

} // anonymous namespace

MainWindow::MainWindow(QWidget *parent)
  : annotations_(new ImageAnnotationList)
  , scene_(new QGraphicsScene)
  , ui_(new Ui::MainWindow)
  , species_controls_(new SpeciesControls(this))
  , image_files_() {
  ui_->setupUi(this);
#ifdef _WIN32
  setWindowIcon(QIcon(":/icons/FishAnnotator.ico"));
#endif
  ui_->next->setIcon(QIcon(":/icons/image_controls/next.svg"));
  ui_->prev->setIcon(QIcon(":/icons/image_controls/prev.svg"));
  ui_->sideBarLayout->addWidget(species_controls_.get());
  QObject::connect(species_controls_.get(), 
      SIGNAL(individualAdded(std::string, std::string)), 
      this, SLOT(addIndividual(std::string, std::string)));
  fs::path current_path(QDir::currentPath().toStdString());
  fs::path default_species = current_path / fs::path("default.species");
  if(fs::exists(default_species)) {
    species_controls_->loadSpeciesFile(
        QString(default_species.string().c_str()));
  }
  
}

void MainWindow::on_next_clicked() {
  int next_val = ui_->imageSlider->value() + 1;
  if(next_val <= ui_->imageSlider->maximum()) {
    ui_->imageSlider->setValue(next_val);
  }
}

void MainWindow::on_prev_clicked() {
  int prev_val = ui_->imageSlider->value() - 1;
  if(prev_val >= ui_->imageSlider->minimum()) {
    ui_->imageSlider->setValue(prev_val);
  }
}

void MainWindow::on_loadImageDir_triggered() {
  QString image_dir = QFileDialog::getExistingDirectory(this, 
    "Select an image directory.");
  if(!image_dir.isEmpty()) {
    onLoadDirectorySuccess(image_dir);
  }
}

void MainWindow::on_saveAnnotations_triggered() {
  if(image_files_.size() > 0) {
    annotations_->write(image_files_);
  }
}

void MainWindow::on_imageSlider_valueChanged() {
  scene_->clear();
  ui_->idSelection->clear();
#ifdef _WIN32
  QString filename(image_files_[ui_->imageSlider->value()].string().c_str());
#else
  QString filename(image_files_[ui_->imageSlider->value()].c_str());
#endif
  QImage current(filename);
  if(!current.isNull()) {
    scene_->addPixmap(QPixmap::fromImage(current));
    scene_->setSceneRect(current.rect());
    ui_->imageWindow->setScene(scene_.get());
    ui_->imageWindow->fitInView(scene_->sceneRect(), Qt::KeepAspectRatio);
    ui_->imageWindow->show();
    ui_->fileNameValue->setText(filename);
    fs::path img_path(filename.toStdString());
    auto annotations = 
      annotations_->getImageAnnotations(img_path.filename());
    for(auto annotation : annotations) {
      if(ui_->showAnnotations->isChecked()) {
          auto region = new AnnotatedRegion<ImageAnnotation>(
                annotation->id_, annotation, current.rect());
          scene_->addItem(region);
      }
      ui_->idSelection->addItem(QString::number(annotation->id_));
    }
    species_controls_->resetCounts();
    auto counts = annotations_->getCounts(filename.toStdString());
    for(auto it = counts.begin(); it != counts.end(); it++) {
      species_controls_->setCount(it->second, it->first);
    }
    updateTypeMenus();
  }
  else {
    QMessageBox err;
    err.critical(0, "Error", std::string(
        std::string("Error loading image ")
      + filename.toStdString()
      + std::string(".")).c_str());
  }
}

void MainWindow::on_showAnnotations_stateChanged() {
  on_imageSlider_valueChanged();
}

void MainWindow::on_idSelection_currentIndexChanged(const QString &id) {
  updateTypeMenus();
}

void MainWindow::on_typeMenu_activated(const QString &text) {
  auto ann = currentAnnotation();
  if(ann != nullptr) {
    ann->species_ = text.toStdString();
    updateTypeMenus();
  }
}

void MainWindow::on_subTypeMenu_activated(const QString &text) {
  auto ann = currentAnnotation();
  if(ann != nullptr) {
    ann->subspecies_ = text.toStdString();
    updateTypeMenus();
  }
}

void MainWindow::on_removeAnnotation_clicked() {
  if(image_files_.size() > 0 && ui_->imageSlider->isEnabled()) {
    auto current_image = image_files_[ui_->imageSlider->value()];
    int id = ui_->idSelection->currentText().toInt();
    annotations_->remove(current_image, id);
    on_imageSlider_valueChanged();
  }
}

void MainWindow::addIndividual(std::string species, std::string subspecies) {
  if(image_files_.size() > 0 && ui_->imageSlider->isEnabled()) {
    auto current_image = image_files_[ui_->imageSlider->value()];
    uint64_t id = annotations_->nextId(current_image);
    auto annotation = std::make_shared<ImageAnnotation>(
      current_image.filename().string(), species, subspecies, id, 
      Rect(0, 0, 0, 0));
    annotations_->insert(annotation);
    on_imageSlider_valueChanged();
  }
}

void MainWindow::onLoadDirectorySuccess(const QString &image_dir) {
  image_files_.clear();
  fs::directory_iterator dir_it(image_dir.toStdString());
  fs::directory_iterator dir_end;
  for(; dir_it != dir_end; ++dir_it) {
    fs::path ext(dir_it->path().extension());
    for(auto &ok_ext : kDirExtensions) {
      if(ext == ok_ext) {
        image_files_.push_back(dir_it->path());
      }
    }
  }
  std::sort(image_files_.begin(), image_files_.end());
  if(image_files_.size() > 0) {
    ui_->idLabel->setEnabled(true);
    ui_->speciesLabel->setEnabled(true);
    ui_->subspeciesLabel->setEnabled(true);
    ui_->idSelection->setEnabled(true);
    ui_->typeMenu->setEnabled(true);
    ui_->subTypeMenu->setEnabled(true);
    ui_->removeAnnotation->setEnabled(true);
    ui_->showAnnotations->setEnabled(true);
    ui_->setMetadata->setEnabled(true);
    ui_->next->setEnabled(true);
    ui_->prev->setEnabled(true);
    ui_->saveAnnotations->setEnabled(true);
    ui_->imageSlider->setEnabled(true);
    ui_->imageSlider->setMinimum(0);
    ui_->imageSlider->setMaximum(static_cast<int>(image_files_.size() - 1));
    ui_->imageSlider->setSingleStep(1);
    ui_->imageSlider->setValue(0);
    annotations_->read(image_files_);
    species_controls_->loadFromVector(annotations_->getAllSpecies());
    on_imageSlider_valueChanged();
  }
  else {
    QMessageBox err;
    err.critical(0, "Error", "No images found in this directory.");
  }
}

void MainWindow::updateTypeMenus() {
  auto ann = currentAnnotation();
  if(ann != nullptr) {
    ui_->typeMenu->clear();
    ui_->subTypeMenu->clear();
    auto species = species_controls_->getSpecies();
    for(auto &s : species) {
      ui_->typeMenu->addItem(s.getName().c_str());
      if(s.getName() == ann->species_) {
        ui_->typeMenu->setCurrentText(s.getName().c_str());
        auto subspecies = s.getSubspecies();
        for(auto &sub : subspecies) {
          ui_->subTypeMenu->addItem(sub.c_str());
          if(sub == ann->subspecies_) {
            ui_->subTypeMenu->setCurrentText(sub.c_str());
          }
        }
      }
    }
  }
}

std::shared_ptr<ImageAnnotation> MainWindow::currentAnnotation() {
  if(image_files_.size() > 0 && ui_->imageSlider->isEnabled()) {
    auto id = ui_->idSelection->currentText();
    auto current_image = image_files_[ui_->imageSlider->value()];
    auto annotations = 
      annotations_->getImageAnnotations(current_image);
    for(auto annotation : annotations) {
      if(annotation->id_ == id.toInt()) {
        return annotation;
      }
    }
  }
  return nullptr;
}

#include "../../include/fish_annotator/image_annotator/moc_mainwindow.cpp"

}} // namespace fish_annotator::image_annotator
