#include "Engine/Core/Strings/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"

#ifdef PLATFORM_WINDOWS
#include "Engine/Core/Graphics/Rgba.hpp"
#endif

#include <stdexcept>
#include <stdarg.h>
#include <stack>


//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)



	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf( textLiteral, maxLength, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)


	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

//--------------------------------------------------------------------------
/**
* SplitStringOnDelitmiter
*/
std::vector< std::string >SplitStringOnDelitmiter( const char* text, const char* delimiter )
{
	std::vector<std::string> result;
	size_t split;
	std::string curText = text;
	do 
	{
		split = curText.find_first_of( delimiter );
		std::string left( curText, 0, split );
		std::string right( curText, split + 1 );
		if( left != "" )
		{
			result.push_back(left);
		}
		curText = right;
	} while ( split != std::string::npos && !curText.empty() );

	return result;
}

//--------------------------------------------------------------------------
/**
* FloatToString
*/
std::string FloatToString( float convert )
{
	return Stringf( "%f", convert );
}

//--------------------------------------------------------------------------
/**
* FloatToString
*/
std::string IntToString( int convert )
{
	return Stringf( "%d", convert );
}

//--------------------------------------------------------------------------
/**
* StringToFloat
*/
float StringToFloat( const std::string& convert )
{
	return (float) atof( convert.c_str() );
}

//--------------------------------------------------------------------------
/**
* StringToInt
*/
int StringToInt( const std::string& convert )
{
	return atoi( convert.c_str() );
}


//--------------------------------------------------------------------------
// Helpers for equating string
//--------------------------------------------------------------------------
static bool isClosed( const std::string& check_if_closed)
{
	if (check_if_closed == ")" || check_if_closed == "]" || check_if_closed == "}")
	{
		return true;
	}
	return false;
}
//--------------------------------------------------------------------------
bool IsOpen( std::string check_if_open)
{
	if (check_if_open == "(" || check_if_open == "[" || check_if_open == "{")
	{
		return true;
	}
	return false;
}
//--------------------------------------------------------------------------
static int GetOperator( const std::string& check_what_Operator ) // returns 1 if (, returns 2 if +,- returns 3 if /*%, else, is ) then return 4, else return 0
{
	if (check_what_Operator == "(" || check_what_Operator == "[" || check_what_Operator == "{")
	{
		return 1;
	}
	else if (check_what_Operator == "+" || check_what_Operator == "-")
	{
		return 2;
	}
	else if (check_what_Operator == "/" || check_what_Operator == "*" )
	{
		return 3;
	}
	else if( check_what_Operator == "^" )
	{
		return 4;
	}
	else if (check_what_Operator == ")" || check_what_Operator == "]" || check_what_Operator == "}")
	{
		return 5;
	}
	else
	{
		return 0;
	}
}
//--------------------------------------------------------------------------
static bool IsFloat( const std::string& string )
{
	try 
	{
		stof( string );
		return true;
	}
	catch (const std::invalid_argument& ia) {
		UNUSED( ia );
		// Not a float
		return false;
	}
}
//--------------------------------------------------------------------------
static bool IsBalanced( const std::vector<std::string>& expression )
{
	std::stack<std::string> my_stack;
	for( unsigned int optIdx = 0; optIdx < (unsigned int) expression.size(); ++optIdx )
	{
		const std::string& to_test = expression[optIdx];
		if (IsOpen(to_test))
		{
			my_stack.push(to_test);
		}
		else if (isClosed(to_test))
		{
			if (!my_stack.empty())
			{
				if ((my_stack.top() == "[" && to_test == "]") || (my_stack.top() == "(" && to_test == ")") || (my_stack.top() == "{" && to_test == "}"))
				{
					my_stack.pop();
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	}
	if (my_stack.size() == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//--------------------------------------------------------------------------
// Didn't end up using, though correct. Could be of use later.
//--------------------------------------------------------------------------
// static std::string postfixToInfix( const std::string& postfixExpression )
// {
// 	std::vector<std::string> splitOpt = SplitStringOnDelitmiter( postfixExpression.c_str(), " " );
// 
// 	if( IsBalanced( splitOpt ) )
// 	{
// 		std::stack<std::string> my_stack;
// 
// 		int int_c = 0;
// 		int op_c = 0;
// 
// 		for( unsigned int optIdx = 0; optIdx < (unsigned int) splitOpt.size(); ++optIdx )
// 		{
// 			const std::string& x = splitOpt[optIdx];
// 
// 			if( IsFloat( x ) )
// 			{
// 				my_stack.push( x );
// 				++int_c;
// 			}
// 			else if ( GetOperator( x ) == 4 || GetOperator( x ) == 3 || GetOperator( x ) == 2 )
// 			{
// 				if ( my_stack.size() < 2 )
// 				{
// 					ERROR_AND_DIE( "INVALID USE of OP" );
// 				}
// 				else
// 				{
// 					std::string y;
// 					std::string z;
// 					std::string res;
// 
// 					y = my_stack.top();
// 					my_stack.pop();
// 					z = my_stack.top();
// 					my_stack.pop();
// 					res = "( " + z + " " + x + " " + y + " )";	//	( z x y )
// 
// 					my_stack.push(res);
// 					op_c++;
// 				}
// 			}
// 			else
// 			{
// 				ERROR_AND_DIE( "UNKNOWN OPT" );
// 			}
// 		}
// 		if ( my_stack.size() == 1 && ( ( int_c - 1 ) == op_c ) )
// 		{
// 			return my_stack.top();
// 		}
// 		else
// 		{
// 			ERROR_AND_DIE( "Not enough Opts" );
// 		}
// 	}
// 	else
// 	{
// 		ERROR_AND_DIE( "Not Balanced" );
// 	}
// }
//--------------------------------------------------------------------------
static std::string InfixToPostfix( const std::string& infixExpression )
{
	std::vector<std::string> splitOpt = SplitStringOnDelitmiter( infixExpression.c_str(), " " );

	if( IsBalanced( splitOpt ) )
	{
		std::stack<std::string> my_stack;
		std::string res = "";
		int int_c = 0;
		int op_c = 0;

		for( unsigned int optIdx = 0; optIdx < (unsigned int) splitOpt.size(); ++optIdx )
		{
			const std::string& x = splitOpt[optIdx];

			if ( IsFloat( x ) )
			{
				if (res == "")
				{
					res = x;
					++int_c;
				}
				else
				{
					res = Stringf( "%s %s", res.c_str(), x.c_str() );
					++int_c;
				}
			}
			else if ( GetOperator(x) == 1 )
			{
				my_stack.push(x);
			}
			else if ( GetOperator(x) == 2 )
			{
				op_c++;
				if (my_stack.empty())
				{
					my_stack.push(x);
				}
				else
				{

					while( GetOperator( my_stack.top() ) >= 2 ) // make sure it doesnt call top on an empty stack
					{

						res = Stringf( "%s %s",  res.c_str(), my_stack.top().c_str() );
						my_stack.pop();

						if ( my_stack.empty() )
						{
							break;
						}
					}
					if ( my_stack.empty() )
					{
						my_stack.push(x);
					}
					else if ( GetOperator(my_stack.top() ) < 2)
					{
						my_stack.push(x);
					}
				}
			}
			else if ( GetOperator(x) == 3 )
			{
				op_c++;
				if ( my_stack.empty() )
				{
					my_stack.push(x);
				}
				else
				{
					while ( GetOperator( my_stack.top() ) >= 3 ) // make sure it doesnt call top on an empty stack
					{
						res = Stringf( "%s %s",  res.c_str(), my_stack.top().c_str() );
						my_stack.pop();

						if (my_stack.empty())
						{
							break;
						}
					}
					if (my_stack.empty())
					{
						my_stack.push(x);
					}
					else if ( GetOperator(my_stack.top() ) < 3 )
					{
						my_stack.push(x);
					}
				}
			}
			else if ( GetOperator(x) == 4 )
			{
				op_c++;
				if ( my_stack.empty() )
				{
					my_stack.push(x);
				}
				else
				{
					while ( GetOperator( my_stack.top() ) >= 4 ) // make sure it doesnt call top on an empty stack
					{
						res = Stringf( "%s %s",  res.c_str(), my_stack.top().c_str() );
						my_stack.pop();

						if (my_stack.empty())
						{
							break;
						}
					}
					if (my_stack.empty())
					{
						my_stack.push(x);
					}
					else if ( GetOperator(my_stack.top() ) < 4 )
					{
						my_stack.push(x);
					}
				}
			}
			else if ( GetOperator(x) == 5 )
			{
				if (my_stack.empty())
				{
					ERROR_AND_DIE( Stringf( "Invalid us if %s", x.c_str() ).c_str() );
				}
				else
				{
					while (my_stack.top() != "(")
					{
						res = Stringf( "%s %s",  res.c_str(), my_stack.top().c_str() );
						my_stack.pop();
						if (my_stack.empty())
						{
							ERROR_AND_DIE( Stringf( "Invalid string: %s", infixExpression.c_str() ) );
						}
					}
					my_stack.pop();
				}
			}
			else
			{
				return "invalid";
			}
		}
		if ((op_c + 1) == int_c)
		{
			while (!my_stack.empty())
			{
				res = Stringf( "%s %s",  res.c_str(), my_stack.top().c_str() );
				my_stack.pop();
			}
			return res;
		}
		else
		{
			ERROR_AND_DIE( Stringf( "Invalid string: %s", infixExpression.c_str() ) );
		}
	}
	else
	{
		ERROR_AND_DIE( Stringf( "Invalid string: %s", infixExpression.c_str() ) );
	}

}
//--------------------------------------------------------------------------
// End Helpers
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
/**
* EquateString
*/
float EquateString( std::string string )
{
//	return StringToFloat( string );

	std::string postFixExp = InfixToPostfix( string );
	std::vector<std::string> splitOpt = SplitStringOnDelitmiter( postFixExp.c_str(), " " );
	
	std::stack<float> my_stack;
	for( unsigned int optIdx = 0; optIdx < (unsigned int) splitOpt.size(); ++optIdx )
	{
		std::string& curSeg = splitOpt[optIdx];
		if( IsFloat( curSeg ) )
		{
			my_stack.push( StringToFloat( curSeg ) );
		}
		else if ( GetOperator(curSeg) == 1 )
		{
			ERROR_AND_DIE( Stringf( "Unsupported Opertator: %s", curSeg.c_str() ).c_str() );
		}
		else if ( GetOperator(curSeg) == 2 )
		{
			if (my_stack.size() >= 2)
			{
				float a = my_stack.top();
				my_stack.pop();

				float b = my_stack.top();
				my_stack.pop();

				float curAns;
				if (curSeg == "+")
				{
					curAns = b + a;		//	( b curSeg a )
				}
				else if (curSeg == "-")
				{
					curAns =  b - a;	//	( b curSeg a )
				}

				my_stack.push( curAns );
			}
			else
			{
				ERROR_AND_DIE( "Incorrect Placement of args" );
			}
		}
		else if( GetOperator(curSeg) == 3 ||  GetOperator(curSeg) == 4 )
		{
			if( my_stack.size() >= 2 )
			{
				float a = my_stack.top();
				my_stack.pop();

				float b = my_stack.top();
				my_stack.pop();

				float tempAns = 0;
				if (curSeg == "*")
				{
					tempAns = b * a;	//	( b curSeg a )
				}
				else if (curSeg == "/")
				{
					if (a != 0)
					{
						tempAns = (b / a); //	( b curSeg a )
					}
					else
					{
						ERROR_AND_DIE( "Can't divide by 0" );
					}
				}
				else if (curSeg == "^")
				{
						
					tempAns = powf( b, a );
				}
				my_stack.push( tempAns );
			}
			else
			{
				ERROR_AND_DIE( "Incorrect Placement of args" );
			}
		}
		else if ( GetOperator(curSeg) == 5 )
		{
			ERROR_AND_DIE( Stringf( "Unsupported Opertator: %s", curSeg.c_str() ).c_str() );
		}
	}
	return my_stack.top();
}

//--------------------------------------------------------------------------
/**
* ToString
*/
std::string ToString( const std::string& string )
{
	return string;
}

//--------------------------------------------------------------------------
/**
* ToString
*/
std::string ToString( void const*const ptr )
{
	UNUSED(ptr);
	return "";
}

//--------------------------------------------------------------------------
/**
* ToString
*/
std::string ToString( const Vec2& vec )
{
	return Stringf( "%f,%f", vec.x, vec.y );
}

//--------------------------------------------------------------------------
/**
* ToString
*/
std::string ToString( const IntVec2& vec )
{
	return Stringf( "%d,%d", vec.x, vec.y );
}

//--------------------------------------------------------------------------
/**
* ToString
*/
std::string ToString( const Vec3& vec )
{
	return Stringf( "%f,%f,%f", vec.x, vec.y, vec.z );
}

//--------------------------------------------------------------------------
/**
* ToString
*/
std::string ToString( const Vec4& vec )
{
	return Stringf( "%f,%f,%f,%f", vec.x, vec.y, vec.z, vec.w );
}

//--------------------------------------------------------------------------
/**
* ToString
*/
#ifdef PLATFORM_WINDOWS
std::string ToString( const Rgba& color )
{
	return Stringf( "%f,%f,%f,%f", color.r, color.g, color.b, color.a );
}
#endif

//--------------------------------------------------------------------------
/**
* ToString
*/
std::string ToString( const FloatRange& range )
{
	return Stringf( "%f,%f", range.min, range.max );
}

//--------------------------------------------------------------------------
/**
* ToString
*/
std::string ToString( const IntRange& range )
{
	return Stringf( "%d,%d", range.min, range.max );
}

//--------------------------------------------------------------------------
/**
* ToString
*/
std::string ToString( float mFloat )
{
	return FloatToString( mFloat );
}

//--------------------------------------------------------------------------
/**
* ToString
*/
std::string ToString( int mInt )
{
	return IntToString( mInt );
}

//--------------------------------------------------------------------------
/**
* ToString
*/
std::string ToString( bool mBool )
{
	return mBool ? "true" : "false";
}

