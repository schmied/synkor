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

#include "../../contrib/spdlog/spdlog.h"
#include "../../contrib/spdlog/sinks/basic_file_sink.h"

#include "../global.hpp"

#include "window.hpp"

#include "item_view.hpp"
#include "list_view.hpp"

#include "ui_window.h"

static const auto logger = spdlog::basic_logger_mt(stdfs::path(__FILE__).stem().string(), "synkor.log");

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
	context_menu_->addSeparator();
	action_context_delete = context_menu_->addAction("Delete");
	connect(action_context_delete, SIGNAL(triggered()), this, SLOT(context_delete_action()));
	context_menu_->addSeparator();
	context_menu_archive_ = context_menu_->addMenu("Archive");
	action_context_archive_create = context_menu_archive_->addAction("Create");
	connect(action_context_archive_create, SIGNAL(triggered()), this, SLOT(context_archive_create()));
	action_context_archive_extract = context_menu_archive_->addAction("Extract");
	connect(action_context_archive_extract, SIGNAL(triggered()), this, SLOT(context_archive_extract()));

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
	delete context_menu_archive_;
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

void window::context_delete_action() {
	ui_->status_bar->showMessage(action_items_src.join(" : ").append(" CONTEXT DELETE"));
}

void window::context_archive_create() {
	ui_->status_bar->showMessage(action_items_src.join(" : ").append(" CONTEXT ARCHIVE CREATE"));
}

void window::context_archive_extract() {
	ui_->status_bar->showMessage(action_items_src.join(" : ").append(" CONTEXT ARCHIVE EXTRACT"));
}

#include <fstream>
#include <thread>
#include <contrib/asio/asio.hpp>
#include <mutex>
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <chrono>

//const std::size_t bufsize = 1048576;
static const std::size_t bufsize = 16777216;
static const std::size_t bufsize_fstream = 262144;
//const std::size_t bufsize = 8192;
static asio::streambuf sb {bufsize};
static std::mutex mtx;

static void thread_read(const std::string action_items_src) {
	char buf[bufsize_fstream];
	std::ios_base::sync_with_stdio(false);
	bool run = true;
	const std::filesystem::path path {action_items_src.substr(8)}; // remove 'file:///'
	std::ifstream ifs;
	ifs.rdbuf()->pubsetbuf(buf, bufsize);
	ifs.open(path, std::ios_base::binary | std::ios_base::in);
	while (run) {
		while (ifs) {
			logger->info("read lock 1");
			mtx.lock();
			logger->info("read lock 2");
			const std::size_t read_size_max = std::min(sb.max_size() - sb.size(), bufsize);
			logger->info("read size max {:d}", read_size_max);
			auto buf_dst = sb.prepare(read_size_max);
			logger->info("read {:d}", buf_dst.size());
			ifs.read((char*) buf_dst.data(), read_size_max);
			const std::size_t read_size = (std::size_t) ifs.gcount();
			logger->info("read read {:d}", read_size);
			sb.commit(read_size);
			mtx.unlock();
			logger->info("read unlock");

			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1ms);
		}
		run = false;
	}
	logger->info("reader before");
	ifs.close();
	logger->info("reader after");
}

static void thread_write(QString action_dir_dst) {
	char buf[bufsize_fstream];
	std::ios_base::sync_with_stdio(false);
	bool run = true;
	int i = 0;
	std::ofstream ofs;
	ofs.rdbuf()->pubsetbuf(buf, bufsize);
	ofs.open(action_dir_dst.append("/blaaaa.txt").toStdString().c_str(), std::ios_base::binary | std::ios_base::out);
	while (run) {
		logger->info("write lock 1");
		mtx.lock();
		logger->info("write lock 2");
		const auto buf_src = sb.data();
		logger->info("write buf src {:d}", buf_src.size());
		const std::size_t write_size = std::min(buf_src.size(), bufsize);
		logger->info("write size {:d}", buf_src.size());
		if (write_size > 0) {
			ofs.write((const char*) buf_src.data(), write_size);
			//ofs.flush();
			sb.consume(write_size);
		} else {
			i++;
		}
		mtx.unlock();
		logger->info("write unlock");

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1ms);

		if (i > 10)
			run = false;
	}
	logger->info("write close before");
	ofs.close();
	logger->info("write close after");
}

void window::drop_copy_action() {

	ui_->status_bar->showMessage(action_items_src.join(" : ").append(" -COPY-> ").append(action_dir_dst));

	std::thread thr_write {thread_write, action_dir_dst};
	std::thread thr_read {thread_read, action_items_src.first().toStdString()};
	thr_write.detach();
	thr_read.detach();

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
