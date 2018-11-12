
#include "window.hpp"

#include "item_view.hpp"
#include "list_view.hpp"

#include "ui_window.h"

window::window(QWidget *parent) :
	QMainWindow(parent),
	ui_(new Ui::window) {

	ui_->setupUi(this);

	rl_item_left_ = new item_view {ui_->rl_item_left_tree, ui_->rl_item_left_list, ui_->rl_item_left_head_locator, ui_->status_bar};
	ui_->rl_item_left_tree->set_item_view(rl_item_left_);
	ui_->rl_item_left_list->set_item_view(rl_item_left_);
	rl_item_right_ = new item_view {ui_->rl_item_right_tree, ui_->rl_item_right_list, ui_->rl_item_right_head_locator, ui_->status_bar};
	ui_->rl_item_right_tree->set_item_view(rl_item_right_);
	ui_->rl_item_right_list->set_item_view(rl_item_right_);
}

window::~window() {
	delete rl_item_left_;
	delete rl_item_right_;
	delete ui_;
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
