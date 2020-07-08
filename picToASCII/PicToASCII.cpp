#include "PicToASCII.h"
/*
pta::scheduled_sender::scheduled_sender(const std::string& s, double d, double t) :
	url(s),
	interval(int(d* proton::duration::SECOND.milliseconds())), // Send interval.
	timeout(int(t* proton::duration::SECOND.milliseconds())), // Cancel after timeout.
	work_queue(0),
	ready(true),            // Ready to send.
	canceled(false)         // Canceled.
{}

pta::receiver::receiver(const std::string& s, const std::string& u, const std::string& p, int c) :
	url(s), user(u), password(p), expected(c), received(0) {}*/


pta::PicToASCII::PicToASCII(unsigned int w, unsigned int h) :
	std_out(GetStdHandle(STD_OUTPUT_HANDLE)),
	window_size({ 0,0, (SHORT)(2 * w),(SHORT)h }),
	home_({ 0,0 }),
	last_({ (SHORT)(2 * w),(SHORT)h }),
	capture_(0),
	ready_(false),
	user_("Vlatko"),
	password_("Patak3005"),
	//address("amqp://guest:guest@localhost/"),
	//mReceiver("127.0.0.1:5672",mUser, mPassword,0), 
	//mSender("127.0.0.1:5672",30,40), 
	height_((SHORT)(h)),
	width_((SHORT)(w)),
	scale_(L" `^\",:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$◙◘█")//◙◘█
{
}

pta::PicToASCII::PicToASCII() :
	std_out(GetStdHandle(STD_OUTPUT_HANDLE)),
	window_size({ 0,0,(SHORT)2 * 168,94 }),
	home_({ 0,0 }),
	last_({ (SHORT)(2 * 168),94 }),
	capture_(0),
	ready_(false),
	user_("Vlatko"),
	password_("Patak3005"),
	//address("amqp://guest:guest@localhost/"),
	height_(168),
	width_(94),
	scale_(L" `^\",:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$◙◘█")//◙◘█
	//scale_(L"███████████████████████████████████████████████████████████████████████")
{
}


pta::PicToASCII::~PicToASCII()
{
}

void pta::PicToASCII::console_setup(std::string window_name)
{
	SetConsoleTitle(TEXT("window_name"));
	SetConsoleWindowInfo(std_out, TRUE, &window_size);
	SetConsoleScreenBufferSize(std_out, last_);
}

void pta::PicToASCII::frame_catch_and_provide()
{
	cv::Mat frame;
	if (!capture_.isOpened())
		std::cerr << "ERROR! Unable to open camera\n";
	else
		for (;;)
		{
			frame = catch_frame();
			cv::imshow("Live", frame);
			resize_and_grayscale(frame);
			img_to_deq(frame);
			if (cv::waitKey(30) == 27)
				return;

		}
}

void pta::PicToASCII::img_to_ASCII()
{
	for (;;) 
	{
		cv::Mat img(get_frame());
		char_infos.push_front(img_to_char_infos(img));
	}

}

void pta::PicToASCII::show_myself_local()
{
	SetConsoleTitle(TEXT("You in ASCII"));
	SetConsoleWindowInfo(std_out, TRUE, &window_size);
	SetConsoleScreenBufferSize(std_out, last_);
	SetConsoleOutputCP(CP_UTF8);
	for (;;)
	{
		show_picture();
	}
}

void pta::PicToASCII::show_myself_remote()
{
	HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTitle(TEXT("You in ASCII"));
	SetConsoleWindowInfo(stdOut, TRUE, &window_size);
	SetConsoleScreenBufferSize(stdOut, last_);
	for (;;)
	{

	}

}

void pta::PicToASCII::run()
{
	std::thread runCamera(&PicToASCII::frame_catch_and_provide, this);
	std::thread runConverter(&PicToASCII::img_to_ASCII, this);
	std::thread runOutput(&PicToASCII::show_myself_local, this);
	if (runCamera.joinable())
		runCamera.join();
	else
		runCamera.detach();
	if (runConverter.joinable())
		runConverter.join();
	else
		runConverter.detach();
	if (runOutput.joinable())
		runOutput.join();
	else
		runOutput.detach();
}

void pta::PicToASCII::run_remote()
{
	std::thread runCamera(&PicToASCII::frame_catch_and_provide, this);
	std::thread runConverter(&PicToASCII::img_to_ASCII, this);
	std::thread runOutput(&PicToASCII::show_myself_local, this);
	if (runCamera.joinable())
		runCamera.join();
	else
		runCamera.detach();
	if (runConverter.joinable())
		runConverter.join();
	else
		runConverter.detach();
	if (runOutput.joinable())
		runOutput.join();
	else
		runOutput.detach();
}

void pta::PicToASCII::clear_screen()
{
	HANDLE hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD count;
	DWORD cellCount;
	COORD homeCoords = { 0, 0 };
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE)
		return;
	/* Get the number of cells in the current buffer */
	if (!GetConsoleScreenBufferInfo(hStdOut, &csbi))
		return;
	cellCount = csbi.dwSize.X * csbi.dwSize.Y;
	/* Fill the entire buffer with spaces */
	if (!FillConsoleOutputCharacter(hStdOut, (TCHAR)' ', cellCount, homeCoords, &count))
		return;
	/* Fill the entire buffer with the current colors and attributes */
	if (!FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, cellCount, homeCoords, &count))
		return;
	SetConsoleCursorPosition(hStdOut, homeCoords);
}

void pta::PicToASCII::show_picture()
{
	std::unique_lock<std::mutex> lck(mtx_output);
	cond.wait(lck, [this]() {return !char_infos.empty(); });
	SetConsoleCursorPosition(std_out, home_);
	if (!char_infos.empty())
	{
		WriteConsoleOutputW(std_out, char_infos.back(), last_, home_, &window_size);
		char_infos.pop_back();
	}
}
void pta::PicToASCII::resize_and_grayscale(cv::Mat& img)
{
	cv::flip(img, img, 1);
	cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
	cv::resize(img, img, cv::Size(height_, width_));
}
void pta::PicToASCII::img_to_deq(cv::Mat& img)
{
	std::unique_lock<std::mutex> lock(mtx_producer);
	imgs_.push_front(img.clone());
	ready_ = true;
	lock.unlock();
	cond.notify_all();
}
cv::Mat pta::PicToASCII::get_frame()
{
	std::unique_lock<std::mutex> lck(mtx_consumer);
	cond.wait(lck, [this]() {return ready_; });
	cv::Mat img(imgs_.back());
	imgs_.pop_back();
	lck.unlock();
	ready_ = false;
	cond.notify_all();
	return img.clone();
}
CHAR_INFO* pta::PicToASCII::img_to_char_infos(cv::Mat& img)
{
	CHAR_INFO* buf = new CHAR_INFO[2 * height_ * width_];
	for (unsigned int i = 0; i < height_ * width_; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			buf[2 * i + j].Char.UnicodeChar = (unsigned int)scale_[img.data[i] / 3.72];
			buf[2 * i + j].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
		}
	}
	return buf;
}
cv::Mat pta::PicToASCII::catch_frame()
{
	cv::Mat frame;
	std::unique_lock<std::mutex> lock(mtx_producer);
	cond.wait(lock, [this] {return !ready_; });
	capture_ >> frame;
	cond.notify_all();
	return frame;
}
/*
pta::scheduled_sender::scheduled_sender(const std::string& s, double d, double t):
	url(s),
	interval(int(d* proton::duration::SECOND.milliseconds())), // Send interval.
	timeout(int(t* proton::duration::SECOND.milliseconds())), // Cancel after timeout.
	work_queue(0),
	ready(true),            // Ready to send.
	canceled(false)         // Canceled.
{}

void pta::scheduled_sender::on_container_start(proton::container& c)
{
	c.open_sender(url);
}

void pta::scheduled_sender::on_sender_open(proton::sender& s)
{
	work_queue = &s.work_queue();
	work_queue->schedule(timeout, make_work(&scheduled_sender::cancel, this, s));
	work_queue->schedule(interval, make_work(&scheduled_sender::tick, this, s));
}

void pta::scheduled_sender::cancel(proton::sender sender)
{
	canceled = true;
	sender.connection().close();
}

void pta::scheduled_sender::tick(proton::sender sender)
{
	if (!canceled) {
		work_queue->schedule(interval, make_work(&scheduled_sender::tick, this, sender)); // Next tick
		if (sender.credit() > 0) // Only send if we have credit
			send(sender);
		else
			ready = true; // Set the ready flag, send as soon as we get credit.
	}
}

void pta::scheduled_sender::on_sendable(proton::sender& sender)
{
	if (ready)              // We have been ticked since the last send.
		send(sender);
}

void pta::scheduled_sender::send(proton::sender& sender)
{
	std::cout << "send" << std::endl;
	sender.send(proton::message("ping"));
	ready = false;
}

void pta::receiver::on_container_start(proton::container& c)
{
	proton::connection_options co;
	if (!user.empty()) co.user(user);
	if (!password.empty()) co.password(password);
	receiver_ = c.open_receiver(url, co);
}

void pta::receiver::on_message(proton::delivery& d, proton::message& msg)
{
	if (!msg.id().empty() && proton::coerce<int>(msg.id()) < received) {
		return; // Ignore if no id or duplicate
	}


	if (received == expected)
	{
		d.receiver().close();
		d.connection().close();
	}
}
*/