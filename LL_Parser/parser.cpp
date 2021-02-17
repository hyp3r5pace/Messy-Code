#include <bits/stdc++.h>
using namespace std;

/* Array to store non terminal characters present in the given grammar which are represented by uppercase roman alphabets*/
unordered_map<string, int> nonTerminal;

/* unordered map (hashmap) to store the list of terminal characters*/
unordered_map<string, int> terminal;

/* hashmap to store the production rules
where head of production rules (non terminal characters) act as the key, and the remaining production body is the value*/
map<string, vector<string>> production;

/* parsing table */
map<string, map<string, string>> parsingTable;

/* hashmap to store the first of non terminal characters */
unordered_map<string, vector<string>> first;

/* hashmap to store the follow of non terminal characters */
unordered_map<string, vector<string>> follow;

/* variable to store the starting non terminal*/
string startingNonTerminal;

int num_production;

vector<string> First(string s);
vector<string> Follow(string s);

/*------------------------------
-------------DEBUG--------------
------------------------------*/
bool debug = false;

void readFromFile()
{
    ifstream fin;
    fin.open("grammar.txt");
    string line;
    int counter = 0;
    while (getline(fin, line) && ++counter)
    {
        if (counter == 1)
        {
            line.erase(remove(line.begin(), line.end(), '\r'), line.end());
            stringstream check1(line);
            string temp;
            while (getline(check1, temp, ' '))
            {
                terminal.insert({temp, 1});
            }
        }
        else
        {
            if (line.length() != 1)
            {
                int index = line.find_first_of(' ');
                if (index == -1)
                {
                    cout << "ERROR: Wrong format of grammar\n";
                    exit(0);
                }
                string head = line.substr(0, index);
                if (counter == 3)
                {
                    startingNonTerminal = head;
                }
                // updating the nonTerminal array
                nonTerminal.insert({head, 1});
                vector<string> rhs_production;
                int i = line.find_first_of(' ', index + 1) + 1;
                int v = i;
                for (; i < line.length(); i++)
                {
                    if (line[i] == '|')
                    {
                        num_production++;
                        rhs_production.push_back(line.substr(v, i - v));
                        v = i + 1;
                    }
                }
                num_production++;
                rhs_production.push_back(line.substr(v, i - v));
                if (production.find(head) == production.end())
                {
                    production.insert({head, rhs_production});
                }
                else
                {
                    production[head].insert(production[head].end(), rhs_production.begin(), rhs_production.end());
                }
            }
        }
    }
    fin.close();
}

void beautifyProduction()
{
    for (auto itr = production.begin(); itr != production.end(); itr++)
    {
        int len = itr->second.size();
        for (int i = 0; i < len; i++)
        {
            itr->second[i].erase(remove(itr->second[i].begin(), itr->second[i].end(), '\r'), itr->second[i].end());
            itr->second[i].erase(remove(itr->second[i].begin(), itr->second[i].end(), '\n'), itr->second[i].end());
        }
    }
}

/* function to check for left recursion in the production rules */
void eliminateLeftRecursion()
{
    /* loop to check for direct left recursion */
    for (auto itr = production.begin(); itr != production.end(); itr++)
    {
        int flag = 0;
        int len = itr->second.size();
        for (int i = 0; i < len; i++)
        {
            if (itr->first[0] == itr->second[i][0])
            {
                cout << "Left recursion in production rule: " << itr->first << " --> " << itr->second[i] << "\n";
                flag = 1;
            }
        }
        for (int i = 0; i < len; i++)
        {
            if (flag)
            {
                if (itr->first[0] != itr->second[i][0] && itr->second[i][itr->second[i].length() - 1] != '\'')
                {
                    itr->second[i] = itr->second[i] + itr->first + "'";
                }
            }
        }
        // ^ is defined as epsilon character
        if (flag)
        {
            vector<string> s{"^"};
            production.insert({itr->first + "'", s});
            num_production++;
            nonTerminal.insert({itr->first + "'", 1});
            terminal.insert({"^", 1}); // since epsilon is introduced to the grammar, had to add it to terminal character list
            vector<int> temp;
            for (int i = 0; i < len; i++)
            {
                if (itr->first[0] == itr->second[i][0])
                {
                    temp.push_back(i);
                    production[itr->first + "'"].push_back(itr->second[i].substr(1, itr->second[i].length() - 1) + itr->first + "'");
                }
            }
            len = temp.size();
            int count = 0;
            for (int i = 0; i < len; i++)
            {
                itr->second.erase(itr->second.begin() + temp[i] - count);
                count++;
            }
        }
    }
}

string findNTChar(string s, int &ind)
{
    //ind - character index to start from
    char c = s[ind];
    string result = "";
    result += c;

    int i = ind + 1;
    while (i < s.length() && s[i++] == '\'')
    {
        result += '\'';
    }
    ind = i - 1;

    return result;
}

void getFirst()
{
    for (auto itr = production.begin(); itr != production.end(); itr++)
    {
        int len = itr->second.size();
        for (int i = 0; i < len; i++)
        {
            if (itr->second[i][0] >= 'A' && itr->second[i][0] <= 'Z')
            {  // case: non terminal character
                string word = itr->second[i];
                int j = 0;
                {
                    string NT = findNTChar(word, j); //j got updated to last ' index
                    vector<string> tmpFirst = First(NT);
                    //loop for handling if first has epsilon
                    while(find(tmpFirst.begin(), tmpFirst.end(), "^") != tmpFirst.end()) {
                        // if non terminal is last character of a sub production
                        if (j == itr->second[i].length()-1) {
                            break;
                        }

                        // removing epsilon from first
                        tmpFirst.erase(find(tmpFirst.begin(), tmpFirst.end(), "^"));

                        if (itr->second[i][j+1] >= 'A' && itr->second[i][j+1] <= 'Z') {
                            // non terminal case
                            NT = findNTChar(itr->second[i], ++j);
                            vector <string> tmptmpfirst;
                            tmptmpfirst = First(NT);
                            // check for duplicate and insert in the first
                            for (auto tmpit: tmptmpfirst) {
                                if (find(tmpFirst.begin(), tmpFirst.end(), tmpit) == tmpFirst.end()) {
                                    tmpFirst.push_back(tmpit);
                                }
                            }

                        } else {
                            // terminal case
                            // inserting the terminal character to the first
                            for (auto it= terminal.begin(); it != terminal.end(); it++) {
                                if (itr->second[i].find(it->first, j+1) == j+1) {
                                    tmpFirst.push_back(it->first);
                                    break;
                                }
                            }
                        }

                    }
                    // adding first of non terminals to the first map
                    int tmp_len = tmpFirst.size();
                    if (first.find(itr->first) == first.end())
                    {
                        first.insert({itr->first, tmpFirst});
                    }
                    else
                    {
                        for (int k = 0; k < tmp_len; k++)
                        {
                            first[itr->first].push_back(tmpFirst[k]);
                        }
                    }
                }
            }
            else
            {   // case: terminal character
                for (auto it = terminal.begin(); it != terminal.end(); it++)
                {
                    if (itr->second[i].find(it->first) == 0)
                    {
                        if (first.find(itr->first) == first.end())
                        {
                            vector<string> tmp{it->first};
                            first.insert({itr->first, tmp});
                        }
                        else
                        {
                            first[itr->first].push_back(it->first);
                        }
                    }
                }
            }
        }
    }
}

vector<string> First(string s)
{
    vector<string> result;
    vector<string> temp = production[s];
    int len = temp.size();
    for (int i = 0; i < len; i++)
    {
        if (temp[i][0] >= 'A' && temp[i][0] <= 'Z')
        {
            {
                int j = 0;
                string NT = findNTChar(temp[i], j);
                vector<string> tmp_res = First(NT);
                int tmp_len = tmp_res.size();
                for (int k = 0; k < tmp_len; k++)
                {
                    result.push_back(tmp_res[k]);
                }
            }
        }
        else
        {
            for (auto it = terminal.begin(); it != terminal.end(); it++)
            {
                if (temp[i].find(it->first) == 0)
                {
                    result.push_back(it->first);
                }
            }
        }
    }
    return result;
}

void getFollow()
{
    // follow[startingNonTerminal] = vector<string>{"$"};
    for (auto nt : nonTerminal)
    {
        vector<string> tmpFollow = Follow(nt.first);
        if (follow.find(nt.first) == follow.end())
        {
            follow[nt.first] = tmpFollow;
        }
        else
        {
            for (auto x : tmpFollow)
            {
                follow[nt.first].push_back(x);
            }
        }
    }
}

//deprecated -- param - callStart - is start being called from another production? Yes, then return $
vector<string> Follow(string s)
{ //return $ if start
    vector<string> result;
    if (s == startingNonTerminal)
    {
        result.push_back("$");
    }
    for (auto p : production)
    {
        for (auto sp : p.second)
        {
            if (sp.find(s) != -1) //diff X & X'
            {
                //debug
                if (debug)
                    cout << "sp1-----debug-------" << s << "\t" << p.first << " -> " << sp << endl;

                int ind = sp.find(s) + s.length();
                if (debug)
                    cout << "ind: " << ind << "\tsp[ind]: " << sp[ind] << endl; //debug
                if (ind < sp.length() && isupper(sp[ind]))
                { //next is non - terminal
                    if (debug)
                        cout << "\tsp2-----debug-------" << endl; //debug
                    string nt;
                    for (auto it = nonTerminal.begin(); it != nonTerminal.end(); it++)
                    {
                        if (sp.find(it->first, ind) == ind)
                        {
                            nt = it->first;
                            break;
                        }
                    }
                    //debug
                    if (debug)
                        cout << "\t" << nt << "\t" << sp << endl;

                    //got next nt now
                    int breakSP = 0;
                    int flag = 1; //assume epsilon to exist by default
                    while (flag)
                    {
                        vector<string> firsts = first[nt];
                        for (auto f : firsts)
                        {
                            if (find(result.begin(), result.end(), f) == result.end())
                            {
                                if (f[0] != '^')
                                {
                                    result.push_back(f);
                                }
                            }
                        }

                        if (find(firsts.begin(), firsts.end(), "^") == firsts.end())
                        { //no epsilion -> break;
                            flag = 0;
                            break;
                        }
                        else
                        {
                            //check if last nt and has episilon in FIRST
                            if (ind + nt.length() == sp.length())
                            {
                                //if s and production.first is same -> break out of sp
                                if (s == p.first)
                                {
                                    breakSP = 1;
                                    break;
                                }
                                //otherwise FOLLOW(production.first)
                                vector<string> tmpResult = Follow(p.first);
                                for (auto r : tmpResult)
                                {
                                    if (find(result.begin(), result.end(), r) == result.end())
                                    {
                                        result.push_back(r);
                                    }
                                }
                            }

                            flag = 0;
                            break;
                        }

                        ind += nt.length();
                        //terminal -> break;
                        int tmpFlag = 1;
                        for (auto it = terminal.begin(); it != terminal.end(); it++)
                        {
                            if (sp.find(it->first, ind) == ind)
                            {
                                result.push_back(it->first);
                                tmpFlag = 0;
                                break;
                            }
                        }
                        if (tmpFlag == 0)
                        {
                            break;
                        }

                        //nt -> continue
                        for (auto it = nonTerminal.begin(); it != nonTerminal.end(); it++)
                        {
                            if (sp.find(it->first, ind) == ind)
                            {
                                nt = it->first;
                                break;
                            }
                        }
                    }
                    if (breakSP)
                    {
                        continue;
                    }
                }
                else
                {
                    if (ind >= sp.length())
                    {
                        //handle - s is last NT
                        //debug
                        if (debug)
                            cout << "\t---last nt \n";
                        //if s and production.first is same -> break out of sp
                        if (s == p.first)
                        {
                            continue;
                        }
                        //otherwise FOLLOW(production.first)
                        vector<string> tmpResult = Follow(p.first);
                        for (auto r : tmpResult)
                        {
                            if (find(result.begin(), result.end(), r) == result.end())
                            {
                                result.push_back(r);
                            }
                        }
                        continue;
                    }
                    //terminal
                    if (debug)
                        cout << "\tTerminal: " << sp[ind] << endl; //debug
                    for (auto it = terminal.begin(); it != terminal.end(); it++)
                    {
                        if (sp.find(it->first, ind) == ind)
                        {
                            result.push_back(it->first);
                            break;
                        }
                    }
                }
            }
        }
    }
    return result;
}

void constructParsingTable()
{
    // initialization of parsing table
    for (auto itr = nonTerminal.begin(); itr != nonTerminal.end(); itr++)
    {
        map<string, string> temp;
        for (auto it = terminal.begin(); it != terminal.end(); it++)
        {
            temp.insert({it->first, ""});
        }
        temp.insert({"$", ""});
        parsingTable.insert({itr->first, temp});
    }

    for (auto itr = production.begin(); itr != production.end(); itr++)
    {
        int len = itr->second.size();
        for (int i = 0; i < len; i++)
        {
            if (itr->second[i][0] >= 'A' && itr->second[i][0] <= 'Z')
            {
                // for non terminal starting case
                for (auto it = nonTerminal.begin(); it != nonTerminal.end(); it++)
                {
                    // finding the non terminal
                    if (itr->second[i].find(it->first) == 0)
                    {
                        vector<string> tmp_vec = first[it->first];
                        int temp_len = tmp_vec.size();
                        for (int j = 0; j < temp_len; j++)
                        {
                            if (parsingTable[itr->first][tmp_vec[j]] != "")
                            {
                                // multiple input to a single place in the parsing table
                                cout << "ERROR: Not a LL1 grammar\n";
                                exit(-1);
                            }
                            parsingTable[itr->first][tmp_vec[j]] = itr->second[i];
                        }
                    }
                }
            }
            else
            {
                // for terminal starting case
                for (auto it = terminal.begin(); it != terminal.end(); it++)
                {
                    // finding the terminal
                    if (itr->second[i].find(it->first) == 0)
                    {
                        if (it->first != "^")
                        {
                            if (parsingTable[itr->first][it->first] != "")
                            {
                                cout << "ERROR: Not a LL1 grammar\n";
                                exit(-1);
                            }
                            parsingTable[itr->first][it->first] = itr->second[i];
                        }
                        else
                        {
                            vector<string> tmp_vec = follow[itr->first];
                            int temp_len = tmp_vec.size();
                            for (int j = 0; j < temp_len; j++)
                            {
                                if (parsingTable[itr->first][tmp_vec[j]] != "")
                                {
                                    cout << "ERROR: Not a LL1 grammar\n";
                                    exit(-1);
                                }
                                parsingTable[itr->first][tmp_vec[j]] = itr->second[i];
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
}

void printParsingTable()
{
    cout << "    ";
    for (auto it = parsingTable[startingNonTerminal].begin(); it != parsingTable[startingNonTerminal].end(); it++)
    {
        cout << it->first << "\t";
    }
    cout << "\n";
    for (auto itr = parsingTable.begin(); itr != parsingTable.end(); itr++)
    {
        cout << itr->first << ": ";
        for (auto it = itr->second.begin(); it != itr->second.end(); it++)
        {
            cout << it->second << "\t";
        }
        cout << "\n";
    }
}

/* reading tokens from the file */
vector<vector<string>> getTokens()
{
    vector<vector<string>> tokens;
    vector<string> l;
    tokens.push_back(l);
    ifstream fin;
    fin.open("token.txt");
    string line;
    int counter = 0;
    while (getline(fin, line))
    {
        if (line.length() == 0)
        {
            counter++;
            vector<string> l;
            tokens.push_back(l);
        }
        else
        {
            tokens[counter].push_back(line);
        }
    }
    return tokens;
}

void printStack(stack<string> s)
{
    cout << "\n";
    while (!s.empty())
    {
        cout << "\t" << s.top();
        s.pop();
    }
    cout << "\n";
}

//stack implementation
void verifyExpression()
{
    vector<vector<string>> tokens = getTokens();
    for (auto x : tokens)
    {
        //iterating through each line
        stack<string> s;
        s.push("$");
        s.push(startingNonTerminal);
        x.push_back("$"); // append $ in each line of input at end

        int i = 0, found = 1;
        while (!s.empty())
        {
            while (s.top() == "^")
            {
                s.pop();
            }

            // cout << "d1-----------\n";
            // printStack(s);

            string top = s.top();
            if (!isupper(top[0]))
            {
                //top is terminal character
                if (s.top() == x[i])
                {
                    s.pop();
                    i++;
                }
                else
                {
                    cout << "-> Error: terminal character not equal!!\n";
                    found = 0;
                    cout << "Expected: " << s.top() << " but found " << x[i] << "\n";
                    cout << "Error after ";
                    for (int j = 0; j <= i; ++j)
                    {
                        cout << x[j] << " ";
                    }
                    cout << " in ";
                    for (auto y : x)
                    {
                        if (y != "$")
                        {
                            cout << y << " ";
                        }
                    }
                    cout << ".\n";

                    break;
                    // exit(0);
                }
                // cout << "dupper-----------\n";
                // printStack(s);
            }
            else
            {
                s.pop();
                string sp = parsingTable[top][x[i]];
                // cout << "dsp-----------\n";
                // cout<<sp<<endl;
                vector<string> tAndNT; // terminal and non-terminal of sp;
                for (int i = 0; i < sp.length(); ++i)
                {
                    if (isupper(sp[i]))
                    {
                        //nt
                        string t = "";
                        t += sp[i];
                        while (sp[++i] == '\'')
                        {
                            t += '\'';
                        }
                        tAndNT.push_back(t);
                        --i;
                    }
                    else
                    {
                        //terminal
                        for (auto it = terminal.begin(); it != terminal.end(); it++)
                        {
                            if (sp.find(it->first) == 0)
                            {
                                tAndNT.push_back(it->first);
                                i += it->first.length() - 1;
                                break;
                            }
                        }
                    }
                }
                for (int i = tAndNT.size() - 1; i >= 0; --i)
                {
                    s.push(tAndNT[i]);
                }
                // cout << "dlower-----------\n";
                // printStack(s);
            }
        }
        if (found)
        {
            cout << "-> valid input: ";
            for (auto y : x)
            {
                if (y != "$")
                {
                    cout << y << " ";
                }
            }
            cout << "\n";
        }
    }
}

int main()
{
    readFromFile();
    beautifyProduction();
    eliminateLeftRecursion();
    cout << "\n------------------------Production---------------------\n\n";
    for (auto itr = production.begin(); itr != production.end(); itr++)
    {
        int len = itr->second.size();
        cout << itr->first << ": ";
        for (int i = 0; i < len; i++)
        {
            cout << itr->second[i] << " ";
        }
        cout << "\n";
    }
    cout << "\n\n\n------------------------Terminal---------------------\n\n";
    for (auto itr = terminal.begin(); itr != terminal.end(); itr++)
    {
        cout << itr->first << " ";
    }
    cout << "\n\n\n------------------------FIRST---------------------\n\n";
    getFirst();
    for (auto itr = first.begin(); itr != first.end(); itr++)
    {
        cout << itr->first << ":\t";
        int len = itr->second.size();
        for (int i = 0; i < len; i++)
        {
            cout << itr->second[i] << "\t";
        }
        cout << "\n";
    }

    cout << "\n\n\n------------------------FOLLOW---------------------\n\n";
    getFollow();
    for (auto x : follow)
    {
        cout << x.first << ": \t";
        for (auto y : x.second)
        {
            cout << y << " \t";
        }
        cout << endl;
    }

    cout << "\n\n------------------------PARSING TABLE---------------------\n\n";
    constructParsingTable();
    printParsingTable();

    cout << "\n\n------------------------Verifying given input---------------------\n\n";
    verifyExpression();
    return 0;
}