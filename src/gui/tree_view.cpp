
#include <QDragEnterEvent>
#include <QMimeData>

#include "tree_view.hpp"

#include "window.hpp"

#include "ui_window.h"

tree_view::tree_view(QWidget *parent) : QTreeView(parent) {
	setColumnHidden(2, true);
	setColumnHidden(3, true);
	model_.setReadOnly(true);
	model_.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Hidden);
	model_.sort(0);
	setModel(&model_);
	model_.setRootPath(QDir::drives().first().filePath());
}

void tree_view::set_ui(Ui::window *ui) {
	ui_ = ui;
}

const QFileSystemModel* tree_view::model() const {
	return &model_;
}

void tree_view::dragEnterEvent(QDragEnterEvent *event) {
	event->acceptProposedAction();
}

void tree_view::dragLeaveEvent(QDragLeaveEvent *event) {
//	QWidget::setCursor(Qt::UpArrowCursor);
	selectionModel()->clearSelection();
	event->accept();
}

void tree_view::dragMoveEvent(QDragMoveEvent *event) {
	ui_->label_dst->setText(std::to_string(0).c_str());
	if (!event->mimeData()->hasUrls())
		return;
	auto urls = event->mimeData()->urls();
	QString joined;
	for (const auto &url : urls) {
		if (url.isLocalFile()) {
			if (!joined.isEmpty())
				joined.append(" ");
			joined.append(url.toString());
		}
	}
	if (joined.isEmpty())
		return;
	ui_->label_src->setText(joined);
	ui_->label_dst->setText(model_.filePath(indexAt(event->pos())));
//	ui_->label_src->setText(QString(std::to_string(event->pos().rx()).c_str()));
	selectionModel()->clearSelection();
	selectionModel()->select(indexAt(event->pos()), QItemSelectionModel::Select);
	event->acceptProposedAction();
}
