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

#include "window.hpp"

#include "item_view.hpp"
#include "list_view.hpp"

#include "ui_window.h"

window::window(QWidget *parent) :
	QMainWindow(parent),
	ui_(new Ui::window) {

	ui_->setupUi(this);

	context_menu_ = new QMenu();
	context_menu_->setStyleSheet("QMenu { padding: 4px; } QMenu::indicator { width: 0px; height: 0px; }");
	action_context_copy = context_menu_->addAction("Copy");
	connect(action_context_copy, SIGNAL(triggered()), this, SLOT(context_copy_action()));
	action_context_cut = context_menu_->addAction("Cut");
	connect(action_context_cut, SIGNAL(triggered()), this, SLOT(context_cut_action()));
	action_context_paste = context_menu_->addAction("Paste");
	connect(action_context_paste, SIGNAL(triggered()), this, SLOT(context_paste_action()));

	drop_menu_ = new QMenu();
	drop_menu_->setStyleSheet("QMenu { padding: 4px; } QMenu::indicator { width: 0px; height: 0px; }");
	action_drop_copy = drop_menu_->addAction("Copy");
	connect(action_drop_copy, SIGNAL(triggered()), this, SLOT(drop_copy_action()));
	action_drop_move = drop_menu_->addAction("Move");
	connect(action_drop_move, SIGNAL(triggered()), this, SLOT(drop_move_action()));

	rl_item_left_ = new item_view {this, ui_->rl_item_left_tree, ui_->rl_item_left_list, ui_->rl_item_left_head_locator};
	ui_->rl_item_left_tree->set_item_view(rl_item_left_);
	ui_->rl_item_left_list->set_item_view(rl_item_left_);
	rl_item_right_ = new item_view {this, ui_->rl_item_right_tree, ui_->rl_item_right_list, ui_->rl_item_right_head_locator};
	ui_->rl_item_right_tree->set_item_view(rl_item_right_);
	ui_->rl_item_right_list->set_item_view(rl_item_right_);
}

window::~window() {
	delete rl_item_left_;
	delete rl_item_right_;
	delete context_menu_;
	delete drop_menu_;
	delete ui_;
}

void window::context_copy_action() {
	ui_->status_bar->showMessage(action_items_src.join(" : ").append(" CONTEXT COPY"));
}

void window::context_cut_action() {
	ui_->status_bar->showMessage(action_items_src.join(" : ").append(" CONTEXT CUT"));
}

void window::context_paste_action() {
	ui_->status_bar->showMessage(action_items_src.join(" : ").append(" CONTEXT PASTE"));
}

#include <fstream>

void window::drop_copy_action() {
	ui_->status_bar->showMessage(action_items_src.join(" : ").append(" -COPY-> ").append(action_dir_dst));

	char buffer[1000];
	std::string a { "sdf" };
	a.copy(buffer, a.length());
	std::filebuf fb;
	fb.pubsetbuf(buffer, 1000);
	fb.open(action_dir_dst.append("/blaaaat.txt").toStdString().c_str(), std::ios_base::binary | std::ios_base::out);
	fb.sputn(buffer, 50);
	fb.close();
}

void window::drop_move_action() {
	ui_->status_bar->showMessage(action_items_src.join(" : ").append(" -MOVE-> ").append(action_dir_dst));
}

Ui::window *window::ui() {
	return ui_;
}

void window::on_rl_item_left_head_filter_clicked() {
	rl_item_left_->toggleFilter();
}

void window::on_rl_item_right_head_filter_clicked() {
	rl_item_right_->toggleFilter();
	rl_item_right_->list()->baseSize().setWidth(100);
	rl_item_right_->list()->setMaximumWidth(100);
	rl_item_right_->list()->setMinimumWidth(100);
}
