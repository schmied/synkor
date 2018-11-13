
#include <QMenu>
#include <QMouseEvent>
#include <QMimeData>
#include <QFileIconProvider>

#include "item_view.hpp"

#include "list_view.hpp"
#include "tree_view.hpp"
#include "window.hpp"

#include "ui_window.h"

item_view::item_view(window *window, tree_view *tree_view, list_view *list_view, QLineEdit *head) {

	window_ = window;
	list_ = list_view;
	tree_ = tree_view;
	head_ = head;

	const auto root = QDir::drives().first().filePath();

	tree_model_.setReadOnly(true);
	tree_model_.setFilter(FILTER_BASE_TREE);
	tree_model_.iconProvider()->setOptions(QFileIconProvider::DontUseCustomDirectoryIcons);
	tree_model_.sort(0);
	tree_model_.setRootPath(root);
	tree_->setRootIsDecorated(false);
	tree_->setIndentation(10);
//	tree->setColumnHidden(2, true);
//	tree->setColumnHidden(3, true);
	tree_->setModel(&tree_model_);

	list_model_.setReadOnly(true);
	list_model_.setFilter(FILTER_BASE_LIST);
	tree_model_.iconProvider()->setOptions(QFileIconProvider::DontUseCustomDirectoryIcons);
	list_model_.sort(0);
	list_model_.setRootPath(root);
	list_->setModel(&list_model_);

	head_->setText(QDir::toNativeSeparators(root));
}

window *item_view::main_window() {
	return window_;
}

list_view* item_view::list() {
	return list_;
}

tree_view* item_view::tree() {
	return tree_;
}

QLineEdit* item_view::head() {
	return head_;
}

QStatusBar* item_view::status_bar() {
	return window_->ui()->status_bar;
}

QFileSystemModel *item_view::list_model() {
	return &list_model_;
}

QFileSystemModel *item_view::tree_model() {
	return &tree_model_;
}

void item_view::dragEnterEvent(QDragEnterEvent *event) {
	event->acceptProposedAction();
}

void item_view::dragLeaveEvent(QDragLeaveEvent *event, QAbstractItemView *view) {
	view->selectionModel()->clearSelection();
	event->accept();
}

void item_view::dragMoveEvent(QDragMoveEvent *event, QAbstractItemView *view, QFileSystemModel *model, QStatusBar *status_bar) {
	view->selectionModel()->clearSelection();
	QString status {};
	const auto mime = event->mimeData();
	if (mime != nullptr && mime->hasUrls()) {
		QList<QString> action_data2;
		auto urls = mime->urls();
		for (const auto &url : urls) {
			if (url.isLocalFile())
				action_data2.push_back(url.toString());
		}
		window_->action_data2 = action_data2;
		if (!window_->action_data2.empty()) {
			auto index = view->indexAt(event->pos());
			if (index.isValid()) {
				auto path = model->filePath(index);
				if (model->isDir(index))
					view->selectionModel()->select(index, QItemSelectionModel::Select);
				else
					index = model->parent(index);
				window_->action_data1 = model->filePath(index);
			} else {
				window_->action_data1.clear();
			}
			status.append("Copy / Move / Sync ");
			status.append(window_->action_data2.join(" : "));
			status.append(" to ");
			status.append(window_->action_data1.isEmpty() ? "..." : window_->action_data1);
		}
	}
	status_bar->showMessage(status);

	event->acceptProposedAction();
}

void item_view::dropEvent(QDropEvent *event, QAbstractItemView *view) {
	window_->drop_menu_->popup(view->viewport()->mapToGlobal(event->pos()));
	event->acceptProposedAction();
}

void item_view::mousePressEventTree(QMouseEvent *event) {
//	if (!(event->button() & Qt::MouseButton::LeftButton)) {
//		event->ignore();
//		return;
//	}
	tree_->selectionModel()->clearSelection();
	head_->setText("");
	auto index = tree_->indexAt(event->pos());
	if (index.isValid()) {
		tree_->selectionModel()->select(index, QItemSelectionModel::Select);
		const auto path {tree_model_.filePath(index)};
		head_->setText(QDir::toNativeSeparators(path));

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

void item_view::mouseDoubleClickEventList(QMouseEvent *event) {
	if (!(event->button() & Qt::MouseButton::LeftButton)) {
		event->ignore();
		return;
	}
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

			head_->setText(QDir().toNativeSeparators(path));
		}
	}
	event->accept();
}

void item_view::toggleFilter() {
	auto filter_tree = FILTER_BASE_TREE;
	auto filter_list = FILTER_BASE_LIST;
	if (list_model_.filter() & QDir::Hidden || tree_model_.filter() & QDir::Hidden) {
		status_bar()->showMessage("Do not show hidden entries.");
	} else {
		filter_tree |= FILTER_ADD_HIDDEN;
		filter_list |= FILTER_ADD_HIDDEN;
		status_bar()->showMessage("Show hidden entries.");
	}
	tree_model_.setFilter(filter_tree);
	list_model_.setFilter(filter_list);
	//tree_->update();
	//list_->update();
}
