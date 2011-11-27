/*
 * Logger.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Für Informationen in einer Log-Datei zu speichern

#ifndef LOGGER_H
#define LOGGER_H

#include <list>
#include <string>
#include <fstream>
#include <ostream>
#include <sstream>
#include <boost/shared_ptr.hpp>

// forward declare ptree (Boost PropertyTree), quite complex but we don't want to include the big header here
namespace boost { namespace property_tree {
    template<class Key, class Data, class KeyCompare> class basic_ptree;
    typedef basic_ptree<std::string, std::string, std::less<std::string> > ptree;
}}

class Logger;

enum LogLevel { Fatal, Error, Warning, Info, Detail, Debug, Off };

Logger& log(LogLevel level);

/*
 * Prop tree structure:
 *
 * LogConsoleLevel LOGLEVEL
 * LogFileLevel LOGLEVEL
 * LogFileName FILENAME
 *
 */
void setUpLoggerFromPropTree(boost::property_tree::ptree&);

class LogHandler;

/*
    Diese Klasse kann Texte in einer Log-Datei speichern.
*/

class Logger : public std::ostream
{
public:
	Logger(bool buffered=false);
    void addHandler(boost::shared_ptr<LogHandler> handler);
    void setLevel(LogLevel level);
    void writeHeader(const std::string& str); // writes a formatted header containing str and the current time

private:

    std::list<boost::shared_ptr<LogHandler> > m_handlers;

    class LoggerStringBuf : public std::stringbuf {
	public:
    	LoggerStringBuf(const Logger& logger);
		int sync();
	private:
		const Logger& m_logger;
	};
    LoggerStringBuf m_stringBuf;
    LogLevel m_logLevel;
};

class LogHandler {
public:
	LogHandler();
	virtual ~LogHandler() {}
	void setLevel(LogLevel);
	void writeFilter(LogLevel, const std::string&);
protected:
	virtual void write(const std::string&) = 0;
private:
	LogLevel m_filterLevel;
};

class OstreamHandler : public LogHandler {
public:
	OstreamHandler(std::ostream& ostream);
	void write(const std::string&);
private:
	std::ostream& m_ostream;
};

class ConsoleHandler : public OstreamHandler {
public:
	ConsoleHandler();
};

class FileHandler : public OstreamHandler {
public:
	FileHandler(const std::string& fileName);
private:
	std::ofstream m_ofstream;
};

#endif
