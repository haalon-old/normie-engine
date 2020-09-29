#include "iostream"
#include <string.h>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <stack>
#include <deque>


/*
S → { function TYPE IDENT( [TYPE IDENT] {, TYPE IDENT} ) { D1 OPS } } program { D1 OPS } @

D1 → {  TYPE ID { , ID } ; } 
TYPE == [ int | real ]
ID → IDENT | IDENT = CONST 

OPS → { OP }
OP →  if ( TERM ) OP else OP | while( TERM ) OP | { OPS } | read (IDENT) ; | write (  TERM { , TERM } ) ; | return TERM; | TERM ;

TERM → TERM2 = TERM2 | TERM2
TERM2 → TERM3 { [ or | and  TERM3 } | TERM3
TERM3 → TERM$ { [< | > | == etc] TERM4 } | TERM4
TERM4 → TERM5 { [ + | -   ] TERM5 }
TERM5 → TERM6 { [ * | /  ] TERM6 }
TERM6 → IDENT | NUMB | RNUMB | not TERM6 | ( TERM ) | IDENT( [TERM] {, TERM})
*/


#define DBG 0

using namespace std;
enum type_of_data {NONE, REAL, INT , STRING, FUNCTION };

template <  typename ValueType > class Stack
{
	deque<ValueType> st;

public:
	
	void push( ValueType v) { st.push_back(v);}
	ValueType pop_back( ) { ValueType temp = st.back(); st.pop_back(); return temp; }
	ValueType pop_front( ) { ValueType temp = st.front(); st.pop_front(); return temp; }
	ValueType top( ) { return st.back();}
	void clear() { while(st.size()) st.pop_back();}
	int size() { return st.size();}
	long find_last(ValueType v) 
	{ 
		long j=0,i=-1;
		while(j++< st.size())
		{
			if(st[j]==v)
				i=j;
		}
		return i;
	}
	ValueType & operator[](long index) { return st[index]; }

	
};

enum type_of_lex
{
    LEX_NULL,
    LEX_AND,  LEX_BOOL, LEX_DO, LEX_CASE, LEX_ELSE, LEX_END,LEX_IF, LEX_FALSE, LEX_FOR, LEX_INT, LEX_GOTO, LEX_NOT,LEX_OR, LEX_OF,
    		  LEX_PROGRAM, LEX_READ, LEX_REAL, LEX_STRING, LEX_TRUE, 
    		  LEX_WHILE, LEX_FUNCTION, LEX_RETURN, LEX_WRITE,
    LEX_SEMICOLON, LEX_FIN, LEX_COMMA, LEX_2DOTS, LEX_ASSIGN, LEX_OPBRAKET,
    LEX_CLBRAKET, LEX_OPFIG, LEX_CLFIG, LEX_EQ, LEX_LESS, LEX_GREAT, LEX_PLUS, LEX_MINUS, LEX_MUL, LEX_DIV, LEX_LEQ, LEX_GEQ, LEX_NEQ,
    LEX_NUMB, LEX_RNUMB, LEX_CONST, LEX_UMINUS,
    LEX_ID,
    POLIZ_LABEL,
    POLIZ_ADDRESS,
    POLIZ_COUNT,
    POLIZ_GO,
    POLIZ_FGO,
    POLIZ_TGO,   
    POLIZ_FUNCTION
};

type_of_lex words[] = {LEX_NULL, LEX_AND,  LEX_BOOL, LEX_DO, LEX_CASE, LEX_ELSE, LEX_END,LEX_IF, LEX_FALSE, LEX_FOR, LEX_FUNCTION, LEX_INT, LEX_GOTO, LEX_NOT,LEX_OR, LEX_OF,
                                 LEX_PROGRAM, LEX_READ, LEX_REAL, LEX_RETURN , LEX_STRING,  LEX_TRUE, LEX_WHILE,
                                 LEX_WRITE
                                };
type_of_lex  dlms[] = {LEX_NULL, LEX_SEMICOLON, LEX_FIN, LEX_COMMA, LEX_2DOTS, LEX_ASSIGN, LEX_OPBRAKET,
                                 LEX_CLBRAKET, LEX_OPFIG, LEX_CLFIG, LEX_EQ, LEX_LESS, LEX_GREAT, LEX_PLUS, LEX_MINUS, LEX_MUL, LEX_DIV, LEX_LEQ, LEX_GEQ, LEX_NEQ
                               };
const char *  TW [ ] = { NULL, "and","bool","do","case","else","end",
                                  "if","false", "for","function", "int","goto","not","or","of","program","read", "real", "return" ,"string",
                                  "true","while","write"
                                };

const char * TD [ ] = {NULL, ";", "@", ",", ":", "=", "(", ")","{", "}",
                                 "==","<", ">", "+", "-", "*", "/", "<=", ">=", "!="
                                };

const char* TP[] = {NULL, "GO", "FGO", "TGO"};

enum class_of_lex { LNULL, LDELIM, LID, LWORD, LNUMB, LRNUMB, LPOLIZ, LSTR};


struct Function
{	
	type_of_data type;
	long enter_point;
	int par_count;
	Stack<type_of_data> par_type;
	Stack<long> par_adr;


	Function( type_of_data t= NONE, long e =0, int p=0) {type = t; enter_point = e; par_count =p;}
	
};


class Lex : public Function
{
	type_of_lex t_lex;
	class_of_lex c_lex;
	long v_lex;
	
public:
	Lex ( type_of_lex t = LEX_NULL, long v = 0, class_of_lex c = LNULL ) {
		t_lex = t;
		v_lex = v;
		c_lex = c;
		
	}
	void put_type (type_of_lex t ) { t_lex	=t ;}

	type_of_lex get_type ( ) {	return t_lex; }
	long get_value ( ) {	return v_lex; }
	friend ostream& operator << (ostream & s, Lex l );
};


class Ident
{
	char *name;
	bool declare;
	bool function;
	type_of_data type;
	bool assign;
	void * value;
public:
	Ident ( ) { declare = false; assign = false; function = false; }
	~Ident();
char *get_name ( ) { return name; }
void put_name ( const char * n ) 
{
	name = new char [ strlen ( n ) + 1];
	strcpy ( name, n );
}
Function& get_function( ) { return *((Function*)value); }
//void put_function ( ) { function = true; }
bool get_declare ( ) { return declare; }
void put_declare ( ) { declare = true; }
type_of_data get_type ( ) { return type; }
void put_type ( type_of_data t ) {	type = t; }
bool get_assign ( ) { return assign; }
void put_assign ( ) { assign = true; }
void * get_value ( ) { return value; }
void put_value ( long v ) { void* oldvalue = value;   value = new long; *( (long*) value ) = v; if(assign) delete (long*) oldvalue; } // !!!!
void put_value ( double v ) { void* oldvalue = value;   value = new double; *( (double*) value ) = v; if(assign) delete (double*) oldvalue;  }
void put_value (const char * v ) { void* oldvalue = value;   value = new char [ strlen(v)+1];  strcpy ( (char*)value, v ); if(assign) delete (char*) oldvalue;  }
void put_value ( int v ) { void* oldvalue = value;    value = new int; *( (int*) value ) = v; if(assign) delete (int*) oldvalue; }
void put_value ( Function v ) { void* oldvalue = value;  value = new Function(); *( (Function*) value ) = v;  if(assign) delete (Function*) oldvalue;  }
};

Ident::~Ident()
{
	/*switch(type)
	{
		case INT: delete (int*)value; break;
		case REAL: delete (double*)value; break;
		case STRING: delete (char*)value; break;
	}
	*/
}




class tabl_ident
{
	Ident *p;
	long size;
	long top;
public:
	tabl_ident ( long max_size ) {
		p = new Ident [ size = max_size ];
		top = 1;
	}
	~tabl_ident ( ) { delete [ ] p; }
	Ident & operator [ ] ( long k ) { return p [ k ]; }
	long put (const char * buf);	
};

class Const
{
	type_of_data type;
	void* value;
public:
	Const(type_of_data t = NONE){type =t;}
	~Const()
	{
		/*switch(type)
		{
			case INT: delete ((int*)value); break;
			case REAL: delete (double*)value; break;
			case STRING: delete (char*)value; break;
		} */
	}
	
	type_of_data get_type ( ) { return type; }
	void put_type ( type_of_data t ) { type = t; }
	void * get_value ( ) { return value; }
	//void put_value ( long v ) { value = new long; *( (long*) value ) = v; }
	void put_value ( double v ) { value = new double; *( (double*) value ) = v; }
	void put_value (const char * v ) { value = new char [ strlen ( v ) + 1]; strcpy ( (char*)value, v ); }
	void put_value ( int v ) { value = new int; *( (int*) value ) = v; }
	
};

class tabl_const
{
	Const *p;
	long size;
	long top;
public:
	tabl_const ( long max_size ) {
		p = new Const [ size = max_size ];
		top = 1;
	}
	~tabl_const ( ) { delete [ ] p; }
	Const & operator [ ] ( long k ) { return p [ k ]; }
	long put (Const);	
};

long tabl_const::put(Const c)
{	
	p [ top ] = c;
	return top++	;
	
}


class Scanner
{
	enum state { H, IDENT, NUMB, COM, ALE, DELIM, NEQ, RNUMB, STR };	
	bool function;
	//static char* TW [ ];
	//static type_of_lex words [ ];
	//static char * TD [ ];
	//static type_of_lex dlms [ ];
	state CS;
	FILE *fp;
	char c;
	char buf [ 80 ];
	int buf_top;
	void clear ( ) ;
	void add ( );
	
	void gc ( ) ;
	public:
	int look (const char * buf,const char * * list);
	int line;
	Scanner (const char * program);
	//~Scanner() { fclose(fp); delete this;}
	Lex get_lex ();	
};


tabl_ident TID (100);
tabl_const TC (100);

ostream& operator << (ostream & s, Lex l )
{
	cout << " ";
	
	switch(l.c_lex)
	{
		case LNUMB: s   << * ( (int*)TC[l.v_lex].get_value() ); break;
		case LID: s   << TID[l.v_lex].get_name();
		if(TID[l.v_lex].get_type() == FUNCTION) 
		{
			cout << "[";
			for(int i = 0; i< TID[l.v_lex].get_function().par_count; i++ )
				cout  << TID[ TID[ l.v_lex ].get_function().par_adr[ i ] ].get_name() << " - " << TID[l.v_lex].get_function().par_type[i] << ", ";
			cout << "]";
		}
		break;
		case LWORD: s   << TW[l.v_lex]  ; break;
		case LDELIM: s  << TD[l.v_lex] ; break;
		case LSTR : s << '"' <<(char*) TC[l.v_lex].get_value() << '"'; break;
		case LNULL: s << l.v_lex  ; break;
		case LRNUMB: s   << *( (double*) TC[l.v_lex].get_value() ); break;
		case LPOLIZ: if(l.t_lex  == POLIZ_LABEL || l.t_lex  == POLIZ_COUNT)  s << l.v_lex;  else  s << TP[l.v_lex]  ; break;


	}

		
	return s;
}


long tabl_ident::put(const char * buf)
{
	for (long j = 1; j < top; j++)
		if ( ! strcmp ( buf,p [ j ].get_name ( ) ) ) return j;
	p [ top ].put_name ( buf );
	
	return top++;
}

void Scanner::clear ( ) 
{
	buf_top = 0;
	for (int j = 0; j < 80; j++ )
	buf [ j ] = '\0';
}

void Scanner::add ( ) 
{
	buf [ buf_top ++ ] = c;
}
void Scanner::gc()
{
	static bool lineflag =false;
	c = fgetc ( fp ); 
	if( feof(fp) ) c = '@';  
	if(lineflag && DBG) { cout<<endl; lineflag=false;} 
	lineflag = c=='\n';
	line += c=='\n';
	

}
int Scanner::look (const char * buf,const char * * list) 
{
	int i = 1;
	while (list [ i ])
	{
		if ( ! strcmp (buf, list [ i ] ) )
		return i;
		i++;
	}
	return 0;
}

Scanner::Scanner (const char * program) 
{
	fp = fopen ( program, "r" );
	CS = H;
	clear();
	line = 1;
	gc();
}

Lex Scanner::get_lex ( ) 
{
	int d, j;
	bool flag = false;
	double dd;
	char * str;
	CS = H;
	do
	{
		//if(flag) gc();
		//flag=true;
		switch(CS)
		{
			case H:		
			if ( c == '"') { clear();  CS = STR; }
			else	
			if ( c == ' ' || c == '\n' || c == '\r' || c == '\t'){}
			else
			if ( isalpha(c)) { clear(); add(); CS = IDENT; }
			else
			if ( isdigit (c) ) { d = c - '0'; CS = NUMB; }
			else
			if ( c == '/' ) {  gc(); if( c == '*') { CS = COM; } else { j= look("/", TD);  return Lex(dlms[j], j, LDELIM) ;  }  }
			else
			if ( c == '=' || c == '<' || c == '>') { clear(); add(); CS = ALE; }
			else			
			if ( c == '@') return Lex (LEX_FIN,look ( "@", TD ), LDELIM);
			else
			if ( c == '!' ) { clear(); add (); CS = NEQ; }
			else { CS = DELIM; clear( ); add( ); }
			break;

			case IDENT:			
			if ( isalpha(c) || isdigit(c) ) {	add(); }
			else
			{
				
				if ( j = look (buf, TW) ) return Lex (words[j], j, LWORD);
				else 
				{
					j = TID.put (buf);
					return Lex (LEX_ID, j, LID);
				}
			}			
			break;

			case STR:	
			if(c != '"')
			{
				if(flag)
				{
					switch(c)
					{
						case 'n': c = '\n'; break;
						case '\\': c = '\\'; break;
					}
				}
				//cout << "!!!";
				if(c == '\\' && !flag)
					flag = true;
				else
				{
					add();
					flag = false;
				}
			}
			else
			{
				//cout << buf;
					j = TC.put( Const(STRING));
					TC[j].put_value(buf);
					gc();
					return Lex ( LEX_CONST, j, LSTR);
			}
			break;



			case NUMB:			
			if ( isdigit (c) ) {d = d * 10 + (c - '0'); }
			else
			{
				if(c =='.')
					{ CS = RNUMB; dd=d;d = 0; j=10; }
				else
				{
					j = TC.put( Const(INT));
					TC[j].put_value(d);
					return Lex ( LEX_CONST, j, LNUMB);
				}
						
			}			
			break;

			case RNUMB:			
			if ( isdigit (c) ) {dd = dd  + (double)(c - '0')/j ;j=j*10; }
			else
			{
				if(c =='.')
					{ throw c; }
				else
				{
					j = TC.put( Const(REAL));
					TC[j].put_value(dd);
					return Lex ( LEX_CONST, j, LRNUMB);
				}		
			}			
			break;

			case COM:			
			if ( c == '*' ) { gc(); if( c == '/') CS = H; }
			break;

			case ALE:			
			if(c=='=')
			{
				add(); gc();
				j=look(buf, TD);
				return Lex(dlms[j],j, LDELIM);
			} else
			{				
				j=look(buf, TD);
				return Lex(dlms[j],j, LDELIM);
			}
			break;

			case NEQ:			
			if ( c == '=' )
			{
				add(); gc(); j = look ( buf, TD );
				return Lex ( dlms[j], j , LDELIM); 
			}
			else throw '!';
			break;

			case DELIM:			
			if ( j = look(buf, TD) ) {
			return Lex ( dlms[j], j , LDELIM); }
			else throw c;
			break;

		}
		gc();

	}while(true);
}


class Poliz
{
	long size;
	long index;
	
	Lex * lex_arr;
	public: 
		Poliz(long s)
		{
			size = s;
			lex_arr = new Lex [size];
			index=0;
			

		} 
	void put(Lex);
	void put(Lex, long);
	void put_index( long l){ index = l;}
	long get_index() {return index;}
	void blank() {index++;}


	Lex get() {return lex_arr[index];}
	Lex & operator[](long i) {return lex_arr[i];}
	void show();

};




void Poliz::show()
{
	cout << endl;
	for(long i =0; i < index ; i++ )
		if( (i +1 )% 15)	
			cout << lex_arr[i] ;
		else
			cout << lex_arr[i] << endl;
	cout << endl;
}

void Poliz::put(Lex l)
{
	lex_arr[index++] = l;
	//max++;
}

void Poliz::put(Lex l , long i)
{
	lex_arr[i] = l;
	//max++;
}

Poliz prog(420);


class Parser
{
	long lex_num;
	Lex curr_lex;
	type_of_lex c_type;
	int c_val;
	Scanner scan;

	//Stack<long> st_label;
	Stack<Lex> st_lex;
	Stack<type_of_data> st_type;

	long  S(); void D1();
	type_of_data TYPE(); void ID(type_of_data); void OPS(); int OP();
	void TERM(); void TERM2(); void TERM3(); void TERM4(); void TERM5(); void TERM6(); void CONST();
	void check_op ( ) ;
	void gl ( ) {
		curr_lex = scan.get_lex ();
		c_type = curr_lex.get_type ();

		c_val = curr_lex.get_value ();
		lex_num++;
		//poliz[lex_num ++ ] = curr_lex;
		if (DBG) cout << curr_lex;
	} 
public:
	Parser (const char *program) : scan (program) { lex_num = 0; }
	long analyze();
	
	
};

long Parser::analyze () 
{
	long buf;
	try
	{
		gl ();
		buf = S ();
	
		if( DBG) cout << endl << "Syntax correct" << endl;
		if (DBG) prog.show();
	} catch(Lex l) { cout << l << endl; throw;}
		catch(char c) { cout  << endl << endl << "ERROR; In line: " << scan.line << " lexical error on character: " << c; throw; }
	return buf;
}

long Parser::S()
{
	long buf;

	type_of_data flag;
	while(c_type == LEX_FUNCTION)
	{
		
		gl();
		if(! (flag = TYPE()) ) { cout  << endl << endl << "ERROR; In line: " << scan.line << " expected type, but got: " ; throw curr_lex; }
		if(c_type == LEX_ID)
			{ 
				if( TID[c_val].get_declare() ) { cout  << endl << endl << "ERROR; In line: " << scan.line << " identificator already declared: " ; throw curr_lex;  }
				Function f(flag, prog.get_index());
				buf=c_val;
				//TID[c_val].put_function();
				TID[c_val].put_value(f);
				TID[c_val].put_declare();
				TID[c_val].put_assign();
				TID[c_val].put_type(FUNCTION);
				gl();
			}
		else { cout  << endl << endl << "ERROR; In line: " << scan.line << " expected identificator, but got: " ; throw curr_lex; }
		if(c_type == LEX_OPBRAKET )
			gl();
		else { cout << endl  << endl << "ERROR; In line: " << scan.line << " expected '(', but got: " ; throw curr_lex; }
		int count=0;
		if(flag =TYPE())
		{
			
			if(c_type == LEX_ID)
				{ 
					TID[buf].get_function().par_type.push(flag);
					TID[buf].get_function().par_adr.push(c_val);  
					TID[c_val].put_declare(); 
					TID[c_val].put_type(flag);  
					gl();
				}
			else { cout  << endl << endl << "ERROR; In line: " << scan.line << " expected identificator, but got: " ; throw curr_lex; }
			count++;
			while(c_type == LEX_COMMA)
			{
				gl();
				if(! (flag = TYPE())) { cout << endl  << endl << "ERROR; In line: " << scan.line << " expected type, but got: " ; throw curr_lex; }
				if(c_type == LEX_ID)
					{

						TID[buf].get_function().par_type.push(flag);
						TID[buf].get_function().par_adr.push(c_val);  
						TID[c_val].put_declare(); 
						TID[c_val].put_type(flag);  
						gl();
					}
				else { cout  << endl << endl << "ERROR; In line: " << scan.line << " expected identificator, but got: " ; throw curr_lex; }
				count++;
			}
			//TID[buf].get_value()
			TID[buf].get_function().par_count = count;

		}
		if(c_type == LEX_CLBRAKET)
			gl();
		else { cout << endl  << endl << "ERROR; In line: " << scan.line << " expected ')', but got: " ; throw curr_lex; }
		if(c_type == LEX_OPFIG )
			gl();
		else { cout  << endl << endl << "ERROR; In line: " << scan.line << " expected '{', but got: " ; throw curr_lex; }
		D1();
		OPS();
		prog.put(Lex(LEX_FIN, scan.look("@", TD), LDELIM ));
	

	}
	if(c_type == LEX_PROGRAM)
		gl();
	else { cout << endl  << endl << "ERROR; In line: " << scan.line << " expected 'program', but got: " ; throw curr_lex; }
	buf = prog.get_index();
	if(c_type == LEX_OPFIG )
		gl();
	else { cout << endl  << endl << "ERROR; In line: " << scan.line << " expected '{', but got: " ; throw curr_lex; }
	D1();
	OPS();
	
	if(c_type == LEX_FIN )
		{prog.put(curr_lex);}
	else { cout << endl  << endl << "ERROR; In line: " << scan.line << " expected end of the program, but got: " ; throw curr_lex; }
	return buf;


}

void Parser::CONST()
{

}

void Parser::D1()
{
	type_of_data flag;
	while( flag =  TYPE())
	{
		ID(flag);		
		while(c_type == LEX_COMMA)
			{
				gl();
				ID(flag);
			}

		if(c_type == LEX_SEMICOLON )
			{ prog.put (curr_lex ); gl();}
		else { cout  << endl << endl << "ERROR; In line: " << scan.line << " expected ';', but got: " ; throw curr_lex; }
	}
		

}

type_of_data Parser::TYPE()
{
	if(c_type == LEX_INT )
		{ gl(); return INT; }
	if( c_type == LEX_REAL )
		{ gl(); return REAL; }
	if( c_type == LEX_STRING )
		{ gl(); return STRING; }
	return NONE;
}

void Parser::ID(type_of_data f)
{
	Lex buf;
	int val = c_val;
	if(c_type == LEX_ID)
		{ 
			if( TID[c_val].get_declare() ) { cout  << endl << endl << "ERROR; In line: " << scan.line << " identificator already declared: " ; throw curr_lex;  }
			TID[c_val].put_declare(); 
			TID[c_val].put_type(f);  
			buf = curr_lex;
			gl(); 
		}
	else { cout  << endl << endl << "ERROR; In line: " << scan.line << " expected identificator, but got: " ; throw curr_lex; }
	if(c_type == LEX_ASSIGN)
	{
		prog.put(buf);
		prog[prog.get_index()-1].put_type(POLIZ_ADDRESS);
		Lex buf = curr_lex;
		//
		gl();
		if(c_type == LEX_CONST)		
			{ TID[val].put_assign(); prog.put(curr_lex); gl(); } // assign ?
		else { cout  << endl << endl << "ERROR; In line: " << scan.line << " expected some constant, but got: " ; throw curr_lex; }
		prog.put(buf);
	}

}

void Parser::OPS()
{
	while(true)
	{

 		if(c_type == LEX_CLFIG)
 		{
  			gl();  break;
 		}
 		OP();
	}
}




int Parser::OP()
{

	if(c_type == LEX_IF)
	{
		long pnt1, pnt2;
		gl();
		if(c_type == LEX_OPBRAKET )
			gl();
		else { cout  << endl << endl << "ERROR; In line: " << scan.line << " expected '(', but got: " ; throw curr_lex; }
		TERM();
		pnt1 = prog.get_index();
		prog.blank();
		prog.put(Lex(POLIZ_FGO, 2 , LPOLIZ ));
		if(c_type == LEX_CLBRAKET )
			gl();
		else { cout  << endl << endl << "ERROR; In line: " << scan.line << " expected ')', but got: " ; throw curr_lex; }
		OP();
		pnt2 = prog.get_index();
		prog.blank();
		prog.put(Lex(POLIZ_GO, 1 , LPOLIZ ));
		if(c_type == LEX_ELSE )
			gl();
		else { cout  << endl << endl << "ERROR; In line: " << scan.line << " expected 'else', but got: " ; throw curr_lex; }
		prog.put( Lex( POLIZ_LABEL, prog.get_index(), LPOLIZ), pnt1 );
		OP();
		prog.put( Lex( POLIZ_LABEL, prog.get_index(), LPOLIZ), pnt2 );
		return 1;
	} 
	 
	if(c_type == LEX_WHILE)
	{
		long pnt1, pnt2;
		gl();
		if(c_type == LEX_OPBRAKET )
			gl();
		else { cout << endl  << endl << "ERROR; In line: " << scan.line << " expected '(', but got: " ; throw curr_lex; }
		pnt2 = prog.get_index();
		TERM();
		pnt1 = prog.get_index();
		prog.blank();
		prog.put(Lex(POLIZ_FGO, 2 , LPOLIZ ));
		if(c_type == LEX_CLBRAKET )
			gl();
		else { cout  << endl << endl << "ERROR; In line: " << scan.line << " expected ')', but got: " ; throw curr_lex; }

		OP();
		prog.put( Lex( POLIZ_LABEL, pnt2, LPOLIZ) );
		prog.put(Lex(POLIZ_GO, 1 , LPOLIZ ));
		prog.put( Lex( POLIZ_LABEL, prog.get_index(), LPOLIZ), pnt1 );
		return 1;
	}
	
	if( c_type == LEX_OPFIG)
	{
		gl();
		OPS();
	
	
		return 1;
	}
	
	if(c_type == LEX_READ)
	{
		Lex buf = curr_lex;
		gl();
		if(c_type == LEX_OPBRAKET )
			gl();
		else { cout << endl  << endl << "ERROR; In line: " << scan.line << " expected '(', but got: " ; throw curr_lex; }
		if(c_type == LEX_ID)
				{ 
					prog.put (curr_lex ); 
					prog[ prog.get_index()-1 ].put_type(POLIZ_ADDRESS); 
					gl();
				}
		else { cout << endl  << endl << "ERROR; In line: " << scan.line << " expected identificator, but got: " ; throw curr_lex; }
		if(c_type == LEX_CLBRAKET )
			gl();
		else { cout  << endl << endl << "ERROR; In line: " << scan.line << " expected ')', but got: " ; throw curr_lex; }
		 prog.put ( buf);
		if(c_type == LEX_SEMICOLON)
			{ prog.put (curr_lex ); gl();}
		else { cout  << endl << endl << "ERROR; In line: " << scan.line << " expected ';', but got: " ; throw curr_lex; }
		return 1;
	}
	
	if(c_type == LEX_WRITE)
	{
		int count=0;
		Lex buf = curr_lex;
		gl();
		if(c_type == LEX_OPBRAKET )
			gl();
		else { cout << endl  << endl << "ERROR; In line: " << scan.line << " expected '(', but got: " ; throw curr_lex; }
		TERM();
		count ++;
		while(c_type == LEX_COMMA)
		{
			gl();
			TERM();
			count ++;
		}
		if(c_type == LEX_CLBRAKET )
			gl();
		else { cout << endl  << endl << "ERROR; In line: " << scan.line << " expected ')', but got: " ; throw curr_lex; }
		prog.put( Lex(POLIZ_COUNT, count, LPOLIZ));
		prog.put ( buf);
		if(c_type == LEX_SEMICOLON )
			{ prog.put (curr_lex ); gl();}
		else { cout << endl  << endl << "ERROR; In line: " << scan.line << " expected ';', but got: " ; throw curr_lex; }
		return 1;
	}

	if(c_type == LEX_RETURN )
	{
		gl();
		TERM();
		prog.put ( Lex( LEX_RETURN, scan.look( "return" , TW), LWORD) );
		if(c_type == LEX_SEMICOLON )
			{ prog.put (curr_lex ); gl();}
		else { cout << endl << endl << "ERROR; In line: " << scan.line << " expected ';', but got: " ; throw curr_lex; }
		return 1;
	}
	else
	if( c_type == LEX_SEMICOLON )
	{
		gl();
		return 1;
		
	}
	else
	{		
		
		TERM();
		if(c_type == LEX_SEMICOLON )
			{ prog.put (curr_lex ); gl();}
		else { cout << endl << endl << "ERROR; In line: " << scan.line << " expected ';', but got: " ; throw curr_lex; }
		return 1;
	}


}

void Parser::check_op ( ) 
{
	Lex t1, t2, op;
	type_of_data typ1, typ2;
	t2 = st_lex.pop_back();
	op = st_lex.pop_back();
	t1 =  st_lex.pop_back();
	typ2 = st_type.pop_back();
	typ1 = st_type.pop_back();
	if(op.get_type() == LEX_ASSIGN)
	{
		if(t1.get_type() != LEX_ID) { cout  << endl << endl << "ERROR; In line: " << scan.line << " cannot assign value to a constant: " ; throw curr_lex; }
	}
	

	if( typ1 ==STRING || typ2 == STRING)
	{
		if(typ1+typ2 != STRING*2) { cout  << endl << endl << "ERROR; In line: " << scan.line << " cannot perform any operation to string and not string: " << typ1 << typ2 ; throw curr_lex; }
		if( (op.get_type() == LEX_PLUS) || ( op.get_type() == LEX_ASSIGN ) ) 
		{
			st_lex.push(LEX_CONST);
			st_type.push(STRING);
			prog.put (op );
			return;
		}
		else
			{ cout  << endl << endl << "ERROR; In line: " << scan.line << " cannot perform such operation to string : " ; throw op; }
	}

	if( typ1 ==REAL || typ2 == REAL)
	{
		if(op.get_type() == LEX_MUL  || op.get_type() == LEX_DIV  || op.get_type() == LEX_PLUS || op.get_type() == LEX_MINUS)
			st_type.push(REAL);		
		else
			st_type.push(INT);		
		st_lex.push(LEX_CONST);
		prog.put (op );
		return;
	}
	/*if (op == LEX_PLUS || op == LEX_MINUS ||op == LEX_TIMES ||op == LEX_SLASH)
		r = LEX_INT;
	if (op == LEX_OR || op == LEX_AND)
		t = LEX_BOOL;
	if (t1 == t2 && t1 == t) st_lex.push( r );
	else throw "wrong types are in operation";*/
	st_lex.push(Lex(LEX_CONST));
	st_type.push(INT);		
	prog.put (op );
}

void Parser::TERM()
{
	TERM2();
	if(c_type == LEX_ASSIGN )
	{
		prog[prog.get_index()-1].put_type(POLIZ_ADDRESS);
		st_lex.push(curr_lex);
		gl();
		TERM2();
		check_op();
	}
	

}

void Parser::TERM2()
{
	
	TERM3();
	while(c_type == LEX_OR || c_type == LEX_AND )
	{
		st_lex.push(curr_lex);
		gl();
		TERM3();
		check_op();

	}
	

}

void Parser::TERM3()
{
	TERM4();
	while(c_type == LEX_LESS || c_type == LEX_GREAT || c_type == LEX_EQ || c_type == LEX_NEQ || c_type == LEX_LEQ || c_type == LEX_GEQ)
	{
		st_lex.push(curr_lex);
		gl();
		TERM4();
		check_op();
	}
	
}

void Parser::TERM4()
{
	TERM5();
	while(c_type == LEX_PLUS || c_type == LEX_MINUS )
	{
		st_lex.push(curr_lex);
		gl();
		TERM5();
		check_op();
		//if( !TERM5()) { cout << endl << endl << "ERROR; In line: " << scan.line << " expected second operand, but got: " ; throw curr_lex; }
	}
	
}

void Parser::TERM5()
{
	TERM6();
	while(c_type == LEX_DIV || c_type == LEX_MUL  )
	{
		st_lex.push(curr_lex);
		gl();
		TERM6();
		check_op();
		
	}
	
}
void Parser::TERM6()
{
	if( c_type == LEX_ID)
	{
		if( ! TID[c_val].get_declare() ) { cout  << endl << endl << "ERROR; In line: " << scan.line << " identificator is not declared: " ; throw curr_lex;  }
		
		if(	 TID[c_val].get_type() != FUNCTION	)
		{

			prog.put(curr_lex);
			st_lex.push(curr_lex);
			st_type.push( TID[curr_lex.get_value()].get_type());
			gl();
		}
		else
		{		
			int count = 0;
		//if( ! TID[c_val].get_assign() ) { cout  << endl << endl << "ERROR; In line: " << scan.line << " identificator is not assigned: " ; throw curr_lex;  }
			Lex buf = curr_lex;
			gl();
			if(c_type == LEX_OPBRAKET )			
				gl();
			else  { cout << endl << endl << "ERROR; In line: " << scan.line << " expected '(', but got: " ; throw curr_lex; }

			if(c_type == LEX_CLBRAKET)
			{	
				if(! TID[buf.get_value()].get_function().par_count ) { cout  << endl << endl << "ERROR; In line: " << scan.line << " wrong amount of aprameters in function: " ; throw buf;  }
				gl();
			}
			else				
			{
				TERM();
				if( TID[buf.get_value()].get_function().par_type[count] != st_type.pop_back() ) { cout  << endl << endl << "ERROR; In line: " << scan.line << " wrong types of aprameters in function: " ; throw buf;  }
				st_lex.pop_back();
				count ++;
				while(c_type == LEX_COMMA)
				{
					if( count > TID[buf.get_value()].get_function().par_count )  { cout  << endl << endl << "ERROR; In line: " << scan.line << " wrong amount of aprameters in function: " ; throw buf;  }
					gl();
					TERM();
					if( TID[buf.get_value()].get_function().par_type[count] != st_type.pop_back() ) { cout  << endl << endl << "ERROR; In line: " << scan.line << " wrong types of aprameters in function: " ; throw buf;  }
					st_lex.pop_back();
					count++;
				}
				if(c_type == LEX_CLBRAKET )
					gl();
				else { cout << endl << endl << "ERROR; In line: " << scan.line << " expected ')', but got: " ; throw curr_lex; }
				if( count < TID[buf.get_value()].get_function().par_count )  { cout  << endl << endl << "ERROR; In line: " << scan.line << " wrong amount of aprameters in function: " ; throw buf;  }
			}
			buf.put_type(POLIZ_FUNCTION);
			st_lex.push(Lex(LEX_CONST));
			//cout << TID[buf.get_value()].get_function().type ;
			st_type.push( TID[buf.get_value()].get_function().type );
			prog.put(buf);

			
		}
		
	}
	else
	if( c_type == LEX_CONST)
	{
		prog.put(curr_lex);
		st_lex.push(  curr_lex  );
		st_type.push( TC[curr_lex.get_value()].get_type());
		gl();
		
	}
	else	
	if( c_type == LEX_NOT)
	{
		gl();
		TERM6();
		prog.put (Lex (LEX_NOT));
		
	}
	else
	/*if(c_type == LEX_MINUS)
    {
    	curr_lex.put_type(LEX_UMINUS);
        gl ();
      	TERM6();
    }
 	 else*/
	if (c_type == LEX_OPBRAKET )
	{
		gl();
		TERM();
		if(c_type == LEX_CLBRAKET )
			gl();
		else { cout << endl << endl << "ERROR; In line: " << scan.line << " expected ')', but got: " ; throw curr_lex; }
		
	}
	else { cout << endl << endl << "ERROR; In line: " << scan.line << " expected operand, but got: " ; throw curr_lex; }
	
}


class  Interpreter
{
	Parser pars;
	Stack<type_of_data> st_switch;

	
	Stack<char*> st_string;
	Stack<double> st_real;
	Stack<long> st_long;
	Stack<int> st_int;	
	void FunctionHandler(Lex l);
	
public:
	Interpreter (const char *program) : pars (program) { }
	void run();
	void execute(long);
	
};


void Interpreter::run()
{
	//Stack<long> st_long;
	
	long enter_point;
	//cout <<'#';
	enter_point = pars.analyze();
	//cout <<'#';
	execute(enter_point);

}

void Interpreter::FunctionHandler(Lex l)
{
	
	Function f = TID[l.get_value()].get_function();
	long  size = st_switch.size() ,icount=0, rcount=0,scount=0, count = f.par_count, shift = size - count;

	for(int i = shift; i< size; i++)
	{
		//cout << "{" << st_switch[i] << "}";
		switch(st_switch[i])
		{
			case INT: icount++; break;
 			case REAL: rcount ++; break;
 			case STRING: scount ++; break;
		}
	}

	for(int i =0; i < count; i++)  // Assignment of the parameters of the function
	{
		//TID[f.par_adr[i]].put_value( st_int[st_int.size()-icount--]  )
		switch(st_switch[shift + i])
		{
			case INT: TID[f.par_adr[i]].put_value( st_int[st_int.size()-icount--]  ); break;
			case REAL: TID[f.par_adr[i]].put_value( st_real[st_real.size()-rcount--]  );break;
			case STRING: TID[f.par_adr[i]].put_value( st_string[st_string.size()-scount--]  ); break;
		}
		TID[f.par_adr[i]].put_assign();
	}

	for(int i = 0; i< count; i++) // Deletion of the parameters of the function
	{
		//cout << "{" << st_switch[i] << "}";
		switch(st_switch.pop_back())
		{
			case INT: st_int.pop_back(); break;
		 	case REAL: st_real.pop_back(); break;
		 	case STRING: st_string.pop_back(); break;
		}
	}

	st_switch.push(NONE); // // NONE - is a marker of another function's stack area
	execute(f.enter_point);
	switch(st_switch.pop_back()) // Here we remove NONE marker
	{
		case INT: st_switch.pop_back(); st_switch.push(INT); break;
 		case REAL: st_switch.pop_back(); st_switch.push(REAL);break;
 		case STRING: st_switch.pop_back(); st_switch.push(STRING); break;
	}

}

void Interpreter::execute(long l)
{
	
	Lex curr_lex;
	double temp_d,j;
	long temp_l,i;
	int temp_i;
	char* temp_s;
	
	while( prog[l].get_type() != LEX_FIN  )
	{
		curr_lex = prog[l];
		switch(prog[l].get_type())  
		{	
			case POLIZ_COUNT: case POLIZ_LABEL: case POLIZ_ADDRESS:
			st_long.push ( curr_lex.get_value () ); //st_switch.push(1);
			break;

			case POLIZ_FUNCTION:
			FunctionHandler(curr_lex);
			break;

			case LEX_ID:
			i = curr_lex.get_value ( );
			if ( TID [ i ].get_assign ( ) )	
			{
			//cout << *((int*)TID[i].get_value ()) ;
		 		switch(TID[i].get_type())
		 		{
		 			case INT: st_int.push ( *((int*)TID[i].get_value ()) ); st_switch.push(INT); break;
		 			case REAL: st_real.push ( *((double*)TID[i].get_value ()) ); st_switch.push(REAL); break;
		 			case STRING: char* temp = new char[strlen( (char*) TID[i].get_value()) +1]; strcpy(temp, (char*) TID[i].get_value() ) ; st_string.push ( temp ); st_switch.push(STRING); break;
		 			//default: break;
		 		}
		 		//cout << *((int*)TID[i].get_value()) << '#' << i << '#' << st_int.top() << endl;		
				//st_long.push ( TID[i].get_value () );
				break;
			}
			else
				{	cout << endl << endl << "ERROR; indefinite identifier: "  ; throw TID[i].get_name(); }
			

			case LEX_CONST:
			i = curr_lex.get_value ( );	
			//cout << *((int*)TC[i].get_value ()) << " - ";
		 	switch(TC[i].get_type())
		 	{
		 		case INT: st_int.push ( *((int*)TC[i].get_value ()) );st_switch.push(INT); break;
		 		case REAL: st_real.push ( *((double*)TC[i].get_value ()) );st_switch.push(REAL); break;
		 		case STRING: st_string.push ( (char*) TC[i].get_value() ); st_switch.push(STRING); break;
		 	}
		 	//cout << st_long.top() << endl;
			//st_long.push ( TID[i].get_value () );
			break;
			

			case LEX_NOT:
			switch(st_switch.pop_back())
			{
				case INT: st_int.push( !st_int.pop_back() );break;
				case REAL: st_int.push( !st_real.pop_back() );break;
			}
			st_switch.push(INT);			
			break;

			case LEX_OR:	
			switch(st_switch.pop_back())
			{
				case INT: temp_d = st_int.pop_back() ;break;
				case REAL: temp_d = st_real.pop_back() ;break;
			}			
			switch(st_switch.pop_back())
			{
				case INT: st_int.push( temp_d || st_int.pop_back() ) ;break;
				case REAL: st_int.push( temp_d || st_real.pop_back() );break;
			}
			st_switch.push(INT);	
			break;

			case LEX_AND:
			switch(st_switch.pop_back())
			{
				case INT: temp_d = st_int.pop_back() ;break;
				case REAL: temp_d = st_real.pop_back() ;break;
			}			
			switch(st_switch.pop_back())
			{
				case INT: st_int.push( temp_d && st_int.pop_back() ) ;break;
				case REAL: st_int.push( temp_d && st_real.pop_back() );break;
			}
			st_switch.push(INT);	// result of logic operations is always int
			break;

			case POLIZ_GO:
			l = st_long.pop_back() - 1; //st_switch.pop_back();
			break;

			case LEX_SEMICOLON:	
			{
				int j = st_switch.size() ;
				type_of_data type;
				while(st_switch.size() && (st_switch.top()) != NONE) // NONE - is a marker of another function's stack area
				{
					type = st_switch.pop_back();
					switch(type)  // CLEAR all stack area of current function
					{
						case INT: st_int.pop_back(); break;
		 				case REAL: st_real.pop_back(); break;
		 				case STRING: st_string.pop_back(); break;
					}
				}
				//cout << "\n#" << st_switch.size() + st_string.size() << " - " << j << "#\n";
			}
			break;
			/*
			st_real.clear();
			st_long.clear();
			st_switch.clear();
			st_string.clear();
			st_int.clear();		*/
		

			case POLIZ_FGO:
			i = st_long.pop_back();// st_switch.pop_back();
			switch(st_switch.pop_back())
			{
				case INT: temp_d = st_int.pop_back() ;break;
				case REAL: temp_d = st_real.pop_back() ;break;
			}		
			if ( !temp_d) l = i-1;
			break;

			case LEX_WRITE:
			{
				long count = st_long.pop_back(), size = st_switch.size(), i,icount=0, rcount=0,scount=0;
				//cout << "\n@" << size << " " << count << "\n";
				//fflush(NULL);
				for( i = size-count; i< size; i++)
				{
					//cout << "{" << st_switch[i] << "}";
					switch(st_switch[i])
					{
						case INT: icount++; break;
			 			case REAL: rcount ++; break;
			 			case STRING: scount ++; break;
					}
				}
			
				
				for( i = size-count; i< size; i++)
				{
					//cout << "{" << st_switch[i] << "}";
					switch(st_switch[i])
					{
						case INT: cout << st_int[st_int.size()-icount--]; break;
			 			case REAL: cout << st_real[st_real.size()-rcount--]; break;
			 			case STRING: cout <<  st_string[st_string.size()-scount--]; break;
					}
				}


					

			}
			break;

			/*
		


				while(st_switch.size())
				{
					switch(st_switch.pop_back())
					{
						case INT: cout << st_int.pop_back(); break;
			 			case REAL: cout <<st_real.pop_back(); break;
			 			case STRING:  cout << st_string.pop_back(); break;
					}

				}
			*/
		
			case LEX_READ: 
			{ 
				
				i = st_long.pop_back ( );//st_switch.pop_back();
				switch ( TID [ i ].get_type () ) 
				{
					case INT: cin >> temp_i; TID [ i ].put_value (temp_i); break;
					case STRING: cin >> temp_s; TID[ i ].put_value(temp_s); break;
					case REAL: cin >> temp_d; TID [ i ].put_value (temp_d); break;  // TODO: string s/better put_value for ident/ functions? / contecstual checks (i.e type checks) / rest of execute
				}		
				TID [ i ].put_assign ();
			
			}
			break; 

			case LEX_PLUS:
			{
				type_of_data type = INT;
				switch(st_switch.pop_back())
				{
					case INT: temp_i = st_int.pop_back() ; break;
					case REAL: temp_d = st_real.pop_back() ; type =REAL; break;
					case STRING: temp_s = st_string.pop_back() ;break;
				}
				//cout << st_string.top() << " + " << temp_s << endl;
				//fflush(NULL);			
				switch(st_switch.pop_back())
				{
					case INT: st_switch.push(type);
					if(type==INT) st_int.push( temp_i + st_int.pop_back() );
					else  st_real.push( temp_d + st_int.pop_back() );
					break;
					
					case REAL: st_switch.push(REAL);
					if(type==INT) st_real.push( temp_i + st_real.pop_back() );
					else  st_real.push( temp_d + st_real.pop_back() );
					break;

					case STRING: char* temp = new char[ strlen(temp_s) + strlen( st_string.top()) + 1]; 
					*temp ='\0';
					strcat(temp, st_string.pop_back() ); 
					st_string.push( strcat( temp, temp_s )     ) ; 
					st_switch.push(STRING); 
					break;
				}	
				
			}		
			break;

			case LEX_MUL:
			{
				type_of_data type = INT;
				switch(st_switch.pop_back())
				{
					case INT: temp_i = st_int.pop_back() ; break;
					case REAL: temp_d = st_real.pop_back() ; type =REAL; break;
					
				}			
				switch(st_switch.pop_back())
				{
					case INT: st_switch.push(type);
					if(type==INT) st_int.push( temp_i * st_int.pop_back() );
					else  st_real.push( temp_d * st_int.pop_back() );
					break;
					
					case REAL: st_switch.push(REAL);
					if(type==INT) st_real.push( temp_i * st_real.pop_back() );
					else  st_real.push( temp_d * st_real.pop_back() );
					break;					
				}	
				
			}
			break;

			case LEX_MINUS:			
			{
				type_of_data type = INT;
				switch(st_switch.pop_back())
				{
					case INT: temp_i = st_int.pop_back() ; break;
					case REAL: temp_d = st_real.pop_back() ; type =REAL; break;
					
				}			
				switch(st_switch.pop_back())
				{
					case INT: st_switch.push(type);
					if(type==INT) st_int.push( st_int.pop_back() - temp_i );
					else  st_real.push(st_int.pop_back() - temp_d );
					break;
					
					case REAL: st_switch.push(REAL);
					if(type==INT) st_real.push( st_real.pop_back() - temp_i);
					else  st_real.push( st_real.pop_back() - temp_d);
					break;					
				}	
				
			}
			break;

			case LEX_DIV:
			{
				type_of_data type = INT;
				switch(type = st_switch.pop_back())
				{
					case INT: temp_i = st_int.pop_back() ; if(!temp_i) throw "POLIZ:divide by zero"; break;
					case REAL: temp_d = st_real.pop_back() ;  if(!temp_d) throw "POLIZ:divide by zero"; break;
					
				}			
				switch(st_switch.pop_back())
				{
					case INT: st_switch.push(type);
					if(type==INT) st_int.push( st_int.pop_back() / temp_i );
					else  st_real.push(st_int.pop_back() / temp_d );
					break;
					
					case REAL: st_switch.push(REAL);
					if(type==INT) st_real.push( st_real.pop_back() / temp_i);
					else  st_real.push( st_real.pop_back() / temp_d);
					break;					
				}	
				
			}
			break;

			case LEX_EQ:
			switch(st_switch.pop_back())
			{
				case INT: temp_d = st_int.pop_back() ;break;
				case REAL: temp_d = st_real.pop_back() ;break;
			}			
			switch(st_switch.pop_back())
			{
				case INT: st_int.push( st_int.pop_back() == temp_d) ;break;
				case REAL: st_int.push( st_real.pop_back() ==temp_d );break;
			}
			st_switch.push(INT);
			break;

			case LEX_LESS:			
			switch(st_switch.pop_back())
			{
				case INT: temp_d = st_int.pop_back() ;break;
				case REAL: temp_d = st_real.pop_back() ;break;
			}			
			switch(st_switch.pop_back())
			{
				case INT: st_int.push( st_int.pop_back() < temp_d) ;break;
				case REAL: st_int.push( st_real.pop_back() < temp_d );break;
			}
			st_switch.push(INT);
			break;

			case LEX_GREAT:			
			switch(st_switch.pop_back())
			{
				case INT: temp_d = st_int.pop_back() ;break;
				case REAL: temp_d = st_real.pop_back() ;break;
			}			
			switch(st_switch.pop_back())
			{
				case INT: st_int.push( st_int.pop_back() > temp_d) ;break;
				case REAL: st_int.push( st_real.pop_back() > temp_d );break;
			}
			st_switch.push(INT);
			break;

			case LEX_LEQ:			
			switch(st_switch.pop_back())
			{
				case INT: temp_d = st_int.pop_back() ;break;
				case REAL: temp_d = st_real.pop_back() ;break;
			}			
			switch(st_switch.pop_back())
			{
				case INT: st_int.push( st_int.pop_back() <= temp_d) ;break;
				case REAL: st_int.push( st_real.pop_back() <=temp_d );break;
			}
			st_switch.push(INT); 
			break;

			case LEX_GEQ:
			switch(st_switch.pop_back())
			{
				case INT: temp_d = st_int.pop_back() ;break;
				case REAL: temp_d = st_real.pop_back() ;break;
			}			
			switch(st_switch.pop_back())
			{
				case INT: st_int.push( st_int.pop_back() >= temp_d) ;break;
				case REAL: st_int.push( st_real.pop_back() >=temp_d );break;
			}
			st_switch.push(INT);
			break;

			case LEX_NEQ:			
			switch(st_switch.pop_back())
			{
				case INT: temp_d = st_int.pop_back() ;break;
				case REAL: temp_d = st_real.pop_back() ;break;
			}			
			switch(st_switch.pop_back())
			{
				case INT: st_int.push( st_int.pop_back() != temp_d) ;break;
				case REAL: st_int.push( st_real.pop_back() !=temp_d );break;
			}
			st_switch.push(INT);
			break;

			case LEX_RETURN:
			return;

			case LEX_ASSIGN:
			{
				type_of_data type;
				switch ( type = st_switch.pop_back() ) 
				{
					case INT: temp_i = st_int.pop_back() ;break;
					case STRING: temp_s = st_string.pop_back() ;break;
					case REAL: temp_d = st_real.pop_back() ;break;  
				}		
				i = st_long.pop_back();
				switch ( TID[i].get_type() ) 
				{
					case INT:  
					TID [ i ].put_value ( ( type == INT ? temp_i : (int)temp_d ) ); 
					st_int.push( ( type == INT ? temp_i : (int)temp_d ) ) ;
					st_switch.push(INT);
					break;

					case STRING:  
					TID [ i ].put_value ( temp_s); 
					st_string.push( temp_s ) ;
					st_switch.push(STRING);
					break;

					case REAL:  
					TID [ i ].put_value ( ( type == REAL ? temp_d : (double)temp_i ) ); 
					st_real.push( ( type == REAL ? temp_d : (double)temp_i ) ) ;
					st_switch.push(REAL);
					break;  // TODO: string s/better put_value for ident/ functions? / contecstual checks (i.e type checks) / rest of execute
				}
				TID[i].put_assign(); 
			}		
			//cout << *((int*)TID[i].get_value()) << '#' << i << '#' << temp_i << endl;		
			break;

			default: throw "POLIZ: unexpected elem"; 
		
		} //end of switch
		
		l++;

	}

}


int main(int argc, char const *argv[])
{
	try{
	Interpreter p(argv[1]);	
	p.run();
} catch(const char * s) { cout << s;}
catch(...){ cout << "huh";}
//Ident i;
//i.put_value( Function(INT, 32));
//i.get_function().enter_point = 42;
//cout << ((Function*)i.get_value())->enter_point;
//  void * pt = new int;
//tabl_ident TID(22);
//Parser p(argv[1]);
//p.analyze();
	//long j = TC.put( Const(STRING));
	//Const a(STRING);
	//TC[j].put_value("Hello world!\n");
	//cout << (char*) TC[j].get_value();
	//long j=TID.put("cock1");
	
	//TID[j].put_type(INT);  
	//TID[j].put_value(42.3);
	//cout << *((int*)TID[j].get_value()) <<j;
 // *( (int*) pt) = 4;
 // cout << *( (int*) pt);
 // delete (int*) pt;
	//Ident a;
	//a.put_value(5);
	//cout << *((int*)a.get_value() );
	return 0;
}
