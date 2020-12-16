#ifndef logger_h_
#define logger_h_

#include <list>
#include <tickable.h>

class LoggerClass : public Tickable {
public:
  virtual ~LoggerClass(){};
  virtual bool printf(const char *format, ...)
      __attribute__((format(printf, 2, 3))) = 0;
  virtual bool print(const char *) = 0;
  virtual bool print(unsigned int, int = DEC) = 0;
  virtual bool println(const char *) = 0;
  virtual bool println(char) = 0;
  virtual bool println(unsigned int, int = DEC) = 0;
  virtual bool println(const Printable &) = 0;
  virtual bool println(void) = 0;
};

class SerialLogger : public LoggerClass {
public:
  virtual ~SerialLogger();
  void setUp();
  void tick();
  bool printf(const char *format, ...) __attribute__((format(printf, 2, 3)));
  bool print(const char *);
  bool print(int, int = DEC);
  bool print(unsigned int, int = DEC);
  bool println(const char *);
  bool println(unsigned int, int = DEC);
  bool println(char);
  bool println(const Printable &);
  bool println(void);
};

class ActualLogger : public LoggerClass {
public:
  virtual ~ActualLogger();
  void setUp();
  void tick();
  bool printf(const char *format, ...) __attribute__((format(printf, 2, 3)));
  bool print(const char *);
  bool print(unsigned int, int = DEC);
  bool println(const char *);
  bool println(char);
  bool println(unsigned int, int = DEC);
  bool println(const Printable &);
  bool println(void);

  void registerLogger(LoggerClass *logger);

private:
  std::list<LoggerClass *> innerLoggers;
};

extern ActualLogger Logger;

#endif
