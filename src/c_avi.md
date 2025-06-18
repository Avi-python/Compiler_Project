`<Program>`
- First set
	- int, char, void (from `<Type>`)
- Follow set
	- EOF

`<ExternalDeclaration>`
- First set
	- int, char, void (from `<Type>`)
- Follow set
	- int, char, void, EOF

`<Declarations>`
- First set
	- (, =, 
	- ,
	- ;
- Follow set
	- int, char, void, EOF

`<FunctionDefinition>`
- First set
	- (
- Follow set
	- int, char, void, EOF

`<VariableDeclarationGlobal>`
- First set
	- =,
	- ,
	- epsilon
- Follow set
	- ;

`<ParameterListOpt>`
- First set
	- int, char, void (from `<Type>`)
	- epsilon
- Follow set
	- )

`<ParameterList>`
- First set
	- int, char, void (from `<Type>`)
- Follow set
	- )

`<ParameterDeclaration>`
- First set
	- int, char, void (from `<Type>`)
- Follow set
	 - ,
	 - )

`<CompoundStatement>`
- First set
	- {
- Follow set
	- EOF
	-  Identifier
	- { ( from `<CompoundStatement>` )
	- int, char, void ( from `<DeclareStatement>` )
	- if ( from `<IfStatement>` )
	- while ( from `<WhileStatement>` )
	- return
	- } 
	- print
	- printf
	- else

`<StatementList>`
- First set
	- { (from `<CompoundStatement>`)
	- int, char, void (from `<DeclareStatement>`)
	- Identifier (from identifier statements)
	- if (from `<IfStatement>`)
	- while (from `<WhileStatement>`)
	- return (from `<ReturnStatement>`)
	- print (from `<PrintStatement>`)
	- printf (from `<PrintfStatement>`)
	- epsilon
- Follow set
	- }

`<Statement>`
- First set
	- { (from `<CompoundStatement>`)
	- int, char, void (from `<DeclareStatement>`)
	- Identifier (from identifier statements)
	- if (from `<IfStatement>`)
	- while (from `<WhileStatement>`)
	- return (from `<ReturnStatement>`)
	- print (from `<PrintStatement>`)
	- printf (from `<PrintfStatement>`)
	- epsilon
- Follow set
	- {
	- int, char, void
	- Identifier
	- if
	- while
	- return
	- print
	- printf
	- }

`<AssignOrFuncCall>`
- First set
	- = ( from `<AssignmentStatement>` )
	- ( ( from `<FunctionCallStatement>` )
- Follow set
	- ;

`<AssignmentStatement>`
- First set
	- =
- Follow set
	- ;

`<FunctionCallStatement>`
- First set
	- (
- Follow set
	- \*, /
	- +, -
	- ), ;
	- <, <=, >, >=, ==, !=
	- ,

`<ArgumentListOpt>`
- First set
	- CharLiteral, Identifier, Number, ( ( from `<Expression>` )
	- epsilon
- Follow set
	- )

`<ArgumentList>`
- First set
	- CharLiteral, Identifier, Number, ( ( from `<Expression>` )
- Follow set
	- )

`<InitDeclarator>`
- First set
	- Identifier
- Follow set
	- ,
	- ;

`<DeclareStatement>`
- First set
	- int, char, void
- Follow set
	- ;

`<IfStatement>`
- First set
	- if
- Follow set
	-  Identifier
	- { ( from `<CompoundStatement>` )
	- int, char, void ( from `<DeclareStatement>` )
	- if ( from `<IfStatement>` )
	- while ( from `<WhileStatement>` )
	- return
	- } 
	- print
	- printf

`<WhileStatement>`
- First set
	- while
- Follow set
	-  Identifier
	- { ( from `<CompoundStatement>` )
	- int, char, void ( from `<DeclareStatement>` )
	- if ( from `<IfStatement>` )
	- while ( from `<WhileStatement>` )
	- return
	- } 
	- print
	- printf

`<DeclareStatement>`
- First set
	- int, char, void
- Follow set
	- ;

`<InitDeclarator>`
- First set
	- Identifier
- Follow set
	- ,
	- ;

`<Expression>`
- First set
	- Identifier 
    - (
    - Number
	- CharLiteral
- Follow set
	- )
	- ;
	- ,

`<ArithmeticExpression>`
- First set
	- Identifier
	- (
	- Number
- Follow set
	- )
	- ;
	- <, <=, >, >=, ==, !=
	- ,

`<RelationalPrime>`
- First set
	- <, <=, >, >=, ==, !=
	- epsilon
- Follow set
	- )
	- ;
	- ,

`<ArithmeticPrime>`
- First set
	- +, -
	- epsilon
- Follow set
	- )
	- ;
	- <, <=, >, >=, ==, !=
	- ,

`<Term>`
- First set
	- Identifier 
    - Number
    - (
	- CharLiteral
- Follow set
	- +, -
	- ), ;
	- <, <=, >, >=, ==, !=
	- ,

`<TermPrime>`
- First set
	- \*, /
	- epsilon
- Follow set
	- +, -
	- ), ;
	- <, <=, >, >=, ==, !=
	- ,

`<Factor>`
- First set
	- Identifier
	- Number 
	- (
	- CharLiteral
- Follow set
	- \*, /
	- +, -
	- ), ;
	- <, <=, >, >=, ==, !=
	- ,

`<EpsilonOrFuncCall>`
- First set
	- ( ( from `<FunctionCallStatement>` )
	- epsilon
- Follow set
	- \*, /
	- +, -
	- ), ;
	- <, <=, >, >=, ==, !=
	- ,

`<ReturnStatement>`
- First set
	- return
- Follow set
	- ;

`<PrintStatement>`
- First set
	- print
- Follow set
	- ;

`<PrintfStatement>`
- First set
	- printf
- Follow set
	- ;

`<Identifier>`
- First set
	- [A-Za-z]
- Follow set
	- \*, /
	- +, -
	- ), ;
	- <, <=, >, >=, ==, !=
	- ,
	- =
	- (

`<Number>`
- First set
	- [0-9]
- Follow set
	- \*, /
	- +, -
	- ), ;
	- <, <=, >, >=, ==, !=
	- ,

`<Type>`
- First set
	- int, char, void
- Follow set
	- Identifier 

`<Alpha>`
- First set
	- [A-Za-z]
- Follow set
	- [A-Za-z]
	- [0-9]
	- \*, /
	- +, -
	- ), ;
	- <, <=, >, >=, ==, !=
	- ,
	- =
	- (

`<Digit>`
- First set
	- [0-9]
- Follow set
	- [A-Za-z]
	- [0-9]
	- \*, /
	- +, -
	- ), ;
	- <, <=, >, >=, ==, !=
	- ,
	- =
	- (