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

#include "list_view.hpp"
#include "window.hpp"

list_view::list_view(QWidget *parent) : QListView(parent) {
	setContextMenuPolicy(Qt::DefaultContextMenu);
}

void list_view::set_item_view(item_view *item_view) {
	item_view_ = item_view;
}

void list_view::dragMoveEvent(QDragMoveEvent *event) {
	item_view_->dragMoveEvent(event, this, item_view_->list_model(), item_view_->status_bar());
}

void list_view::dropEvent(QDropEvent *event) {
	item_view_->dropEvent(event, this);
}

void list_view::mousePressEvent(QMouseEvent *event) {
//	if (!(event->button() & Qt::MouseButton::LeftButton)) {
//		event->ignore();
//		return;
//	}
	QListView::mousePressEvent(event);
	event->accept();;
}

void list_view::mouseDoubleClickEvent(QMouseEvent *event) {
	item_view_->mouseDoubleClickEventList(event);
}

void list_view::contextMenuEvent(QContextMenuEvent *event) {
	const auto main_window = item_view_->main_window();
	main_window->action_items_src.clear();
	for (const auto &index : selectedIndexes())
		main_window->action_items_src.push_back(item_view_->tree_model()->filePath(index));
	main_window->context_menu_->popup(viewport()->mapToGlobal(event->pos()));
	event->accept();
}
