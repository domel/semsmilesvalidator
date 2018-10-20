#include <stdexcept>
#include <string>
#include <vector>
#include <list>
#include <sstream>

namespace nt
{
class Error : public std::runtime_error
{

public:
    Error(const std::string &msg):
        std::runtime_error(std::string("NT parser : ").append(msg))
    {
    }
};

class Line
{
public:
    Line(const std::vector<std::string> &);
    ~Line(void);

public:
    unsigned int size(void) const;
    void push(const std::string &);
    bool set(const std::string &, const std::string &);

private:
    const std::vector<std::string> _header;
    std::vector<std::string> _values;

public:

    template<typename T>
    const T getValue(unsigned int pos) const
    {
        if (pos < _values.size())
        {
            T res;
            std::stringstream ss;
            ss << _values[pos];
            ss >> res;
            return res;
        }
        throw Error("can't return this value (doesn't exist)");
    }
    const std::string operator[](unsigned int) const;
    const std::string operator[](const std::string &valueName) const;
    friend std::ostream& operator<<(std::ostream& os, const Line &line);
    friend std::ofstream& operator<<(std::ofstream& os, const Line &line);
};

enum DataType {
    eFILE = 0,
    ePURE = 1
};

class Parser
{

public:
    Parser(const std::string &, const DataType &type = eFILE, char sep = ' ');
    ~Parser(void);

public:
    Line &getLine(unsigned int line) const;
    unsigned int lineCount(void) const;
    unsigned int columnCount(void) const;
    const std::string &getFileName(void) const;

protected:
    void parseContent(void);

private:
    std::string _file;
    const DataType _type;
    const char _sep;
    std::vector<std::string> _originalFile;
    std::vector<std::string> _header;
    std::vector<Line *> _content;

public:
    Line &operator[](unsigned int line) const;
};
}

