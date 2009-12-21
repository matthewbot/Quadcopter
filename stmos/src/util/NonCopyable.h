#ifndef STMOS_UTIL_NONCOPYABLE_H
#define STMOS_UTIL_NONCOPYABLE_H

namespace stmos {
	class NonCopyable {
		public:
			inline NonCopyable() { }
			inline ~NonCopyable() { }
	
		private:
			NonCopyable(const NonCopyable &);
			NonCopyable &operator=(const NonCopyable &);
	};
}

#endif
