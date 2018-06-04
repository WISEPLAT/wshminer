/*
	This file is part of cpp-wiseplat.

	cpp-wiseplat is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	cpp-wiseplat is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with cpp-wiseplat.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file Log.h
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 *
 * The logging subsystem.
 */

#pragma once

#include <ctime>
#include <chrono>
#include "vector_ref.h"
#include "Common.h"
#include "CommonData.h"
#include "FixedHash.h"
#include "Terminal.h"

namespace boost { namespace asio { namespace ip { template<class T>class basic_endpoint; class tcp; } } }

namespace dev
{

/// The null output stream. Used when logging is disabled.
class NullOutputStream
{
public:
	template <class T> NullOutputStream& operator<<(T const&) { return *this; }
};

/// A simple log-output function that prints log messages to stdout.
void simpleDebugOut(std::string const&);

/// The logging system's current verbosity.
extern int g_logVerbosity;
extern bool g_useColor;

class ThreadContext
{
public:
	ThreadContext(std::string const& _info) { push(_info); }
	~ThreadContext() { pop(); }

	static void push(std::string const& _n);
	static void pop();
	static std::string join(std::string const& _prior);
};

/// Set the current thread's log name.
void setThreadName(char const* _n);

/// Set the current thread's log name.
std::string getThreadName();

/// The default logging channels. Each has an associated verbosity and three-letter prefix (name() ).
/// Channels should inherit from LogChannel and define name() and verbosity.
struct LogChannel { static const char* name(); static const int verbosity = 1; static const bool debug = true; };
struct LeftChannel: public LogChannel { static const char* name(); };
struct RightChannel: public LogChannel { static const char* name(); };
struct WarnChannel: public LogChannel { static const char* name(); static const int verbosity = 0; static const bool debug = false; };
struct NoteChannel: public LogChannel { static const char* name(); static const bool debug = false; };
struct DebugChannel: public LogChannel { static const char* name(); static const int verbosity = 0; };

enum class LogTag
{
	None,
	Url,
	Error,
	Special
};

class LogOutputStreamBase
{
public:
	LogOutputStreamBase(char const* _id, std::type_info const* _info, unsigned _v, bool _autospacing);

	void comment(std::string const& _t)
	{
		switch (m_logTag)
		{
		case LogTag::Url: m_sstr << WshNavyUnder; break;
		case LogTag::Error: m_sstr << WshRedBold; break;
		case LogTag::Special: m_sstr << WshWhiteBold; break;
		default:;
		}
		m_sstr << _t << WshReset;
		m_logTag = LogTag::None;
	}

	void append(unsigned long _t) { m_sstr << WshWhite << _t << WshReset; }
	void append(long _t) { m_sstr << WshWhite << _t << WshReset; }
	void append(unsigned int _t) { m_sstr << WshWhite << _t << WshReset; }
	void append(int _t) { m_sstr << WshWhite << _t << WshReset; }
	void append(bigint const& _t) { m_sstr << WshNavy << _t << WshReset; }
	void append(u256 const& _t) { m_sstr << WshNavy << _t << WshReset; }
	void append(u160 const& _t) { m_sstr << WshNavy << _t << WshReset; }
	void append(double _t) { m_sstr << WshBlue << _t << WshReset; }
	template <unsigned N> void append(FixedHash<N> const& _t) { m_sstr << WshTeal "#" << _t.abridged() << WshReset; }
	void append(h160 const& _t) { m_sstr << WshRed "@" << _t.abridged() << WshReset; }
	void append(h256 const& _t) { m_sstr << WshWhite "#" << _t.abridged() << WshReset; }
	void append(h512 const& _t) { m_sstr << WshTeal "##" << _t.abridged() << WshReset; }
	void append(std::string const& _t) { m_sstr << WshGreen "\"" + _t + "\"" WshReset; }
	void append(bytes const& _t) { m_sstr << WshYellow "%" << toHex(_t) << WshReset; }
	void append(bytesConstRef _t) { m_sstr << WshYellow "%" << toHex(_t) << WshReset; }
	template <class T> void append(std::vector<T> const& _t)
	{
		m_sstr << WshWhite "[" WshReset;
		int n = 0;
		for (auto const& i: _t)
		{
			m_sstr << (n++ ? WshWhite ", " WshReset : "");
			append(i);
		}
		m_sstr << WshWhite "]" WshReset;
	}
	template <class T> void append(std::set<T> const& _t)
	{
		m_sstr << WshYellow "{" WshReset;
		int n = 0;
		for (auto const& i: _t)
		{
			m_sstr << (n++ ? WshYellow ", " WshReset : "");
			append(i);
		}
		m_sstr << WshYellow "}" WshReset;
	}
	template <class T, class U> void append(std::map<T, U> const& _t)
	{
		m_sstr << WshLime "{" WshReset;
		int n = 0;
		for (auto const& i: _t)
		{
			m_sstr << (n++ ? WshLime ", " WshReset : "");
			append(i.first);
			m_sstr << (n++ ? WshLime ": " WshReset : "");
			append(i.second);
		}
		m_sstr << WshLime "}" WshReset;
	}
	template <class T> void append(std::unordered_set<T> const& _t)
	{
		m_sstr << WshYellow "{" WshReset;
		int n = 0;
		for (auto const& i: _t)
		{
			m_sstr << (n++ ? WshYellow ", " WshReset : "");
			append(i);
		}
		m_sstr << WshYellow "}" WshReset;
	}
	template <class T, class U> void append(std::unordered_map<T, U> const& _t)
	{
		m_sstr << WshLime "{" WshReset;
		int n = 0;
		for (auto const& i: _t)
		{
			m_sstr << (n++ ? WshLime ", " WshReset : "");
			append(i.first);
			m_sstr << (n++ ? WshLime ": " WshReset : "");
			append(i.second);
		}
		m_sstr << WshLime "}" WshReset;
	}
	template <class T, class U> void append(std::pair<T, U> const& _t)
	{
		m_sstr << WshPurple "(" WshReset;
		append(_t.first);
		m_sstr << WshPurple ", " WshReset;
		append(_t.second);
		m_sstr << WshPurple ")" WshReset;
	}
	template <class T> void append(T const& _t)
	{
		m_sstr << toString(_t);
	}

protected:
	bool m_autospacing = false;
	unsigned m_verbosity = 0;
	std::stringstream m_sstr;	///< The accrued log entry.
	LogTag m_logTag = LogTag::None;
};

/// Logging class, iostream-like, that can be shifted to.
template <class Id, bool _AutoSpacing = true>
class LogOutputStream: LogOutputStreamBase
{
public:
	/// Construct a new object.
	/// If _term is true the the prefix info is terminated with a ']' character; if not it ends only with a '|' character.
	LogOutputStream(): LogOutputStreamBase(Id::name(), &typeid(Id), Id::verbosity, _AutoSpacing) {}

	/// Destructor. Posts the accrued log entry to the g_logPost function.
	~LogOutputStream() { if (Id::verbosity <= g_logVerbosity) simpleDebugOut(m_sstr.str()); }

	LogOutputStream& operator<<(std::string const& _t) { if (Id::verbosity <= g_logVerbosity) { if (_AutoSpacing && m_sstr.str().size() && m_sstr.str().back() != ' ') m_sstr << " "; comment(_t); } return *this; }

	LogOutputStream& operator<<(LogTag _t) { m_logTag = _t; return *this; }

	/// Shift arbitrary data to the log. Spaces will be added between items as required.
	template <class T> LogOutputStream& operator<<(T const& _t) { if (Id::verbosity <= g_logVerbosity) { if (_AutoSpacing && m_sstr.str().size() && m_sstr.str().back() != ' ') m_sstr << " "; append(_t); } return *this; }
};

// Kill all logs when when NLOG is defined.
#if NLOG
#define clog(X) nlog(X)
#define cslog(X) nslog(X)
#else
#if NDEBUG
#define clog(X) if (X::debug) {} else dev::LogOutputStream<X, true>()
#define cslog(X) if (X::debug) {} else dev::LogOutputStream<X, false>()
#else
#define clog(X) dev::LogOutputStream<X, true>()
#define cslog(X) dev::LogOutputStream<X, false>()
#endif
#endif

// Simple cout-like stream objects for accessing common log channels.
// Dirties the global namespace, but oh so convenient...
#define cdebug clog(dev::DebugChannel)
#define cnote clog(dev::NoteChannel)
#define cwarn clog(dev::WarnChannel)

// Null stream-like objects.
#define ndebug if (true) {} else dev::NullOutputStream()
#define nlog(X) if (true) {} else dev::NullOutputStream()
#define nslog(X) if (true) {} else dev::NullOutputStream()

}
