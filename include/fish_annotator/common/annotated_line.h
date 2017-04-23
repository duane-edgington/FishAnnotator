#ifndef ANNOTATED_LINE_H
#define ANNOTATED_LINE_H

#include <QGraphicsLineItem>

namespace fish_annotator {

/// @brief Displays line annotations.
///
/// @tparam Info Contains data associated with a line.
template<typename Info>
class AnnotatedLine: public QGraphicsLineItem {
public:
  /// @brief Constructor.
  ///
  /// @param uid Unique ID associated with this region.
  /// @param annotation Annotation associated with this region.
  /// @param bounding_rect Bounding rect for this region.
  AnnotatedLine(uint64_t uid, 
                  std::shared_ptr<Info> annotation,
                  const QRectF &bounding_rect);

  /// @brief Reimplemented from QGraphicsItem.
  ///
  /// @param painter Qt painter pointer.
  /// @param option Qt option pointer.
  /// @param widget Qt widget pointer.
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget);

  /// @brief Gets the ID associated with this region.
  ///
  /// @return Unique ID associated with this region.
  uint64_t getUID();

  /// @brief Gets the line associated with this annotation.
  ///
  /// @return Bounding box associated with this annotation.
  QLineF getAnnLine();

  ///@brief Whether to draw this annotation or not
  bool isValid();
private:
  /// @brief Pointer to the annotation location.
  std::shared_ptr<Info> annotation_;

  /// @brief ID associated with this object.
  uint64_t uid_;

  /// @brief Bounding rectangle.
  QRectF bounding_rect_;

  /// @brief Min dimension of bounding rect for scaling drawing sizes.
  qreal min_dim_;

  /// @brief Font.
  QFont font_;

  /// @brief Pen.
  QPen pen_;

  /// @brief Whether this annotation is valid.
  bool valid_;

  /// @brief Updates annotation with this object's current rect.
  void updateAnnotation();
};

//
// Implementations
//

template<typename Info>
AnnotatedLine<Info>::AnnotatedLine(
    uint64_t uid,
    std::shared_ptr<Info> annotation,
    const QRectF &bounding_rect)
  : annotation_(annotation)
  , uid_(uid)
  , bounding_rect_(bounding_rect)
  , min_dim_(std::min(bounding_rect_.width(), bounding_rect_.height()))
  , font_("Helvetica", min_dim_ * 0.02)
  , pen_(QColor(255, 0, 0))
  , valid_(true) {
  if(annotation_->area_.x < 0) {
    annotation_->area_.x = 0;
  }
  if(annotation_->area_.y < 0) {
    annotation_->area_.y = 0;
  }
  if(annotation_->area_.w < 0) {
    annotation_->area_.w = 0;
  }
  if(annotation_->area_.h < 0) {
    annotation_->area_.h = 0;
  }
  if(annotation_->area_.x > bounding_rect_.width()) {
    annotation_->area_.x = bounding_rect_.width();
  }
  if(annotation_->area_.y > bounding_rect_.height()) {
    annotation_->area_.y = bounding_rect_.height();
  }
  if(annotation_->area_.w > bounding_rect_.width()) {
    annotation_->area_.w = bounding_rect_.width();
  }
  if(annotation_->area_.h > bounding_rect_.height()) {
    annotation_->area_.h = bounding_rect_.height();
  }
  setLine(QLineF(
        annotation_->area_.x,
        annotation_->area_.y,
        annotation_->area_.w,
        annotation_->area_.h));
  pen_.setWidthF(std::min(
        bounding_rect_.width(), bounding_rect_.height()) * 0.005);
  updateAnnotation();
}

template<typename Info>
void AnnotatedLine<Info>::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option,
    QWidget *widget) {
  painter->setFont(font_);
  painter->setPen(pen_);
  QString text("000000");
  QFontMetrics fm = painter->fontMetrics();
  int width = fm.width(text);
  QBrush brush;
  brush.setColor(Qt::gray);
  QRectF text_area = QRectF(
      (line().x1() + line().x2()) / 2 - width, 
      (line().y1() + line().y2()) / 2 - fm.height(), 
      width, fm.height());
  painter->fillRect(text_area, QBrush(QColor(64, 64, 64, 64)));
  painter->drawText(
      text_area,
      QString::number(uid_),
      QTextOption(Qt::AlignRight));
  painter->drawLine(line());
}

template<typename Info>
uint64_t AnnotatedLine<Info>::getUID() {
  return uid_;
}

template<typename Info>
QLineF AnnotatedLine<Info>::getAnnLine() {
  return QLineF(
      annotation_->area_.x,
      annotation_->area_.y,
      annotation_->area_.w,
      annotation_->area_.h);
}

template<typename Info>
bool AnnotatedLine<Info>::isValid() {
  return valid_;
}

template<typename Info>
void AnnotatedLine<Info>::updateAnnotation() {
  annotation_->area_.x = line().x1();
  annotation_->area_.y = line().y1();
  annotation_->area_.w = line().x2();
  annotation_->area_.h = line().y2();
}

} // namespace fish_annotator

#endif // ANNOTATED_LINE_H

