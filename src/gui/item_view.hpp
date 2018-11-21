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

#ifndef BASE_VIEW_HPP
#define BASE_VIEW_HPP

#include <QAbstractItemView>
#include <QFileSystemModel>
#include <QLineEdit>
#include <QPushButton>
#include <QStatusBar>

class window;
class list_view;
class tree_view;

static const auto FILTER_BASE_TREE = QDir::AllDirs | QDir::NoDotAndDotDot;
static const auto FILTER_BASE_LIST = QDir::AllEntries | QDir::NoDotAndDotDot;
static const auto FILTER_ADD_HIDDEN = QDir::Hidden | QDir::System;

class item_view {

private:
	window *window_;
	list_view *list_;
	tree_view *tree_;
	QLineEdit *head_;
	QFileSystemModel list_model_;
	QFileSystemModel tree_model_;

public:
	item_view(window*, tree_view*, list_view*, QLineEdit*);

	window* main_window();
	list_view* list();
	tree_view* tree();
	QLineEdit* head();
	QStatusBar* status_bar();
	QFileSystemModel* list_model();
	QFileSystemModel* tree_model();

	void mousePressEventTree(QMouseEvent*);
	void mouseDoubleClickEventList(QMouseEvent*);

	void toggleFilter();

	void dropEvent(QDropEvent*, QAbstractItemView*);
	static void dragEnterEvent(QDragEnterEvent*);
	static void dragLeaveEvent(QDragLeaveEvent*, QAbstractItemView*);
	void dragMoveEvent(QDragMoveEvent*, QAbstractItemView*, QFileSystemModel*, QStatusBar*);

};

#endif
