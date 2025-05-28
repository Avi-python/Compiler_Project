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
	- } 

`<StatementList>`
- First set
	- Identifier ( from `<AssignmentStatement>` )
	- { ( from `<CompoundStatement>` )
	- int, char ( from `<DeclareStatement>` )
	- epsilon
- Follow set
	- }

`<Statement>`
- First set
	- Identifier ( from `<AssignmentStatement>` )
	- { ( from `<CompoundStatement>` )
	- int, char ( from `<DeclareStatement>` )
	- epsilon
- Follow set
	- Identifier ( from `<AssignmentStatement>` )
	- { ( from `<CompoundStatement>` )
	- int, char ( from `<DeclareStatement>` )
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

`<Expression>`
- First set
	- Identifier 
    - (
    - Number
- Follow set
	- )
	- ;

`<ExpressionPrime>`
- First set
	- +, -
	- epsilon
- Follow set
	- )
	- ;

`<Term>`
- First set
	- Identifier 
    - Number
    - (
- Follow set
	- +, -
	- ), ;

`<TermPrime>`
- First set
	- \*, /
	- epsilon
- Follow set
	- +, -
	- ), ;

`<Factor>`
- First set
	- Identifier
	- Number 
	- (
- Follow set
	- \*, /
	- +, -
	- ), ;

`<Identifier>`
- First set
	- [A-Za-z]
- Follow set
	- \*, /
	- +, -
	- ), ;

`<Number>`
- First set
	- [0-9]
- Follow set
	- \*, /
	- +, -
	- ), ;

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