#include "cf/Logger.h"

#include <boost/format.hpp>
#include <boost/units/io.hpp>

namespace cf {

void
LogBuffer::Commit()
{
	LogItem item;
	while (buffer_.dequeue(item)) {
		CommitOne(item);
	}
}

LogBuffer::LogBuffer(std::ostream & stream)
	: buffer_(256)
	, stream_(stream)
{
}

void
LogBuffer::RegisterToButler(boost::shared_ptr<ButlerThread> butler)
{
	butler_ = butler;
	callbackHandle_ = butler_->AddCallback(boost::bind(&LogBuffer::Commit, this));
}

void
LogBuffer::CommitOne(LogItem const & item) const
{
	std::string str;
	for(int i = 0; i < LogItem::MaxFmtLen; ++i)
	{
		char ch = item.format_[i];
		if (ch == '\0') { break; }
		str += ch;
	}
	boost::format fmt(str);

	for (int i = 0; i < LogItem::MaxArgs; ++i) {
		LogItem::Arg const & arg = item.args_[i];
		if (arg == LogItem::Arg()) { break; }
		fmt % arg;
	}

	stream_ <<  fmt << std::endl;
}

} // namespace cf
