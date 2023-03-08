/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yoelhaim <yoelhaim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/24 19:33:13 by yoelhaim          #+#    #+#             */
/*   Updated: 2023/03/08 16:37:48 by yoelhaim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

//    <--------- function trim espace --------->
string ft_trim(string str)
{
    int lengthReverse = str.length();
    int i = 0;
    for (; str[i] && (str[i] == 32 || str[i] == '\t'); i++)
    {
    }
    for (; str[lengthReverse] && (str[lengthReverse] == 32 || str[lengthReverse] == '\t'); lengthReverse++)
    {
    }

    return (str.substr(i, lengthReverse));
}
string searchCommentInLine(string line)
{
    size_t i = 0;
    string comment = "";

    int s = line.find('#');
    return line.substr(0, s);
}

//    <--------- fin function trim espace --------->

/*****************************************
 **** start Constructor& destroctur*******
 *****************************************/
ConfigParser::ConfigParser(){}

ConfigParser::ConfigParser(string content)
{
    initalConfig(content);
}

void ConfigParser::initalConfig(string content)
{
    this->_lenght_server = 0;
    if (content.empty())
        errorLogs("Error: file is empty");
    _index = 0;
    vector<string> conf;
    conf = Request::getVector(content, '\n');

    for (size_t index = 0; index < conf.size(); index++)
        if (conf[index] != "\n" && ft_trim(conf[index]).length() != 0)
            _lines.append(searchCommentInLine(ft_trim(conf[index]))).append("\n");

    parseLine(_lines);
    tokenez();
    synaxError();
    Configuration initail(this->_tokens, _lenght_server);
}

ConfigParser::~ConfigParser()
{
}

ConfigParser::ConfigParser(const ConfigParser &src)
{
    *this = src;
}

ConfigParser &ConfigParser::operator=(const ConfigParser &src)
{
    if (this != &src)
    {
        this->_lines = src._lines;
        this->_index = src._index;
        this->_lenght_server = src._lenght_server;
        this->_data = src._data;
        this->_tokens = src._tokens;
    }
    return *this;
}

/*****************************************
 ****** end Constructor& destroctur ******
 *****************************************/

void ConfigParser::errorLogs(string titleError)
{
    cout << titleError << endl;
    exit(1);
}

bool checkLineIsComment(string line)
{
    size_t i = 0;

    for (; (line[i] == ' ' || line[i] == '\t') && line[i]; i++)
    {
    }
    if (line[i] == '#' || !strlen(line.c_str() + i))
        return true;
    return false;
}

// ?? push context yo tokonez

void ConfigParser::pushTokinez(int currentIndex, string context)
{
    int word;
    if (currentIndex != _index)
    {
        vector<string> ctx = Request::getVector(context);
        for (size_t i = 0; i < ctx.size(); i++)
        {
            i == 0 ? word = CONTEXT : word = WORD;
            _tokens.push_back(make_pair(ctx[i], word));
        }
    }
}

void ConfigParser::pushSemiCurly(string type)
{
    int word;
    type == "}" ? word = CLOSE_CURLY : word = NEWLINE;
    _tokens.push_back(make_pair(type, word));
}

void ConfigParser::checkContext()
{
    string context;
    string tmp;
    size_t currentIndex = this->_index;

    while ((_data[_index] != "{") && _index < _data.size())
    {

        tmp.append(_data[_index]).append(" ");
        if (_data[_index] == "\n" || _data[_index] == "}")
        {
            checkDirectives(tmp);
            pushSemiCurly(_data[_index]);
            tmp.clear();
            context.clear();
        }
        else
            context.append(_data[_index]).append(" ");
        _index++;
    }
    pushTokinez(currentIndex, context);
}

void ConfigParser::pushDirective(string directive)
{
    int word;
    vector<string> direct = Request::getVector(directive);
    for (size_t i = 0; i < direct.size(); i++)
    {
        i == 0 ? word = DIRECTIVE : direct[i] != ";" ? word = WORD
                                                     : word = SEMI_COLON;
        _tokens.push_back(make_pair(direct[i], word));
    }
}

void ConfigParser::checkDirectives(string data)
{
    string directive;
    size_t i = 0;
    for (; i < data.size() && (data[i] != '\n'); i++)
    {
        if (data[i] == '}')
        {
            i++;
            continue;
        }
        directive.append(data, i, 1);
    }

    if (directive.size() != 0)
        pushDirective(directive);
}

void ConfigParser::tokenez()
{

    for (_index = 0; _index < _data.size(); _index++)
    {
        checkContext();
        if (_data[_index] == "{")
            _tokens.push_back(make_pair("{", OPEN_CURLY));
    }
}

void ConfigParser::checkPunctuation(string line)
{
    if (line[_index] == ';')
    {
        _data.push_back(";");
        _index++;
    }
}

void ConfigParser::checkOpenCurly(string line)
{
    if (line[_index] == '{')
    {
        _data.push_back("{");
        _index++;
    }
}

void ConfigParser::checkCloseCurly(string line)
{

    if (line[_index] == '}')
    {
        _data.push_back("}");
        _index++;
    }
}

void ConfigParser::skipSpaces(string line)
{
    while (line[_index] && (line[_index] == ' ' || line[_index] == '\t'))
        _index++;
}

void ConfigParser::checkWord(string line)
{
    int currentIndex = _index;
    string word;

    while (line[_index] && !strchr(" \t{};\n", line[_index]))
    {
        word.append(1, line[_index]);
        _index++;
    }

    if (currentIndex != _index)
        this->_data.push_back(word);
}
void ConfigParser::checkNewLine(string line)
{
    if (line[_index] == '\n')
    {
        _data.push_back("\n");
        _index++;
    }
}
void ConfigParser::parseLine(string line)
{

    while (line[_index])
    {
        skipSpaces(line);
        checkWord(line);
        checkOpenCurly(line);
        checkCloseCurly(line);
        checkPunctuation(line);
        checkNewLine(line);
    }
}
/*****************************************
 ******** start syntax Error *************
 *****************************************/
void ConfigParser::checkSynatxCurly()
{
    size_t countCurly = 0;

    for (size_t i = 0; i < _tokens.size(); i++)
    {
        if (_tokens[i].second == OPEN_CURLY && _tokens[i - 1].second != WORD)
        {
            if (_tokens[i - 1].second != CONTEXT || _tokens[i].second == WORD)
                errorLogs("ERROR File");
        }
        if (_tokens[i].second == OPEN_CURLY || _tokens[i].second == CLOSE_CURLY)
            countCurly++;
    }
    // if (_tokens[_tokens.size() - 2].second != CLOSE_CURLY)
    // {
    //     errorLogs("Error find closed curly");

    // }
    if (countCurly % 2 != 0)
        errorLogs("Error find closed curlys");
}

void ConfigParser::checkSyntaxMain()
{

    if (_tokens[0].first != "server" || _tokens[1].second != OPEN_CURLY || _tokens[2].second == OPEN_CURLY)
        errorLogs("error find server");
}
void ConfigParser::checkSyntackInServer(int currentIndex, int index)
{
    while (currentIndex < index)
    {
        if (_tokens[currentIndex].second == CONTEXT && _tokens[currentIndex].first == "server")
            errorLogs("error find server");
        currentIndex++;
    }
}
void ConfigParser::lenghtServer(int index)
{
    int breackts = 0;
    int currentIndex = index;

    while (++index < _tokens.size())
    {
        if (_tokens[index].second == OPEN_CURLY)
            breackts++;
        if (_tokens[index].second == CLOSE_CURLY)
            breackts--;
        if (breackts == 0)
        {
            this->_lenght_server++;
            break;
        }
        checkSyntackInServer(currentIndex + 1, index);
    }
    while (index < _tokens.size() && _tokens[index].second != OPEN_CURLY)
    {
        if (_tokens[index].second == CONTEXT && _tokens[index].first != "server")
            errorLogs("error file");
        index++;
    }
}

void ConfigParser::checkSyntaxContext()
{
    for (size_t i = 0; i < _tokens.size(); i++)
    {
        if (_tokens[i].second == NEWLINE)
            continue;

        if (_tokens[i].second == CONTEXT && _tokens[i + 1].second == OPEN_CURLY)
        {
            lenghtServer(i);
            if (_tokens[i].first != "server")
                errorLogs("error server synatx");
        }
        if (_tokens[i].second == CONTEXT && _tokens[i + 1].second != OPEN_CURLY)
        {
            if (_tokens[i].first != "location" || (_tokens[i + 1].second != WORD || _tokens[i + 2].second != OPEN_CURLY))
                errorLogs("error Location synatx");
        }
    }
}

bool ConfigParser::checkSyntaxDirectiveCondition(size_t index)
{
    int i = 0;

    for (; (_tokens[index].second != SEMI_COLON) && index < _tokens.size(); index++, i++)
    {
    }
    if (_tokens[index].second != SEMI_COLON)
        errorLogs("error word");
    return true;
}
size_t ConfigParser::lengthDirective(size_t index)
{
    size_t i = 0;
    for (; _tokens[index].second != SEMI_COLON && index < _tokens.size(); index++, i++)
    {
    }
    return i;
}

void ConfigParser::checkSyntaxMethod(size_t index)
{
    string methods[3] = {"GET", "POST", "DELETE"};
    int length = 0;

    while (index < _tokens.size() && _tokens[index].second != SEMI_COLON)
    {
        int j = 0;

        while (j < 3)
        {
            if (_tokens[index].first == methods[j])
            {
                length += methods[j].length();
                methods[j] == "DELETE" ? length += 1 : length += 0;
                break;
            }
            j++;
        }

        if (j == 3)
            errorLogs("error method");
        index++;
    }
    if (length != 14 && length != 10 && length != 3 && length != 7 && length != 4)
        errorLogs("error method length");
}

void ConfigParser::checkSynaxDirective()
{
    int i = 0;

    while (i < _tokens.size())
    {
        if (_tokens[i].second == DIRECTIVE)
        {
            if ((_tokens[i].first == "error_page" || _tokens[i].first == "return") && lengthDirective(i + 1) != 2)
                errorLogs("error error_page");
            if (_tokens[i].first == "allow" && lengthDirective(i + 1) > 3)
                errorLogs("error allow");
            else if (_tokens[i].first == "allow")
                checkSyntaxMethod(i + 1);
            if (_tokens[i].first != "error_page" && _tokens[i].first != "index" && _tokens[i].first != "return" && _tokens[i].first != "allow")
            {
                if (lengthDirective(i + 1) != 1)
                    errorLogs("error directive length");
            }
        }
        i++;
    }
}

void ConfigParser::checkSyntaxCemiColom()
{
    bool checkCemiColom = true;

    for (size_t i = 0; i < _tokens.size(); i++)
    {
        if (_tokens[i].second == DIRECTIVE)
        {
            while (_tokens[i].second != NEWLINE && i < _tokens.size())
                i++;
            if (_tokens[i - 1].second != SEMI_COLON)
                errorLogs("error cemi colom");
            else
            {
                if (_tokens[i - 2].second == SEMI_COLON)
                    errorLogs("error cemi colom");
            }
        }
    }
}

void ConfigParser::checkSyntaxAfterCemiColom()
{
    int i = 0;

    while (i < _tokens.size())
    {
        if (_tokens[i].second == SEMI_COLON)
        {
            if (_tokens[i + 1].second != NEWLINE)
                errorLogs("error cemi colom");
        }
        i++;
    }
}

void ConfigParser::checkSyntaxDirective()
{
    string myDirective[10] = {"server_name", "listen", "allow", "autoindex", "index", "error_page", "return", "host", "root", "cli_max_size"};
    for (size_t i = 0; i < _tokens.size(); i++)
    {
        if (_tokens[i].second == DIRECTIVE)
        {
            checkSyntaxDirectiveCondition(i);
            size_t j = 0;
            for (; j < 10; j++)
                if (myDirective[j] == _tokens[i].first)
                    break;
            if (j == 10)
                errorLogs("error variable  " + _tokens[i].first);
        }
    }
}

void ConfigParser::checkSyntaxDiplicatedLocation(size_t index, map<string, bool> &directiveLocation)
{
    bool check = false;
    string forbidenDirectiveLocation[3] = {"server_name", "listen", "host"};
    
    while (index < _tokens.size() && _tokens[index].second != CLOSE_CURLY)
    {
        if (_tokens[index].second == DIRECTIVE)
        {
            if (directiveLocation.find(_tokens[index].first) != directiveLocation.end())
                errorLogs("error directive location");
            else
                directiveLocation.insert(make_pair(_tokens[index].first, check));
            for (size_t i = 0; i < 3; i++)
            {
                if (_tokens[index].first == forbidenDirectiveLocation[i])
                    errorLogs("error directive location  " + _tokens[index].first);
            }
        }
        index++;
    }
    directiveLocation.clear();

    // check = (directive.find(_tokens[index].first) != directive.end());
    // directive.insert(make_pair(_tokens[index].first, check));
}

void ConfigParser::checkSyntaxDiplicated()
{
    size_t index = 1;
    map<string, bool> m;
    map<string, bool> directiveLocation;

    for (size_t i = 0; i < _lenght_server; i++)
    {
        bool cxt = false;

        while (index < _tokens.size())
        {
            if (_tokens[index].first == "server")
            {
                index++;
                break;
            }
            if (_tokens[index].second == CONTEXT && _tokens[index + 1].second == WORD)
            {
                checkSyntaxDiplicatedLocation(index + 1, directiveLocation);
                cxt = true;
            }
            if (_tokens[index].second == DIRECTIVE && !cxt)
            {
                if (m.find(_tokens[index].first) != m.end())
                {
                    m[_tokens[index].first] = true;
                    break;
                }
                m.insert(make_pair(_tokens[index].first, false));
            }
            if (_tokens[index].second == CLOSE_CURLY)
                cxt = false;
            index++;
        }
        for (map<string, bool>::iterator it = m.begin(); it != m.end(); it++)
    {
        if (it->second)
            errorLogs("error diplicated " + it->first);
    }
    m.clear();
    }

    
}

void ConfigParser::synaxError()
{
    checkSynatxCurly();
    checkSyntaxMain();
    checkSyntaxContext();
    checkSyntaxDirective();
    checkSyntaxCemiColom();
    checkSynaxDirective();
    checkSyntaxAfterCemiColom();
    checkSyntaxDiplicated();
}

/*****************************************
 ******** end syntax Error ***************
 *****************************************/

void ConfigParser::printData()
{

    // for (size_t i = 0; i < _tokens.size(); i++)
    // {
    //     cout << _tokens[i].first << " =>" << _tokens[i].second << endl;
    // }
}