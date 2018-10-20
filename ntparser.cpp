#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace nt {

Parser::Parser(const std::string &data, const DataType &type, char sep) : _type(type), _sep(sep) {
    std::string line;
    if (type == eFILE) {
        _file = data;
        std::ifstream ifile(_file.c_str());
        if (ifile.is_open()) {
            while (ifile.good()) {
                getline(ifile, line);
                if (line != "")
                    _originalFile.push_back(line);
            }
            ifile.close();

            if (_originalFile.size() == 0)
                throw Error(std::string("No Data in ").append(_file));

            parseContent();
        } else
            throw Error(std::string("Failed to open ").append(_file));
    } else {
        std::istringstream stream(data);
        while (std::getline(stream, line))
            if (line != "")
                _originalFile.push_back(line);
        if (_originalFile.size() == 0)
            throw Error(std::string("No Data in pure content"));

        parseContent();
    }
}

Parser::~Parser(void) {
    std::vector<Line *>::iterator it;

    for (it = _content.begin(); it != _content.end(); it++)
        delete *it;
}

void Parser::parseContent(void) {
    std::vector<std::string>::iterator it;

    it = _originalFile.begin();

    for (; it != _originalFile.end(); it++) {
        bool quoted = false;
        int tokenStart = 0;
        unsigned int i = 0;

        Line *line = new Line(_header);

        for (; i != it->length(); i++) {
            if (it->at(i) == '"')
                quoted = ((quoted) ? (false) : (true));
            else if (it->at(i) == ' ' && !quoted) {
                line->push(it->substr(tokenStart, i - tokenStart));
                tokenStart = i + 1;
            }
        }

        line->push(it->substr(tokenStart, it->length() - tokenStart));

        _content.push_back(line);
    }
}

Line &Parser::getLine(unsigned int linePosition) const {
    if (linePosition < _content.size())
        return *(_content[linePosition]);
    throw Error("can't return this line (doesn't exist)");
}

Line &Parser::operator[](unsigned int linePosition) const {
    return Parser::getLine(linePosition);
}

unsigned int Parser::lineCount(void) const {
    return _content.size();
}

const std::string &Parser::getFileName(void) const {
    return _file;
}

Line::Line(const std::vector<std::string> &header) : _header(header) {}

Line::~Line(void) {}

unsigned int Line::size(void) const {
    return _values.size();
}

void Line::push(const std::string &value) {
    _values.push_back(value);
}

const std::string Line::operator[](unsigned int valuePosition) const {
    if (valuePosition < _values.size())
        return _values[valuePosition];
    throw Error("can't return this value (doesn't exist)");
}
}

bool replace(std::string &str, const std::string &from, const std::string &to) {
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

int main(int argc, char **argv) {
    try {
        nt::Parser file = nt::Parser("files/readme.nt");
        int c = (int)file.lineCount();

        for (int i = 0; i < c; i++) {
            std::string p = file[i][1];
            if (p == "<http://www.w3.org/2004/02/skos/core#notation>") {
                std::string string(file[i][2]);
                replace(string, "^^<http://chemskos.com/ns#SMILES>", "");
                replace(string, "\"", "");
                replace(string, "\"", "");
                std::cout << string << std::endl;
            }
        }
    } catch (nt::Error &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
