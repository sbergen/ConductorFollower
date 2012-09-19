#include <boost/test/unit_test.hpp>

#include <boost/thread.hpp>

#include "cf/ChenBuffer.h"

using namespace cf;

BOOST_AUTO_TEST_SUITE(ChenBufferTests)


struct SlowAssignable
{
	SlowAssignable()
		: i1(0), i2(0) {}

	SlowAssignable & operator=(int i)
	{
		i1 = i;

		int sleepTime = rand() % 20;
		boost::this_thread::sleep(boost::posix_time::milliseconds(sleepTime));
		
		i2 = i;
		return *this;
	}

	SlowAssignable & operator=(SlowAssignable const & other)
	{
		i1 = other.i1;
		
		int sleepTime = rand() % 20;
		boost::this_thread::sleep(boost::posix_time::milliseconds(sleepTime));

		i2 = other.i2;
		return *this;
	}

	bool IsIntact() const { return i1 == i2; }

private:
	int i1;
	int i2;
};

BOOST_AUTO_TEST_CASE(TestInit)
{
	typedef ChenBuffer<int, 1> Buffer;
	Buffer buffer;

	buffer.Init([](int & val) { val = 42; });
	Buffer::Reader reader(buffer);
	BOOST_CHECK_EQUAL(*reader, 42);
}

BOOST_AUTO_TEST_CASE(TestCongestedReaderCase)
{
	// See that writes succeed even if nobody is reading
	typedef ChenBuffer<int, 1> Buffer;
	Buffer buffer;

	for (int i = 0; i < 10; ++i) {
		Buffer::Writer writer(buffer);
		*writer = i;
	}

	Buffer::Reader reader(buffer);
	BOOST_CHECK_EQUAL(*reader, 9);
}

BOOST_AUTO_TEST_CASE(TestCongestedWriterCase)
{
	// See that reads succeed even if nobody is writing
	typedef ChenBuffer<int, 1> Buffer;
	Buffer buffer;
	
	{
		Buffer::Writer writer(buffer);
		*writer = 42;
	}

	Buffer::Reader reader(buffer);
	for (int i = 0; i < 10; ++i) {
		BOOST_CHECK_EQUAL(*reader, 42);	
	}
}

BOOST_AUTO_TEST_CASE(TestSlowAssignCase)
{
	static const int readers = 5;
	const int rounds = 100;

	typedef ChenBuffer<SlowAssignable, readers> Buffer;
	Buffer buffer;

	boost::barrier barrier(readers + 2);

	boost::thread writerThread(
		[&barrier, &buffer, rounds]()
		{
			for (int i = 0; i < rounds; ++i) {
				auto writer = buffer.GetWriter();
				*writer = i;
			}
			barrier.wait();
		});
	
	for (int i = 0; i < readers; ++i) {
		boost::thread readerThread(
			[&barrier, &buffer, rounds]()
			{
				for (int i = 0; i < rounds; ++i) {
					auto reader = buffer.GetReader();
					SlowAssignable var;
					var = *reader;
					// BOOST_CHECK is not thread safe
					assert(var.IsIntact());
				}
				barrier.wait();
			});
	}

	barrier.wait();
}

BOOST_AUTO_TEST_SUITE_END()