#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include <boost/make_shared.hpp>
#include <boost/lockfree/ringbuffer.hpp>
#include <boost/utility.hpp>

#include "cf/ButlerThread.h"
#include "cf/LogItem.h"

namespace cf {

class LogBuffer : public boost::noncopyable
{
public:
	LogBuffer(std::ostream & stream);

	// Register to butler
	void RegisterToButler(boost::shared_ptr<ButlerThread> butler);

	// add loggable item to queue in a lock-free way
	void Log(LogItem const & item) { buffer_.enqueue(item); }

	// Commit log to stream, most possibly not lock-free
	void Commit();

private:
	void CommitOne(LogItem const & item) const;

private:
	typedef boost::lockfree::ringbuffer<LogItem, 0> Buffer;

	Buffer buffer_;
	std::ostream & stream_;
	boost::shared_ptr<ButlerThread> butler_;
	ButlerThread::CallbackHandle callbackHandle_;
};

// A logger that logs to a file, needs a butler thread
class FileLogger
{
public:
	FileLogger(std::string const & filename, boost::shared_ptr<ButlerThread> butler)
		: stream_(filename.c_str(), std::ios_base::out | std::ios_base::trunc)
	{
		assert(!stream_.fail());
		logBuffer_ = boost::make_shared<LogBuffer>(stream_);
		logBuffer_->RegisterToButler(butler);
	}

	~FileLogger()
	{
		logBuffer_->Commit();
	}

	void Log(LogItem const & item) { logBuffer_->Log(item); }

private:
	std::ofstream stream_;
	boost::shared_ptr<LogBuffer> logBuffer_;
};


} // namespace cf
