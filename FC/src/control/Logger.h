#ifndef FC_CONTROL_LOGGER_H
#define FC_CONTROL_LOGGER_H

#include <FC/drivers/EEPROM.h>
#include <stmos/util/NonCopyable.h>
#include <stmos/util/Callback.h>
#include <stmos/util/Task.h>
#include <stmos/util/Mutex.h>
#include <stmos/util/Notifier.h>
#include <stdint.h>

namespace FC {
	class BaseLogger {
		protected:
			struct EntryHeader {
				uint8_t id;
				uint8_t length;
				uint8_t checksum;
			} __attribute__((packed));
			
			static const int DATALENGTH = EEPROM::PAGELENGTH - sizeof(EntryHeader);
		
			struct Entry {
				EntryHeader header;
				uint8_t data[DATALENGTH];
			} __attribute__((packed));
			
			static uint8_t computeChecksum(const uint8_t *data, size_t length);
	};

	class Logger : BaseLogger, stmos::NonCopyable, stmos::Callback {
		public:
			Logger(EEPROM &eeprom, int startpage);
			
			void write(const uint8_t *data, size_t len);
			void print(const char *msg);
			void printf(const char *fmt, ...);
			
			void flush();
			
		private:
			void flipBuf();
			void doWrite();
			virtual void call();
		
			EEPROM &eeprom;
			int curpage;
			
			Entry *curbuf, *writebuf;
			volatile bool writebuf_written;
			
			Entry bufa, bufb;
			stmos::Task task;
			stmos::Mutex writemutex;
			char *printfbuf;
	};
	
	class LogReader : BaseLogger, stmos::NonCopyable {
		public:
			LogReader(EEPROM &eeprom, int startpage);
			
			int read(uint8_t *data, size_t len);
		private:
			void nextPage();
			int verifyPage();
		
			EEPROM &eeprom;
			int nextpage;
			int curpos;
			uint8_t curid;
			
			Entry buf;
	};
}

#endif
