/*
 * Copyright 2018 Michael Schmiedgen
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright notice
 * and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS.IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */

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
	tree_->setModel(&tree_model_);

	list_model_.setReadOnly(true);
	list_model_.setFilter(FILTER_BASE_LIST);
	list_model_.iconProvider()->setOptions(QFileIconProvider::DontUseCustomDirectoryIcons);
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

void item_view::dragMoveEvent(QDragMoveEvent *event, QAbstractItemView *view, QFileSystemModel *model, QStatusBar *status_bar) {
	window_->action_items_src.clear();
	window_->action_dir_dst.clear();
	QString status {};
	const auto mime = event->mimeData();
	if (mime != nullptr && mime->hasUrls()) {
		auto urls = mime->urls();
		for (const auto &url : urls) {
			if (url.isLocalFile())
				window_->action_items_src.push_back(url.toString().toStdString());
		}
		if (!window_->action_items_src.empty()) {
			auto index = view->indexAt(event->pos());
			if (index.isValid()) {
				if (!model->isDir(index))
					index = model->parent(index);
				window_->action_dir_dst = model->filePath(index).toStdString();
			} else {
				window_->action_dir_dst.clear();
			}
			status.append("Copy / Move / Sync ");
			status.append(window::join(window_->action_items_src));
			status.append(" -> ");
			status.append(window_->action_dir_dst.empty() ? "..." : QString::fromStdString(window_->action_dir_dst.string()));
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

			list_->selectionModel()->clearSelection();
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
