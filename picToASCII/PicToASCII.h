#pragma once

#include "Socket.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <mutex>
#include <vector>
#include <deque>
#include <WinSock2.h>
#include <windows.h>
#include <thread>
#include <condition_variable>
#include <memory>
/*#include <proton/container.hpp>
#include <proton/connection.hpp>
#include <proton/duration.hpp>
#include <proton/message.hpp>
#include <proton/messaging_handler.hpp>
#include <proton/sender.hpp>
#include <proton/tracker.hpp>
#include <proton/work_queue.hpp>
#include <proton/connection_options.hpp>
#include <proton/delivery.hpp>
#include <proton/link.hpp>
#include <proton/message_id.hpp>
#include <proton/value.hpp>*/




namespace pta
{
	/*class receiver : public proton::messaging_handler {
	private:
		std::string url;
		std::string user;
		std::string password;
		proton::receiver receiver_;
		int expected;
		int received;

	public:
		receiver(const std::string& s, const std::string& u, const std::string& p, int c);

		void on_container_start(proton::container& c) override;

		void on_message(proton::delivery& d, proton::message& msg) override;
	};
	class scheduled_sender : public proton::messaging_handler {
	private:
		std::string url;
		proton::duration interval, timeout;
		proton::work_queue* work_queue;
		bool ready, canceled;
	public:
		scheduled_sender(const std::string&, double, double);
		void on_container_start(proton::container& c) override;
		void on_sender_open(proton::sender& s) override;
		void cancel(proton::sender sender);
		void tick(proton::sender sender);
		void on_sendable(proton::sender& sender) override;
		void send(proton::sender& sender);
	};*/
	class PicToASCII
	{
	private:
		cv::VideoCapture capture_;
		HANDLE std_out;
		COORD home_, last_;
		SMALL_RECT window_size;
		std::condition_variable cond;
		std::mutex mtx_producer, mtx_consumer, mtx_output;
		bool ready_;
		std::deque<cv::Mat> imgs_;
		std::deque<std::wstring> wstrings_;
		std::deque<CHAR_INFO*> char_infos;
		const std::wstring scale_;
		const std::string user_, password_;
		unsigned int height_, width_;
		//scheduled_sender mSender;
		//receiver mReceiver;

	public:
		PicToASCII(unsigned int, unsigned int);
		PicToASCII();
		~PicToASCII();
		void console_setup(std::string);
		void frame_catch_and_provide();
		void img_to_ASCII();
		void show_myself_local();
		void show_myself_remote();
		void run();
		void run_remote();
		void clear_screen();
		void show_picture();
		void resize_and_grayscale(cv::Mat&);
		void img_to_deq(cv::Mat&);
		cv::Mat get_frame();
		CHAR_INFO* img_to_char_infos(cv::Mat&);
		cv::Mat catch_frame();
	};
};