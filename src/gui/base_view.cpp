
#include <QMenu>
#include <QMouseEvent>
#include <QMimeData>

#include "base_view.hpp"

#include "list_view.hpp"
#include "tree_view.hpp"

base_view::base_view(tree_view *tree_view, list_view *list_view, QLabel *label, QStatusBar *status_bar) {

	list_ = list_view;
	tree_ = tree_view;
	label_ = label;
	status_bar_ = status_bar;

	const auto root = QDir::drives().first().filePath();

	tree_model_.setReadOnly(true);
	tree_model_.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Hidden);
	tree_model_.sort(0);
	tree_model_.setRootPath(root);
	tree_->setModel(&tree_model_);
	tree_->setRootIsDecorated(false);
	tree_->setStyleSheet( "QTreeView::branch { border-image: url(none.png); image: url(none.png); }");
	tree_->setIndentation(16);
//	tree->setColumnHidden(2, true);
//	tree->setColumnHidden(3, true);

	list_model_.setReadOnly(true);
	list_model_.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
	list_model_.sort(0);
	list_model_.setRootPath(root);
	list_->setModel(&list_model_);

	label_->setText(QDir::toNativeSeparators(root));
}

list_view* base_view::list() {
	return list_;
}

tree_view* base_view::tree() {
	return tree_;
}

QLabel* base_view::label() {
	return label_;
}

QStatusBar* base_view::status_bar() {
	return status_bar_;
}

QFileSystemModel *base_view::list_model() {
	return &list_model_;
}

QFileSystemModel *base_view::tree_model() {
	return &tree_model_;
}

void base_view::dragEnterEvent(QDragEnterEvent *event) {
	event->acceptProposedAction();
}

void base_view::dragLeaveEvent(QDragLeaveEvent *event, QAbstractItemView *view) {
	view->selectionModel()->clearSelection();
	event->accept();
}

void base_view::dragMoveEvent(QDragMoveEvent *event, QAbstractItemView *view, QFileSystemModel *model, QStatusBar *status_bar) {
	view->selectionModel()->clearSelection();
	QString status {};
	const auto mime = event->mimeData();
	if (mime != nullptr && mime->hasUrls()) {
		auto urls = mime->urls();
		int count = 0;
		for (const auto &url : urls) {
			if (url.isLocalFile())
				count++;
		}
		if (count == 0)
			return;
		status.append("Copy/Move/Sync ");
		status.append(std::to_string(count).c_str());
		if (count == 1)
			status.append(" item to ");
		else
			status.append(" items to ");
		auto index = view->indexAt(event->pos());
		if (index.isValid()) {
			auto path = model->filePath(index);
			if (model->isDir(index))
				view->selectionModel()->select(index, QItemSelectionModel::Select);
			else
				index = model->parent(index);
			status.append(model->filePath(index));
		} else {
			status.append("...");
		}
	}
	status_bar->showMessage(status);

	event->acceptProposedAction();
}

void base_view::dropEvent(QDropEvent *event, QAbstractItemView *view) {
	QMenu *menu =new QMenu(view);
	menu->addAction(new QAction("Copy", view));
	menu->addAction(new QAction("Move", view));
	menu->addAction(new QAction("Sync", view));
	menu->popup(view->viewport()->mapToGlobal(event->pos()));

	event->acceptProposedAction();
}

void base_view::mousePressEventTree(QMouseEvent *event) {
	tree_->selectionModel()->clearSelection();
	label_->setText("");
	auto index = tree_->indexAt(event->pos());
	if (index.isValid()) {
		tree_->selectionModel()->select(index, QItemSelectionModel::Select);
		const auto path {tree_model_.filePath(index)};
		label_->setText(QDir::toNativeSeparators(path));

		if (event->button() & Qt::MouseButton::LeftButton) {
			if (tree_->isExpanded(index)) {
				tree_->setExpanded(index, false);
			} else {
				while (index.isValid()) {
					tree_->expand(index);
					index = index.parent();
				}
			}
		}

	//	base_view_->list_model()->setRootPath(path);
		list_->selectionModel()->clearSelection();
		const auto list_index = list_model_.index(path);
		if (list_index.isValid())
			list_->setRootIndex(list_index);
	}
	event->accept();
}

void base_view::mouseDoubleClickEventList(QMouseEvent *event) {
	auto list_index = list_->indexAt(event->pos());
	if (list_index.isValid()) {
		const QString path {list_model_.filePath(list_index)};
		auto tree_index = tree_model_.index(path);
		if (tree_index.isValid()) {
			tree_->selectionModel()->clearSelection();
			tree_->selectionModel()->select(tree_index, QItemSelectionModel::Select);
			while (tree_index.isValid()) {
				tree_->expand(tree_index);
				tree_index = tree_index.parent();
			}

			list_model_.setRootPath(path);
			list_->setRootIndex(list_model_.index(path));

			label_->setText(QDir().toNativeSeparators(path));
		}
	}
	event->accept();
}
