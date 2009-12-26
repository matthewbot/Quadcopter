#include "Logger.h"
#include <cstring>
#include <cstdarg>
#include <cstdio>

using namespace FC;
using namespace stmos;

Logger::Logger(EEPROM &eeprom, int startpage)
: eeprom(eeprom),
  curpage(startpage),
  curbuf(&bufa),
  writebuf(&bufb),
  writebuf_written(true),
  task("logger", Task::PRIORITY_LOW, *this),
  printfbuf(NULL) {
	EntryHeader prevheader;
	eeprom.read(EEPROM::pageAddress(startpage), (uint8_t *)&prevheader, sizeof(EntryHeader)); // read the header of the current page
	bufb.header.id = bufa.header.id = prevheader.id + 1; // set the ID of our two buffered entries to one unique from the previous log
	
	curbuf->header.length = 0; // clear the length field of the current buffer
}

void Logger::write(const uint8_t *data, size_t len) {
	while (len > 0) {
		size_t avail = DATALENGTH - curbuf->header.length; // compute the amount available in the current buffer
		size_t amt = len < avail ? len : avail; // compute the amount to transfer
		
		memcpy(&curbuf->data[curbuf->header.length], data, amt); // transfer the data into the buffer
		curbuf->header.length += amt; // adjust counters
		data += amt;
		len -= amt;
		
		if (curbuf->header.length == DATALENGTH) // if the current buffer is full
			flipBuf(); // move it to the write buffer, and get a blank current buffer
	}
}

void Logger::print(const char *msg) {
	write((const uint8_t *)msg, strlen(msg));
}

#define PRINTF_MAXSIZE 100

void Logger::printf(const char *fmt, ...) {
	if (!printfbuf)
		printfbuf = new char [PRINTF_MAXSIZE];
		
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(printfbuf, PRINTF_MAXSIZE, fmt, ap);
	va_end(ap);
	
	print(printfbuf);
}

extern "C" {
	#include <stmos/crt/debug.h>
}

void Logger::flush() {
	debug_print("flush a\n");
	flipBuf(); // move current buffer to the write buffer
	debug_print("flush b\n");
	doWrite(); // perform the write
	debug_print("flush c\n");
}

void Logger::flipBuf() {
	if (!writebuf_written) {
		debug_print("blah\n");
		doWrite();
	}
	
	Entry *tmp = curbuf;
	curbuf = writebuf;
	writebuf = tmp;
	writebuf_written = false;
	curbuf->header.length = 0;
}

void Logger::doWrite() {
	Lock writelock(writemutex);
	
	if (writebuf_written)
		return;
		
	writebuf->header.checksum = computeChecksum(writebuf->data, writebuf->header.length);
	eeprom.write(EEPROM::pageAddress(curpage), (const uint8_t *)writebuf, sizeof(EntryHeader) + writebuf->header.length);
	curpage++;
	writebuf_written = true;
}

__attribute__((noreturn))
void Logger::call() {
	while (true) {
		eeprom.waitReady();
		
		while (writebuf_written) {
			Task::sleep(1);
		}
		
		doWrite();
	}
}

LogReader::LogReader(EEPROM &eeprom, int startpage)
: eeprom(eeprom),
  nextpage(startpage),
  curpos(0) {
 	 nextPage();
 	 curid = buf.header.id;
}

int LogReader::read(uint8_t *data, size_t len) {
	int verify = verifyPage();
	if (verify < 0)
		return verify;
		
	int totamt=0;
	while (len > 0) {
		size_t avail = buf.header.length - curpos;
		size_t amt = len < avail ? len : avail;
		
		memcpy(data, &buf.data[curpos], amt);
		totamt += amt;
		curpos += amt;
		data += amt;
		len -= amt;
		
		if (buf.header.length == curpos) {
			nextPage();
			
			if (verifyPage() < 0)
				break;
		}
	}
	
	return totamt;
}

void LogReader::nextPage() {
	eeprom.read(EEPROM::pageAddress(nextpage), (uint8_t *)&buf, EEPROM::PAGELENGTH);
	nextpage++;
	curpos = 0;
}

int LogReader::verifyPage() {
	if (buf.header.id != curid) 
		return -1;
	else if (buf.header.checksum != computeChecksum(buf.data, buf.header.length))
		return -2;
	
	return 0;
}

uint8_t BaseLogger::computeChecksum(const uint8_t *data, size_t len) {
	uint8_t checksum=0;
	while (len--)
		checksum += *data++;
	return checksum;
}

