#pragma once

#include <exception>
#include <fstream>
#include <iostream>
#include <string>

#include <boost/variant.hpp>
#include <boost/array.hpp>
#include <boost/lockfree/ringbuffer.hpp>

#include "cf/cf.h"
#include "cf/ButlerThread.h"

namespace cf {

class LogItem
{
private:
	typedef boost::variant<
		boost::blank,
		int, unsigned, long,
		float, double, 
		timestamp_t, duration_t, milliseconds_t, seconds_t> Arg;

public:
	LogItem()
		: argIndex_(0)
	{
		args_[0] = Arg();
	}

	template<typename T>
	LogItem(T const & fmt)
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

	LogItem(std::string const & fmt)
		: argIndex_(0)
	{
		assert (fmt.length() < MaxFmtLen);
		for (int i = 0; i < fmt.length(); ++i) {
			format_[i] = fmt[i];
		}
		format_[fmt.length()] = '\0';
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

	enum { MaxFmtLen = 512, MaxArgs = 5 };

	int argIndex_;
	boost::array<std::string::value_type, MaxFmtLen> format_;
	boost::array<Arg, MaxArgs> args_;
};

class Logger
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

// A logger that logs to a file and owns its own butler thread
class FileLogger
{
public:
	FileLogger(std::string const & filename)
		: butler_(milliseconds_t(100))
		, stream_(filename.c_str(), std::ios_base::out | std::ios_base::trunc)
	{
		assert(!stream_.fail());
		logger_.reset(new Logger(stream_));
		butler_.AddCallback(boost::bind(&Logger::Commit, logger_.get()));
	}

	~FileLogger() {  }

	void Log(LogItem const & item) { logger_->Log(item); }

private:
	ButlerThread butler_;
	std::ofstream stream_;
	boost::scoped_ptr<Logger> logger_;
};


} // namespace cf
