Operation manual and assumptions:

    For input grammar:

    1. The grammar must be written in a pre-defined format as mentioned below.
    2. The grammar must be put in a file named grammar.txt
    3. The first line of grammar.txt file should be a line consisting of space separated terminal characters.
    4. The non terminal characters should be uppercase alphabets.
    5. The format in which production rules are mentioned is:
        S -> AbC|bS|bC

    6.There should be a whitespace on both side of ->
    7. Multiple production rule for same non terminal character can be written in the same line by separating with a "|".
    8. Production rules for different non terminal characters should be written in different lines.
    9. A sample file for grammar.txt is provided along.





Contributions:

    Memebers: Sameer Kumar Kushwaha    Roll no: 1801151
              Soumyajit Deb            Roll no: 1801176
    
    Sameer Kumar kushwaha's Contribution: (1801151)
        - Functions implemented:
            First()
            findNTChar()
            getFollow()
            Follow()
            printStack()
            verifyExpression()
            lex program
        - Helped in developing the program structure
        - Helped in debugging process
    
    Soumyajit Deb's Contribution: (1801176)
        - Function implemented:
            First()
            getFollow()
            readFromFile()
            beautifyProduction()
            eliminateLeftRecursion()
            getFirst()
            constructParsingTable()
            printParsingTable()
        - Helped in developing the program structure
        - Helped in debugging process