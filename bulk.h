#pragma once

#include <list>
#include <string>
#include <ctime>
#include <memory>

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>


/*!
 * \brief ImpBulk интерфейс для бульков
 */
class ImpBulk
{
protected:
    std::list<std::string> _commands;   //!< Контейнер для хранения бульков
    time_t _beginTime;                  //!< Время записи первого элемента

public:
    virtual ~ImpBulk() {}

    /*!
     * \brief appendCmd Добаляет команду и сбрасывает таймер, если команда первая
     * \param newCmd Новая команда
     */
    virtual void appendCmd(const std::string &newCmd);

    /*!
     * \brief clear Очищает список команд
     */
    virtual void clear();

    /*!
     * \brief logCommands Записывает бульку в файл
     * \return количество записанных команд
     */
    virtual int logCommands() const = 0;

    /*!
     * \brief printCommands Выводит бульку на экран
     * \return количество выведенных команд
     */
    virtual int printCommands() const = 0;

    /*!
     * \brief isEmpty Проверяет есть ли команды в бульке
     * \return
     */
    virtual bool isEmpty() const {return _commands.empty();}

};

/*!
 * \brief Bulk класс реализации бульки
 */
class Bulk : public ImpBulk
{
public:
    int logCommands() const override;
    int printCommands() const override;
};

/*!
 * \brief ImpWorker интерфейс для выполнения работы.
 */
struct ImpWorker
{
    virtual ~ImpWorker();
    virtual void operator ()(ImpBulk &bulk) = 0;
};

struct Worker : public ImpWorker
{
    void operator ()(ImpBulk &bulk) override;
};

struct MTWorker : public ImpWorker
{
private:
    std::queue<std::thread> _printQueue;
    std::queue<std::thread> _logQueue;
    std::condition_variable _cv;
    std::mutex              _cvMutex;

public:
    void operator ()(ImpBulk &bulk) override;
};

/*!
 * \brief The BulkController
 * класс содержащий логику работы с бульками и логику заполнения булек
 */
class BulkController
{
    ImpBulk    &_bulk;
    ImpWorker  &_worker;
    size_t      _commandsCount;
    size_t      _currentNumber;
    size_t      _stackSize;
    std::string _signShiftUp;
    std::string _signShiftDown;

    size_t      _bulksCounter;

public:
    /*!
     * \brief BulkController конструктор
     * \param bulk указатель на бульку
     * \param commandsCount колличество команд в бульке
     */
    BulkController(size_t commandsCount, ImpBulk &bulk, ImpWorker &worker);

    /*!
     * \brief addString добавляет строку, если булька заполнена, выполняет соответствующие действия
     * \param str новая строка
     */
    void addString(const std::string &str);

    /*!
     * \brief flush выводит и очищает буффер
     * \param printInternalBlock если true будет напечатан незавершённый блок
     * не завершённые круппы команд вне блока выводятся всегда
     */
    void flush(bool printInternalBlock = false);

    /*!
     * \brief signShiftUp возвращает подпись начала блока
     * \return сторока обозначающая начало блока
     */
    std::string signShiftUp() const;

    /*!
     * \brief signShiftDown возвращает подпись конца блока
     * \return сторока обозначающая конец блока
     */
    std::string signShiftDown() const;

    /*!
     * \brief setSignShiftUp метод установки подписи начала блока
     * \param signShiftUp сторока обозначающая начало блока
     */
    void setSignShiftUp(const std::string &signShiftUp);

    /*!
     * \brief setSignShiftDown метод установки подписи конца блока
     * \param signShiftDown сторока обозначающая конец блока
     */
    void setSignShiftDown(const std::string &signShiftDown);
    size_t bulksCounter() const;
};
