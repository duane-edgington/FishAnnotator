/// @file
/// @brief Defines class for making box annotations

#ifndef ANNOTATEDREGION_H
#define ANNOTATEDREGION_H

#include <cmath>
#include <algorithm>

#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

namespace fish_annotator {

/// Possible types of mouse movements while dragging.
enum Drag {
  DRAG_NONE = 0,
  DRAG_TOP,
  DRAG_BOTTOM,
  DRAG_LEFT,
  DRAG_RIGHT,
  DRAG_TOP_LEFT,
  DRAG_TOP_RIGHT,
  DRAG_BOTTOM_LEFT,
  DRAG_BOTTOM_RIGHT
};


/// Displays box annotations.
///
/// @tparam Info Contains data associated with a bounding box.
template<typename Info>
class AnnotatedRegion: public QGraphicsRectItem {
public:
  /// Constructor.
  ///
  /// @param uid Unique ID associated with this region.
  /// @param annotation Annotation associated with this region.
  /// @param bounding_rect Bounding rect for this region.
  /// @param box_color Color of the box.
  AnnotatedRegion(uint64_t uid,
                  std::shared_ptr<Info> annotation,
                  const QRectF &bounding_rect,
                  QColor box_color);

  /// Reimplemented from QGraphicsItem.
  ///
  /// @param event Qt event pointer.
  void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override final;

  /// Reimplemented from QGraphicsItem.
  ///
  /// @param event Qt event pointer.
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override final;

  /// Reimplemented from QGraphicsItem.
  ///
  /// @param event Qt event pointer.
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override final;

  /// Reimplemented from QGraphicsItem.
  ///
  /// @param painter Qt painter pointer.
  /// @param option Qt option pointer.
  /// @param widget Qt widget pointer.
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override final;

  /// Gets the ID associated with this region.
  ///
  /// @return Unique ID associated with this region.
  uint64_t getUID();

  /// Gets the bounding box associated with this region.
  ///
  /// @return Bounding box associated with this region.
  QRectF getAnnBox();

  ///Whether to draw this annotation or not
  bool isValid();

private:
  /// Pointer to the annotation location.
  std::shared_ptr<Info> annotation_;

  /// ID associated with this object.
  uint64_t uid_;

  /// Bounding rectangle.
  QRectF bounding_rect_;

  /// Min dimension of bounding rect for scaling drawing sizes.
  qreal min_dim_;

  /// Type of move event.
  Drag drag_;

  /// Margin for initiating a drag.
  qreal margin_;

  /// Font.
  QFont font_;

  /// Pen.
  QPen pen_;

  /// Whether to draw this annotation.
  bool valid_;

  /// Updates annotation with this object's current rect.
  void updateAnnotation();
};

//
// Implementations
//

template<typename Info>
AnnotatedRegion<Info>::AnnotatedRegion(
  uint64_t uid,
  std::shared_ptr<Info> annotation,
  const QRectF &bounding_rect,
  QColor box_color)
  : annotation_(annotation)
  , uid_(uid)
  , bounding_rect_(bounding_rect)
  , drag_()
  , min_dim_(std::min(bounding_rect_.width(), bounding_rect_.height()))
  , margin_(min_dim_ * 0.02)
  , font_("Helvetica", min_dim_ * 0.02)
  , pen_(box_color)
  , valid_(true) {
  if(annotation_->area_.w == 0 && annotation_->area_.h == 0) {
    // Default rectangle.
    setRect(QRectF(bounding_rect_.x() * 0.5, bounding_rect_.y() * 0.5,
          0.1 * min_dim_, 0.1 * min_dim_));
  }
  else {
    // Rectangle from annotation. Make sure we don't have offscreen boxes
    if (annotation_->area_.x < 0) {
      annotation_->area_.w = annotation_->area_.w + annotation_->area_.x;
      annotation_->area_.x = 0;
    }
    if (annotation_->area_.y < 0) {
      annotation_->area_.h = annotation_->area_.h + annotation_->area_.y;
      annotation_->area_.y = 0;
    }
    if (annotation_->area_.x > bounding_rect_.width()) {
      valid_ = false;
    }
    if (annotation_->area_.y > bounding_rect_.height()) {
      valid_ = false;
    }
    if(annotation_->area_.x + annotation_->area_.w > bounding_rect_.width()) {
      annotation_->area_.w = bounding_rect_.width() - annotation_->area_.x;
    }
    if(annotation_->area_.y + annotation_->area_.h > bounding_rect_.height()) {
      annotation_->area_.h = bounding_rect_.height() - annotation_->area_.y;
    }
    setRect(QRectF(
          annotation_->area_.x,
          annotation_->area_.y,
          annotation_->area_.w,
          annotation_->area_.h));
  }
  pen_.setWidthF(std::min(
    bounding_rect_.width(), bounding_rect_.height()) * 0.005);
  updateAnnotation();
}

template<typename Info>
void AnnotatedRegion<Info>::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  QPointF pos = event->pos();
  qreal from_right = pos.x() - rect().right();
  qreal from_left = pos.x() - rect().left();
  qreal from_top = pos.y() - rect().top();
  qreal from_bottom = pos.y() - rect().bottom();
  if (std::abs(from_top) < margin_) {
    if (std::abs(from_left) < margin_) {
      drag_ = DRAG_TOP_LEFT;
      setCursor(QCursor(Qt::SizeFDiagCursor));
    } else if (std::abs(from_right) < margin_) {
      drag_ = DRAG_TOP_RIGHT;
      setCursor(QCursor(Qt::SizeBDiagCursor));
    } else {
      drag_ = DRAG_TOP;
      setCursor(QCursor(Qt::SizeVerCursor));
    }
  }
  else if (std::abs(from_bottom) < margin_) {
    if (std::abs(from_left) < margin_) {
      drag_ = DRAG_BOTTOM_LEFT;
      setCursor(QCursor(Qt::SizeBDiagCursor));
    }
    else if (std::abs(from_right) < margin_) {
      drag_ = DRAG_BOTTOM_RIGHT;
      setCursor(QCursor(Qt::SizeFDiagCursor));
    }
    else {
      drag_ = DRAG_BOTTOM;
      setCursor(QCursor(Qt::SizeVerCursor));
    }
  }
  else if (std::abs(from_left) < margin_) {
    drag_ = DRAG_LEFT;
    setCursor(QCursor(Qt::SizeHorCursor));
  }
  else if (std::abs(from_right) < margin_) {
    drag_ = DRAG_RIGHT;
    setCursor(QCursor(Qt::SizeHorCursor));
  }
  else {
    drag_ = DRAG_NONE;
    setCursor(QCursor());
  }
  if (drag_ != DRAG_NONE) {
    setFlag(ItemIsMovable, false);
  }
  else {
    setFlag(ItemIsMovable, true);
  }
  QGraphicsItem::hoverMoveEvent(event);
}

template<typename Info>
void AnnotatedRegion<Info>::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  setCursor(QCursor());
  QGraphicsItem::hoverLeaveEvent( event );
}

template<typename Info>
void AnnotatedRegion<Info>::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  QRectF area = rect();
  QPointF pos = event->pos();
  qreal from_left = pos.x() - rect().left();
  qreal from_top = pos.y() - rect().top();
  switch (drag_) {
  case DRAG_TOP: {
    qreal new_height = area.height() - from_top;
    if (new_height > 2*margin_) {
      area.setHeight(area.height() - from_top);
      area.translate(0, from_top);
    }
    break;
  }
  case DRAG_BOTTOM: {
    qreal new_height = pos.y() - area.top();
    if (new_height > 2*margin_) {
      area.setHeight(new_height);
    }
    break;
  }
  case DRAG_LEFT: {
    qreal new_width = area.width() - from_left;
    if (new_width > 2*margin_) {
      area.setWidth(area.width() - from_left);
      area.translate(from_left, 0);
    }
    break;
  }
  case DRAG_RIGHT: {
    qreal new_width = pos.x() - area.left();
    if (new_width > 2*margin_) {
      area.setWidth(new_width);
    }
    break;
  }
  case DRAG_TOP_LEFT: {
    qreal new_width = area.width() - from_left;
    if (new_width > 2*margin_) {
      area.setWidth(area.width() - from_left);
      area.translate(from_left, 0);
    }
    qreal new_height = area.height() - from_top;
    if (new_height > 2*margin_) {
      area.setHeight(area.height() - from_top);
      area.translate(0, from_top);
    }
    break;
  }
  case DRAG_TOP_RIGHT: {
    qreal new_height = area.height() - from_top;
    if (new_height > 2*margin_) {
      area.setHeight(area.height() - from_top);
      area.translate(0, from_top);
    }
    qreal new_width = pos.x() - area.left();
    if (new_width > 2*margin_) {
      area.setWidth(new_width);
    }
    break;
  }
  case DRAG_BOTTOM_LEFT: {
    qreal new_height = pos.y() - area.top();
    if (new_height > 2*margin_) {
      area.setHeight(new_height);
    }
    qreal new_width = area.width() - from_left;
    if (new_width > 2*margin_) {
      area.setWidth(area.width() - from_left);
      area.translate(from_left, 0);
    }
    break;
  }
  case DRAG_BOTTOM_RIGHT: {
    qreal new_height = pos.y() - area.top();
    if (new_height > 2*margin_) {
      area.setHeight(new_height);
    }
    qreal new_width = pos.x() - area.left();
    if (new_width > 2*margin_) {
      area.setWidth(new_width);
    }
    break;
  }
  default:
    auto old_pos = event->lastPos();
    area.translate(pos.x() - old_pos.x(), pos.y() - old_pos.y());
    break;
  }
  if(area.left() >= bounding_rect_.left()
    && area.top() >= bounding_rect_.top()
    && area.width() + area.left() <=
      bounding_rect_.width() + bounding_rect_.left()
    && area.height() + area.top() <=
      bounding_rect_.height() + bounding_rect_.top()) {
    setRect(area);
    prepareGeometryChange();
    update();
    updateAnnotation();
  }
}

template<typename Info>
void AnnotatedRegion<Info>::paint(QPainter *painter,
  const QStyleOptionGraphicsItem *option, QWidget *widget) {
  painter->setFont(font_);
  painter->setPen(pen_);
  // draw main rectangle
  painter->drawRect(rect());
  // draw UID
  QString text("000000");
  QFontMetrics fm = painter->fontMetrics();
  int width = fm.width(text);
  QBrush brush;
  brush.setColor(Qt::gray);
  QRectF text_area = QRectF(
    rect().right() - width,
    rect().bottom() - fm.height(),
    width,
    fm.height()
    );
  painter->fillRect(text_area, QBrush(QColor(64, 64, 64, 64)));
  painter->drawText(
      text_area,
      QString::number(uid_),
      QTextOption(Qt::AlignRight));
}

template<typename Info>
uint64_t AnnotatedRegion<Info>::getUID() {
  return uid_;
}

template<typename Info>
QRectF AnnotatedRegion<Info>::getAnnBox() {
  return QRectF(
    annotation_->area_.x,
    annotation_->area_.y,
    annotation_->area_.w,
    annotation_->area_.h);
}

template<typename Info>
bool AnnotatedRegion<Info>::isValid() {
  return valid_;
}

template<typename Info>
void AnnotatedRegion<Info>::updateAnnotation() {
  QRectF area = rect();
  annotation_->area_.x = area.x();
  annotation_->area_.y = area.y();
  annotation_->area_.w = area.width();
  annotation_->area_.h = area.height();
}

} // namespace fish_annotator

#endif // ANNOTATEDREGION_H
