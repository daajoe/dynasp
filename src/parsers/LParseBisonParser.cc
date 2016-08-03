// A Bison parser, made by GNU Bison 3.0.4.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// Take the name prefix into account.
#define yylex   dynasplex

// First part of user declarations.
#line 1 "src/parsers/LParseBisonParser.yy" // lalr1.cc:404


#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "LParseBisonParser.hpp"
#include "LParseLexer.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <cstddef>

#undef yylex
#define yylex lexer.lex

#define CHECK(name, pos, expect, actual)	\
	if((expect) != (actual))				\
	{										\
		std::ostringstream ss;				\
		ss << (name) << " was "	<< (expect)	\
		   << " but " << (actual)		\
		   << " in list";					\
		error((pos), ss.str());				\
		YYERROR;							\
	}


#line 70 "src/parsers/LParseBisonParser.cc" // lalr1.cc:404

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "src/parsers/LParseBisonParser.hh"

// User implementation prologue.

#line 84 "src/parsers/LParseBisonParser.cc" // lalr1.cc:412


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace dynasp {
#line 170 "src/parsers/LParseBisonParser.cc" // lalr1.cc:479

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
   LParseBisonParser ::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              // Fall through.
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }


  /// Build a parser object.
   LParseBisonParser :: LParseBisonParser  (LParseLexer &lexer_yyarg, IGroundAspInstance &instance_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      lexer (lexer_yyarg),
      instance (instance_yyarg)
  {}

   LParseBisonParser ::~ LParseBisonParser  ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/

  inline
   LParseBisonParser ::syntax_error::syntax_error (const location_type& l, const std::string& m)
    : std::runtime_error (m)
    , location (l)
  {}

  // basic_symbol.
  template <typename Base>
  inline
   LParseBisonParser ::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
   LParseBisonParser ::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
    , location (other.location)
  {
    value = other.value;
  }


  template <typename Base>
  inline
   LParseBisonParser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  inline
   LParseBisonParser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  inline
   LParseBisonParser ::basic_symbol<Base>::~basic_symbol ()
  {
    clear ();
  }

  template <typename Base>
  inline
  void
   LParseBisonParser ::basic_symbol<Base>::clear ()
  {
    Base::clear ();
  }

  template <typename Base>
  inline
  bool
   LParseBisonParser ::basic_symbol<Base>::empty () const
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  inline
  void
   LParseBisonParser ::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
    value = s.value;
    location = s.location;
  }

  // by_type.
  inline
   LParseBisonParser ::by_type::by_type ()
    : type (empty_symbol)
  {}

  inline
   LParseBisonParser ::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
   LParseBisonParser ::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
   LParseBisonParser ::by_type::clear ()
  {
    type = empty_symbol;
  }

  inline
  void
   LParseBisonParser ::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  inline
  int
   LParseBisonParser ::by_type::type_get () const
  {
    return type;
  }


  // by_state.
  inline
   LParseBisonParser ::by_state::by_state ()
    : state (empty_state)
  {}

  inline
   LParseBisonParser ::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
   LParseBisonParser ::by_state::clear ()
  {
    state = empty_state;
  }

  inline
  void
   LParseBisonParser ::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  inline
   LParseBisonParser ::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
   LParseBisonParser ::symbol_number_type
   LParseBisonParser ::by_state::type_get () const
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  inline
   LParseBisonParser ::stack_symbol_type::stack_symbol_type ()
  {}


  inline
   LParseBisonParser ::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
    value = that.value;
    // that is emptied.
    that.type = empty_symbol;
  }

  inline
   LParseBisonParser ::stack_symbol_type&
   LParseBisonParser ::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }


  template <typename Base>
  inline
  void
   LParseBisonParser ::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    switch (yysym.type_get ())
    {
            case 12: // ID

#line 56 "src/parsers/LParseBisonParser.yy" // lalr1.cc:614
        { delete (yysym.value.string); }
#line 424 "src/parsers/LParseBisonParser.cc" // lalr1.cc:614
        break;

      case 13: // ERROR

#line 56 "src/parsers/LParseBisonParser.yy" // lalr1.cc:614
        { delete (yysym.value.string); }
#line 431 "src/parsers/LParseBisonParser.cc" // lalr1.cc:614
        break;


      default:
        break;
    }
  }

#if YYDEBUG
  template <typename Base>
  void
   LParseBisonParser ::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
   LParseBisonParser ::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
   LParseBisonParser ::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
   LParseBisonParser ::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
   LParseBisonParser ::debug_stream () const
  {
    return *yycdebug_;
  }

  void
   LParseBisonParser ::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


   LParseBisonParser ::debug_level_type
   LParseBisonParser ::debug_level () const
  {
    return yydebug_;
  }

  void
   LParseBisonParser ::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  inline  LParseBisonParser ::state_type
   LParseBisonParser ::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
   LParseBisonParser ::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
   LParseBisonParser ::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
   LParseBisonParser ::parse ()
  {
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, &yyla.location));
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;

      // Compute the default @$.
      {
        slice<stack_symbol_type, stack_type> slice (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, slice, yylen);
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 3:
#line 68 "src/parsers/LParseBisonParser.yy" // lalr1.cc:859
    {
				if((yystack_[0].value.rule)) instance.addRule((yystack_[0].value.rule));
			}
#line 672 "src/parsers/LParseBisonParser.cc" // lalr1.cc:859
    break;

  case 5:
#line 75 "src/parsers/LParseBisonParser.yy" // lalr1.cc:859
    {
				(yylhs.value.vector)->push_back((yystack_[0].value.number));
				instance.addAtom((yystack_[0].value.number));
			}
#line 681 "src/parsers/LParseBisonParser.cc" // lalr1.cc:859
    break;

  case 6:
#line 79 "src/parsers/LParseBisonParser.yy" // lalr1.cc:859
    { (yylhs.value.vector) = new std::vector<std::size_t>(); }
#line 687 "src/parsers/LParseBisonParser.cc" // lalr1.cc:859
    break;

  case 7:
#line 83 "src/parsers/LParseBisonParser.yy" // lalr1.cc:859
    {
				(yylhs.value.number) = (yystack_[0].value.number) - 1;
				instance.addAtom((yylhs.value.number));
			}
#line 696 "src/parsers/LParseBisonParser.cc" // lalr1.cc:859
    break;

  case 8:
#line 90 "src/parsers/LParseBisonParser.yy" // lalr1.cc:859
    {
				(yylhs.value.vector)->push_back((yystack_[0].value.number) - 1);
				instance.addAtom((yystack_[0].value.number) - 1);
			}
#line 705 "src/parsers/LParseBisonParser.cc" // lalr1.cc:859
    break;

  case 9:
#line 94 "src/parsers/LParseBisonParser.yy" // lalr1.cc:859
    { (yylhs.value.vector) = new std::vector<std::size_t>(); }
#line 711 "src/parsers/LParseBisonParser.cc" // lalr1.cc:859
    break;

  case 10:
#line 98 "src/parsers/LParseBisonParser.yy" // lalr1.cc:859
    {
				(yylhs.value.vector)->push_back((yystack_[0].value.number) - 1);
				instance.addAtom((yystack_[0].value.number) - 1);
			}
#line 720 "src/parsers/LParseBisonParser.cc" // lalr1.cc:859
    break;

  case 11:
#line 102 "src/parsers/LParseBisonParser.yy" // lalr1.cc:859
    { (yylhs.value.vector) = new std::vector<std::size_t>(); }
#line 726 "src/parsers/LParseBisonParser.cc" // lalr1.cc:859
    break;

  case 12:
#line 106 "src/parsers/LParseBisonParser.yy" // lalr1.cc:859
    {
				(yylhs.value.vector)->push_back((yystack_[0].value.number));
			}
#line 734 "src/parsers/LParseBisonParser.cc" // lalr1.cc:859
    break;

  case 13:
#line 109 "src/parsers/LParseBisonParser.yy" // lalr1.cc:859
    { (yylhs.value.vector) = new std::vector<std::size_t>(); }
#line 740 "src/parsers/LParseBisonParser.cc" // lalr1.cc:859
    break;

  case 14:
#line 113 "src/parsers/LParseBisonParser.yy" // lalr1.cc:859
    {
				instance.addAtomName((yystack_[1].value.number) - 1, *(yystack_[0].value.string));
			}
#line 748 "src/parsers/LParseBisonParser.cc" // lalr1.cc:859
    break;

  case 18:
#line 124 "src/parsers/LParseBisonParser.yy" // lalr1.cc:859
    {
				CHECK("#neg", yystack_[2].location, (yystack_[2].value.number), (yystack_[1].value.vector)->size());
				CHECK(	"#lit - #neg",
						yystack_[3].location,
						(yystack_[3].value.number) - (yystack_[2].value.number),
						(yystack_[0].value.vector)->size());

				(yylhs.value.rule) = create::rule();

				(yylhs.value.rule)->addHeadAtom((yystack_[4].value.number));
				for(atom_t atom : *(yystack_[0].value.vector))
					(yylhs.value.rule)->addPositiveBodyAtom(atom);
				for(atom_t atom : *(yystack_[1].value.vector))
					(yylhs.value.rule)->addNegativeBodyAtom(atom);

				delete (yystack_[1].value.vector);
				delete (yystack_[0].value.vector);
			}
#line 771 "src/parsers/LParseBisonParser.cc" // lalr1.cc:859
    break;

  case 19:
#line 143 "src/parsers/LParseBisonParser.yy" // lalr1.cc:859
    {
				CHECK("#neg", yystack_[3].location, (yystack_[3].value.number), (yystack_[1].value.vector)->size());
				CHECK(	"#lit - #neg",
						yystack_[4].location,
						(yystack_[4].value.number) - (yystack_[3].value.number),
						(yystack_[0].value.vector)->size());

				(yylhs.value.rule) = create::rule();

				(yylhs.value.rule)->addHeadAtom((yystack_[5].value.number));
				for(atom_t atom : *(yystack_[0].value.vector))
					(yylhs.value.rule)->addPositiveBodyAtom(atom);
				for(atom_t atom : *(yystack_[1].value.vector))
					(yylhs.value.rule)->addNegativeBodyAtom(atom);
				(yylhs.value.rule)->setMinimumBodyWeight((yystack_[2].value.number));

				delete (yystack_[1].value.vector);
				delete (yystack_[0].value.vector);
			}
#line 795 "src/parsers/LParseBisonParser.cc" // lalr1.cc:859
    break;

  case 20:
#line 163 "src/parsers/LParseBisonParser.yy" // lalr1.cc:859
    {
				CHECK("#head", yystack_[5].location, (yystack_[5].value.number), (yystack_[4].value.vector)->size());
				CHECK("#neg", yystack_[2].location, (yystack_[2].value.number), (yystack_[1].value.vector)->size());
				CHECK(	"#lit - #neg",
						yystack_[3].location,
						(yystack_[3].value.number) - (yystack_[2].value.number),
						(yystack_[0].value.vector)->size());

				(yylhs.value.rule) = create::rule();

				(yylhs.value.rule)->makeChoiceHead();
				for(atom_t atom : *(yystack_[4].value.vector))
					(yylhs.value.rule)->addHeadAtom(atom);
				for(atom_t atom : *(yystack_[0].value.vector))
					(yylhs.value.rule)->addPositiveBodyAtom(atom);
				for(atom_t atom : *(yystack_[1].value.vector))
					(yylhs.value.rule)->addNegativeBodyAtom(atom);
	
				delete (yystack_[4].value.vector);
				delete (yystack_[1].value.vector);
				delete (yystack_[0].value.vector);
			}
#line 822 "src/parsers/LParseBisonParser.cc" // lalr1.cc:859
    break;

  case 21:
#line 187 "src/parsers/LParseBisonParser.yy" // lalr1.cc:859
    {
				CHECK("#neg", yystack_[3].location, (yystack_[3].value.number), (yystack_[2].value.vector)->size());
				CHECK("#weights", yystack_[4].location, (yystack_[4].value.number), (yystack_[0].value.vector)->size());
				CHECK(	"#lit - #neg",
						yystack_[4].location,
						(yystack_[4].value.number) - (yystack_[3].value.number),
						(yystack_[1].value.vector)->size());

				(yylhs.value.rule) = create::rule();

				int pos = 0;
				(yylhs.value.rule)->addHeadAtom((yystack_[6].value.number));
				for(atom_t atom : *(yystack_[2].value.vector))
					(yylhs.value.rule)->addNegativeBodyAtom(atom, (yystack_[0].value.vector)->at(pos++));
				for(atom_t atom : *(yystack_[1].value.vector))
					(yylhs.value.rule)->addPositiveBodyAtom(atom, (yystack_[0].value.vector)->at(pos++));
				(yylhs.value.rule)->setMinimumBodyWeight((yystack_[5].value.number));

				delete (yystack_[2].value.vector);
				delete (yystack_[1].value.vector);
				delete (yystack_[0].value.vector);
			}
#line 849 "src/parsers/LParseBisonParser.cc" // lalr1.cc:859
    break;

  case 22:
#line 210 "src/parsers/LParseBisonParser.yy" // lalr1.cc:859
    {
				CHECK("#neg", yystack_[3].location, (yystack_[3].value.number), (yystack_[2].value.vector)->size());
				CHECK(	"#lit - #neg",
						yystack_[4].location,
						(yystack_[4].value.number) - (yystack_[3].value.number),
						(yystack_[1].value.vector)->size());
				CHECK("#lit (weights)", yystack_[4].location, (yystack_[4].value.number), (yystack_[0].value.vector)->size());

				std::size_t listIndex, weightIndex = 0;
				for(listIndex = 0; listIndex < (yystack_[2].value.vector)->size(); ++listIndex)
					instance.addWeight(
							(*(yystack_[2].value.vector))[listIndex],
							true,
							(*(yystack_[0].value.vector))[weightIndex++]);

				for(listIndex = 0; listIndex < (yystack_[1].value.vector)->size(); ++listIndex)
					instance.addWeight(
							(*(yystack_[1].value.vector))[listIndex],
							false,
							(*(yystack_[0].value.vector))[weightIndex++]);

				delete (yystack_[2].value.vector);
				delete (yystack_[1].value.vector);
				delete (yystack_[0].value.vector);

				(yylhs.value.rule) = nullptr;
			}
#line 881 "src/parsers/LParseBisonParser.cc" // lalr1.cc:859
    break;

  case 23:
#line 238 "src/parsers/LParseBisonParser.yy" // lalr1.cc:859
    {
				CHECK("#head", yystack_[5].location, (yystack_[5].value.number), (yystack_[4].value.vector)->size());
				CHECK("#neg", yystack_[2].location, (yystack_[2].value.number), (yystack_[1].value.vector)->size());
				CHECK(	"#lit - #neg",
						yystack_[3].location,
						(yystack_[3].value.number) - (yystack_[2].value.number),
						(yystack_[0].value.vector)->size());

				(yylhs.value.rule) = create::rule();
			
				for(atom_t atom : *(yystack_[4].value.vector))
					(yylhs.value.rule)->addHeadAtom(atom);
				for(atom_t atom : *(yystack_[0].value.vector))
					(yylhs.value.rule)->addPositiveBodyAtom(atom);
				for(atom_t atom : *(yystack_[1].value.vector))
					(yylhs.value.rule)->addNegativeBodyAtom(atom);
	
				delete (yystack_[4].value.vector);
				delete (yystack_[1].value.vector);
				delete (yystack_[0].value.vector);
			}
#line 907 "src/parsers/LParseBisonParser.cc" // lalr1.cc:859
    break;


#line 911 "src/parsers/LParseBisonParser.cc" // lalr1.cc:859
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state, yyla));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    yyerror_range[1].location = yystack_[yylen - 1].location;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
   LParseBisonParser ::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
   LParseBisonParser ::yysyntax_error_ (state_type yystate, const symbol_type& yyla) const
  {
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    size_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state
         merging (from LALR or IELR) and default reductions corrupt the
         expected token list.  However, the list is correct for
         canonical LR with one exception: it will still contain any
         token that will not be accepted due to an error action in a
         later state.
    */
    if (!yyla.empty ())
      {
        int yytoken = yyla.type_get ();
        yyarg[yycount++] = yytname_[yytoken];
        int yyn = yypact_[yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yyterror_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
        YYCASE_(0, YY_("syntax error"));
        YYCASE_(1, YY_("syntax error, unexpected %s"));
        YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    size_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char  LParseBisonParser ::yypact_ninf_ = -43;

  const signed char  LParseBisonParser ::yytable_ninf_ = -1;

  const signed char
   LParseBisonParser ::yypact_[] =
  {
     -43,     1,    18,   -43,   -43,   -11,    -6,   -11,   -11,     7,
      16,   -43,    -3,   -43,    17,   -43,    20,    -7,    21,   -43,
      36,    28,    23,     0,    24,    26,    27,    19,   -43,   -43,
     -43,    29,   -43,    22,    30,   -43,    31,    -2,    32,   -43,
     -43,   -43,    32,   -43,    42,   -43,   -43,    33,    32,    32,
      32,    33,    32,   -43,   -43,    33,    33,    33,    34,    33,
      -1,    34,   -43,    38,   -43
  };

  const unsigned char
   LParseBisonParser ::yydefact_[] =
  {
       4,     0,     0,     1,    15,     0,     0,     0,     0,     0,
       0,     3,     0,     7,     0,     6,     0,     0,     0,     6,
       0,     0,     0,     0,     0,     0,     0,     0,    17,    14,
      11,     0,     5,     0,     0,    11,     0,     0,     9,    11,
      11,    11,     9,    11,     0,    16,    10,    18,     9,     9,
       9,    13,     9,    17,     8,    20,    19,    13,    22,    23,
       0,    21,    12,     0,     2
  };

  const signed char
   LParseBisonParser ::yypgoto_[] =
  {
     -43,   -43,   -43,    35,    25,   -42,   -21,     2,   -43,     3,
     -43
  };

  const signed char
   LParseBisonParser ::yydefgoto_[] =
  {
      -1,     1,     2,    23,    32,    47,    38,    58,    12,    37,
      11
  };

  const unsigned char
   LParseBisonParser ::yytable_[] =
  {
      51,     3,    20,    44,    63,    13,    55,    56,    57,    15,
      59,    21,    45,    45,    42,    25,    13,    31,    48,    49,
      50,    18,    52,     4,     5,     6,     7,     8,     9,    10,
      14,    19,    16,    17,    22,    13,    36,    24,    26,    28,
      29,    30,    33,    34,    40,    35,    53,    39,    41,    43,
       0,    46,    64,    54,    27,    62,    60,     0,     0,    61
  };

  const signed char
   LParseBisonParser ::yycheck_[] =
  {
      42,     0,     5,     5,     5,    16,    48,    49,    50,    15,
      52,    14,    14,    14,    35,    22,    16,    17,    39,    40,
      41,    14,    43,     5,     6,     7,     8,     9,    10,    11,
       5,    15,     7,     8,    17,    16,    17,    17,    17,     3,
      12,    18,    18,    17,    22,    18,     4,    18,    18,    18,
      -1,    19,    14,    20,    19,    21,    53,    -1,    -1,    57
  };

  const unsigned char
   LParseBisonParser ::yystos_[] =
  {
       0,    24,    25,     0,     5,     6,     7,     8,     9,    10,
      11,    33,    31,    16,    27,    15,    27,    27,    14,    15,
       5,    14,    17,    26,    17,    22,    17,    26,     3,    12,
      18,    17,    27,    18,    17,    18,    17,    32,    29,    18,
      22,    18,    29,    18,     5,    14,    19,    28,    29,    29,
      29,    28,    29,     4,    20,    28,    28,    28,    30,    28,
      32,    30,    21,     5,    14
  };

  const unsigned char
   LParseBisonParser ::yyr1_[] =
  {
       0,    23,    24,    25,    25,    26,    26,    27,    28,    28,
      29,    29,    30,    30,    31,    31,    32,    32,    33,    33,
      33,    33,    33,    33
  };

  const unsigned char
   LParseBisonParser ::yyr2_[] =
  {
       0,     2,    11,     2,     0,     2,     0,     1,     2,     0,
       2,     0,     2,     0,     3,     0,     2,     0,     6,     7,
       7,     8,     7,     7
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const  LParseBisonParser ::yytname_[] =
  {
  "$end", "error", "$undefined", "BP", "BM", "ZERO", "BASIC_TYPE",
  "CHOICE_TYPE", "CONSTR_TYPE", "WEIGHT_TYPE", "MIN_TYPE",
  "BASIC_EXT_TYPE", "ID", "ERROR", "UWORD", "HEAD_CNT", "HEAD",
  "LITLISTCNT", "LITLISTNCNT", "LITLISTN", "LITLISTP", "LITLISTWEIGHT",
  "BOUND", "$accept", "lparse", "rules", "hlist", "head", "list", "nlist",
  "wlist", "mapping", "compute", "rule", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned char
   LParseBisonParser ::yyrline_[] =
  {
       0,    64,    64,    67,    71,    74,    79,    82,    89,    94,
      97,   102,   105,   109,   112,   116,   119,   120,   123,   142,
     162,   186,   209,   237
  };

  // Print the state stack on the debug stream.
  void
   LParseBisonParser ::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
   LParseBisonParser ::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  // Symbol number corresponding to token number t.
  inline
   LParseBisonParser ::token_number_type
   LParseBisonParser ::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
     0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22
    };
    const unsigned int user_token_number_max_ = 277;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }


} // dynasp
#line 1362 "src/parsers/LParseBisonParser.cc" // lalr1.cc:1167
#line 261 "src/parsers/LParseBisonParser.yy" // lalr1.cc:1168


namespace dynasp
{
	void LParseBisonParser::error(
		const LParseBisonParser::location_type &l,
		const std::string &m)
	{
		std::cerr << l << ": " << m << "; last parsed word: '"
			<< lexer.YYText() << "'" << std::endl;
	}
}
