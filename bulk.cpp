#include "bulk.h"

#include <iostream>
#include <fstream>

#include <stdexcept>

static std::mutex logFileMutex;

inline bool fexists(const std::string& filename) {
  std::ifstream ifile(filename.c_str());
  return ifile.operator bool();
}

int Bulk::logCommands() const
{
    int fileCounter = 0;
    int comandsCounter = 0;

    std::string fileName;

    logFileMutex.lock();
    do {
        char buffer[256];

        sprintf(buffer, "bulk_%ld_%.4d.log", _beginTime, ++fileCounter);

        fileName = buffer;
    }
    while(fexists(fileName));

    std::ofstream logFile;

    logFile.open(fileName, std::ios_base::app);

    logFileMutex.unlock();

    logFile << "bulk: ";

    for (auto cmd : _commands) {
        ++comandsCounter;
        logFile << cmd << " ";
    }

    logFile << std::endl;

    logFile.close();

    return comandsCounter;
}

int Bulk::printCommands() const
{
    int comandsCounter = 0;
    std::cout << "bulk: ";

    for (auto cmd : _commands) {
        ++comandsCounter;
        std::cout << cmd << " ";
    }

    std::cout << std::endl;

    return comandsCounter;
}

void ImpBulk::appendCmd(const std::string &newCmd)
{
    if (isEmpty())
    {
        _beginTime = time(nullptr);
    }

    _commands.push_back(newCmd);
}

void ImpBulk::clear()
{
    _commands.clear();
}

std::string BulkController::signShiftDown() const
{
    return _signShiftDown;
}

void BulkController::setSignShiftDown(const std::string &signShiftDown)
{
    _signShiftDown = signShiftDown;
}

std::string BulkController::signShiftUp() const
{
    return _signShiftUp;
}

void BulkController::setSignShiftUp(const std::string &signShiftUp)
{
    _signShiftUp = signShiftUp;
}

size_t BulkController::bulksCounter() const
{
    return _bulksCounter;
}

BulkController::BulkController(size_t commandsCount, ImpBulk &bulk, ImpWorker &worker)
    : _bulk(bulk), _worker(worker),
      _commandsCount(commandsCount), _currentNumber(0),
      _stackSize(0), _signShiftUp("{"), _signShiftDown("}"),
      _bulksCounter(0)
{}

void BulkController::addString(const std::string &str)
{
    if (str == _signShiftUp)
    {
        if (_stackSize == 0)
        {
            _worker(_bulk);
        }

        ++_stackSize;
    }
    else if (str == _signShiftDown)
    {
        if (_stackSize == 0)
        {
            throw std::out_of_range("stack breakdown below");
        }

        --_stackSize;

        if (_stackSize == 0)
        {
            _worker(_bulk);
            _currentNumber = 0;
        }
    }
    else
    {
        _bulk.appendCmd(str);

        ++_currentNumber;

        if (_stackSize == 0 && _currentNumber == _commandsCount)
        {
            _currentNumber = 0;
            _worker(_bulk);
            ++_bulksCounter;
        }
    }
}

void BulkController::flush(bool printInternalBlock)
{
    if (!printInternalBlock && _stackSize > 0)
    {
        return;
    }

    _worker(_bulk);
}


void Worker::operator ()(ImpBulk &bulk)
{
    bulk.printCommands();
    bulk.logCommands();
    bulk.clear();
}

ImpWorker::~ImpWorker() {}

void MTWorker::operator ()(ImpBulk &bulk)
{

}
