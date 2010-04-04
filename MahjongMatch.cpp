
//
// MahjongMatch.cpp
//

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string>
#include <set>


typedef std::set<std::string> StringSet;
StringSet Result;


struct TE {
	char L;
	char R[10];
	char K[10];
	char S[8];
	char M[2];

	TE( void ) {
		Clear();
	}

	void Clear( void ) {
		L = 0;
		memset( R, 0, sizeof(R) );
		memset( K, 0, sizeof(K) );
		memset( S, 0, sizeof(S) );
		memset( M, 0, sizeof(M) );
	}

	bool Parse( int Length, const char *Input ) {
		Clear();
		if ( Length < 1 || Input == NULL ) {
			return false;
		}
		L = Length;
		while ( --Length >= 0 ) {
			static const char Symbols[] = "123456789";
			const char *index = strchr( Symbols, *Input++ );
			if ( index == NULL ) {
				return false;
			}
			++(R[index - Symbols + 1]);
		}
		return true;
	}

	void Dump( void ) const {
		int i;
		for ( i = 1; i <= 9; ++i ) {
			int j;
			for ( j = 0; j < R[i]; ++j ) {
				printf( "%d", i );
			}
		}
		printf( "\n" );
	}

	// add match result to Result
	void OK( void ) const {
		char buf[64];
		buf[0] = 0;
		int i;
		for ( i = 1; i <= 9; ++i ) {
			if ( R[i] > 0 ) {
				strcat( buf, "(" );
				int c = R[i];
				while ( --c >= 0 ) {
					sprintf( buf + strlen(buf), "%d", i );
				}
				strcat( buf, ")" );
			}
			if ( K[i] > 0 ) {
				sprintf( buf + strlen(buf), "(%d%d%d)", i, i, i );
			}
			if ( i <= 7 ) {
				int c = S[i];
				while ( --c >= 0 ) {
					sprintf( buf + strlen(buf), "(%d%d%d)", i, i + 1, i + 2 );
				}
			}
		}
		sprintf( buf + strlen(buf), "[%d", M[0] );
		if ( M[1] > 0 ) {
			sprintf( buf + strlen(buf), "%d", M[1] );
		}
		strcat( buf, "]" );

		Result.insert( std::string(buf) );
	}

	void Toitoi( void ) const {
		TE te = *this;
		int t = 0;
		int i;
		for ( i = 1; i <= 9; ++i ) {
			if ( R[i] == 1 ) {
				te.L -= 1;
				te.R[i] -= 1;
				te.M[0] = i;
			}
			if ( R[i] == 2 ) {
				++t;
			}
		}
		if ( t == 6 ) {
			te.OK();
		}
	}

	void Match( void ) const {
		Toitoi();

		int m;
		for ( m = 1; m <= 9; ++m ) {
			if ( R[m] < 1 ) {
				continue;
			}

			// tanki
			TE te;
			te = *this;
			te.L -= 1;
			te.R[m] -= 1;
			te.M[0] = m;
			te.Match( 0 );

			// syabo
			if ( R[m] >= 2 ) {
				te.L -= 1;
				te.R[m] -= 1;
				te.M[1] = m;
				te.Match( 0 );
			}

			// ryanmen or penchan
			if ( m <= 8 && R[m + 1] >= 1 ) {
				te = *this;
				te.L -= 2;
				te.R[m] -= 1;
				te.R[m + 1] -= 1;
				te.M[0] = m;
				te.M[1] = m + 1;
				te.Match( 0 );
			}

			// kanchan
			if ( m <= 7 && R[m + 2] >= 1 ) {
				te = *this;
				te.L -= 2;
				te.R[m] -= 1;
				te.R[m + 2] -= 1;
				te.M[0] = m;
				te.M[1] = m + 2;
				te.Match( 0 );
			}
		}
	}

	bool End( void ) const {
		if ( L == 0 ) {
			// tanki
			if ( M[1] == 0 ) {
				OK();
			}
			return true;
		}
		if ( L == 2 ) {
			// syano or ryanmen
			if ( M[1] != 0 ) {
				int r;
				for ( r = 1; r <= 9; ++r ) {
					if ( R[r] == 2 ) {
						OK();
						return true;
					}
				}
			}
			return true;
		}
		return false;
	}

	// koutu match
	void Match( int k ) {
		SubMatch( 1 );
		while ( ++k <= 9 ) {
			if ( R[k] >= 3 ) {
				TE te = *this;
				te.L -= 3;
				te.R[k] -= 3;
				te.K[k] = 1;
				te.Match( k );
			}
		}
	}

	// shuntu match
	void SubMatch( int s ) {
		if ( End() ) {
			return;
		}
		if ( s <= 7 ) {
			if ( R[s] > 0 && R[s + 1] > 0 && R[s + 2] > 0 ) {
				TE te = *this;
				te.L -= 3;
				--te.R[s];
				--te.R[s + 1];
				--te.R[s + 2];
				++te.S[s];
				te.SubMatch( s );
			}
			SubMatch( s + 1 );
		}
	}
};


//

static bool Enumerate( const char *Input )
{
#define MATCH_LENGTH 13
	TE te;
	if ( Input == NULL || !te.Parse( MATCH_LENGTH, Input ) ) {
		fprintf( stderr, "invalid input.\n" );
		return false;
	}

//	te.Dump();
	te.Match();

	StringSet::const_iterator it;
	for ( it = Result.begin(); it != Result.end(); ++it ) {
		printf( "%s\n", it->c_str() );
	}

	return true;
}


//

int main(int argc, char * argv[])
{
	if ( argc == 2 ) {
		Enumerate( argv[1] );
	}
	return 0;
}

