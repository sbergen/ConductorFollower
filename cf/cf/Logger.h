#pragma once

#include <exception>
#include <fstream>
#include <iostream>
#include <string>

#include <boost/variant.hpp>
#include <boost/array.hpp>
#include <boost/make_shared.hpp>
#include <boost/lockfree/ringbuffer.hpp>
#include <boost/utility.hpp>

#include "cf/ButlerThread.h"
#include "cf/time.h"

namespace cf {

class LogItem
{
private:
	typedef boost::variant<
		boost::blank,
		int, unsigned, long,
		float, double, 
		char const *,
		timestamp_t, duration_t, milliseconds_t, seconds_t,
		boost::chrono::microseconds> Arg;

public:
	LogItem()
		: argIndex_(0)
	{
		args_[0] = Arg();
	}

	LogItem(char const * fmt)
		: argIndex_(0)
	{
		for (int i = 0; i < MaxFmtLen; ++i) {
			format_[i] = fmt[i];
			if (fmt[i] == '\0') {
				args_[0] = Arg();
				return;
			}
		}

		throw std::invalid_argument("Too long format string given to Logger");
	}
	
	// The Boost.Format convention
	template<typename T>
	LogItem & operator% (T const & arg)
	{
		args_[argIndex_] = arg;
		++argIndex_;
		assert(argIndex_ < MaxArgs);
		args_[argIndex_] = Arg();
		return *this;
	}

	// For variadric macro convenience
	template<typename T>
	LogItem & operator, (T const & arg)
	{
		return *this % arg;
	}

private:
	friend class Logger;

	enum { MaxFmtLen = 512, MaxArgs = 6 };

	int argIndex_;
	boost::array<std::string::value_type, MaxFmtLen> format_;
	boost::array<Arg, MaxArgs> args_;
};

class Logger : public boost::noncopyable
{
public:
	Logger(std::ostream & stream)
		: stream_(stream)
		, buffer_(256)
	{}

	// add loggable item to queue in a lock-free way
	void Log(LogItem const & item) { buffer_.enqueue(item); }

	// Commit log to stream, most possibly not lock-free
	void Commit();

private:
	void CommitOne(LogItem const & item) const;

private:
	typedef boost::lockfree::ringbuffer<LogItem, 0> Buffer;

	std::ostream & stream_;
	Buffer buffer_;
};

// A logger that logs to a file, needs a butler thread
class FileLogger
{
public:
	FileLogger(std::string const & filename, boost::shared_ptr<ButlerThread> butler)
		: butler_(butler)
		, stream_(filename.c_str(), std::ios_base::out | std::ios_base::trunc)
	{
		assert(!stream_.fail());
		logger_ = boost::make_shared<Logger>(stream_);
		callbackHandle_ = butler_->AddCallback(
			boost::bind(&Logger::Commit, logger_));
	}

	~FileLogger()
	{
		// Ensure proper destruction order
		logger_->Commit();
		callbackHandle_.Cancel();
	}

	void Log(LogItem const & item) { logger_->Log(item); }

private:
	boost::shared_ptr<ButlerThread> butler_;
	std::ofstream stream_;
	boost::shared_ptr<Logger> logger_;
	ButlerThread::CallbackHandle callbackHandle_;
};


} // namespace cf
