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

#pragma once

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <QMainWindow>

#include "item_view.hpp"

namespace Ui {
class window;
}

class window : public QMainWindow {
	Q_OBJECT

private:
	Ui::window *ui_;
	item_view *rl_item_left_;
	item_view *rl_item_right_;

	QAction *action_context_copy;
	QAction *action_context_cut;
	QAction *action_context_paste;
	QAction *action_context_delete;
	QAction *action_context_archive_create;
	QAction *action_context_archive_extract;
	QAction *action_drop_copy;
	QAction *action_drop_move;

private slots:
	void on_rl_item_left_head_filter_clicked();
	void on_rl_item_right_head_filter_clicked();
	void context_copy_action();
	void context_cut_action();
	void context_paste_action();
	void context_delete_action();
	void context_archive_create();
	void context_archive_extract();
	void drop_copy_action();
	void drop_move_action();

protected:

public:
	explicit window(QWidget *parent = nullptr);
	~window();

	Ui::window *ui();

	QMenu *context_menu_;
	QMenu *context_menu_archive_;
	QMenu *drop_menu_;

	QString action_dir_dst;
	QList<QString> action_items_src;

};

#endif
