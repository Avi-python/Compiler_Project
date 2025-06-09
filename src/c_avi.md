`<Program>`
- First set
	- int
- Follow set
	- EOF

`<Block>`
- First set
	- int
- Follow set
	- EOF

`<CompoundStatement>`
- First set
	- {
- Follow set
	- EOF
	-  Identifier ( from `<AssignmentStatement>` )
	- { ( from `<CompoundStatement>` )
	- int, char ( from `<DeclareStatement>` )
	- if ( from `<IfStatement>` )
	- while ( from `<WhileStatement>` )
	- } 

`<StatementList>`
- First set
	- Identifier ( from `<AssignmentStatement>` )
	- { ( from `<CompoundStatement>` )
	- int, char ( from `<DeclareStatement>` )
	- if ( from `<IfStatement>` )
	- while ( from `<WhileStatement>` )
	- epsilon
- Follow set
	- }

`<Statement>`
- First set
	- Identifier ( from `<AssignmentStatement>` )
	- { ( from `<CompoundStatement>` )
	- int, char ( from `<DeclareStatement>` )
	- if ( from `<IfStatement>` )
	- while ( from `<WhileStatement>` )
	- epsilon
- Follow set
	- Identifier ( from `<AssignmentStatement>` )
	- { ( from `<CompoundStatement>` )
	- int, char ( from `<DeclareStatement>` )
	- if ( from `<IfStatement>` )
	- while ( from `<WhileStatement>` )
	- }

`<IfStatement>`
- First set
	- if
- Follow set
	- Identifier ( from `<AssignmentStatement>` )
	- { ( from `<CompoundStatement>` )
	- int, char ( from `<DeclareStatement>` )
	- if ( from `<IfStatement>` )
	- while ( from `<WhileStatement>` )
	- } 

`<WhileStatement>`
- First set
	- while
- Follow set
	- Identifier ( from `<AssignmentStatement>` )
	- { ( from `<CompoundStatement>` )
	- int, char ( from `<DeclareStatement>` )
	- if ( from `<IfStatement>` )
	- while ( from `<WhileStatement>` )
	- }

`<AssignmentStatement>`
- First set
	-Identifier 
- Follow set
	- ;

`<DeclareStatement>`
- First set
	- int, char
- Follow set
	- ;

`<Condition>`
- First set
	- Identifier 
    - (
    - Number
- Follow set
	- )

`<Expression>`
- First set
	- Identifier 
    - (
    - Number
- Follow set
	- )
	- ;

`<ArithmeticExpression>`
- First set
	- Identifier
	- (
	- Number
- Follow set
	- )
	- ;
	- <, <=, >, >=, ==, !=

`<RelationalPrime>`
- First set
	- <, <=, >, >=, ==, !=
	- epsilon
- Follow set
	- )
	- ;

`<ArithmeticPrime>`
- First set
	- +, -
	- epsilon
- Follow set
	- )
	- ;
	- <, <=, >, >=, ==, !=

`<Term>`
- First set
	- Identifier 
    - Number
    - (
- Follow set
	- +, -
	- ), ;
	- <, <=, >, >=, ==, !=

`<TermPrime>`
- First set
	- \*, /
	- epsilon
- Follow set
	- +, -
	- ), ;
	- <, <=, >, >=, ==, !=

`<Factor>`
- First set
	- Identifier
	- Number 
	- (
- Follow set
	- \*, /
	- +, -
	- ), ;
	- <, <=, >, >=, ==, !=

`<Identifier>`
- First set
	- [A-Za-z]
- Follow set
	- \*, /
	- +, -
	- ), ;
	- <, <=, >, >=, ==, !=

`<Number>`
- First set
	- [0-9]
- Follow set
	- \*, /
	- +, -
	- ), ;
	- <, <=, >, >=, ==, !=

`<Type>`
- First set
	- int, char
- Follow set
	- Identifier 

`<Alpha>`
- First set
	- [A-Za-z]
- Follow set
	- {

`<Digit>`
- First set
	- [0-9]
- Follow set
	-