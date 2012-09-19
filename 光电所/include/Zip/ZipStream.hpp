#ifndef __GZSTREAM_HPP
#define __GZSTREAM_HPP

// standard C++ with new header file names and std:: namespace
#include <iostream>
#include <fstream>
#include "ZLib/zlib.h"

namespace zip 
{


	// ----------------------------------------------------------------------------
	// Internal classes to implement gzstream. See below for user classes.
	// ----------------------------------------------------------------------------

	class ZipStreamBuf 
		: public std::streambuf 
	{
	private:
		static const int bufferSize = 47+256;    // size of data buff
		
		gzFile           file;               // file handle for compressed file
		char             buffer[bufferSize]; // data buffer
		char             opened;             // open/close state of stream
		int              mode;               // I/O mode

		int flush_buffer();

	public:
		ZipStreamBuf() 
			: opened(0) 
		{
			setp( buffer, buffer + (bufferSize-1));
			setg( buffer + 4,     // beginning of putback area
				buffer + 4,     // read position
				buffer + 4);    // end position      
			// ASSERT: both input & output capabilities will not be used together
		}
		int is_open() { return opened; }
		ZipStreamBuf* open( const char* name, int open_mode);
		ZipStreamBuf* close();
		~ZipStreamBuf() { close(); }

		virtual int     overflow( int c = EOF);
		virtual int     underflow();
		virtual int     sync();
	};

	class ZipStreamBase 
		: virtual public std::ios 
	{
	protected:
		ZipStreamBuf buf;

	public:
		ZipStreamBase() { init(&buf); }
		ZipStreamBase( const char* name, int open_mode);
		~ZipStreamBase();
		void open( const char* name, int open_mode);
		void close();
		ZipStreamBuf* rdbuf() { return &buf; }
	};

	// ----------------------------------------------------------------------------
	// User classes. Use igzstream and ogzstream analogously to ifstream and
	// ofstream respectively. They read and write files based on the gz* 
	// function interface of the zlib. Files are compatible with gzip compression.
	// ----------------------------------------------------------------------------

	class IZipStream 
		: public ZipStreamBase
		, public std::istream
	{
	public:
		IZipStream() : std::istream( &buf) {} 
		IZipStream( const char* name, int open_mode = std::ios::in)
			: ZipStreamBase( name, open_mode), std::istream( &buf) {}  
		ZipStreamBuf* rdbuf() { return ZipStreamBase::rdbuf(); }
		void open( const char* name, int open_mode = std::ios::in) 
		{
			ZipStreamBase::open( name, open_mode);
		}
	};

	class OZipStream 
		: public ZipStreamBase
		, public std::ostream 
	{
	public:
		OZipStream() : std::ostream( &buf) {}
		OZipStream( const char* name, int mode = std::ios::out)
			: ZipStreamBase( name, mode)
			, std::ostream( &buf) 
		{}  
		ZipStreamBuf* rdbuf() { return ZipStreamBase::rdbuf(); }
		void open( const char* name, int open_mode = std::ios::out) 
		{
			ZipStreamBase::open( name, open_mode);
		}
	};
}

#endif 
