/*
 * Logger.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "Logger.h"

#include "common/Foreach.h"

#include <ctime>
#include <iostream>
#include <boost/make_shared.hpp>
#include <boost/property_tree/ptree.hpp>

namespace {
LogLevel strToLogLevel(std::string str)
{
	if (str == "Fatal") return Fatal;
	if (str == "Warning") return Warning;
	if (str == "Info") return Info;
	if (str == "Detail") return Detail;
	if (str == "Debug") return Debug;
	if (str == "Off") return Off;
	return Off;
}

std::string getCurrentTimeStr()
{
	time_t t = time(NULL);
	return ctime(&t);
}
} // end namespace

Logger& log(LogLevel level)
{
	static Logger log;
	log.setLevel(level);
	return log;
}

void setUpLoggerFromPropTree(boost::property_tree::ptree& ptree)
{
	LogLevel consoleLevel = strToLogLevel(ptree.get<std::string>("LogConsoleLevel"));
	LogLevel fileLevel    = strToLogLevel(ptree.get<std::string>("LogFileLevel"));
	std::string fileName  = ptree.get<std::string>("LogFileName");

	if (consoleLevel != Off)
	{
		boost::shared_ptr<ConsoleHandler> console = boost::make_shared<ConsoleHandler>();
		log(consoleLevel).addHandler(console);
	}
	if (fileLevel != Off)
	{
		boost::shared_ptr<FileHandler> file = boost::make_shared<FileHandler>(fileName);
		log(fileLevel).addHandler(file);
	}
}

Logger::Logger(bool buffered)
: std::ostream (&m_stringBuf),
  m_stringBuf (*this)
{
	if (!buffered)
		setf(std::ios::unitbuf); // set to unbuffered
}

void Logger::addHandler(boost::shared_ptr<LogHandler> handler)
{
	handler->setLevel(m_logLevel);
	m_handlers.push_back(handler);
}

void Logger::setLevel(LogLevel level)
{
	m_logLevel = level;
}

void Logger::writeHeader(const std::string& str)
{
    *this << "*** " << str << " ***" << "\nStart: " << getCurrentTimeStr() << "-------------------------------\n\n";
}

Logger::LoggerStringBuf::LoggerStringBuf(const Logger& logger)
: m_logger (logger)
{}

// synchronize output
int Logger::LoggerStringBuf::sync()
{
	if (!str().empty())
	{
		foreach(boost::shared_ptr<LogHandler> handler, m_logger.m_handlers)
		{
			handler->writeFilter(m_logger.m_logLevel, str());
		}
		str("");
	}
	return 0;
}

LogHandler::LogHandler()
: m_filterLevel (Error)
{}

void LogHandler::setLevel(LogLevel level)
{
	m_filterLevel = level;
}

void LogHandler::writeFilter(LogLevel level, const std::string& str)
{
	if (level <= m_filterLevel)
		write(str);
}

OstreamHandler::OstreamHandler(std::ostream& ostream)
: m_ostream (ostream)
{}

void OstreamHandler::write(const std::string& str)
{
	m_ostream << str;
	m_ostream.flush();
}

ConsoleHandler::ConsoleHandler()
: OstreamHandler(std::cout)
{}

FileHandler::FileHandler(const std::string& fileName)
: OstreamHandler (m_ofstream),
  m_ofstream (fileName.c_str())
{}
