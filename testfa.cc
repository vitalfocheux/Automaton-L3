#include "gtest/gtest.h"

#include "Automaton.h"
#include <iostream>
#include <climits>
#include <cstdlib>
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define PURPLE "\x1b[35m"
#define CYAN    "\x1b[36m"
#define NC   "\x1b[0m"

const long unsigned int ZERO = 0;
const long unsigned int ONE = 1;


// =========================================================================
// Je m'excuse pour la longueur du fichier de test... + de 6700 lignes à 
// éplucher je sais pertinnement à quel point ça peut être long, mais je
// commente ce qui me semble pertinent afin que vous puissiez comprendre
// quel cas de figure dans tel test j'ai voulu prendre en compte
// =========================================================================



// ======================================================================================================
// Cette fonction permet de vérifier sur un automate s'il contient uniquement l'alphabet qu'il
// est censé contenir et non pas un autre. Si par exemple l'alphabet de fa est "abc", il est censé
// expect false si la lettre d est passé par exemple
// PS : dans alphabet, on passe uniquement sous forme d'une string l'alphabet que contient fa, car c'est 
// le but de cette fonction, pour faire gagner un peu de temps et quelques lignes de codes sur les tests
// ======================================================================================================
void do_not_have_symbol(fa::Automaton fa, std::string alphabet) {
  for (char ch = 32 ; ch <= 126 ; ++ch) {
    // PS : j'ai voulu faire avec l'opérateur ternaire mais warning à la compilation...
    // C++ parfois....
    if (alphabet.find(ch) == std::string::npos) {
      EXPECT_FALSE(fa.hasSymbol(ch));
    } else {
      EXPECT_TRUE(fa.hasSymbol(ch));
    }
  }
}

// ======================================================================================================
// Cette fonction permet de voir si le langage reconnu par fa et fa2 sont inclus mutuellement
// Autrement dit, fa et fa2 reconnaissent le même langage
// ======================================================================================================
bool sameLanguage(fa::Automaton fa, fa::Automaton fa2) {
  return fa.isIncludedIn(fa2) && fa2.isIncludedIn(fa);
}

// ======================================================================================================
// Cette fonction permet de print fa avec la fonction prettyPrint
// J'utilise cette fonction uniquement en débugage, donc normalement elle ne devrait apparaître
// dans aucun test (normalement... sauf si j'ai oublié par mégarde de la supprimer dans un test)
// ======================================================================================================
void print_automaton(fa::Automaton fa) {
  std::stringstream output;
  fa.prettyPrint(output);
  std::cout << output.str() << '\n';
}

// ======================================================================================================
// ------------------------------------ isValid ------------------------------------
TEST(AutomatonTest, Default) {
  fa::Automaton fa;
  EXPECT_FALSE(fa.isValid());
}

TEST(AutomatonTest, IsValidWithStates) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_FALSE(fa.isValid());
}

TEST(AutomatonTest, IsValidWithoutTransitions) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.isValid());
}

TEST(AutomatonTest, isValidWithAlphabetButWithoutState) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_FALSE(fa.isValid());
}

TEST(AutomatonTest, IsValidWithoutStateButTransitions) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_FALSE(fa.addTransition(1, 'a', 2));
  EXPECT_FALSE(fa.isValid());
}

TEST(AutomatonTest, IsValidWithTransitionsAndStates) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.isValid());
}
// ======================================================================================================


// ======================================================================================================
// ------------------------------------ addSymbol ------------------------------------
TEST(addSymbolTest, IsEpsilonValid) {
  fa::Automaton fa;
  EXPECT_FALSE(fa.addSymbol(fa::Epsilon));  
}

TEST(addSymbolTest, IsNonLetterValid) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('['));  
}

TEST(addSymbolTest, IsTheLetter_a_valid) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
}

TEST(addSymbolTest, IsABigZPlus1IsValid) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('Z' + 1));
}

TEST(addSymbolTest, AddSameSymbolTwice) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_FALSE(fa.addSymbol('a'));
}

TEST(addSymbolTest, AddASymbolWhichIsNotAPrintableCharacter_alias_antislashn) {
  fa::Automaton fa;
  EXPECT_FALSE(fa.addSymbol('\n'));
}

TEST(addSymbolTest, AddASymbolWhichIsNotAPrintableCharacter_alias_antislasht) {
  fa::Automaton fa;
  EXPECT_FALSE(fa.addSymbol('\t'));
}

TEST(addSymbolTest, AddTheEmptyCharacter) {
  fa::Automaton fa;
  EXPECT_FALSE(fa.addSymbol(' '));
}
// ======================================================================================================


// ======================================================================================================
// ------------------------------------ removeSymbol ------------------------------------
TEST(removeSymbolTest, IsEpsilonValid) {
  fa::Automaton fa;
  EXPECT_FALSE(fa.removeSymbol(fa::Epsilon));
}

TEST(removeSymbolTest, IsNonLetterValid) {
  fa::Automaton fa;
  EXPECT_FALSE(fa.removeSymbol('['));
}

TEST(removeSymbolTest, IsTheLetter_a_valid) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a')); 
  EXPECT_TRUE(fa.removeSymbol('a'));
}

TEST(removeSymbolTest, RemoveNonExistentSymbol) {
  fa::Automaton fa;
  EXPECT_FALSE(fa.removeSymbol('a'));
}

TEST(removeSymbolTest, RemoveSymbolWithTransitions) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.hasTransition(1, 'a', 2));
  EXPECT_TRUE(fa.removeSymbol('a'));
  EXPECT_FALSE(fa.hasTransition(1, 'a', 2));
  EXPECT_EQ(fa.countTransitions(), ZERO);
  EXPECT_FALSE(fa.isValid());
}
// ======================================================================================================

// ======================================================================================================
// ------------------------------------ addState ------------------------------------
TEST(addStateTest, NegativeValue) {
  fa::Automaton fa;
  EXPECT_FALSE(fa.addState(-1));
}

TEST(addStateTest, Zero) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
}

TEST(addStateTest, AddSameStateTwice) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_FALSE(fa.addState(1));
}

TEST(addStateTest, IntMaxValue) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(INT_MAX));
}

// PS : si je place INT_MAX + 1, le compilateur C++ considère la valeur comme "UB"
// Raison pour laquelle je met -INT_MAX (ou INT_MIN), afin de gérer le comportement du
// programme face à un test... à vrai dire particulier
TEST(addStateTest, overflowAboutIntMaxValue) {
  fa::Automaton fa;
  EXPECT_FALSE(fa.addState(-INT_MAX));
}
// En fait je ne suis pas sûr qu'il serve à grand chose ce test....
// ======================================================================================================

// ======================================================================================================
// ------------------------------------ removeState ------------------------------------
TEST(removeStateTest, NegativeValue) {
  fa::Automaton fa;
  EXPECT_FALSE(fa.addState(-1));
  EXPECT_FALSE(fa.removeState(-1));
}

TEST(removeStateTest, Zero) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.removeState(0));
}

TEST(removeStateTest, One) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.removeState(1));
}

TEST(removeStateTest, TwoStates) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.removeState(2));
  EXPECT_TRUE(fa.removeState(1));
}

TEST(removeStateTest, RemoveNonExistentState) {
  fa::Automaton fa;
  EXPECT_FALSE(fa.removeState(1));
}

TEST(removeStateTest, RemoveStateWithTransitions) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.removeState(1));
  EXPECT_EQ(fa.countTransitions(), ZERO);
}

TEST(removeStateTest, Add100StatesAndRemoveIt) {
  fa::Automaton fa;
  int lowerBound = 0;
  int upperBound = 26;
  for (int i = lowerBound; i < upperBound; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  // Car l'automate ne contient pas de symbole, il est censé être invalide
  EXPECT_FALSE(fa.isValid());
  for (int i = lowerBound; i < upperBound; ++i) {
    EXPECT_TRUE(fa.removeState(i));
  }
}
// ======================================================================================================

// ======================================================================================================
// ------------------------------------ hasState ------------------------------------
TEST(hasState, testIfAnAutomatonHasState) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_TRUE(fa.removeState(1));
  EXPECT_FALSE(fa.hasState(1));
  EXPECT_FALSE(fa.isValid());
}

TEST(hasState, testWithAnSingleAutomatonWithStateAndTestWithNoExistentState) {
  fa::Automaton fa;
  for (int i = 0 ; i < 10 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.hasState(5));
  EXPECT_FALSE(fa.hasState(10));
  EXPECT_FALSE(fa.hasState(77));
  EXPECT_FALSE(fa.hasState(99));
  EXPECT_FALSE(fa.hasState(-1));
  EXPECT_TRUE(fa.hasState(9));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(7));
  EXPECT_TRUE(fa.removeState(7));
  EXPECT_FALSE(fa.hasState(7));
}
// Je pense que 2 (gros) tests suffisent pour hasState : il contient l'état ou non, point
// ======================================================================================================


// ======================================================================================================
// ------------------------------------ countStates ------------------------------------
TEST(countStates, testIfThisFunctionIsGood) {
  fa::Automaton fa;
  EXPECT_FALSE(fa.isValid());
  EXPECT_EQ(ZERO, fa.countStates());
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_FALSE(fa.isValid());
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.isValid());
  EXPECT_EQ(ONE*2, fa.countStates());
}

TEST(countStates, testWithAnSingleAutomatonWithStateAndTestWithNoExistentState) {
  fa::Automaton fa;
  for (int i = 0 ; i < 10 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_EQ(fa.countStates(), ONE*10);
  EXPECT_TRUE(fa.removeState(7));
  EXPECT_FALSE(fa.hasState(7));
  EXPECT_EQ(fa.countStates(), ONE*9);
}
// Pareil, je pense que 2 tests suffisent pour une fonction comme celle-ci... 
// c'est le bon nombre ou non, point
// ======================================================================================================


// ======================================================================================================
// ------------------------------------ isStateInitial ------------------------------------
TEST(isStateInitial, testWithNonValidValue) {
  fa::Automaton fa;
  EXPECT_FALSE(fa.isStateInitial(-1));
}

TEST(isStateInitial, testWith0) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.isStateInitial(0));
}

TEST(isStateInitial, nonInitial) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_FALSE(fa.isStateInitial(1));
}

TEST(isStateInitial, nonInitialButFinal) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  fa.setStateFinal(1);
  EXPECT_FALSE(fa.isStateInitial(1));
}

TEST(isStateInitial, withASetValueThisTime) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  fa.setStateInitial(1);
  EXPECT_TRUE(fa.isStateInitial(1));
}

TEST(isStateInitial, anAddThenARemoveState) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  fa.setStateInitial(1);
  EXPECT_TRUE(fa.removeState(1));
  EXPECT_FALSE(fa.isStateInitial(1));
}

TEST(isStateInitial, aStateInitialTwiceSameValue) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  fa.setStateInitial(1);
  fa.setStateInitial(1);
  EXPECT_TRUE(fa.isStateInitial(1));
}
// ======================================================================================================

// ======================================================================================================
// ------------------------------------ isStateFinal ------------------------------------
TEST(isStateFinal, testWithNegativeValue) {
  fa::Automaton fa;
  EXPECT_FALSE(fa.addState(-1));
  fa.setStateFinal(-1);
  EXPECT_FALSE(fa.isStateFinal(-1));
}

TEST(isStateFinal, testWithZero) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  fa.setStateFinal(0);
  EXPECT_TRUE(fa.isStateFinal(0));
}

TEST(isStateFinal, testWithAValidValue) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.isStateFinal(1));
}

TEST(isStateFinal, stateIsInitialButNonFinal) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  fa.setStateInitial(1);
  EXPECT_FALSE(fa.isStateFinal(1));
}

TEST(isStateFinal, testIfIsFinalEndWithTwiceSameValue) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  fa.setStateFinal(1);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.isStateFinal(1));
}
// ======================================================================================================

// ======================================================================================================
// ------------------------------------ isStateInitial+Final ------------------------------------
TEST(isStateInitialAndFinal, anInitialAndFinalState) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  fa.setStateInitial(1);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.isStateInitial(1));
  EXPECT_TRUE(fa.isStateFinal(1));
}
// C'est plus pour voir si on accepte bien le fait d'avoir un état qui soit à la fois initial et final
// ======================================================================================================


// ======================================================================================================
// ------------------------------------ addTransition ------------------------------------
TEST(addTransition, noExistentTransition) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_FALSE(fa.addTransition(1,'a',3));
}

TEST(addTransition, negativeValues) {
  fa::Automaton fa;
  EXPECT_FALSE(fa.addState(-1));
  EXPECT_FALSE(fa.addState(-2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_FALSE(fa.addTransition(1,'a',3));
}

TEST(addTransition, addTransitionSimpleTest) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(1,'a',2));
}

TEST(addTransition, addTransitionExisting) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(1,'a',2));
  EXPECT_FALSE(fa.addTransition(1,'a',2));
}

TEST(addTransition, addEpsilonTransition) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_FALSE(fa.addSymbol(fa::Epsilon));
  EXPECT_TRUE(fa.addTransition(1,fa::Epsilon,2));
}

TEST(addTransition, nonValidChar) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('['));
  EXPECT_TRUE(fa.addTransition(1,'[',2));
}


TEST(addTransition, addTransitionSameDigitSymbolDifferent) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(1,'b',2));
  EXPECT_TRUE(fa.addTransition(1,'a',2));
}

TEST(addTransition, addTwiceTheSameTransition) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(1,'b',2));
  EXPECT_TRUE(fa.addTransition(1,'a',2));
  EXPECT_FALSE(fa.addTransition(1,'a',2));
}
// ======================================================================================================


// ======================================================================================================
// ------------------------------------ removeTransition ------------------------------------
TEST(removeTransition, removeTransitionExampleTest) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(1,'a',2));
  EXPECT_TRUE(fa.removeTransition(1,'a',2));
}

TEST(removeTransition, removeTransition2) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(1,'a',2));
  EXPECT_TRUE(fa.addTransition(1,'b',2));
  EXPECT_TRUE(fa.hasTransition(1,'b',2));
  EXPECT_TRUE(fa.removeTransition(1,'b',2));
  EXPECT_FALSE(fa.removeTransition(1,'b',2));
  EXPECT_TRUE(fa.hasTransition(1,'a',2));
  EXPECT_FALSE(fa.hasTransition(1,'b',2));
}

TEST(removeTransition, removeTransition3) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(1,'a',2));
  EXPECT_FALSE(fa.addTransition(1,'b',2));
  EXPECT_TRUE(fa.removeTransition(1,'a',2));
  EXPECT_FALSE(fa.hasTransition(1,'a',2));
}
TEST(removeTransition, removeTransition4) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(1,'a',2));
  EXPECT_FALSE(fa.removeTransition(1,'a',3));
  EXPECT_TRUE(fa.hasTransition(1,'a',2));
}

TEST(removeTransition, removeTransition5_invalidCharacter) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('['));
  EXPECT_TRUE(fa.addTransition(1,'[',2));
  EXPECT_TRUE(fa.removeTransition(1,'[',2));
}

TEST(removeTransition, removeTransition6_epsilon) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_FALSE(fa.addSymbol(fa::Epsilon));
  EXPECT_TRUE(fa.addTransition(1,fa::Epsilon,2));
  EXPECT_TRUE(fa.removeTransition(1,fa::Epsilon,2));
}
// ======================================================================================================


// ======================================================================================================
// ------------------------------------ prettyPrint ------------------------------------
TEST(prettyPrint, PrettyPrintTest) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.setStateInitial(0);
  fa.setStateInitial(1);
  fa.setStateFinal(1);
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'b', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  EXPECT_TRUE(fa.isValid());
  std::stringstream output;
  fa.prettyPrint(output);
  std::cout << output.str() << '\n';
  EXPECT_TRUE(!output.str().empty());
}
// ======================================================================================================


// ======================================================================================================
// ------------------------------------ hasEpsilonTransition ------------------------------------
TEST(hasEpsilonTransition, testWithNoOne) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(1,'a',2));
  EXPECT_FALSE(fa.hasEpsilonTransition());
}

TEST(hasEpsilonTransition, testWithEpsilon) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_FALSE(fa.addSymbol(fa::Epsilon));
  EXPECT_TRUE(fa.addTransition(1,fa::Epsilon,2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 1));
  EXPECT_TRUE(fa.hasEpsilonTransition());
}

TEST(hasEpsilonTransition, testWithEpsilonInvalidCase) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_FALSE(fa.addSymbol(fa::Epsilon));
  EXPECT_TRUE(fa.addTransition(1,'a',2));
  EXPECT_FALSE(fa.addTransition(1,fa::Epsilon,3));
  EXPECT_FALSE(fa.hasEpsilonTransition());
}
// ======================================================================================================


// ======================================================================================================
// ------------------------------------ isDeterministic ------------------------------------
TEST(isDeterministic, testWithTheAutomatonSlides84) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  for (int i = 0 ; i <= 3 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(0);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addTransition(0, 'b', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 3));
  EXPECT_TRUE(fa.isDeterministic());
}

TEST(isDeterministic, testWithASmallOne) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(1));
  fa.setStateFinal(1);
  fa.setStateInitial(1);
  EXPECT_TRUE(fa.addTransition(1,'a',1));
  EXPECT_TRUE(fa.isDeterministic());
}

TEST(isDeterministic, testWithANonDeterministicOneSlides113) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  for (int i = 0 ; i <= 3 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(0);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addTransition(0, 'b', 0));
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.addTransition(0, 'b', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 3));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_FALSE(fa.isDeterministic());
}

TEST(isDeterministic, testWithTwoInitialStates) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateFinal(1);
  fa.setStateInitial(1);
  fa.setStateInitial(2);
  EXPECT_TRUE(fa.addTransition(1,'a',1));
  EXPECT_FALSE(fa.isDeterministic());
}

TEST(isDeterministic, nonInitialState) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addTransition(1,'a',2));
  EXPECT_FALSE(fa.isDeterministic());
}

TEST(isDeterministic, nonFinalState) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(1);
  EXPECT_TRUE(fa.addTransition(1,'a',2));
  EXPECT_TRUE(fa.isDeterministic());
}

TEST(isDeterministic, testWithoutTransition) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isDeterministic());
}
// ======================================================================================================


// ======================================================================================================
// ------------------------------------ isComplete ------------------------------------
TEST(isComplete, testWithACompleteOneSlide104) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.addTransition(0, 'b', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  EXPECT_FALSE(fa.hasEpsilonTransition());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
}

TEST(isComplete, testWithANonCompleteOneSlide105) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  for (int i = 0 ; i <= 3 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(0);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addTransition(0, 'b', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 3));
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
}

TEST(isComplete, testWithaHUGEAutomaton) {
  fa::Automaton fa;
  srand(time(NULL));
  const int min = 0, max = 26;
  for (char c = 'a' ; c <= 'z' ; ++c) {
    EXPECT_TRUE(fa.addSymbol(c));
  }
  for (int i = 0 ; i < max ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(min);
  fa.setStateFinal(max - 1);
  int test = 0, count = 0;
  for (int state = 0; state < max; ++state) {
    for (char c1 = 'z'; c1 >= 'a'; --c1) {
      test = min + (rand() % (max - min));
      EXPECT_TRUE(fa.addTransition(state, c1, test));
      if (count == 0) {
        EXPECT_TRUE(fa.removeTransition(state, c1, test));
      }
      ++count;
    }
  }
  EXPECT_FALSE(fa.isComplete());
}

TEST(isComplete, testWithoutTransition) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.isValid());
  // S'il n'y a pas de transition l'automate n'est évidemment pas complet
  EXPECT_FALSE(fa.isComplete());
}

TEST(isComplete, isItCompleteWithACompleteOneRemovingOneTransition) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.addTransition(0, 'b', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  EXPECT_FALSE(fa.hasEpsilonTransition());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  EXPECT_TRUE(fa.removeTransition(0, 'a', 0));
  EXPECT_FALSE(fa.isComplete());
}
// ======================================================================================================

// ======================================================================================================
// ------------------------------------ createComplete ------------------------------------
TEST(createComplete, testWithANonCompleteOneSlide105) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  for (int i = 0 ; i <= 3 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(0);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addTransition(0, 'b', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 3));
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  fa::Automaton fa2 = fa2.createComplete(fa);
  EXPECT_TRUE(fa2.hasSymbol('a'));
  EXPECT_TRUE(fa2.hasSymbol('b'));
  for (int i = 32 ; i < 128 ; ++i) {
    if ((char)i != 'a' && (char)i != 'b') {
      EXPECT_FALSE(fa2.hasSymbol((char)i));
    }
  }
  EXPECT_TRUE(fa2.countSymbols() == 2);
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createComplete, testWithACompleteOneSlide104) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.addTransition(0, 'b', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  EXPECT_FALSE(fa.hasEpsilonTransition());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  fa::Automaton fa2 = fa2.createComplete(fa);
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.hasSymbol('a'));
  EXPECT_TRUE(fa2.hasSymbol('b'));
  for (int i = 32 ; i < 128 ; ++i) {
    if ((char)i != 'a' && (char)i != 'b') {
      EXPECT_FALSE(fa2.hasSymbol((char)i));
    }
  }
  EXPECT_TRUE(fa2.countSymbols() == 2);
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createComplete, testWithaHUGEAutomaton) {
  fa::Automaton fa;
  srand(time(NULL));
  const int min = 0, max = 26;
  for (char c = 'a' ; c <= 'z' ; ++c) {
    EXPECT_TRUE(fa.addSymbol(c));
  }
  for (int i = 0 ; i < max ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(min);
  fa.setStateFinal(max - 1);
  int test = 0, count = 0;
  for (int state = 0; state < max; ++state) {
    for (char c1 = 'z'; c1 >= 'a'; --c1) {
      test = min + (rand() % (max - min));
      EXPECT_TRUE(fa.addTransition(state, c1, test));
      if (count == 0) {
        EXPECT_TRUE(fa.removeTransition(state, c1, test));
      }
      ++count;
    }
  }
  EXPECT_FALSE(fa.isComplete());
  fa::Automaton fa2;
  fa2 = fa.createComplete(fa);
  EXPECT_TRUE(fa2.isComplete());
  for (char c = 'a' ; c <= 'z' ; ++c) {
    EXPECT_TRUE(fa2.hasSymbol(c));
  }
  EXPECT_TRUE(fa2.countSymbols() == max);
}

TEST(createComplete, testWithASmallOne) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateFinal(1);
  fa.setStateInitial(1);
  EXPECT_TRUE(fa.addTransition(1,'a',1));
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  fa::Automaton fa2 = fa.createComplete(fa);
  EXPECT_TRUE(fa2.hasSymbol('a'));
  for (int i = 32 ; i < 128 ; ++i) {
    if ((char)i != 'a') {
      EXPECT_FALSE(fa2.hasSymbol((char)i));
    }
  }
  EXPECT_TRUE(fa2.countSymbols() == 1);
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createComplete, testFromTheAutomatonTD5Exo18) {
  fa::Automaton fa;
  for (int i = 0 ; i < 6 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addSymbol('c'));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(0, 'b', 5));
  EXPECT_TRUE(fa.addTransition(0, 'c', 4));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  EXPECT_TRUE(fa.addTransition(2, 'c', 0));
  EXPECT_TRUE(fa.addTransition(3, 'c', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(5, 'b', 3));
  EXPECT_TRUE(fa.countSymbols() == 3);
  do_not_have_symbol(fa, "abc");
  EXPECT_TRUE(fa.match(""));
  EXPECT_TRUE(fa.match("abbb"));
  EXPECT_TRUE(fa.match("abca"));
  EXPECT_TRUE(fa.match("bcbb"));
  EXPECT_TRUE(fa.match("bcca"));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_TRUE(fa.match("bc"));
  EXPECT_TRUE(fa.match("bb"));
  EXPECT_TRUE(fa.match("ca"));
  EXPECT_TRUE(fa.match("abababababababababababab"));
  EXPECT_TRUE(fa.match("bcbcbcbcbcbcbcbcbcbcbcbc"));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("cacacacacacacacacacacaca"));
  EXPECT_TRUE(fa.match("ababababababbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("ccccccccccccccccccccccccccccccc"));
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  fa::Automaton fa2 = fa.createDeterministic(fa);
  fa2 = fa2.createComplete(fa2);
  do_not_have_symbol(fa2, "abc");
  EXPECT_TRUE(fa2.countSymbols() == fa.countSymbols());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.match(""));
  EXPECT_TRUE(fa2.match("abbb"));
  EXPECT_TRUE(fa2.match("abca"));
  EXPECT_TRUE(fa2.match("bcbb"));
  EXPECT_TRUE(fa2.match("bcca"));
  EXPECT_TRUE(fa2.match("ab"));
  EXPECT_TRUE(fa2.match("bc"));
  EXPECT_TRUE(fa2.match("bb"));
  EXPECT_TRUE(fa2.match("ca"));
  EXPECT_TRUE(fa2.match("abababababababababababab"));
  EXPECT_TRUE(fa2.match("bcbcbcbcbcbcbcbcbcbcbcbc"));
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("cacacacacacacacacacacaca"));
  EXPECT_TRUE(fa2.match("ababababababbbbbbbbbbbbb"));
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("ccccccccccccccccccccccccccccccc"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createComplete, testWithMaxStates) {
  fa::Automaton fa;
  for (int i = INT_MAX-10 ; i < INT_MAX ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addState(INT_MAX));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  for (int i = INT_MAX-10 ; i <= INT_MAX-1 ; ++i) {
    EXPECT_TRUE(fa.addTransition(i, 'a', i+1));
  }
  EXPECT_TRUE(fa.addTransition(INT_MAX, 'a', INT_MAX-10));
  EXPECT_FALSE(fa.isComplete());
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  do_not_have_symbol(fa, "ab");
  fa::Automaton fa2 = fa.createComplete(fa);
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createComplete, testWithMaxState) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(INT_MAX));
  fa.setStateInitial(INT_MAX);
  fa.setStateFinal(INT_MAX);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(INT_MAX, 'a', INT_MAX));
  EXPECT_TRUE(fa.match(""));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.isComplete());
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  do_not_have_symbol(fa, "ab");
  fa::Automaton fa2 = fa.createComplete(fa);
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(fa2.match(""));
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createComplete, testWith0AndMaxState) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(INT_MAX));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', INT_MAX));
  EXPECT_TRUE(fa.addTransition(0, 'b', 0));
  EXPECT_TRUE(fa.addTransition(INT_MAX, 'b', 0));
  EXPECT_TRUE(fa.match(""));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("ababababababababababababababababababababababababababab"));
  EXPECT_TRUE(fa.match("abbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.isComplete());
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  do_not_have_symbol(fa, "ab");
  fa::Automaton fa2 = fa.createComplete(fa);
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(fa2.match(""));
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("ababababababababababababababababababababababababababab"));
  EXPECT_TRUE(fa2.match("abbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createComplete, empty) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addSymbol('c'));
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.isLanguageEmpty());
  do_not_have_symbol(fa, "abc");
  EXPECT_FALSE(fa.isComplete());
  fa::Automaton fa2 = fa.createComplete(fa);
  EXPECT_TRUE(fa2.isLanguageEmpty());
  do_not_have_symbol(fa2, "abc");
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(sameLanguage(fa, fa2));
}
// ======================================================================================================


// ======================================================================================================
// ------------------------------------ createComplement ------------------------------------
TEST(createComplement, testWithASmallOneSlides150) {
  fa::Automaton fa;
  for (int i = 0 ; i < 3 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.setStateFinal(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.addTransition(0, 'b', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 0));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 2));
  EXPECT_TRUE(fa.isComplete());
  EXPECT_TRUE(fa.isDeterministic());
  // Maintenant on créer le complémentaire :
  fa::Automaton fa2 = fa.createComplement(fa);
  EXPECT_TRUE(fa2.hasSymbol('a') && fa2.hasSymbol('b'));
  EXPECT_TRUE(fa2.countSymbols() == 2);
  do_not_have_symbol(fa2, "ab");
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("bababababababa"));
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(sameLanguage(fa, fa2));
}

TEST(createComplement, testWithANonComplementOne) {
  fa::Automaton fa;
  fa::Automaton fai;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  fa.addTransition(2, 'b', 2);
  fai = fa.createComplement(fa);
  EXPECT_TRUE(fai.countSymbols() == 2);
  do_not_have_symbol(fai, "ab");
  EXPECT_TRUE(fai.hasSymbol('a'));
  EXPECT_TRUE(fai.hasSymbol('b'));
  EXPECT_TRUE(fai.match("aa"));
  EXPECT_FALSE(fai.match("a"));
  EXPECT_FALSE(sameLanguage(fa, fai));
}

TEST(createComplement, testAutomatonComplementD) {
  fa::Automaton fa;
  fa::Automaton fai;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 2));
  EXPECT_TRUE(fa.match("abbbbbb"));
  EXPECT_FALSE(fa.match("ba"));
  fai = fa.createComplement(fa);
  EXPECT_TRUE(fai.countSymbols() == 2);
  do_not_have_symbol(fai, "ab");
  EXPECT_TRUE(fai.match("ba"));
  EXPECT_FALSE(fai.match("abbbbbb"));
  EXPECT_FALSE(sameLanguage(fa, fai));
}

TEST(createComplement, testDeterministicAutomaton) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.addTransition(0, 'a', 1);
  fa::Automaton fai = fa.createComplement(fa);
  EXPECT_TRUE(fai.isValid());
  do_not_have_symbol(fai, "a");
  EXPECT_TRUE(fai.hasSymbol('a'));
  EXPECT_TRUE(fai.countSymbols() == 1);
  EXPECT_FALSE(sameLanguage(fa, fai));
}

TEST(createComplement, testNonDeterministicAutomaton) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateInitial(1);
  fa.setStateFinal(2);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 3));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.match("a"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("b"));
  do_not_have_symbol(fa, "ab");
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  fa::Automaton fa2 = fa.createComplement(fa);
  do_not_have_symbol(fa2, "ab");
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("b"));
  EXPECT_FALSE(fa2.match("a"));
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_FALSE(fa2.isIncludedIn(fa));
}

TEST(createComplement, stringVide){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  EXPECT_TRUE(fa.match(""));

  fa::Automaton n_fa = fa::Automaton::createComplement(fa);

  EXPECT_FALSE(n_fa.match(""));
}

TEST(createComplement, testComplementOfComplement) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  fa.addTransition(0, 'a', 1);
  fa::Automaton fai = fa.createComplement(fa);
  fa::Automaton fai2 = fai.createComplement(fai);
  EXPECT_TRUE(fai2.isValid() && fai.isValid());
  do_not_have_symbol(fai, "a");
  do_not_have_symbol(fai2, "a");
  EXPECT_TRUE(fai.countSymbols() == 1);
  EXPECT_TRUE(fai2.countSymbols() == 1);
  EXPECT_FALSE(sameLanguage(fa, fai));
  EXPECT_FALSE(sameLanguage(fai, fai2));
  // Le complément du complément donne l'automate de base, donc là on doit expect true
  EXPECT_TRUE(sameLanguage(fa, fai2));
}

TEST(createComplement, testBIGAutomaton) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addSymbol('c'));
  fa.setStateInitial(0);
  fa.setStateFinal(2);
  for (char symbol = 'a'; symbol <= 'c'; ++symbol) {
    EXPECT_TRUE(fa.addTransition(0, symbol, 1));
    EXPECT_TRUE(fa.addTransition(1, symbol, 2));
  }
  fa::Automaton fai = fa.createComplement(fa);
  EXPECT_TRUE(fai.isValid());
  EXPECT_TRUE(fai.countSymbols() == 3);
  do_not_have_symbol(fai, "abc");
  EXPECT_FALSE(sameLanguage(fa, fai));
}


// Faire la fonction createDeterministic pour remettre ce test
// Ok done
TEST(createComplement, testAutomatonComplementC) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 2));
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isLanguageEmpty());
  fa::Automaton fai = fa.createComplement(fa);
  EXPECT_FALSE(fai.isLanguageEmpty());
  EXPECT_FALSE(fai.match("aaaa"));
  do_not_have_symbol(fai, "ab");
  EXPECT_TRUE(fai.hasSymbol('a') && fai.hasSymbol('b'));
  EXPECT_TRUE(fai.countSymbols() == 2);
  EXPECT_FALSE(sameLanguage(fa, fai));
}

// Test basé sur celui de la moulinette où mon implémentation échoue dessus
// (Oui je sais c'est très original... vous pouvez me mettre -10 rien que pour ça)
TEST(createComplement, testWithAnUselessState) {
  fa::Automaton fa;
  fa::Automaton fai;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(3);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 2));
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.match("aaaa"));
  fai = fa.createComplement(fa);
  EXPECT_FALSE(fai.isLanguageEmpty());
  EXPECT_TRUE(fai.match("aaaa"));
  do_not_have_symbol(fai, "ab");
  EXPECT_TRUE(fai.hasSymbol('a') && fai.hasSymbol('b'));
  EXPECT_TRUE(fai.countSymbols() == 2);
  EXPECT_FALSE(sameLanguage(fa, fai));
}

TEST(createComplement, complementWithNoInitialState) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateFinal(2);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 3));
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match("a"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("ababababababababababababababababababababababa"));
  EXPECT_FALSE(fa.match("babababababababababababababababababababababab"));
  EXPECT_FALSE(fa.match("b"));
  do_not_have_symbol(fa, "ab");
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  fa::Automaton fa2 = fa.createComplement(fa);
  do_not_have_symbol(fa2, "ab");
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("ababababababababababababababababababababababa"));
  EXPECT_TRUE(fa2.match("babababababababababababababababababababababab"));
  EXPECT_TRUE(fa2.match("b"));
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_FALSE(fa2.isIncludedIn(fa));
  EXPECT_TRUE(fa.isIncludedIn(fa2));
}

TEST(createComplement, complementWithNoFinalState) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateInitial(1);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 3));
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match("a"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("ababababababababababababababababababababababa"));
  EXPECT_FALSE(fa.match("babababababababababababababababababababababab"));
  EXPECT_FALSE(fa.match("b"));
  do_not_have_symbol(fa, "ab");
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  fa::Automaton fa2 = fa.createComplement(fa);
  do_not_have_symbol(fa2, "ab");
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("ababababababababababababababababababababababa"));
  EXPECT_TRUE(fa2.match("babababababababababababababababababababababab"));
  EXPECT_TRUE(fa2.match("b"));
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_FALSE(fa2.isIncludedIn(fa));
  EXPECT_TRUE(fa.isIncludedIn(fa2));
}
// ======================================================================================================


// ======================================================================================================
// ------------------------------------ createMirror ------------------------------------
// Faire la fonction createDeterministic pour décommenter ce test
// Ok done
TEST(createMirror, testAutomatonMirrorTrans) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(1,'a',2));
  fa::Automaton fai = fa.createMirror(fa);
  EXPECT_TRUE(fai.countStates() == 2);
  EXPECT_TRUE(fai.countTransitions() == 1);
  EXPECT_TRUE(fai.hasSymbol('a'));
  do_not_have_symbol(fai, "a");
  EXPECT_TRUE(sameLanguage(fa, fai));
}

TEST(createMirror, testAutomatonMirrorNormal) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateInitial(1);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(1,'a',2));
  EXPECT_TRUE(fa.addTransition(2,'a',3));
  fa::Automaton fai = fa.createMirror(fa);
  do_not_have_symbol(fai, "a");
  EXPECT_TRUE(fai.countSymbols() == 1 && fai.countStates() >= 3);
  EXPECT_TRUE(sameLanguage(fa, fai));
}

TEST(createMirror, testMirrorWithKnownTransitionValues) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  for (int i = 0; i < 3; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 1));
  fa::Automaton fai = fa.createMirror(fa);
  do_not_have_symbol(fai, "ab");
  EXPECT_TRUE(fai.hasSymbol('a') && fai.hasSymbol('b'));
  EXPECT_TRUE(fai.countSymbols() == 2 && fai.countStates() == 3);
  EXPECT_TRUE(fai.countTransitions() == 4);
  EXPECT_FALSE(sameLanguage(fa, fai));
}


TEST(createMirror, testWithaHUGEAutomaton) {
  fa::Automaton fa;
  srand(time(NULL));
  const int min = 0, max = 26;
  for (char c = 'a' ; c <= 'z' ; ++c) {
    EXPECT_TRUE(fa.addSymbol(c));
  }
  for (int i = 0 ; i < max ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(min);
  fa.setStateFinal(max - 1);
  int count[max][max];
  for (int state = 0; state < max; ++state) {
    for (char c1 = 'z'; c1 >= 'a'; --c1) {
      count[state][(int)c1 - 97] = min + (rand() % (max - min));
      EXPECT_TRUE(fa.addTransition(state, c1, count[state][(int)c1 - 97]));
    }
  }
  EXPECT_TRUE(fa.isComplete());
  fa::Automaton fa2 = fa.createMirror(fa);
  for (char c = 'a' ; c <= 'z' ; ++c) {
    EXPECT_TRUE(fa2.hasSymbol(c));
  }
  EXPECT_TRUE(fa.countStates() == fa2.countStates() && fa.countTransitions() == fa2.countTransitions());
  EXPECT_TRUE(fa2.countSymbols() == max && fa2.countStates() >= max);
}

// Je décide de commenter ce test, car à vrai dire il n'est pas très pertinent et prend
// plus de temps qu'autre chose, de plus je pense que c'est ce dernier qui bouffe énormément
// de temps à la compilation de mon projet et mange plus de 6 secondes avec Valgrind
// Vu le timeout qu'on a, je préfère ne pas prendre de risque...

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
/*TEST(createMirror, testWithaHHHHHHHUUUUUUUUUUGGGGGGGGGGGEEEEEEEAutomaton) {
  fa::Automaton fa;
  srand(time(NULL));
  const int min = 0, max = 52;
  char letters[max];
  for (int i = 0; i < 26; ++i) {
    letters[i] = 'A' + i;
    letters[i + 26] = 'a' + i;
  }
  for (int i = 0 ; i < max ; ++i) {
    EXPECT_TRUE(fa.addSymbol(letters[i]));
  }
  for (int i = 0 ; i < max ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(min);
  fa.setStateFinal(max - 1);
  int count[max][max];
  for (int state = 0; state < max; ++state) {
    for (int index = 0 ; index < max ; ++index) {
      count[state][index] = min + (rand() % (max - min));
      EXPECT_TRUE(fa.addTransition(state, letters[index], count[state][index]));
    }
  }
  EXPECT_TRUE(fa.isComplete());
  fa::Automaton fa2 = fa.createMirror(fa);
  for (int i = 0 ; i < max ; ++i) {
    EXPECT_TRUE(fa2.hasSymbol(letters[i]));
  }
  EXPECT_TRUE(fa.countStates() == fa2.countStates() && fa.countTransitions() == fa2.countTransitions());
  EXPECT_TRUE(fa2.countSymbols() == max && fa2.countStates() >= max);
}*/
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TEST(createMirror, testWithAStrangeCase1) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  for (int i = 0 ; i <= 2 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateFinal(2);
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  EXPECT_TRUE(fa.isStateFinal(0) && fa.isStateFinal(2) && fa.isStateInitial(0));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  EXPECT_FALSE(fa.isDeterministic());
  fa::Automaton fa2 = fa.createMirror(fa);
  EXPECT_TRUE(fa2.countSymbols() == 2 && fa2.countStates() >= 3);
  EXPECT_TRUE(fa2.countTransitions() == 4);
  EXPECT_TRUE(fa2.hasSymbol('a') && fa2.hasSymbol('b'));
  EXPECT_FALSE(sameLanguage(fa, fa2));
}

TEST(createMirror, testWithAStrangeCase2) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  EXPECT_TRUE(fa.isStateFinal(0)&& fa.isStateInitial(0));
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  fa::Automaton fa2 = fa.createMirror(fa);
  EXPECT_TRUE(fa2.countSymbols() == 2 && fa2.countStates() >= 1);
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMirror, testWithAStrangeCase3) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  fa.setStateInitial(1);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.isStateFinal(0)&& fa.isStateInitial(0));
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  fa::Automaton fa2 = fa.createMirror(fa);
  EXPECT_TRUE(fa2.countSymbols() == 2 && fa2.countStates() == 2);
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMirror, testWithAStrangeCase4) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  EXPECT_TRUE(fa.isStateFinal(0) && fa.isStateInitial(0));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  EXPECT_TRUE(fa.addTransition(1, 'b', 1));
  fa::Automaton fa2 = fa.createMirror(fa);
  EXPECT_TRUE(fa2.countSymbols() == 2);
  EXPECT_TRUE(fa2.hasSymbol('a') && fa2.hasSymbol('b'));
  EXPECT_TRUE(fa2.countStates() == fa.countStates());
  EXPECT_TRUE(fa2.countTransitions() == fa.countTransitions());
  EXPECT_FALSE(sameLanguage(fa, fa2));
}

TEST(createMirror, testWithAStrangeCase5_word_finished_by_A) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.addTransition(0, 'b', 0));
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  fa::Automaton fa2 = fa.createMirror(fa);
  EXPECT_TRUE(fa2.hasSymbol('a') && fa2.hasSymbol('b'));
  EXPECT_TRUE(fa2.countStates() == fa.countStates());
  EXPECT_TRUE(fa2.countTransitions() == fa.countTransitions());
  EXPECT_TRUE(fa2.match("aaaaaaaaa"));
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaaaaab"));
  EXPECT_FALSE(fa2.match("ba"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_FALSE(sameLanguage(fa, fa2));
}
// ======================================================================================================


// ======================================================================================================
// ------------------------------------ makeTransition ------------------------------------
TEST(makeTransition, makeASingleAndSimpleTransition) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(1);
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  std::set<int> origin;
  origin.insert(1);
  std::set<int> result = fa.makeTransition(origin, 'a');
  EXPECT_TRUE(result.find(2) != result.end());
  EXPECT_TRUE(result.find(1) == result.end());
}

TEST(makeTransition, testWithLargerAutomaton) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  std::set<int> origin;
  origin.insert(1);
  char alpha = 'b';
  std::set<int> result = fa.makeTransition(origin, alpha);
  // Comme aucune transition avec b existe, result est censé être vide
  EXPECT_TRUE(result.empty());
}

TEST(makeTransition, testWithMissingTransition) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  std::set<int> origin;
  origin.insert(1);
  char alpha = 'b';
  std::set<int> result = fa.makeTransition(origin, alpha);
  EXPECT_TRUE(result.empty());
}

TEST(makeTransition, testWithaHUGEAutomaton) {
  // Crée un automate avec 26 états et 26 symboles correspondant à 'a' à 'z'.
  fa::Automaton fa;
  const int min = 0, max = 26;
  for (char c = 'a'; c <= 'z'; ++c) {
    EXPECT_TRUE(fa.addSymbol(c));
  }
  char letters[max];
  for (int i = 0; i < 26; ++i) {
    letters[i] = 'a' + i;
  }
  for (int i = min; i < max; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(min);
  fa.setStateFinal(max - 1);
  int count[max][max];
  for (int state = 0; state < max; ++state) {
    for (int index = 0 ; index < max ; ++index) {
      count[state][index] = min + (rand() % (max - min));
      EXPECT_TRUE(fa.addTransition(state, letters[index], count[state][index]));
      EXPECT_TRUE(fa.hasTransition(state, letters[index], count[state][index]));
    }
  }
  for (int i = 0 ; i < max ; ++i) {
    std::set<int> origin;
    origin.insert(min);  // État initial dans l'automate : min
    std::set<int> result = fa.makeTransition(origin, letters[i]);
    EXPECT_EQ(result.size(), ONE);
    EXPECT_EQ(*result.begin(), count[0][i]);
  }
}

TEST(makeTransition, testWithTheAutomatonOfLastYearDS) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  for (int i = 0 ; i <= 5 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(0);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 5));
  EXPECT_TRUE(fa.addTransition(0, 'b', 4));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(3, 'b', 0));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(5, 'a', 5));
  EXPECT_TRUE(fa.addTransition(5, 'b', 4));
  EXPECT_FALSE(fa.isDeterministic());
  std::set<int> origin;
  origin.insert(2); origin.insert(5);
  std::set<int> result = fa.makeTransition(origin, 'a');
  EXPECT_TRUE(result.size() > 0);
  EXPECT_EQ(result, origin);
  std::set<int> origin2;
  origin2.insert(0); origin2.insert(4);
  std::set<int> result3 = fa.makeTransition(origin2, 'a');
  EXPECT_EQ(result3.size(), ONE*4);
  EXPECT_TRUE(result3.find(1) != result3.end());
  EXPECT_TRUE(result3.find(3) != result3.end());
  EXPECT_TRUE(result3.find(4) != result3.end());
  EXPECT_TRUE(result3.find(5) != result3.end());
  std::set<int>result2 = fa.makeTransition(origin2, 'b');
  EXPECT_EQ(result2.size(), ONE);
  EXPECT_TRUE(result2.find(4) != result2.end());
}
// ======================================================================================================


// ======================================================================================================
// ------------------------------------ readString ------------------------------------
TEST(readString, testSimple) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  std::string word = "a";
  std::set<int> result = fa.readString(word);
  EXPECT_EQ(result.size(), ONE);
  EXPECT_TRUE(result.count(1) > 0);
}

TEST(readString, testWithLargeAutomaton) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.addTransition(0, 'b', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 0));
  EXPECT_TRUE(fa.addTransition(2, 'b', 1));
  EXPECT_TRUE(fa.addTransition(3, 'a', 2)); 
  EXPECT_TRUE(fa.addTransition(3, 'b', 3));
  std::string word = "bbabbaa";
  std::set<int> result = fa.readString(word);
  EXPECT_EQ(result.size(), ONE);
  EXPECT_TRUE(result.count(0) > 0);
}

TEST(readString, testInvalidString) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  EXPECT_TRUE(fa.addTransition(2, 'a', 0));
  EXPECT_TRUE(fa.addTransition(2, 'b', 3));
  EXPECT_TRUE(fa.addTransition(3, 'a', 2));
  EXPECT_TRUE(fa.addTransition(3, 'b', 1));
  std::string word = "bbbb";
  std::set<int> result = fa.readString(word);
  EXPECT_EQ(result.size(), ONE);
}
// ======================================================================================================

// ======================================================================================================
// ------------------------------------ match ------------------------------------
TEST(match, testWithAAutomatonJusteBefore) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  EXPECT_TRUE(fa.addTransition(2, 'a', 0));
  EXPECT_TRUE(fa.addTransition(2, 'b', 3));
  EXPECT_TRUE(fa.addTransition(3, 'a', 2));
  EXPECT_TRUE(fa.addTransition(3, 'b', 1));
  std::string word = "bbbb";
  EXPECT_TRUE(fa.match(word));
}

TEST(match, testWithASingleState_butNonEmptyWord) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  std::string w = "a";
  for (int i = 0 ; i < 50000 ; ++i) {
    w += "a";
  }
  const std::string word = w;
  EXPECT_TRUE(fa.match(word));
}

TEST(match, testWithASingleState_butEmptyWord) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  std::string word = "";
  EXPECT_TRUE(fa.match(word));
}

TEST(match, testWithASingleState_invalidWord) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  const std::string word = "aaaaaaaaaaaab";
  EXPECT_FALSE(fa.match(word));
}

TEST(match, testWithASingleState_invalidWord_butValidThisTime) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.addTransition(0, 'b', 0));
  const std::string word = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaab";
  EXPECT_TRUE(fa.match(word));
}

TEST(match, testWithaHUGEAutomaton_v1) {
  // Crée un automate avec 26 états et 26 symboles correspondant à 'a' à 'z'.
  srand(time(NULL));
  fa::Automaton fa;
  const int min = 0, max = 26;
  for (char c = 'a'; c <= 'z'; ++c) {
    EXPECT_TRUE(fa.addSymbol(c));
  }
  char letters[max];
  for (int i = min; i < max; ++i) {
    letters[i] = 'a' + i;
  }
  for (int i = min; i < max; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(min);
  fa.setStateFinal(max - 1);
  for (int i = min ; i < max - 1 ; ++i) {
    EXPECT_TRUE(fa.addTransition(i, letters[i], i+1));
  }
  const std::string word = "abcdefghijklmnopqrstuvwxyz";
  EXPECT_FALSE(fa.match(word));
  EXPECT_TRUE(fa.addTransition(max-1, letters[max-1], min));
  EXPECT_FALSE(fa.match(word));
}

TEST(match, testWithaHUGEAutomaton_v2) {
  // Crée un automate avec 26 états et 26 symboles correspondant à 'a' à 'z'.
  srand(time(NULL));
  fa::Automaton fa;
  const int min = 0, max = 26;
  for (char c = 'a'; c <= 'z'; ++c) {
    EXPECT_TRUE(fa.addSymbol(c));
  }
  char letters[max];
  for (int i = min; i < max; ++i) {
    letters[i] = 'a' + i;
  }
  for (int i = min; i < max; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(min);
  fa.setStateFinal(max - 1);
  fa.setStateFinal(min);
  for (int i = min ; i < max - 1 ; ++i) {
    EXPECT_TRUE(fa.addTransition(i, letters[i], i+1));
  }
  const std::string word = "abcdefghijqlmnopqrstuvwxyz";
  
  EXPECT_FALSE(fa.match(word));
  EXPECT_TRUE(fa.addTransition(max-1, letters[max-1], min));
  EXPECT_FALSE(fa.match(word));
}

TEST(match, testWithaHUGEAutomaton_v3) {
  // Crée un automate avec 26 états et 26 symboles correspondant à 'a' à 'z'.
  srand(time(NULL));
  fa::Automaton fa;
  const int min = 0, max = 26;
  for (char c = 'a'; c <= 'z'; ++c) {
    EXPECT_TRUE(fa.addSymbol(c));
  }
  char letters[max];
  for (int i = min; i < max; ++i) {
    letters[i] = 'a' + i;
  }
  for (int i = min; i < max; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(min);
  fa.setStateFinal(max - 1);
  fa.setStateFinal(min);
  for (int i = min ; i < max - 1 ; ++i) {
    EXPECT_TRUE(fa.addTransition(i, letters[i], i+1));
  }
  const std::string word = "abcdefghijklmnopqrstuvwxyz";  
  EXPECT_FALSE(fa.match(word));
  EXPECT_TRUE(fa.addTransition(max-1, letters[max-1], min));
  EXPECT_TRUE(fa.match(word));
}

TEST(match, testWithaHUGEAutomaton_v4) {
  // Crée un automate avec 26 états et 26 symboles correspondant à 'a' à 'z'.
  srand(time(NULL));
  fa::Automaton fa;
  const int min = 0, max = 26;
  for (char c = 'a'; c <= 'z'; ++c) {
    EXPECT_TRUE(fa.addSymbol(c));
  }
  char letters[max];
  for (int i = min; i < max; ++i) {
    letters[i] = 'a' + i;
  }
  for (int i = min; i < max; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(min);
  fa.setStateFinal(max - 1);
  fa.setStateFinal(min);
  for (int i = min ; i < max - 1 ; ++i) {
    EXPECT_TRUE(fa.addTransition(i, letters[i], i+1));
  }
  EXPECT_TRUE(fa.addTransition(min, letters[max-1], max-1));
  for (int i = max-1 ; i > 0 ; --i) {
    EXPECT_TRUE(fa.addTransition(i, letters[i-1], i-1));
  }
  const std::string word = "abcdefghijklmnopqrstuvwxyz";
  // Oui je fais l'alphabet à l'envers, et alors xDD ?
  const std::string word2 = "zyxwvutsrqponmlkjihgfedcba";
  const std::string word3 = "";  
  EXPECT_FALSE(fa.match(word));
  EXPECT_TRUE(fa.addTransition(max-1, letters[max-1], min));
  EXPECT_TRUE(fa.match(word));
  EXPECT_TRUE(fa.match(word2));
  EXPECT_TRUE(fa.match(word3));
  EXPECT_FALSE(fa.match(word + word2 + "a"));
}

TEST(match, testWithaHUGEAutomaton_v5) {
  // Crée un automate avec 26 états et 26 symboles correspondant à 'a' à 'z'.
  srand(time(NULL));
  fa::Automaton fa;
  const int min = 0, max = 26;
  for (char c = 'a'; c <= 'z'; ++c) {
    EXPECT_TRUE(fa.addSymbol(c));
  }
  char letters[max];
  for (int i = min; i < max; ++i) {
    letters[i] = 'a' + i;
  }
  for (int i = min; i < max; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(min);
  fa.setStateFinal(max - 1);
  fa.setStateFinal(min);
  for (int i = min ; i < max - 1 ; ++i) {
    EXPECT_TRUE(fa.addTransition(i, letters[i], i+1));
  }
  EXPECT_TRUE(fa.addTransition(min, letters[max-1], max-1));
  for (int i = max-1 ; i > 0 ; --i) {
    EXPECT_TRUE(fa.addTransition(i, letters[i-1], i-1));
  }
  EXPECT_TRUE(fa.addTransition(max-1, letters[max-1], min));
  const std::string word = "abba";
  const std::string word2 = "abcdefghijklmnopqrstuvwxyzzyxwvutsrqponmlkjihgfedcba";
  const std::string word3 = "aaa"; // Nombre impair de a pas possible
  const std::string word4 = "zz";
  const std::string word5 = "zyxwvutsrqponmmnopqrstuvwxy";
  EXPECT_TRUE(fa.match(word));
  EXPECT_TRUE(fa.match(word2));
  EXPECT_FALSE(fa.match(word3));
  EXPECT_TRUE(fa.match(word4));
  EXPECT_TRUE(fa.match(word5));
}
// ======================================================================================================


// ======================================================================================================
// ------------------------------------ isLanguageEmpty ------------------------------------
TEST(isLanguageEmpty, testAutomatonTestEmptyWithAValidWay) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(1,'a',2));
  EXPECT_FALSE(fa.isLanguageEmpty());
}

TEST(isLanguageEmpty, testAutomatonTestEmpty2WithNoWayToFinishIt) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.isLanguageEmpty());
}

TEST(isLanguageEmpty, testAutomatonTestEmpty3AddingAnInvalidTransition) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_FALSE(fa.addTransition(1,' ',2));
  EXPECT_TRUE(fa.isLanguageEmpty());
}

TEST(isLanguageEmpty, testAutomatonTestEmpty4RemovingASymbol) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(1,'a',2));
  EXPECT_TRUE(fa.removeSymbol('a'));
  EXPECT_TRUE(fa.isLanguageEmpty());
}

TEST(isLanguageEmpty, testAutomatonTestEmpty5RemovingAValidTransition) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(1,'a',2));
  EXPECT_TRUE(fa.removeTransition(1,'a',2));
  EXPECT_TRUE(fa.isLanguageEmpty());
}

TEST(isLanguageEmpty, testWithAnAutomatonRecognizeTheEmptyLanguage) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(1);
  fa.setStateFinal(2);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_FALSE(fa.isLanguageEmpty());  
}
// ======================================================================================================


// ======================================================================================================
// ------------------------------------ removeNonAccessibleStates ------------------------------------
TEST(removeNonAccessibleStates, testWithASimpleAutomaton) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.isValid());
  // Reconnait le langage vide car il n'a pas d'état final
  EXPECT_TRUE(fa.isLanguageEmpty());
  // Ne reconnaît pas "a" car 1 n'est pas un état final
  EXPECT_FALSE(fa.match("a"));
  EXPECT_FALSE(fa.match("aa"));
  // L'état 0 n'est pas considéré comme accessible, il doit donc être supprimé
  fa.removeNonAccessibleStates();
  std::set<int> initialStates;
  std::set<int> statesOfAutomaton = fa.readString("");
  for (int i : statesOfAutomaton) {
    if (fa.isStateInitial(i)) initialStates.insert(i);
  }
  EXPECT_FALSE(initialStates.empty());
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_TRUE(fa.countSymbols() == ONE);
  EXPECT_EQ(fa.countTransitions(), ONE);
  EXPECT_EQ(fa.countStates(), ONE*2);
  // Reconnait le langage vide car il n'a pas d'état final
  EXPECT_TRUE(fa.isLanguageEmpty());
  // Ne reconnaît pas "a" car 1 n'est pas un état final
  EXPECT_FALSE(fa.match("a"));
  EXPECT_FALSE(fa.match("aa"));
}

TEST(removeNonAccessibleStates, testWithABiggerAutomaton) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateFinal(3);
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match("a"));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_FALSE(fa.match("ba"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_TRUE(fa.isValid());
  fa.removeNonAccessibleStates();
  std::set<int> initialStates;
  std::set<int> statesOfAutomaton = fa.readString("");
  for (int i : statesOfAutomaton) {
    if (fa.isStateInitial(i)) initialStates.insert(i);
  }
  EXPECT_FALSE(initialStates.empty());
  // Reconnaît le langage vide car plus d'état final
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match("a"));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_FALSE(fa.match("ba"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_FALSE(fa.hasState(1));
  EXPECT_TRUE(fa.hasTransition(0, 'a', 2));
  EXPECT_FALSE(fa.hasTransition(1, 'b', 3));
  EXPECT_EQ(fa.countTransitions(), ONE);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*2);
}

TEST(removeNonAccessibleStates, testWithAPoorOneLostNoWhere) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateFinal(2);
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  fa.removeNonAccessibleStates();
  std::set<int> initialStates;
  std::set<int> statesOfAutomaton = fa.readString("");
  for (int i : statesOfAutomaton) {
    if (fa.isStateInitial(i)) initialStates.insert(i);
  }
  EXPECT_FALSE(initialStates.empty());
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
  EXPECT_TRUE(fa.hasTransition(1, 'b', 2));
  EXPECT_TRUE(fa.countTransitions() == 2);
  EXPECT_TRUE(fa.countSymbols() == 2);
  EXPECT_EQ(fa.countStates(), ONE*3);
}

TEST(removeNonAccessibleStates, case3) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateInitial(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(1,'a',2));
  EXPECT_TRUE(fa.addTransition(3,'a',2));
  EXPECT_TRUE(fa.match("a"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  fa.removeNonAccessibleStates();
  std::set<int> initialStates;
  std::set<int> statesOfAutomaton = fa.readString("");
  for (int i : statesOfAutomaton) {
    if (fa.isStateInitial(i)) initialStates.insert(i);
  }
  EXPECT_FALSE(initialStates.empty());
  EXPECT_TRUE(fa.match("a"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.hasState(3));
  EXPECT_FALSE(fa.hasTransition(3,'a',2));
  EXPECT_EQ(fa.countTransitions(), ONE);
  EXPECT_EQ(fa.countStates(), ONE*2);
}

TEST(removeNonAccessibleStates, testWithManyInitialStates) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addState(6));
  EXPECT_TRUE(fa.addState(7));
  fa.setStateInitial(0);
  fa.setStateInitial(5);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 4));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 5));
  EXPECT_TRUE(fa.addTransition(7, 'b', 5));
  EXPECT_TRUE(fa.addTransition(7, 'a', 2));
  EXPECT_TRUE(fa.addTransition(3, 'b', 1));
  EXPECT_TRUE(fa.addTransition(3, 'a', 4));
  EXPECT_TRUE(fa.addTransition(4, 'b', 2));
  EXPECT_TRUE(fa.addTransition(5, 'b', 4));
  EXPECT_TRUE(fa.addTransition(5, 'a', 2));
  EXPECT_TRUE(fa.addTransition(6, 'a', 3));
  EXPECT_TRUE(fa.addTransition(6, 'b', 5));
  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_EQ(fa.countTransitions(), ONE*14);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*8);
  EXPECT_TRUE(fa.match("aa"));
  EXPECT_TRUE(fa.match("ababbbbbb"));
  EXPECT_TRUE(fa.match("aabbbbbbb"));
  fa.removeNonAccessibleStates();
  std::set<int> initialStates;
  std::set<int> statesOfAutomaton = fa.readString("");
  for (int i : statesOfAutomaton) {
    if (fa.isStateInitial(i)) initialStates.insert(i);
  }
  EXPECT_FALSE(initialStates.empty());
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.match("aa"));
  EXPECT_TRUE(fa.match("ababbbbbb"));
  EXPECT_TRUE(fa.match("aabbbbbbb"));
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_EQ(fa.countTransitions(), ONE*10);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*6);
}

TEST(removeNonAccessibleStates, testWithAPoorOneLostNoWhereAndNoInitialStates) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(3, 'a', 1));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  fa.removeNonAccessibleStates();
  // =================================================
  std::set<int> initialStates;
  std::set<int> statesOfAutomaton = fa.readString("");
  for (int i : statesOfAutomaton) {
    if (fa.isStateInitial(i)) initialStates.insert(i);
  }
  EXPECT_FALSE(initialStates.empty());
  // Pour permettre de raisonner en terme de langage, 
  // et non en terme de parcours dans la structure de 
  // données, c'est une astuce que je ne connaissais pas, 
  // merci beaucoup pour cela !
  // =================================================
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.hasTransition(0, 'a', 1));
  EXPECT_FALSE(fa.hasTransition(1, 'b', 2));
  EXPECT_FALSE(fa.hasTransition(3, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), ONE*0);
  EXPECT_TRUE(fa.countSymbols() == 2);
  EXPECT_EQ(fa.countStates(), ONE);
}

TEST(removeNonAccessibleStates, testWithASingleState) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_FALSE(fa.match("a"));
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  fa.removeNonAccessibleStates();
  std::set<int> initialStates;
  std::set<int> statesOfAutomaton = fa.readString("");
  for (int i : statesOfAutomaton) {
    if (fa.isStateInitial(i)) initialStates.insert(i);
  }
  EXPECT_FALSE(initialStates.empty());
  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.match("a"));
  EXPECT_TRUE(fa.isLanguageEmpty());
}
// ======================================================================================================


// ======================================================================================================
// ------------------------------ removeNonCoAccessibleStates ------------------------------
TEST(removeNonCoAccessibleStates, testWithASimpleAutomaton_BUT_NOT_FINAL_STATE) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.isValid());
  // Reconnait le langage vide car il n'a pas d'état final
  EXPECT_TRUE(fa.isLanguageEmpty());
  // Ne reconnaît pas "a" car 1 n'est pas un état final
  EXPECT_FALSE(fa.match("a"));
  EXPECT_FALSE(fa.match("aa"));
  // L'état 0 n'est pas considéré comme accessible, il doit donc être supprimé
  fa.removeNonCoAccessibleStates();
  EXPECT_FALSE(fa.hasTransition(0, 'a', 1));
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_TRUE(fa.countSymbols() == ONE);
  EXPECT_EQ(fa.countTransitions(), ONE*0);
  EXPECT_EQ(fa.countStates(), ONE);
  EXPECT_TRUE(fa.isValid());
}

TEST(removeNonCoAccessibleStates, testWithABiggerAutomaton) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateFinal(3);
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match("a"));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_FALSE(fa.match("ba"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_TRUE(fa.isValid());
  fa.removeNonCoAccessibleStates();
  // Reconnaît le langage vide car plus d'état final
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match("a"));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_FALSE(fa.match("ba"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_FALSE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_FALSE(fa.hasTransition(0, 'a', 2));
  EXPECT_TRUE(fa.hasTransition(1, 'b', 3));
  EXPECT_EQ(fa.countTransitions(), ONE);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*2);
  EXPECT_FALSE(fa.hasState(0));
  EXPECT_FALSE(fa.hasState(2));
}


TEST(removeNonCoAccessibleStates, testWithAPoorOneLostNoWhere) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateFinal(2);
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  fa.removeNonCoAccessibleStates();
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
  EXPECT_TRUE(fa.hasTransition(1, 'b', 2));
  EXPECT_TRUE(fa.countTransitions() == 2);
  EXPECT_TRUE(fa.countSymbols() == 2);
  EXPECT_EQ(fa.countStates(), ONE*3);
}

TEST(removeNonCoAccessibleStates, testWithABiggerAutomatonIntoThe2022CC1) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addState(6));
  fa.setStateInitial(0);
  fa.setStateFinal(2);
  fa.setStateFinal(6);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 4));
  EXPECT_TRUE(fa.addTransition(2, 'b', 6));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(4, 'a', 5));
  EXPECT_TRUE(fa.addTransition(4, 'b', 3));
  EXPECT_TRUE(fa.addTransition(5, 'b', 2));
  EXPECT_TRUE(fa.addTransition(5, 'a', 4));
  EXPECT_TRUE(fa.addTransition(6, 'b', 6));
  EXPECT_TRUE(fa.isValid());
  EXPECT_EQ(fa.countTransitions(), ONE*11);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*7);
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaab"));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_TRUE(fa.match("abb"));
  EXPECT_TRUE(fa.match("abbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("bbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  fa.removeNonCoAccessibleStates();
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaab"));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_TRUE(fa.match("abb"));
  EXPECT_TRUE(fa.match("abbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("bbbbbbbbbbbbbbbbb"));
  EXPECT_EQ(fa.countTransitions(), ONE*8);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*6);
  EXPECT_FALSE(fa.hasState(3));
}

TEST(removeNonCoAccessibleStates, testWithNoInitialState_V1) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(3, 'a', 1));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  fa.removeNonCoAccessibleStates();
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
  EXPECT_TRUE(fa.hasTransition(1, 'b', 2));
  EXPECT_TRUE(fa.hasTransition(3, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), ONE*3);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*4);
}

TEST(removeNonCoAccessibleStates, testWithNoInitialState_V2) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(3, 'a', 1));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  fa.removeNonCoAccessibleStates();
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.hasTransition(0, 'a', 0));
  EXPECT_TRUE(fa.hasTransition(1, 'b', 2));
  EXPECT_TRUE(fa.hasTransition(3, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), ONE*2);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*3);
  EXPECT_FALSE(fa.hasState(0));
}

TEST(removeNonCoAccessibleStates, testWithASingleState) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_FALSE(fa.match("a"));
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  fa.removeNonCoAccessibleStates();
  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.match("a"));
  EXPECT_TRUE(fa.isLanguageEmpty());
}
// ======================================================================================================


// ======================================================================================================
// ------------------------------------ createIntersection ------------------------------------
TEST(createIntersection, testWithOneWichFinishedByAAndTheOtherOneBeginByB) {
  fa::Automaton lhs;
  fa::Automaton rhs;
  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  lhs.setStateInitial(0);
  lhs.setStateFinal(1);
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);
  EXPECT_TRUE(lhs.addTransition(0, 'b', 0));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 1));
  fa::Automaton fa = fa.createIntersection(lhs, rhs);
  EXPECT_TRUE(fa.match("ba"));
  EXPECT_FALSE(fa.match("bbb"));
  EXPECT_FALSE(fa.match("aaa"));
  EXPECT_FALSE(fa.match("aba"));
  EXPECT_FALSE(fa.match(""));
}

TEST(createIntersection, testWithAnNonIntersectionOne) {
  fa::Automaton lhs;
  fa::Automaton rhs;
  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('c'));
  EXPECT_TRUE(rhs.addSymbol('c'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  lhs.setStateInitial(0);
  lhs.setStateFinal(1);
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);
  EXPECT_TRUE(lhs.addTransition(0, 'c', 0));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'c', 1));
  fa::Automaton fa = fa.createIntersection(lhs, rhs);
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match("cccccccccccccca"));
  EXPECT_FALSE(fa.match("bcccccccccccbbbbbbbbbbbbbbbcbcbcbc"));
  EXPECT_FALSE(fa.match("ccccccccccc"));
}

TEST(createIntersection, testWithThe2023CC1Exo1) {
  fa::Automaton lhs;
  fa::Automaton rhs;
  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  lhs.setStateInitial(0);
  lhs.setStateFinal(1);
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  EXPECT_TRUE(lhs.addTransition(0, 'b', 0));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(lhs.match("bbbbbbbbbbbbbbbabbbbbbbbbbb"));
  EXPECT_TRUE(lhs.match("baaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(rhs.match("aaaaaaaaaaaaaaaaaaaaaaaaaab"));
  EXPECT_TRUE(rhs.match("bbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  fa::Automaton fa = fa.createIntersection(lhs, rhs);
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbabbbbbbbbb"));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaab"));
}

TEST(createIntersection, testWithTheOneSlide158) {
  fa::Automaton lhs;
  fa::Automaton rhs;
  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  lhs.setStateInitial(0);
  lhs.setStateFinal(1);
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(lhs.match("aaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(lhs.match("ab"));
  EXPECT_TRUE(lhs.match("abbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(lhs.match("bbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 0));
  EXPECT_TRUE(rhs.match("aaaaaaaaaaaaaaab"));
  EXPECT_TRUE(rhs.match("abbbbbbbbbbbbbbbbbbbbbbbbbabbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(rhs.match("aaaaaaaaaaaaaaaa"));
  fa::Automaton fa = fa.createIntersection(lhs, rhs);
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaab"));
  EXPECT_TRUE(fa.match("aabbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaaaaaaaaab"));
  EXPECT_TRUE(fa.match("abbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.isLanguageEmpty());
}

TEST(createIntersection, testWithTheSameAutomaton) {
  fa::Automaton lhs;
  fa::Automaton rhs;
  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  lhs.setStateInitial(0);
  lhs.setStateFinal(1);
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 0));
  EXPECT_TRUE(rhs.match("aaaaaaaaaaaaaaab"));
  EXPECT_TRUE(rhs.match("abbbbbbbbbbbbbbbbbbbbbbbbbabbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(rhs.match("aaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(lhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'a', 0));
  EXPECT_TRUE(lhs.match("aaaaaaaaaaaaaaab"));
  EXPECT_TRUE(lhs.match("abbbbbbbbbbbbbbbbbbbbbbbbbabbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(lhs.match("aaaaaaaaaaaaaaaa"));
  fa::Automaton fa = fa.createIntersection(lhs, rhs);
  // L'intersection du même langage donne comme résultat le même langage
  // Donc les matchs doivent être aussi vrai qu'avant
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaab"));
  EXPECT_TRUE(fa.match("abbbbbbbbbbbbbbbbbbbbbbbbbabbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaa"));
}

TEST(createIntersection, testWithManyInitialStates) {
  fa::Automaton lhs;
  fa::Automaton rhs;
  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(lhs.addState(2));
  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  lhs.setStateInitial(0);
  lhs.setStateInitial(2);
  lhs.setStateFinal(0);
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);
  EXPECT_TRUE(lhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(lhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'a', 2));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(lhs.addTransition(2, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(2, 'b', 0));
  EXPECT_TRUE(lhs.match("aaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbaaaaaaab"));
  EXPECT_TRUE(lhs.match("abab"));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.match("aaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(rhs.match("ab"));
  EXPECT_TRUE(rhs.match("abbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(rhs.match("bbbbbbbbbbbbbbbbb"));
  fa::Automaton fa = fa.createIntersection(lhs, rhs);
  lhs.prettyPrint(std::cout);
  rhs.prettyPrint(std::cout);
  fa.prettyPrint(std::cout);
  EXPECT_TRUE(fa.match("abbbbbbbbbbbbbbbbbbbbbbbaaaaabaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("aaaaaaaabbab"));
  EXPECT_FALSE(fa.match("bbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.isLanguageEmpty());
}

TEST(createIntersection, testWithAnEmptyAutomatonForLHS) {
  fa::Automaton lhs;
  fa::Automaton rhs;
  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  lhs.setStateInitial(0);
  lhs.setStateFinal(0);
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 0));
  EXPECT_TRUE(rhs.match("aaaaaaaaaaaaaaab"));
  EXPECT_TRUE(rhs.match("abbbbbbbbbbbbbbbbbbbbbbbbbabbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(rhs.match("aaaaaaaaaaaaaaaa"));
  fa::Automaton fa = fa.createIntersection(lhs, rhs);
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaab"));
  EXPECT_FALSE(fa.match("abbbbbbbbbbbbbbbbbbbbbbbbbabbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaa"));
}
// ======================================================================================================



// ======================================================================================================
// -------------------------- TESTS FOR EMPTYINTERSECTION WITH -----------------------
TEST(hasEmptyIntersectionWith, testWithOneWichFinishedByAAndTheOtherOneBeginByB) {
  fa::Automaton lhs;
  fa::Automaton rhs;
  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  lhs.setStateInitial(0);
  lhs.setStateFinal(1);
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);
  EXPECT_TRUE(lhs.addTransition(0, 'b', 0));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 1));
  EXPECT_FALSE(lhs.hasEmptyIntersectionWith(lhs));
}

TEST(hasEmptyIntersectionWith, testWithAnNonIntersectionOne) {
  fa::Automaton lhs;
  fa::Automaton rhs;
  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('c'));
  EXPECT_TRUE(rhs.addSymbol('c'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  lhs.setStateInitial(0);
  lhs.setStateFinal(1);
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);
  EXPECT_TRUE(lhs.addTransition(0, 'c', 0));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'c', 1));
  EXPECT_TRUE(lhs.hasEmptyIntersectionWith(rhs));
}

TEST(hasEmptyIntersectionWith, testWithThe2023CC1Exo1) {
  fa::Automaton lhs;
  fa::Automaton rhs;
  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  lhs.setStateInitial(0);
  lhs.setStateFinal(1);
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  EXPECT_TRUE(lhs.addTransition(0, 'b', 0));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(lhs.match("bbbbbbbbbbbbbbbabbbbbbbbbbb"));
  EXPECT_TRUE(lhs.match("baaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(rhs.match("aaaaaaaaaaaaaaaaaaaaaaaaaab"));
  EXPECT_TRUE(rhs.match("bbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(lhs.hasEmptyIntersectionWith(lhs));
}

TEST(hasEmptyIntersectionWith, testWithTheOneSlide158) {
  fa::Automaton lhs;
  fa::Automaton rhs;
  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  lhs.setStateInitial(0);
  lhs.setStateFinal(1);
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(lhs.match("aaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(lhs.match("ab"));
  EXPECT_TRUE(lhs.match("abbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(lhs.match("bbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 0));
  EXPECT_TRUE(rhs.match("aaaaaaaaaaaaaaab"));
  EXPECT_TRUE(rhs.match("abbbbbbbbbbbbbbbbbbbbbbbbbabbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(rhs.match("aaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(lhs.hasEmptyIntersectionWith(rhs));
}

TEST(hasEmptyIntersectionWith, testWithTheSameAutomaton) {
  fa::Automaton lhs;
  fa::Automaton rhs;
  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  lhs.setStateInitial(0);
  lhs.setStateFinal(1);
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 0));
  EXPECT_TRUE(rhs.match("aaaaaaaaaaaaaaab"));
  EXPECT_TRUE(rhs.match("abbbbbbbbbbbbbbbbbbbbbbbbbabbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(rhs.match("aaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(lhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'a', 0));
  EXPECT_TRUE(lhs.match("aaaaaaaaaaaaaaab"));
  EXPECT_TRUE(lhs.match("abbbbbbbbbbbbbbbbbbbbbbbbbabbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(lhs.match("aaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(lhs.hasEmptyIntersectionWith(rhs));
}

TEST(hasEmptyIntersectionWith, testWithManyInitialStates) {
  fa::Automaton lhs;
  fa::Automaton rhs;
  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(lhs.addState(2));
  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  lhs.setStateInitial(0);
  lhs.setStateInitial(2);
  lhs.setStateFinal(0);
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);
  EXPECT_TRUE(lhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(lhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'a', 2));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(lhs.addTransition(2, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(2, 'b', 0));
  EXPECT_TRUE(lhs.match("aaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbaaaaaaab"));
  EXPECT_TRUE(lhs.match("abab"));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.match("aaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(rhs.match("ab"));
  EXPECT_TRUE(rhs.match("abbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(rhs.match("bbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(lhs.hasEmptyIntersectionWith(rhs));
}

TEST(hasEmptyIntersectionWith, testWithAnEmptyAutomatonForLHS) {
  fa::Automaton lhs;
  fa::Automaton rhs;
  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  lhs.setStateInitial(0);
  lhs.setStateFinal(0);
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 0));
  EXPECT_TRUE(rhs.match("aaaaaaaaaaaaaaab"));
  EXPECT_TRUE(rhs.match("abbbbbbbbbbbbbbbbbbbbbbbbbabbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(rhs.match("aaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(lhs.hasEmptyIntersectionWith(rhs));
}
// ======================================================================================================


// ======================================================================================================
// -------------------------- TESTS FOR CREATE DETERMINISTIC -----------------------

TEST(createDeterministic, testWithTheOneIntoThe2022CC1) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 5));
  EXPECT_TRUE(fa.addTransition(0, 'b', 4));
  EXPECT_TRUE(fa.addTransition(1, 'b', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 0));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(5, 'a', 5));
  EXPECT_TRUE(fa.addTransition(5, 'b', 4));
  fa.setStateFinal(3);
  fa.setStateInitial(0);
  EXPECT_FALSE(fa.isDeterministic());
  int symb_before = fa.countSymbols();
  EXPECT_TRUE(fa.match("abbbbbbbbaab"));
  EXPECT_TRUE(fa.match("abbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("baaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("abbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  fa::Automaton fa2 = fa.createDeterministic(fa);
  int symb_after = fa2.countSymbols();
  EXPECT_EQ(symb_after, symb_before);
  EXPECT_TRUE(fa2.hasSymbol('a'));
  EXPECT_TRUE(fa2.hasSymbol('b'));
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.match("abbbbbbbbaab"));
  EXPECT_TRUE(fa2.match("abbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("baaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("abbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.isLanguageEmpty());
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createDeterministic, testWithTheOneIntoThe2023CC1_alias_2InitialsStates) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  fa.setStateInitial(0);
  fa.setStateInitial(5);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 4));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 5));
  EXPECT_TRUE(fa.addTransition(3, 'b', 1));
  EXPECT_TRUE(fa.addTransition(3, 'a', 4));
  EXPECT_TRUE(fa.addTransition(4, 'b', 2));
  EXPECT_TRUE(fa.addTransition(5, 'b', 4));
  EXPECT_TRUE(fa.addTransition(5, 'a', 2));
  EXPECT_TRUE(fa.match("aa"));
  EXPECT_TRUE(fa.match("aab"));
  EXPECT_TRUE(fa.match("aba"));
  EXPECT_TRUE(fa.match("aab"));
  EXPECT_TRUE(fa.match("aababababa"));
  EXPECT_TRUE(fa.match("aabbabababa"));
  EXPECT_TRUE(fa.match("abababababa"));
  EXPECT_TRUE(fa.match("aabbabababa"));
  EXPECT_TRUE(fa.match("aabbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("aabbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("ababbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("aabbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isDeterministic());
  int symb_before = fa.countSymbols();
  fa::Automaton fa2 = fa.createDeterministic(fa);
  int symb_after = fa2.countSymbols();
  EXPECT_EQ(symb_after, symb_before);
  EXPECT_TRUE(fa2.hasSymbol('a'));
  EXPECT_TRUE(fa2.hasSymbol('b'));
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.match("aa"));
  EXPECT_TRUE(fa2.match("aab"));
  EXPECT_TRUE(fa2.match("aba"));
  EXPECT_TRUE(fa2.match("aab"));
  EXPECT_TRUE(fa2.match("aababababa"));
  EXPECT_TRUE(fa2.match("aabbabababa"));
  EXPECT_TRUE(fa2.match("abababababa"));
  EXPECT_TRUE(fa2.match("aabbabababa"));
  EXPECT_TRUE(fa2.match("aabbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("aabbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("ababbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("aabbbbbbbbbbbbb"));
  EXPECT_FALSE(fa2.isLanguageEmpty());
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createDeterministic, testWithADeterministicOneIntoThe2023CC1) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addState(6));
  EXPECT_TRUE(fa.addState(7));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.setStateFinal(3);
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 5));
  EXPECT_TRUE(fa.addTransition(1, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'b', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 7));
  EXPECT_TRUE(fa.addTransition(3, 'a', 1));
  EXPECT_TRUE(fa.addTransition(4, 'b', 1));
  EXPECT_TRUE(fa.addTransition(4, 'a', 5));
  EXPECT_TRUE(fa.addTransition(5, 'b', 6));
  EXPECT_TRUE(fa.addTransition(5, 'a', 4));
  EXPECT_TRUE(fa.addTransition(6, 'b', 7));
  EXPECT_TRUE(fa.addTransition(6, 'a', 3));
  EXPECT_TRUE(fa.addTransition(7, 'b', 6));
  EXPECT_TRUE(fa.addTransition(7, 'a', 4));
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.match("a"));
  EXPECT_TRUE(fa.match("aababaababaababaababa"));
  EXPECT_TRUE(fa.match("bbaba"));
  EXPECT_TRUE(fa.match("bb"));
  EXPECT_TRUE(fa.match("ba"));
  EXPECT_TRUE(fa.match("bbbbababbba"));
  fa::Automaton fa2 = fa.createDeterministic(fa);
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_TRUE(fa2.match("aababaababaababaababa"));
  EXPECT_TRUE(fa2.match("bbaba"));
  EXPECT_TRUE(fa2.match("bb"));
  EXPECT_TRUE(fa2.match("ba"));
  EXPECT_TRUE(fa2.match("bbbbababbba"));
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createDeterministic, testFromTheAutomatonTD5Exo18) {
  fa::Automaton fa;
  for (int i = 0 ; i < 6 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addSymbol('c'));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(0, 'b', 5));
  EXPECT_TRUE(fa.addTransition(0, 'c', 4));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  EXPECT_TRUE(fa.addTransition(2, 'c', 0));
  EXPECT_TRUE(fa.addTransition(3, 'c', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(5, 'b', 3));
  EXPECT_TRUE(fa.countSymbols() == 3);
  do_not_have_symbol(fa, "abc");
  EXPECT_TRUE(fa.match(""));
  EXPECT_TRUE(fa.match("abbb"));
  EXPECT_TRUE(fa.match("abca"));
  EXPECT_TRUE(fa.match("bcbb"));
  EXPECT_TRUE(fa.match("bcca"));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_TRUE(fa.match("bc"));
  EXPECT_TRUE(fa.match("bb"));
  EXPECT_TRUE(fa.match("ca"));
  EXPECT_TRUE(fa.match("abababababababababababab"));
  EXPECT_TRUE(fa.match("bcbcbcbcbcbcbcbcbcbcbcbc"));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("cacacacacacacacacacacaca"));
  EXPECT_TRUE(fa.match("ababababababbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("ccccccccccccccccccccccccccccccc"));
  EXPECT_FALSE(fa.isDeterministic());
  fa::Automaton fa2 = fa.createDeterministic(fa);
  do_not_have_symbol(fa2, "abc");
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.match(""));
  EXPECT_TRUE(fa2.match("abbb"));
  EXPECT_TRUE(fa2.match("abca"));
  EXPECT_TRUE(fa2.match("bcbb"));
  EXPECT_TRUE(fa2.match("bcca"));
  EXPECT_TRUE(fa2.match("ab"));
  EXPECT_TRUE(fa2.match("bc"));
  EXPECT_TRUE(fa2.match("bb"));
  EXPECT_TRUE(fa2.match("ca"));
  EXPECT_TRUE(fa2.match("abababababababababababab"));
  EXPECT_TRUE(fa2.match("bcbcbcbcbcbcbcbcbcbcbcbc"));
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("cacacacacacacacacacacaca"));
  EXPECT_TRUE(fa2.match("ababababababbbbbbbbbbbbb"));
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("ccccccccccccccccccccccccccccccc"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createDeterministic, testWithNoInitialState) {
  fa::Automaton fa;
  for (int i = 0 ; i < 6 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addSymbol('c'));
  fa.setStateFinal(0);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(0, 'b', 5));
  EXPECT_TRUE(fa.addTransition(0, 'c', 4));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  EXPECT_TRUE(fa.addTransition(2, 'c', 0));
  EXPECT_TRUE(fa.addTransition(3, 'c', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(5, 'b', 3));
  EXPECT_TRUE(fa.countSymbols() == 3);
  do_not_have_symbol(fa, "abc");
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match("abbb"));
  EXPECT_FALSE(fa.match("abca"));
  EXPECT_FALSE(fa.match("bcbb"));
  EXPECT_FALSE(fa.match("bcca"));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_FALSE(fa.match("bc"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_FALSE(fa.match("ca"));
  EXPECT_FALSE(fa.match("abababababababababababab"));
  EXPECT_FALSE(fa.match("bcbcbcbcbcbcbcbcbcbcbcbc"));
  EXPECT_FALSE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("cacacacacacacacacacacaca"));
  EXPECT_FALSE(fa.match("ababababababbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("ccccccccccccccccccccccccccccccc"));
  EXPECT_FALSE(fa.isDeterministic());
  fa::Automaton fa2 = fa.createDeterministic(fa);
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  do_not_have_symbol(fa2, "abc");
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isLanguageEmpty());
  EXPECT_FALSE(fa2.match("abbb"));
  EXPECT_FALSE(fa2.match("abca"));
  EXPECT_FALSE(fa2.match("bcbb"));
  EXPECT_FALSE(fa2.match("bcca"));
  EXPECT_FALSE(fa2.match("ab"));
  EXPECT_FALSE(fa2.match("bc"));
  EXPECT_FALSE(fa2.match("bb"));
  EXPECT_FALSE(fa2.match("ca"));
  EXPECT_FALSE(fa2.match("abababababababababababab"));
  EXPECT_FALSE(fa2.match("bcbcbcbcbcbcbcbcbcbcbcbc"));
  EXPECT_FALSE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa2.match("cacacacacacacacacacacaca"));
  EXPECT_FALSE(fa2.match("ababababababbbbbbbbbbbbb"));
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("ccccccccccccccccccccccccccccccc"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createDeterministic, testWithNoFinalState) {
  fa::Automaton fa;
  for (int i = 0 ; i < 6 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addSymbol('c'));
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(0, 'b', 5));
  EXPECT_TRUE(fa.addTransition(0, 'c', 4));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  EXPECT_TRUE(fa.addTransition(2, 'c', 0));
  EXPECT_TRUE(fa.addTransition(3, 'c', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(5, 'b', 3));
  EXPECT_TRUE(fa.countSymbols() == 3);
  do_not_have_symbol(fa, "abc");
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match("abbb"));
  EXPECT_FALSE(fa.match("abca"));
  EXPECT_FALSE(fa.match("bcbb"));
  EXPECT_FALSE(fa.match("bcca"));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_FALSE(fa.match("bc"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_FALSE(fa.match("ca"));
  EXPECT_FALSE(fa.match("abababababababababababab"));
  EXPECT_FALSE(fa.match("bcbcbcbcbcbcbcbcbcbcbcbc"));
  EXPECT_FALSE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("cacacacacacacacacacacaca"));
  EXPECT_FALSE(fa.match("ababababababbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("ccccccccccccccccccccccccccccccc"));
  EXPECT_FALSE(fa.isDeterministic());
  fa::Automaton fa2 = fa.createDeterministic(fa);
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  do_not_have_symbol(fa2, "abc");
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isLanguageEmpty());
  EXPECT_FALSE(fa2.match("abbb"));
  EXPECT_FALSE(fa2.match("abca"));
  EXPECT_FALSE(fa2.match("bcbb"));
  EXPECT_FALSE(fa2.match("bcca"));
  EXPECT_FALSE(fa2.match("ab"));
  EXPECT_FALSE(fa2.match("bc"));
  EXPECT_FALSE(fa2.match("bb"));
  EXPECT_FALSE(fa2.match("ca"));
  EXPECT_FALSE(fa2.match("abababababababababababab"));
  EXPECT_FALSE(fa2.match("bcbcbcbcbcbcbcbcbcbcbcbc"));
  EXPECT_FALSE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa2.match("cacacacacacacacacacacaca"));
  EXPECT_FALSE(fa2.match("ababababababbbbbbbbbbbbb"));
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("ccccccccccccccccccccccccccccccc"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createDeterministic, testWithTheOneIntoTheTD3Exo9) {
  fa::Automaton fa;
  for (int i = 0 ; i < 5 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(0);
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.addTransition(0, 'b', 0));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'b', 3));
  EXPECT_TRUE(fa.addTransition(3, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbabbb"));
  EXPECT_FALSE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.countSymbols() == 2);
  do_not_have_symbol(fa, "ab");
  fa::Automaton fa2 = fa.createDeterministic(fa);
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_FALSE(fa.countSymbols() != fa2.countSymbols());
  do_not_have_symbol(fa2, "ab");
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbabbb"));
  EXPECT_FALSE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createDeterministic, testWithTheOneTD3Exo11) {
  fa::Automaton fa;
  for (int i = 0 ; i < 5 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
    // Dans l'exemple du TD, après élimination des epsilon-transitions,
    // tous les états se retrouvaient finaux
    fa.setStateFinal(i);
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 3));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 0));
  EXPECT_TRUE(fa.addTransition(4, 'a', 2));
  EXPECT_TRUE(fa.addTransition(4, 'b', 3));
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_EQ(fa.countStates(), ONE*5);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  do_not_have_symbol(fa, "ab");
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("abbbabbbabbbabbbbbbbbbbbaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match(""));
  EXPECT_FALSE(fa.match("ba"));
  EXPECT_FALSE(fa.match("baaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  fa::Automaton fa2 = fa.createDeterministic(fa);
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("abbbabbbabbbabbbbbbbbbbbaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match(""));
  EXPECT_FALSE(fa2.match("ba"));
  EXPECT_FALSE(fa2.match("baaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}
// ======================================================================================================


// ======================================================================================================
// -------------------------- TESTS FOR CREATE IS INCLUDED IN -----------------------
TEST(isIncludedIn, testWithTheSameAutomaton) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addState(6));
  EXPECT_TRUE(fa.addState(7));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.setStateFinal(3);
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 5));
  EXPECT_TRUE(fa.addTransition(1, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'b', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 7));
  EXPECT_TRUE(fa.addTransition(3, 'a', 1));
  EXPECT_TRUE(fa.addTransition(4, 'b', 1));
  EXPECT_TRUE(fa.addTransition(4, 'a', 5));
  EXPECT_TRUE(fa.addTransition(5, 'b', 6));
  EXPECT_TRUE(fa.addTransition(5, 'a', 4));
  EXPECT_TRUE(fa.addTransition(6, 'b', 7));
  EXPECT_TRUE(fa.addTransition(6, 'a', 3));
  EXPECT_TRUE(fa.addTransition(7, 'b', 6));
  EXPECT_TRUE(fa.addTransition(7, 'a', 4));
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.match("a"));
  EXPECT_TRUE(fa.match("aababaababaababaababa"));
  EXPECT_TRUE(fa.match("bbaba"));
  EXPECT_TRUE(fa.match("bb"));
  EXPECT_TRUE(fa.match("ba"));
  EXPECT_TRUE(fa.match("bbbbababbba"));
  fa::Automaton fa2 = fa;
  EXPECT_TRUE(fa.isIncludedIn(fa2));
}

TEST(isIncludedIn, testWithTwoAutomatonWithDifferentsAlphabet) {
  fa::Automaton fa, fa2;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa2.addSymbol('c'));
  EXPECT_TRUE(fa2.addSymbol('d'));
  for (int i = 0 ; i < 3 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
    EXPECT_TRUE(fa2.addState(i));
  }
  fa.setStateInitial(0);
  fa2.setStateInitial(0);
  fa.setStateFinal(2);
  fa2.setStateFinal(2);
  for (int i = 0 ; i < 2 ; ++i) {
    EXPECT_TRUE(fa.addTransition(i, 'a', i+1));
    EXPECT_TRUE(fa.addTransition(i, 'b', i+1));
    EXPECT_TRUE(fa2.addTransition(i, 'c', i+1));
    EXPECT_TRUE(fa2.addTransition(i, 'd', i+1));
  }
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 2));
  EXPECT_TRUE(fa2.addTransition(2, 'c', 2));
  EXPECT_TRUE(fa2.addTransition(2, 'd', 2));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("cccccccccccccccccccccccccccccccccccc"));
  EXPECT_TRUE(fa2.match("dddddddddddddddddddddddddddddddddddddd"));
  EXPECT_TRUE(fa.match("bababababababababababababababababababab"));
  EXPECT_TRUE(fa.match("ababababababababababababababababababababababa"));
  EXPECT_TRUE(fa2.match("cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdc"));
  EXPECT_TRUE(fa2.match("dcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdc"));
  EXPECT_FALSE(fa.isIncludedIn(fa2));
}

TEST(isIncludedIn, twoAutomatonsEmpty_alias_noTransitions) {
  fa::Automaton fa, fa2;
  for (int i = 0 ; i < 3 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
    EXPECT_TRUE(fa2.addState(i));
  }
  for (char ch = 'a' ; ch <= 'b' ; ++ch) {
    EXPECT_TRUE(fa.addSymbol(ch));
    EXPECT_TRUE(fa2.addSymbol(ch));
  }
  fa.setStateInitial(0);
  fa2.setStateInitial(0);
  fa.setStateFinal(2);
  fa2.setStateFinal(2);
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa2.isValid());
  // L'intersection du vide avec le vide donne le vide, logique
  EXPECT_TRUE(fa.isIncludedIn(fa2));
}

TEST(isIncludedIn, testWithANonDeterministicOneThenDeterminiseItAndAssertTrue) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  fa.setStateInitial(0);
  fa.setStateInitial(5);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 4));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 5));
  EXPECT_TRUE(fa.addTransition(3, 'b', 1));
  EXPECT_TRUE(fa.addTransition(3, 'a', 4));
  EXPECT_TRUE(fa.addTransition(4, 'b', 2));
  EXPECT_TRUE(fa.addTransition(5, 'b', 4));
  EXPECT_TRUE(fa.addTransition(5, 'a', 2));
  EXPECT_TRUE(fa.match("aa"));
  EXPECT_TRUE(fa.match("aab"));
  EXPECT_TRUE(fa.match("aba"));
  EXPECT_TRUE(fa.match("aab"));
  EXPECT_TRUE(fa.match("aababababa"));
  EXPECT_TRUE(fa.match("aabbabababa"));
  EXPECT_TRUE(fa.match("abababababa"));
  EXPECT_TRUE(fa.match("aabbabababa"));
  EXPECT_TRUE(fa.match("aabbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("aabbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("ababbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("aabbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isDeterministic());
  int symb_before = fa.countSymbols();
  fa::Automaton fa2 = fa.createDeterministic(fa);
  int symb_after = fa2.countSymbols();
  EXPECT_EQ(symb_after, symb_before);
  EXPECT_TRUE(fa2.hasSymbol('a'));
  EXPECT_TRUE(fa2.hasSymbol('b'));
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.match("aa"));
  EXPECT_TRUE(fa2.match("aab"));
  EXPECT_TRUE(fa2.match("aba"));
  EXPECT_TRUE(fa2.match("aab"));
  EXPECT_TRUE(fa2.match("aababababa"));
  EXPECT_TRUE(fa2.match("aabbabababa"));
  EXPECT_TRUE(fa2.match("abababababa"));
  EXPECT_TRUE(fa2.match("aabbabababa"));
  EXPECT_TRUE(fa2.match("aabbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("aabbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("ababbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("aabbbbbbbbbbbbb"));
  EXPECT_FALSE(fa2.isLanguageEmpty());
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(fa.isIncludedIn(fa2));
}

TEST(isIncludedIn, testWithANonMinimalOneThenMinimiseItAndThenAssertTrue) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addState(6));
  EXPECT_TRUE(fa.addState(7));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.setStateFinal(3);
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 5));
  EXPECT_TRUE(fa.addTransition(1, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'b', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 7));
  EXPECT_TRUE(fa.addTransition(3, 'a', 1));
  EXPECT_TRUE(fa.addTransition(4, 'b', 1));
  EXPECT_TRUE(fa.addTransition(4, 'a', 5));
  EXPECT_TRUE(fa.addTransition(5, 'b', 6));
  EXPECT_TRUE(fa.addTransition(5, 'a', 4));
  EXPECT_TRUE(fa.addTransition(6, 'b', 7));
  EXPECT_TRUE(fa.addTransition(6, 'a', 3));
  EXPECT_TRUE(fa.addTransition(7, 'b', 6));
  EXPECT_TRUE(fa.addTransition(7, 'a', 4));
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.match("a"));
  EXPECT_TRUE(fa.match("aababaababaababaababa"));
  EXPECT_TRUE(fa.match("bbaba"));
  EXPECT_TRUE(fa.match("bb"));
  EXPECT_TRUE(fa.match("ba"));
  EXPECT_TRUE(fa.match("bbbbababbba"));
  fa::Automaton fa2 = fa.createMinimalBrzozowski(fa);
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_TRUE(fa2.match("aababaababaababaababa"));
  EXPECT_TRUE(fa2.match("bbaba"));
  EXPECT_TRUE(fa2.match("bb"));
  EXPECT_TRUE(fa2.match("ba"));
  EXPECT_TRUE(fa2.match("bbbbababbba"));
  EXPECT_TRUE(fa.countStates() != fa2.countStates());
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(fa.isIncludedIn(fa2));
}

TEST(isIncludedIn, testWithTheSameAlphabetButNotTheSameLanguage) {
  fa::Automaton fa, fa2;
  for (int i = 0 ; i < 3 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
    EXPECT_TRUE(fa2.addState(i));
  }
  fa.setStateInitial(0);
  fa.setStateFinal(2);
  fa2.setStateInitial(0);
  fa2.setStateFinal(2);
  for (char ch = 'a' ; ch <= 'b' ; ++ch) {
    EXPECT_TRUE(fa.addSymbol(ch));
    EXPECT_TRUE(fa2.addSymbol(ch));
  }
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa2.addTransition(0, 'b', 1));
  EXPECT_TRUE(fa2.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_FALSE(fa.match("ba"));
  EXPECT_FALSE(fa2.match("ab"));
  EXPECT_TRUE(fa2.match("ba"));
  EXPECT_TRUE(fa.isValid() && fa2.isValid());
  EXPECT_TRUE(!fa.isLanguageEmpty() && !fa2.isLanguageEmpty());
  EXPECT_FALSE(fa.isIncludedIn(fa2));
}

TEST(isIncludedIn, testWithTheSameAlphabetInclusionExists) {
  fa::Automaton fa, fa2;
  for (int i = 0 ; i < 3 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
    EXPECT_TRUE(fa2.addState(i));
  }
  fa.setStateInitial(0);
  fa.setStateFinal(2);
  fa2.setStateInitial(0);
  fa2.setStateFinal(2);
  for (char ch = 'a' ; ch <= 'b' ; ++ch) {
    EXPECT_TRUE(fa.addSymbol(ch));
    EXPECT_TRUE(fa2.addSymbol(ch));
  }
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa2.addTransition(0, 'b', 1));
  EXPECT_TRUE(fa2.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_TRUE(fa.match("ba"));
  EXPECT_FALSE(fa2.match("ab"));
  EXPECT_TRUE(fa2.match("ba"));
  EXPECT_TRUE(fa.isValid() && fa2.isValid());
  EXPECT_TRUE(!fa.isLanguageEmpty() && !fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isIncludedIn(fa));
}

TEST(isIncludedIn, testWithAnInitialEmptyOneAndAnOtherRemovingNonAccessibleStates) {
  fa::Automaton fa, fa2;
  EXPECT_TRUE(fa2.addState(1));
  EXPECT_TRUE(fa2.addState(2));
  fa2.setStateInitial(1);
  fa2.setStateFinal(2);
  EXPECT_TRUE(fa2.addSymbol('a'));
  EXPECT_TRUE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateFinal(3);
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match("a"));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_FALSE(fa.match("ba"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_TRUE(fa.isValid());
  fa.removeNonAccessibleStates();
  // Reconnaît le langage vide car plus d'état final
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match("a"));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_FALSE(fa.match("ba"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_FALSE(fa.hasState(1));
  EXPECT_TRUE(fa.hasTransition(0, 'a', 2));
  EXPECT_FALSE(fa.hasTransition(1, 'b', 3));
  EXPECT_EQ(fa.countTransitions(), ONE);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*2);
  EXPECT_TRUE(fa.isIncludedIn(fa2));
}
// ======================================================================================================


// Ce sont les tests pour la minimisation qui me prennent le plus de lignes de code
// + de 2000 rien que pour Moore + Brzozowski....

// ======================================================================================================
// -------------------------- TESTS FOR CREATE MINIMAL MOORE -----------------------
TEST(createMinimalMoore, testFromThe2023CC1) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addState(6));
  EXPECT_TRUE(fa.addState(7));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.setStateFinal(3);
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 5));
  EXPECT_TRUE(fa.addTransition(1, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'b', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 7));
  EXPECT_TRUE(fa.addTransition(3, 'a', 1));
  EXPECT_TRUE(fa.addTransition(4, 'b', 1));
  EXPECT_TRUE(fa.addTransition(4, 'a', 5));
  EXPECT_TRUE(fa.addTransition(5, 'b', 6));
  EXPECT_TRUE(fa.addTransition(5, 'a', 4));
  EXPECT_TRUE(fa.addTransition(6, 'b', 7));
  EXPECT_TRUE(fa.addTransition(6, 'a', 3));
  EXPECT_TRUE(fa.addTransition(7, 'b', 6));
  EXPECT_TRUE(fa.addTransition(7, 'a', 4));
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.match("a"));
  EXPECT_TRUE(fa.match("aababaababaababaababa"));
  EXPECT_TRUE(fa.match("bbaba"));
  EXPECT_TRUE(fa.match("bb"));
  EXPECT_TRUE(fa.match("ba"));
  EXPECT_TRUE(fa.match("bbbbababbba"));
  fa::Automaton fa2 = fa.createMinimalMoore(fa);
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_TRUE(fa2.match("aababaababaababaababa"));
  EXPECT_TRUE(fa2.match("bbaba"));
  EXPECT_TRUE(fa2.match("bb"));
  EXPECT_TRUE(fa2.match("ba"));
  EXPECT_TRUE(fa2.match("bbbbababbba"));
  EXPECT_TRUE(fa.countStates() != fa2.countStates());
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalMoore, testWithThe2022CC1One) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addState(6));
  fa.setStateInitial(0);
  fa.setStateFinal(2);
  fa.setStateFinal(6);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 4));
  EXPECT_TRUE(fa.addTransition(2, 'b', 6));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(4, 'a', 5));
  EXPECT_TRUE(fa.addTransition(4, 'b', 3));
  EXPECT_TRUE(fa.addTransition(5, 'b', 2));
  EXPECT_TRUE(fa.addTransition(5, 'a', 4));
  EXPECT_TRUE(fa.addTransition(6, 'b', 6));
  EXPECT_TRUE(fa.isValid());
  EXPECT_EQ(fa.countTransitions(), ONE*11);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*7);
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaab"));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_TRUE(fa.match("abb"));
  EXPECT_TRUE(fa.match("abbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("bbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.isComplete());
  EXPECT_TRUE(fa.isDeterministic());
  fa::Automaton fa2 = fa.createMinimalMoore(fa);
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaab"));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_TRUE(fa.match("abb"));
  EXPECT_TRUE(fa.match("abbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("bbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa2.countStates() == fa.countStates());
  EXPECT_TRUE(fa2.countSymbols() == fa.countSymbols());
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalMoore, testWithOneOfTheTD5Exo16) {
  fa::Automaton fa;
  for (int i = 0 ; i < 6 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(0);
  fa.setStateFinal(5);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 0));
  EXPECT_TRUE(fa.addTransition(2, 'b', 0));
  EXPECT_TRUE(fa.addTransition(1, 'a', 4));
  EXPECT_TRUE(fa.addTransition(1, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(4, 'b', 5));
  EXPECT_TRUE(fa.addTransition(3, 'a', 5));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(5, 'a', 3));
  EXPECT_TRUE(fa.addTransition(5, 'b', 4));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbab"));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_TRUE(fa.match("aaababababababababababb"));
  EXPECT_FALSE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("babababababababababababababababababaa"));
  EXPECT_TRUE(fa.countStates() == 6);
  EXPECT_TRUE(fa.countSymbols() == 2);
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  fa::Automaton fa2 = fa.createMinimalMoore(fa);
  EXPECT_TRUE(fa.countStates() == fa2.countStates());
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_TRUE(fa2.isValid());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbab"));
  EXPECT_TRUE(fa2.match("ab"));
  EXPECT_TRUE(fa2.match("aaababababababababababb"));
  EXPECT_FALSE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa2.match("babababababababababababababababababaa"));
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalMoore, testWithOneOfTheTD5Exo17) {
  fa::Automaton fa;
  for (int i = 0 ; i < 6 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(0);
  fa.setStateFinal(5);
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 3));
  EXPECT_TRUE(fa.addTransition(1, 'a', 0));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'b', 5));
  EXPECT_TRUE(fa.addTransition(3, 'a', 1));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 0));
  EXPECT_TRUE(fa.addTransition(4, 'b', 4));
  EXPECT_TRUE(fa.addTransition(5, 'a', 1));
  EXPECT_TRUE(fa.addTransition(5, 'b', 4));
  EXPECT_TRUE(fa.countStates() == 6);
  EXPECT_TRUE(fa.countSymbols() == 2);
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaabb"));
  EXPECT_TRUE(fa.match("baabaabaabaabaabaabaabaabaabababababbb"));
  EXPECT_FALSE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba"));
  EXPECT_FALSE(fa.match("b"));
  EXPECT_TRUE(fa.match("bb"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"));
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  fa::Automaton fa2 = fa.createMinimalMoore(fa);
  EXPECT_TRUE(fa.countStates() != fa2.countStates());
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_TRUE(fa2.isValid());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaabb"));
  EXPECT_TRUE(fa2.match("baabaabaabaabaabaabaabaabaabababababbb"));
  EXPECT_FALSE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba"));
  EXPECT_FALSE(fa2.match("b"));
  EXPECT_TRUE(fa2.match("bb"));
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"));
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalMoore, testFromTheAutomatonTD5Exo18) {
  fa::Automaton fa;
  for (int i = 0 ; i < 6 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addSymbol('c'));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(0, 'b', 5));
  EXPECT_TRUE(fa.addTransition(0, 'c', 4));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  EXPECT_TRUE(fa.addTransition(2, 'c', 0));
  EXPECT_TRUE(fa.addTransition(3, 'c', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(5, 'b', 3));
  EXPECT_TRUE(fa.countSymbols() == 3);
  EXPECT_TRUE(fa.countStates() == 6);
  do_not_have_symbol(fa, "abc");
  EXPECT_TRUE(fa.match(""));
  EXPECT_TRUE(fa.match("abbb"));
  EXPECT_TRUE(fa.match("abca"));
  EXPECT_TRUE(fa.match("bcbb"));
  EXPECT_TRUE(fa.match("bcca"));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_TRUE(fa.match("bc"));
  EXPECT_TRUE(fa.match("bb"));
  EXPECT_TRUE(fa.match("ca"));
  EXPECT_TRUE(fa.match("abababababababababababab"));
  EXPECT_TRUE(fa.match("bcbcbcbcbcbcbcbcbcbcbcbc"));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("cacacacacacacacacacacaca"));
  EXPECT_TRUE(fa.match("ababababababbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("ccccccccccccccccccccccccccccccc"));
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  fa::Automaton fa2 = fa.createMinimalMoore(fa);
  do_not_have_symbol(fa2, "abc");
  EXPECT_TRUE(fa.countStates() != fa2.countStates());
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.match(""));
  EXPECT_TRUE(fa2.match("abbb"));
  EXPECT_TRUE(fa2.match("abca"));
  EXPECT_TRUE(fa2.match("bcbb"));
  EXPECT_TRUE(fa2.match("bcca"));
  EXPECT_TRUE(fa2.match("ab"));
  EXPECT_TRUE(fa2.match("bc"));
  EXPECT_TRUE(fa2.match("bb"));
  EXPECT_TRUE(fa2.match("ca"));
  EXPECT_TRUE(fa2.match("abababababababababababab"));
  EXPECT_TRUE(fa2.match("bcbcbcbcbcbcbcbcbcbcbcbc"));
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("cacacacacacacacacacacaca"));
  EXPECT_TRUE(fa2.match("ababababababbbbbbbbbbbbb"));
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("ccccccccccccccccccccccccccccccc"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalMoore, testWithAVerySmallAutomaton) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  fa.setStateFinal(0);
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  EXPECT_TRUE(fa.match(""));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("a"));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_TRUE(fa.countStates() == 1);
  EXPECT_TRUE(fa.countSymbols() == 1);
  do_not_have_symbol(fa, "a");
  fa::Automaton fa2 = fa.createMinimalMoore(fa);
  do_not_have_symbol(fa2, "a");
  EXPECT_TRUE(fa.countStates() == fa2.countStates());
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_FALSE(fa2.match("ab"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalMoore, testWithTheOneIntoTheCarnetDeTravauxLibresExo31) {
  fa::Automaton fa;
  for (int i = 0 ; i < 5 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 0));
  EXPECT_TRUE(fa.addTransition(2, 'b', 3));
  EXPECT_TRUE(fa.addTransition(3, 'a', 2));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_FALSE(fa.isComplete());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.match("a"));
  EXPECT_TRUE(fa.match("abababababababababababababababa"));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("aab"));
  EXPECT_TRUE(fa.match("aabababababababababababababab"));
  EXPECT_TRUE(fa.match("aabbabababababababababababababa"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.match("bababababababababababababababa"));
  EXPECT_FALSE(fa.match("aababababababababababababababb"));
  do_not_have_symbol(fa, "ab");
  EXPECT_EQ(fa.countStates(), ONE*5);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  fa::Automaton fa2 = fa.createMinimalMoore(fa);
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_TRUE(fa2.match("abababababababababababababababa"));
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("aab"));
  EXPECT_TRUE(fa2.match("aabababababababababababababab"));
  EXPECT_TRUE(fa2.match("aabbabababababababababababababa"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_FALSE(fa2.match("bababababababababababababababa"));
  EXPECT_FALSE(fa2.match("aababababababababababababababb"));
  do_not_have_symbol(fa2, "ab");
  EXPECT_FALSE(fa.countStates() == fa2.countStates());
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalMoore, testWithTheOneIntoTheCarnetDeTravauxLibresExo32) {
  fa::Automaton fa;
  for (int i = 0 ; i < 7 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(2);
  fa.setStateFinal(0);
  fa.setStateFinal(3);
  fa.setStateFinal(5);
  EXPECT_TRUE(fa.addTransition(0, 'a', 4));
  EXPECT_TRUE(fa.addTransition(0, 'b', 3));
  EXPECT_TRUE(fa.addTransition(1, 'a', 0));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 0));
  EXPECT_TRUE(fa.addTransition(2, 'b', 3));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 1));
  EXPECT_TRUE(fa.addTransition(4, 'b', 6));
  EXPECT_TRUE(fa.addTransition(5, 'a', 4));
  EXPECT_TRUE(fa.addTransition(5, 'b', 3));
  EXPECT_TRUE(fa.addTransition(6, 'a', 5));
  EXPECT_TRUE(fa.addTransition(6, 'b', 3));
  do_not_have_symbol(fa, "ab");
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*7);
  EXPECT_TRUE(fa.match("a"));
  EXPECT_TRUE(fa.match("baaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("abaaaaaaaaaaaaababbaaaaaaaaaaaaaababbaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("bb"));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("aabb"));
  EXPECT_TRUE(fa.match("aabbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_FALSE(fa.match("aaa"));
  fa::Automaton fa2 = fa.createMinimalMoore(fa);
  do_not_have_symbol(fa2, "ab");
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  EXPECT_FALSE(fa.countStates() == fa2.countStates());
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_TRUE(fa2.match("baaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("abaaaaaaaaaaaaababbaaaaaaaaaaaaaababbaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("bb"));
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("aabb"));
  EXPECT_TRUE(fa2.match("aabbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_FALSE(fa2.match("aa"));
  EXPECT_FALSE(fa2.match("aaa"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalMoore, testWithTheOneIntoThe2023CC1_alias_2InitialsStates) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  fa.setStateInitial(0);
  fa.setStateInitial(5);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 4));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 5));
  EXPECT_TRUE(fa.addTransition(3, 'b', 1));
  EXPECT_TRUE(fa.addTransition(3, 'a', 4));
  EXPECT_TRUE(fa.addTransition(4, 'b', 2));
  EXPECT_TRUE(fa.addTransition(5, 'b', 4));
  EXPECT_TRUE(fa.addTransition(5, 'a', 2));
  EXPECT_TRUE(fa.match("aa"));
  EXPECT_TRUE(fa.match("aab"));
  EXPECT_TRUE(fa.match("aba"));
  EXPECT_TRUE(fa.match("aab"));
  EXPECT_TRUE(fa.match("aababababa"));
  EXPECT_TRUE(fa.match("aabbabababa"));
  EXPECT_TRUE(fa.match("abababababa"));
  EXPECT_TRUE(fa.match("aabbabababa"));
  EXPECT_TRUE(fa.match("aabbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("aabbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("ababbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("aabbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  do_not_have_symbol(fa, "ab");
  EXPECT_EQ(fa.countStates(), ONE*6);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  fa::Automaton fa2 = fa.createMinimalMoore(fa);
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_FALSE(fa.countStates() == fa2.countStates());
  EXPECT_TRUE(fa2.match("aa"));
  EXPECT_TRUE(fa2.match("aab"));
  EXPECT_TRUE(fa2.match("aba"));
  EXPECT_TRUE(fa2.match("aab"));
  EXPECT_TRUE(fa2.match("aababababa"));
  EXPECT_TRUE(fa2.match("aabbabababa"));
  EXPECT_TRUE(fa2.match("abababababa"));
  EXPECT_TRUE(fa2.match("aabbabababa"));
  EXPECT_TRUE(fa2.match("aabbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("aabbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("ababbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("aabbbbbbbbbbbbb"));
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isValid());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalMoore, testWithAPoorStateLostNoWhere) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateFinal(2);
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_FALSE(fa.match("ba"));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_FALSE(fa.match("aab"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  do_not_have_symbol(fa, "ab");
  EXPECT_EQ(fa.countStates(), ONE*4);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  fa::Automaton fa2 = fa.createMinimalMoore(fa);
  EXPECT_TRUE(fa2.match("ab"));
  EXPECT_FALSE(fa2.match("bb"));
  EXPECT_FALSE(fa2.match("ba"));
  EXPECT_FALSE(fa2.match("aa"));
  EXPECT_FALSE(fa2.match("aab"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isValid());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  do_not_have_symbol(fa2, "ab");
  // IL FAUT CONSIDÉRER UN ÉTAT COMME POUBELLE DANS CREATECOMPLETE S'IL N'A AUCUNE TRANSITION
  EXPECT_TRUE(fa.countStates() == fa2.countStates());
  EXPECT_EQ(fa2.countSymbols(), fa.countSymbols());
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalMoore, testWithTwoPoorsStatesLostNoWhere) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.setStateFinal(2);
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_FALSE(fa.match("ba"));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_FALSE(fa.match("aab"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  do_not_have_symbol(fa, "ab");
  EXPECT_EQ(fa.countStates(), ONE*5);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  fa::Automaton fa2 = fa.createMinimalMoore(fa);
  EXPECT_TRUE(fa2.match("ab"));
  EXPECT_FALSE(fa2.match("bb"));
  EXPECT_FALSE(fa2.match("ba"));
  EXPECT_FALSE(fa2.match("aa"));
  EXPECT_FALSE(fa2.match("aab"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isValid());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  do_not_have_symbol(fa2, "ab");
  EXPECT_FALSE(fa.countStates() == fa2.countStates());
  EXPECT_EQ(fa2.countSymbols(), fa.countSymbols());
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalMoore, testWithThreePoorsStatesLostNoWhere) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  fa.setStateFinal(2);
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_FALSE(fa.match("ba"));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_FALSE(fa.match("aab"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  do_not_have_symbol(fa, "ab");
  EXPECT_EQ(fa.countStates(), ONE*6);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  fa::Automaton fa2 = fa.createMinimalMoore(fa);
  EXPECT_TRUE(fa2.match("ab"));
  EXPECT_FALSE(fa2.match("bb"));
  EXPECT_FALSE(fa2.match("ba"));
  EXPECT_FALSE(fa2.match("aa"));
  EXPECT_FALSE(fa2.match("aab"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isValid());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  do_not_have_symbol(fa2, "ab");
  EXPECT_FALSE(fa.countStates() == fa2.countStates());
  EXPECT_EQ(fa2.countSymbols(), fa.countSymbols());
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalMoore, testWithAVerySmallAutomatonButStateIsINT_MAX) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(INT_MAX));
  EXPECT_TRUE(fa.addTransition(INT_MAX, 'a', INT_MAX));
  fa.setStateFinal(INT_MAX);
  fa.setStateInitial(INT_MAX);
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  EXPECT_TRUE(fa.match(""));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("a"));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_TRUE(fa.countStates() == 1);
  EXPECT_TRUE(fa.countSymbols() == 1);
  do_not_have_symbol(fa, "a");
  fa::Automaton fa2 = fa.createMinimalMoore(fa);
  do_not_have_symbol(fa2, "a");
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa.countStates() == fa2.countStates());
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_FALSE(fa2.match("ab"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalMoore, testWithAVerySmallAutomatonButStateIsINT_MAXandINT_MAX_minus_one) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(INT_MAX));
  EXPECT_TRUE(fa.addState(INT_MAX-1));
  EXPECT_TRUE(fa.addTransition(INT_MAX, 'a', INT_MAX));
  EXPECT_TRUE(fa.addTransition(INT_MAX, 'a', INT_MAX-1));
  EXPECT_TRUE(fa.addTransition(INT_MAX-1, 'a', INT_MAX));
  fa.setStateFinal(INT_MAX);
  fa.setStateInitial(INT_MAX);
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  EXPECT_TRUE(fa.match(""));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("a"));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_TRUE(fa.countStates() == 2);
  EXPECT_TRUE(fa.countSymbols() == 1);
  do_not_have_symbol(fa, "a");
  fa::Automaton fa2 = fa.createMinimalMoore(fa);
  do_not_have_symbol(fa2, "a");
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa.countStates() != fa2.countStates());
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_FALSE(fa2.match("ab"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalMoore, testWithAVerySmallAutomatonButStateBigValuesAndNotComplete) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(INT_MAX));
  EXPECT_TRUE(fa.addState(INT_MAX-1));
  EXPECT_TRUE(fa.addTransition(INT_MAX, 'a', INT_MAX));
  EXPECT_TRUE(fa.addTransition(INT_MAX, 'a', INT_MAX-1));
  EXPECT_TRUE(fa.addTransition(INT_MAX-1, 'a', INT_MAX));
  fa.setStateFinal(INT_MAX);
  fa.setStateInitial(INT_MAX);
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  EXPECT_TRUE(fa.match(""));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("a"));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_TRUE(fa.countStates() == 2);
  EXPECT_TRUE(fa.countSymbols() == 2);
  do_not_have_symbol(fa, "ab");
  fa::Automaton fa2 = fa.createMinimalMoore(fa);
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa.countStates() == fa2.countStates());
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_FALSE(fa2.match("ab"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalMoore, empty) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addSymbol('c'));
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.isLanguageEmpty());
  do_not_have_symbol(fa, "abc");
  EXPECT_EQ(fa.countStates(), ONE);
  EXPECT_EQ(fa.countSymbols(), ONE*3);
  EXPECT_FALSE(fa.isComplete());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isLanguageEmpty());
  fa::Automaton fa2 = fa.createMinimalMoore(fa);
  EXPECT_TRUE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.isDeterministic());
  do_not_have_symbol(fa2, "abc");
  EXPECT_EQ(fa2.countStates(), ONE);
  EXPECT_EQ(fa2.countSymbols(), ONE*3);
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalMoore, nonDeterministicNonCompleteAndNoFinalState) {
  fa::Automaton toCompare;
  EXPECT_TRUE(toCompare.addState(0));
  EXPECT_TRUE(toCompare.addSymbol('a'));
  toCompare.setStateInitial(0);
  EXPECT_TRUE(toCompare.isLanguageEmpty());
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 0));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("aabbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isIncludedIn(toCompare));
  EXPECT_TRUE(toCompare.isIncludedIn(fa));
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*3);
  do_not_have_symbol(fa, "ab");
  fa::Automaton fa2 = fa.createMinimalMoore(fa);
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("aabbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"));
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isIncludedIn(toCompare));
  EXPECT_TRUE(toCompare.isIncludedIn(fa2));
  EXPECT_TRUE(fa.isIncludedIn(fa2));
  EXPECT_TRUE(fa2.isIncludedIn(fa));
  EXPECT_EQ(fa2.countSymbols(), fa.countSymbols());
  EXPECT_EQ(fa2.countStates(), ONE);
  do_not_have_symbol(fa2, "ab");
}

TEST(createMinimalMoore, nonDeterministicNonCompleteAndNoInitialState) {
  fa::Automaton toCompare;
  EXPECT_TRUE(toCompare.addState(0));
  EXPECT_TRUE(toCompare.addSymbol('a'));
  toCompare.setStateFinal(0);
  EXPECT_TRUE(toCompare.isLanguageEmpty());
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 0));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("aabbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isIncludedIn(toCompare));
  EXPECT_TRUE(toCompare.isIncludedIn(fa));
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*3);
  do_not_have_symbol(fa, "ab");
  fa::Automaton fa2 = fa.createMinimalMoore(fa);
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("aabbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"));
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isIncludedIn(toCompare));
  EXPECT_TRUE(toCompare.isIncludedIn(fa2));
  EXPECT_TRUE(fa.isIncludedIn(fa2));
  EXPECT_TRUE(fa2.isIncludedIn(fa));
  EXPECT_EQ(fa2.countSymbols(), fa.countSymbols());
  EXPECT_EQ(fa2.countStates(), ONE);
  do_not_have_symbol(fa2, "ab");
}

// Je reprend copier-coller le nom du test de la moulinette
// Ce dernier me laisse perplexe sur ce qu'il reste à faire
// Notamment la suppression d'un état inutile, qui ne doit pas figurer là
// Espérons que ce test échoue sur mon implem, pour que je puisse voir
// l'origine de l'échec et quelle étourderie j'ai (encore) pu faire
TEST(createMinimalMoore, FinalTest_alias_NotMinimal2) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 2));
  EXPECT_TRUE(fa.match("a"));
  EXPECT_TRUE(fa.match("b"));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_TRUE(fa.match("ba"));
  EXPECT_TRUE(fa.match("ababababababababababababbbbbbbaaaaaaaaaabbbbbbbbbaaaababababababababbb"));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("ababababababababababababababababababababababababababababababababababab"));
  EXPECT_TRUE(fa.match("bababababababababababababababababababababababababababababababababababa"));
  EXPECT_TRUE(fa.match("abababababababababababababababababababababababababababababababababbbbb"));
  EXPECT_TRUE(fa.match("bababababababababababababababababababababababababababababababababaaaaa"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match(""));
  EXPECT_EQ(fa.countStates(), ONE*3);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  do_not_have_symbol(fa, "ab");
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  // Automate déterministe, complet, mais pas minimal
  fa::Automaton fa2 = fa.createMinimalMoore(fa);
  EXPECT_EQ(fa2.countStates(), ONE*2);
  EXPECT_EQ(fa2.countSymbols(), ONE*2);
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_TRUE(fa2.match("b"));
  EXPECT_TRUE(fa2.match("ab"));
  EXPECT_TRUE(fa2.match("ba"));
  EXPECT_TRUE(fa2.match("ababababababababababababbbbbbbaaaaaaaaaabbbbbbbbbaaaababababababababbb"));
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("ababababababababababababababababababababababababababababababababababab"));
  EXPECT_TRUE(fa2.match("bababababababababababababababababababababababababababababababababababa"));
  EXPECT_TRUE(fa2.match("abababababababababababababababababababababababababababababababababbbbb"));
  EXPECT_TRUE(fa2.match("bababababababababababababababababababababababababababababababababaaaaa"));
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_FALSE(fa2.match(""));
  EXPECT_TRUE(sameLanguage(fa, fa2));  
}

TEST(createMinimalMoore, testWithA14StatesAutomatonDeterministicAndCompleteButNotMinimal) {
  fa::Automaton fa;
  for (int i = 0 ; i < 14 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addSymbol('c'));
  fa.setStateInitial(0);
  fa.setStateFinal(13);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(0, 'c', 3));
  EXPECT_TRUE(fa.addTransition(1, 'a', 4));
  EXPECT_TRUE(fa.addTransition(1, 'b', 5));
  EXPECT_TRUE(fa.addTransition(1, 'c', 6));
  EXPECT_TRUE(fa.addTransition(2, 'a', 7));
  EXPECT_TRUE(fa.addTransition(2, 'b', 8));
  EXPECT_TRUE(fa.addTransition(2, 'c', 9));
  EXPECT_TRUE(fa.addTransition(3, 'a', 10));
  EXPECT_TRUE(fa.addTransition(3, 'b', 11));
  EXPECT_TRUE(fa.addTransition(3, 'c', 12));
  for (int i = 4 ; i < 11 ; i+=3) {
    EXPECT_TRUE(fa.addTransition(i, 'a', 13));
    EXPECT_TRUE(fa.addTransition(i, 'b', i));
    EXPECT_TRUE(fa.addTransition(i, 'c', i));
  }
  for (int i = 5 ; i < 12 ; i+=3) {
    EXPECT_TRUE(fa.addTransition(i, 'a', i));
    EXPECT_TRUE(fa.addTransition(i, 'b', 13));
    EXPECT_TRUE(fa.addTransition(i, 'c', i));
  }
  for (int i = 6 ; i < 13 ; i+=3) {
    EXPECT_TRUE(fa.addTransition(i, 'a', i));
    EXPECT_TRUE(fa.addTransition(i, 'b', i));
    EXPECT_TRUE(fa.addTransition(i, 'c', 13));
  }
  EXPECT_TRUE(fa.addTransition(13, 'a', 13));
  EXPECT_TRUE(fa.addTransition(13, 'b', 13));
  EXPECT_TRUE(fa.addTransition(13, 'c', 13));
  do_not_have_symbol(fa, "abc");
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  EXPECT_EQ(fa.countStates(), ONE*14);
  EXPECT_EQ(fa.countSymbols(), ONE*3);
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match(""));
  EXPECT_TRUE(fa.match("ccaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbababababccccccccccccccccabababab"));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"));
  EXPECT_FALSE(fa.match("bac"));
  EXPECT_FALSE(fa.match("cac"));
  EXPECT_FALSE(fa.match("cab"));
  EXPECT_FALSE(fa.match("abc"));
  EXPECT_FALSE(fa.match("ccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("bbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("bbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("bbcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"));
  EXPECT_TRUE(fa.match("aaa"));
  EXPECT_TRUE(fa.match("bbb"));
  EXPECT_TRUE(fa.match("ccc"));
  fa::Automaton fa2 = fa.createMinimalMoore(fa);
  do_not_have_symbol(fa2, "abc");
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_EQ(fa2.countStates(), ONE*6);
  EXPECT_FALSE(fa.countStates() == fa2.countStates());
  EXPECT_EQ(fa2.countSymbols(), ONE*3);
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_FALSE(fa2.match(""));
  EXPECT_TRUE(fa2.match("ccaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbababababccccccccccccccccabababab"));
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"));
  EXPECT_FALSE(fa2.match("bac"));
  EXPECT_FALSE(fa2.match("cac"));
  EXPECT_FALSE(fa2.match("cab"));
  EXPECT_FALSE(fa2.match("abc"));
  EXPECT_FALSE(fa2.match("ccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("bbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("bbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("bbcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"));
  EXPECT_TRUE(fa2.match("aaa"));
  EXPECT_TRUE(fa2.match("bbb"));
  EXPECT_TRUE(fa2.match("ccc"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}
// ======================================================================================================


// ======================================================================================================
// -------------------------- TESTS FOR CREATE MINIMAL BRZOZOWSKI -----------------------
TEST(createMinimalBrzozowski, testFromThe2023CC1) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addState(6));
  EXPECT_TRUE(fa.addState(7));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.setStateFinal(3);
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 5));
  EXPECT_TRUE(fa.addTransition(1, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'b', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 7));
  EXPECT_TRUE(fa.addTransition(3, 'a', 1));
  EXPECT_TRUE(fa.addTransition(4, 'b', 1));
  EXPECT_TRUE(fa.addTransition(4, 'a', 5));
  EXPECT_TRUE(fa.addTransition(5, 'b', 6));
  EXPECT_TRUE(fa.addTransition(5, 'a', 4));
  EXPECT_TRUE(fa.addTransition(6, 'b', 7));
  EXPECT_TRUE(fa.addTransition(6, 'a', 3));
  EXPECT_TRUE(fa.addTransition(7, 'b', 6));
  EXPECT_TRUE(fa.addTransition(7, 'a', 4));
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.match("a"));
  EXPECT_TRUE(fa.match("aababaababaababaababa"));
  EXPECT_TRUE(fa.match("bbaba"));
  EXPECT_TRUE(fa.match("bb"));
  EXPECT_TRUE(fa.match("ba"));
  EXPECT_TRUE(fa.match("bbbbababbba"));
  fa::Automaton fa2 = fa.createMinimalBrzozowski(fa);
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa.match("a"));
  EXPECT_TRUE(fa.match("aababaababaababaababa"));
  EXPECT_TRUE(fa.match("bbaba"));
  EXPECT_TRUE(fa.match("bb"));
  EXPECT_TRUE(fa.match("ba"));
  EXPECT_TRUE(fa.match("bbbbababbba"));
  EXPECT_TRUE(fa.countStates() != fa2.countStates());
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalBrzozowski, testWithThe2022CC1One) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addState(6));
  fa.setStateInitial(0);
  fa.setStateFinal(2);
  fa.setStateFinal(6);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 4));
  EXPECT_TRUE(fa.addTransition(2, 'b', 6));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(4, 'a', 5));
  EXPECT_TRUE(fa.addTransition(4, 'b', 3));
  EXPECT_TRUE(fa.addTransition(5, 'b', 2));
  EXPECT_TRUE(fa.addTransition(5, 'a', 4));
  EXPECT_TRUE(fa.addTransition(6, 'b', 6));
  EXPECT_TRUE(fa.isValid());
  EXPECT_EQ(fa.countTransitions(), ONE*11);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*7);
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaab"));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_TRUE(fa.match("abb"));
  EXPECT_TRUE(fa.match("abbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("bbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  fa::Automaton fa2 = fa.createMinimalBrzozowski(fa);
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaab"));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_TRUE(fa.match("abb"));
  EXPECT_TRUE(fa.match("abbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("bbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa2.countStates() == fa.countStates());
  EXPECT_TRUE(fa2.countSymbols() == fa.countSymbols());
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalBrzozowski, testWithOneOfTheTD5Exo16) {
  fa::Automaton fa;
  for (int i = 0 ; i < 6 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(0);
  fa.setStateFinal(5);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 0));
  EXPECT_TRUE(fa.addTransition(2, 'b', 0));
  EXPECT_TRUE(fa.addTransition(1, 'a', 4));
  EXPECT_TRUE(fa.addTransition(1, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(4, 'b', 5));
  EXPECT_TRUE(fa.addTransition(3, 'a', 5));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(5, 'a', 3));
  EXPECT_TRUE(fa.addTransition(5, 'b', 4));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbab"));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_TRUE(fa.match("aaababababababababababb"));
  EXPECT_FALSE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("babababababababababababababababababaa"));
  EXPECT_TRUE(fa.countStates() == 6);
  EXPECT_TRUE(fa.countSymbols() == 2);
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  fa::Automaton fa2 = fa.createMinimalBrzozowski(fa);
  EXPECT_TRUE(fa.countStates() == fa2.countStates());
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_TRUE(fa2.isValid());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbab"));
  EXPECT_TRUE(fa2.match("ab"));
  EXPECT_TRUE(fa2.match("aaababababababababababb"));
  EXPECT_FALSE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa2.match("babababababababababababababababababaa"));
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalBrzozowski, testWithOneOfTheTD5Exo17) {
  fa::Automaton fa;
  for (int i = 0 ; i < 6 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(0);
  fa.setStateFinal(5);
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 3));
  EXPECT_TRUE(fa.addTransition(1, 'a', 0));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'b', 5));
  EXPECT_TRUE(fa.addTransition(3, 'a', 1));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 0));
  EXPECT_TRUE(fa.addTransition(4, 'b', 4));
  EXPECT_TRUE(fa.addTransition(5, 'a', 1));
  EXPECT_TRUE(fa.addTransition(5, 'b', 4));
  EXPECT_TRUE(fa.countStates() == 6);
  EXPECT_TRUE(fa.countSymbols() == 2);
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaabb"));
  EXPECT_TRUE(fa.match("baabaabaabaabaabaabaabaabaabababababbb"));
  EXPECT_FALSE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba"));
  EXPECT_FALSE(fa.match("b"));
  EXPECT_TRUE(fa.match("bb"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"));
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  fa::Automaton fa2 = fa.createMinimalBrzozowski(fa);
  EXPECT_TRUE(fa.countStates() != fa2.countStates());
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_TRUE(fa2.isValid());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaabb"));
  EXPECT_TRUE(fa2.match("baabaabaabaabaabaabaabaabaabababababbb"));
  EXPECT_FALSE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba"));
  EXPECT_FALSE(fa2.match("b"));
  EXPECT_TRUE(fa2.match("bb"));
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"));
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalBrzozowski, testFromTheAutomatonTD5Exo18) {
  fa::Automaton fa;
  for (int i = 0 ; i < 6 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addSymbol('c'));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(0, 'b', 5));
  EXPECT_TRUE(fa.addTransition(0, 'c', 4));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  EXPECT_TRUE(fa.addTransition(2, 'c', 0));
  EXPECT_TRUE(fa.addTransition(3, 'c', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(5, 'b', 3));
  EXPECT_TRUE(fa.countSymbols() == 3);
  EXPECT_TRUE(fa.countStates() == 6);
  do_not_have_symbol(fa, "abc");
  EXPECT_TRUE(fa.match(""));
  EXPECT_TRUE(fa.match("abbb"));
  EXPECT_TRUE(fa.match("abca"));
  EXPECT_TRUE(fa.match("bcbb"));
  EXPECT_TRUE(fa.match("bcca"));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_TRUE(fa.match("bc"));
  EXPECT_TRUE(fa.match("bb"));
  EXPECT_TRUE(fa.match("ca"));
  EXPECT_TRUE(fa.match("abababababababababababab"));
  EXPECT_TRUE(fa.match("bcbcbcbcbcbcbcbcbcbcbcbc"));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("cacacacacacacacacacacaca"));
  EXPECT_TRUE(fa.match("ababababababbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("ccccccccccccccccccccccccccccccc"));
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  fa::Automaton fa2 = fa.createMinimalBrzozowski(fa);
  do_not_have_symbol(fa2, "abc");
  EXPECT_TRUE(fa.countStates() != fa2.countStates());
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.match(""));
  EXPECT_TRUE(fa2.match("abbb"));
  EXPECT_TRUE(fa2.match("abca"));
  EXPECT_TRUE(fa2.match("bcbb"));
  EXPECT_TRUE(fa2.match("bcca"));
  EXPECT_TRUE(fa2.match("ab"));
  EXPECT_TRUE(fa2.match("bc"));
  EXPECT_TRUE(fa2.match("bb"));
  EXPECT_TRUE(fa2.match("ca"));
  EXPECT_TRUE(fa2.match("abababababababababababab"));
  EXPECT_TRUE(fa2.match("bcbcbcbcbcbcbcbcbcbcbcbc"));
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("cacacacacacacacacacacaca"));
  EXPECT_TRUE(fa2.match("ababababababbbbbbbbbbbbb"));
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("ccccccccccccccccccccccccccccccc"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalBrzozowski, testWithAVerySmallAutomaton) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  fa.setStateFinal(0);
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  EXPECT_TRUE(fa.match(""));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("a"));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_TRUE(fa.countStates() == 1);
  EXPECT_TRUE(fa.countSymbols() == 1);
  do_not_have_symbol(fa, "a");
  fa::Automaton fa2 = fa.createMinimalBrzozowski(fa);
  do_not_have_symbol(fa2, "a");
  EXPECT_TRUE(fa.countStates() == fa2.countStates());
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_FALSE(fa2.match("ab"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalBrzozowski, testWithTheOneIntoTheCarnetDeTravauxLibresExo31) {
  fa::Automaton fa;
  for (int i = 0 ; i < 5 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 0));
  EXPECT_TRUE(fa.addTransition(2, 'b', 3));
  EXPECT_TRUE(fa.addTransition(3, 'a', 2));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_FALSE(fa.isComplete());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.match("a"));
  EXPECT_TRUE(fa.match("abababababababababababababababa"));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("aab"));
  EXPECT_TRUE(fa.match("aabababababababababababababab"));
  EXPECT_TRUE(fa.match("aabbabababababababababababababa"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.match("bababababababababababababababa"));
  EXPECT_FALSE(fa.match("aababababababababababababababb"));
  do_not_have_symbol(fa, "ab");
  EXPECT_EQ(fa.countStates(), ONE*5);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  fa::Automaton fa2 = fa.createMinimalBrzozowski(fa);
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_TRUE(fa2.match("abababababababababababababababa"));
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("aab"));
  EXPECT_TRUE(fa2.match("aabababababababababababababab"));
  EXPECT_TRUE(fa2.match("aabbabababababababababababababa"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_FALSE(fa2.match("bababababababababababababababa"));
  EXPECT_FALSE(fa2.match("aababababababababababababababb"));
  do_not_have_symbol(fa2, "ab");
  EXPECT_FALSE(fa.countStates() == fa2.countStates());
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalBrzozowski, testWithTheOneIntoTheCarnetDeTravauxLibresExo32) {
  fa::Automaton fa;
  for (int i = 0 ; i < 7 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(2);
  fa.setStateFinal(0);
  fa.setStateFinal(3);
  fa.setStateFinal(5);
  EXPECT_TRUE(fa.addTransition(0, 'a', 4));
  EXPECT_TRUE(fa.addTransition(0, 'b', 3));
  EXPECT_TRUE(fa.addTransition(1, 'a', 0));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 0));
  EXPECT_TRUE(fa.addTransition(2, 'b', 3));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 1));
  EXPECT_TRUE(fa.addTransition(4, 'b', 6));
  EXPECT_TRUE(fa.addTransition(5, 'a', 4));
  EXPECT_TRUE(fa.addTransition(5, 'b', 3));
  EXPECT_TRUE(fa.addTransition(6, 'a', 5));
  EXPECT_TRUE(fa.addTransition(6, 'b', 3));
  do_not_have_symbol(fa, "ab");
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*7);
  EXPECT_TRUE(fa.match("a"));
  EXPECT_TRUE(fa.match("baaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("abaaaaaaaaaaaaababbaaaaaaaaaaaaaababbaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("bb"));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("aabb"));
  EXPECT_TRUE(fa.match("aabbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_FALSE(fa.match("aaa"));
  fa::Automaton fa2 = fa.createMinimalBrzozowski(fa);
  do_not_have_symbol(fa2, "ab");
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  EXPECT_FALSE(fa.countStates() == fa2.countStates());
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_TRUE(fa2.match("baaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("abaaaaaaaaaaaaababbaaaaaaaaaaaaaababbaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("bb"));
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("aabb"));
  EXPECT_TRUE(fa2.match("aabbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_FALSE(fa2.match("aa"));
  EXPECT_FALSE(fa2.match("aaa"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalBrzozowski, testWithTheOneIntoThe2023CC1_alias_2InitialsStates) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  fa.setStateInitial(0);
  fa.setStateInitial(5);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 4));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 5));
  EXPECT_TRUE(fa.addTransition(3, 'b', 1));
  EXPECT_TRUE(fa.addTransition(3, 'a', 4));
  EXPECT_TRUE(fa.addTransition(4, 'b', 2));
  EXPECT_TRUE(fa.addTransition(5, 'b', 4));
  EXPECT_TRUE(fa.addTransition(5, 'a', 2));
  EXPECT_TRUE(fa.match("aa"));
  EXPECT_TRUE(fa.match("aab"));
  EXPECT_TRUE(fa.match("aba"));
  EXPECT_TRUE(fa.match("aab"));
  EXPECT_TRUE(fa.match("aababababa"));
  EXPECT_TRUE(fa.match("aabbabababa"));
  EXPECT_TRUE(fa.match("abababababa"));
  EXPECT_TRUE(fa.match("aabbabababa"));
  EXPECT_TRUE(fa.match("aabbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("aabbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("ababbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("aabbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  do_not_have_symbol(fa, "ab");
  EXPECT_EQ(fa.countStates(), ONE*6);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  fa::Automaton fa2 = fa.createMinimalBrzozowski(fa);
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_FALSE(fa.countStates() == fa2.countStates());
  EXPECT_TRUE(fa2.match("aa"));
  EXPECT_TRUE(fa2.match("aab"));
  EXPECT_TRUE(fa2.match("aba"));
  EXPECT_TRUE(fa2.match("aab"));
  EXPECT_TRUE(fa2.match("aababababa"));
  EXPECT_TRUE(fa2.match("aabbabababa"));
  EXPECT_TRUE(fa2.match("abababababa"));
  EXPECT_TRUE(fa2.match("aabbabababa"));
  EXPECT_TRUE(fa2.match("aabbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("aabbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("ababbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("aabbbbbbbbbbbbb"));
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isValid());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalBrzozowski, testWithAPoorStateLostNoWhere) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateFinal(2);
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_FALSE(fa.match("ba"));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_FALSE(fa.match("aab"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  do_not_have_symbol(fa, "ab");
  EXPECT_EQ(fa.countStates(), ONE*4);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  fa::Automaton fa2 = fa.createMinimalBrzozowski(fa);
  EXPECT_TRUE(fa2.match("ab"));
  EXPECT_FALSE(fa2.match("bb"));
  EXPECT_FALSE(fa2.match("ba"));
  EXPECT_FALSE(fa2.match("aa"));
  EXPECT_FALSE(fa2.match("aab"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isValid());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(fa.countStates() == fa2.countStates());
  EXPECT_EQ(fa2.countSymbols(), fa.countSymbols());
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalBrzozowski, testWithTwoPoorsStatesLostNoWhere) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.setStateFinal(2);
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_FALSE(fa.match("ba"));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_FALSE(fa.match("aab"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  do_not_have_symbol(fa, "ab");
  EXPECT_EQ(fa.countStates(), ONE*5);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  fa::Automaton fa2 = fa.createMinimalBrzozowski(fa);
  EXPECT_TRUE(fa2.match("ab"));
  EXPECT_FALSE(fa2.match("bb"));
  EXPECT_FALSE(fa2.match("ba"));
  EXPECT_FALSE(fa2.match("aa"));
  EXPECT_FALSE(fa2.match("aab"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isValid());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  do_not_have_symbol(fa2, "ab");
  EXPECT_FALSE(fa.countStates() == fa2.countStates());
  EXPECT_EQ(fa2.countSymbols(), fa.countSymbols());
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalBrzozowski, testWithThreePoorsStatesLostNoWhere) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  fa.setStateFinal(2);
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_FALSE(fa.match("ba"));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_FALSE(fa.match("aab"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  do_not_have_symbol(fa, "ab");
  EXPECT_EQ(fa.countStates(), ONE*6);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  fa::Automaton fa2 = fa.createMinimalBrzozowski(fa);
  EXPECT_TRUE(fa2.match("ab"));
  EXPECT_FALSE(fa2.match("bb"));
  EXPECT_FALSE(fa2.match("ba"));
  EXPECT_FALSE(fa2.match("aa"));
  EXPECT_FALSE(fa2.match("aab"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isValid());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  do_not_have_symbol(fa2, "ab");
  EXPECT_FALSE(fa.countStates() == fa2.countStates());
  EXPECT_EQ(fa2.countSymbols(), fa.countSymbols());
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalBrzozowski, testWithAVerySmallAutomatonButStateIsINT_MAX) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(INT_MAX));
  EXPECT_TRUE(fa.addTransition(INT_MAX, 'a', INT_MAX));
  fa.setStateFinal(INT_MAX);
  fa.setStateInitial(INT_MAX);
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  EXPECT_TRUE(fa.match(""));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("a"));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_TRUE(fa.countStates() == 1);
  EXPECT_TRUE(fa.countSymbols() == 1);
  do_not_have_symbol(fa, "a");
  fa::Automaton fa2 = fa.createMinimalBrzozowski(fa);
  do_not_have_symbol(fa2, "a");
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa.countStates() == fa2.countStates());
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_FALSE(fa2.match("ab"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalBrzozowski, testWithAVerySmallAutomatonButStateIsINT_MAXandINT_MAX_minus_one) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(INT_MAX));
  EXPECT_TRUE(fa.addState(INT_MAX-1));
  EXPECT_TRUE(fa.addTransition(INT_MAX, 'a', INT_MAX));
  EXPECT_TRUE(fa.addTransition(INT_MAX, 'a', INT_MAX-1));
  EXPECT_TRUE(fa.addTransition(INT_MAX-1, 'a', INT_MAX));
  fa.setStateFinal(INT_MAX);
  fa.setStateInitial(INT_MAX);

  fa.prettyPrint(std::cout);

  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  EXPECT_TRUE(fa.match(""));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("a"));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_TRUE(fa.countStates() == 2);
  EXPECT_TRUE(fa.countSymbols() == 1);
  do_not_have_symbol(fa, "a");
  fa::Automaton fa2 = fa.createMinimalBrzozowski(fa);
  fa2.prettyPrint(std::cout);
  do_not_have_symbol(fa2, "a");
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa.countStates() != fa2.countStates());
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_FALSE(fa2.match("ab"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalBrzozowski, testWithAVerySmallAutomatonButStateBigValuesAndNotComplete) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(INT_MAX));
  EXPECT_TRUE(fa.addState(INT_MAX-1));
  EXPECT_TRUE(fa.addTransition(INT_MAX, 'a', INT_MAX));
  EXPECT_TRUE(fa.addTransition(INT_MAX, 'a', INT_MAX-1));
  EXPECT_TRUE(fa.addTransition(INT_MAX-1, 'a', INT_MAX));
  fa.setStateFinal(INT_MAX);
  fa.setStateInitial(INT_MAX);
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  EXPECT_TRUE(fa.match(""));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("a"));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_TRUE(fa.countStates() == 2);
  EXPECT_TRUE(fa.countSymbols() == 2);
  do_not_have_symbol(fa, "ab");
  fa::Automaton fa2 = fa.createMinimalBrzozowski(fa);
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa.countStates() == fa2.countStates());
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_FALSE(fa2.match("ab"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalBrzozowski, empty) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addSymbol('c'));
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.isLanguageEmpty());
  do_not_have_symbol(fa, "abc");
  EXPECT_EQ(fa.countStates(), ONE);
  EXPECT_EQ(fa.countSymbols(), ONE*3);
  EXPECT_FALSE(fa.isComplete());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isLanguageEmpty());
  fa::Automaton fa2 = fa.createMinimalBrzozowski(fa);
  EXPECT_TRUE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.isDeterministic());
  do_not_have_symbol(fa2, "abc");
  EXPECT_EQ(fa2.countStates(), ONE);
  EXPECT_EQ(fa2.countSymbols(), ONE*3);
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalBrzozowski, nonDeterministicNonCompleteAndNoFinalState) {
  fa::Automaton toCompare;
  EXPECT_TRUE(toCompare.addState(0));
  EXPECT_TRUE(toCompare.addSymbol('a'));
  toCompare.setStateInitial(0);
  EXPECT_TRUE(toCompare.isLanguageEmpty());
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 0));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("aabbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isIncludedIn(toCompare));
  EXPECT_TRUE(toCompare.isIncludedIn(fa));
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*3);
  do_not_have_symbol(fa, "ab");
  fa::Automaton fa2 = fa.createMinimalBrzozowski(fa);
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("aabbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"));
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isIncludedIn(toCompare));
  EXPECT_TRUE(toCompare.isIncludedIn(fa2));
  EXPECT_TRUE(fa.isIncludedIn(fa2));
  EXPECT_TRUE(fa2.isIncludedIn(fa));
  EXPECT_EQ(fa2.countSymbols(), fa.countSymbols());
  EXPECT_EQ(fa2.countStates(), ONE);
  do_not_have_symbol(fa2, "ab");
}

TEST(createMinimalBrzozowski, nonDeterministicNonCompleteAndNoInitialState) {
  fa::Automaton toCompare;
  EXPECT_TRUE(toCompare.addState(0));
  EXPECT_TRUE(toCompare.addSymbol('a'));
  toCompare.setStateFinal(0);
  EXPECT_TRUE(toCompare.isLanguageEmpty());
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 0));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("aabbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isIncludedIn(toCompare));
  EXPECT_TRUE(toCompare.isIncludedIn(fa));
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*3);
  do_not_have_symbol(fa, "ab");
  fa::Automaton fa2 = fa.createMinimalBrzozowski(fa);
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("aabbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"));
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isIncludedIn(toCompare));
  EXPECT_TRUE(toCompare.isIncludedIn(fa2));
  EXPECT_TRUE(fa.isIncludedIn(fa2));
  EXPECT_TRUE(fa2.isIncludedIn(fa));
  EXPECT_EQ(fa2.countSymbols(), fa.countSymbols());
  EXPECT_EQ(fa2.countStates(), ONE);
  do_not_have_symbol(fa2, "ab");
}

TEST(createMinimalBrzozowski, FinalTest_alias_NotMinimal2) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 2));
  EXPECT_TRUE(fa.match("a"));
  EXPECT_TRUE(fa.match("b"));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_TRUE(fa.match("ba"));
  EXPECT_TRUE(fa.match("ababababababababababababbbbbbbaaaaaaaaaabbbbbbbbbaaaababababababababbb"));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("ababababababababababababababababababababababababababababababababababab"));
  EXPECT_TRUE(fa.match("bababababababababababababababababababababababababababababababababababa"));
  EXPECT_TRUE(fa.match("abababababababababababababababababababababababababababababababababbbbb"));
  EXPECT_TRUE(fa.match("bababababababababababababababababababababababababababababababababaaaaa"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match(""));
  EXPECT_EQ(fa.countStates(), ONE*3);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  do_not_have_symbol(fa, "ab");
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  // Automate déterministe, complet, mais pas minimal
  fa::Automaton fa2 = fa.createMinimalBrzozowski(fa);
  EXPECT_EQ(fa2.countStates(), ONE*2);
  EXPECT_EQ(fa2.countSymbols(), ONE*2);
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_TRUE(fa2.match("b"));
  EXPECT_TRUE(fa2.match("ab"));
  EXPECT_TRUE(fa2.match("ba"));
  EXPECT_TRUE(fa2.match("ababababababababababababbbbbbbaaaaaaaaaabbbbbbbbbaaaababababababababbb"));
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("ababababababababababababababababababababababababababababababababababab"));
  EXPECT_TRUE(fa2.match("bababababababababababababababababababababababababababababababababababa"));
  EXPECT_TRUE(fa2.match("abababababababababababababababababababababababababababababababababbbbb"));
  EXPECT_TRUE(fa2.match("bababababababababababababababababababababababababababababababababaaaaa"));
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_FALSE(fa2.match(""));
  EXPECT_TRUE(sameLanguage(fa, fa2));  
}

TEST(createMinimalBrzozowski, testWithA14StatesAutomatonDeterministicAndCompleteButNotMinimal) {
  fa::Automaton fa;
  for (int i = 0 ; i < 14 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addSymbol('c'));
  fa.setStateInitial(0);
  fa.setStateFinal(13);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(0, 'c', 3));
  EXPECT_TRUE(fa.addTransition(1, 'a', 4));
  EXPECT_TRUE(fa.addTransition(1, 'b', 5));
  EXPECT_TRUE(fa.addTransition(1, 'c', 6));
  EXPECT_TRUE(fa.addTransition(2, 'a', 7));
  EXPECT_TRUE(fa.addTransition(2, 'b', 8));
  EXPECT_TRUE(fa.addTransition(2, 'c', 9));
  EXPECT_TRUE(fa.addTransition(3, 'a', 10));
  EXPECT_TRUE(fa.addTransition(3, 'b', 11));
  EXPECT_TRUE(fa.addTransition(3, 'c', 12));
  for (int i = 4 ; i < 11 ; i+=3) {
    EXPECT_TRUE(fa.addTransition(i, 'a', 13));
    EXPECT_TRUE(fa.addTransition(i, 'b', i));
    EXPECT_TRUE(fa.addTransition(i, 'c', i));
  }
  for (int i = 5 ; i < 12 ; i+=3) {
    EXPECT_TRUE(fa.addTransition(i, 'a', i));
    EXPECT_TRUE(fa.addTransition(i, 'b', 13));
    EXPECT_TRUE(fa.addTransition(i, 'c', i));
  }
  for (int i = 6 ; i < 13 ; i+=3) {
    EXPECT_TRUE(fa.addTransition(i, 'a', i));
    EXPECT_TRUE(fa.addTransition(i, 'b', i));
    EXPECT_TRUE(fa.addTransition(i, 'c', 13));
  }
  EXPECT_TRUE(fa.addTransition(13, 'a', 13));
  EXPECT_TRUE(fa.addTransition(13, 'b', 13));
  EXPECT_TRUE(fa.addTransition(13, 'c', 13));
  do_not_have_symbol(fa, "abc");
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  EXPECT_EQ(fa.countStates(), ONE*14);
  EXPECT_EQ(fa.countSymbols(), ONE*3);
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match(""));
  EXPECT_TRUE(fa.match("ccaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbababababccccccccccccccccabababab"));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"));
  EXPECT_FALSE(fa.match("bac"));
  EXPECT_FALSE(fa.match("cac"));
  EXPECT_FALSE(fa.match("cab"));
  EXPECT_FALSE(fa.match("abc"));
  EXPECT_FALSE(fa.match("ccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("bbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("bbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("bbcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"));
  EXPECT_TRUE(fa.match("aaa"));
  EXPECT_TRUE(fa.match("bbb"));
  EXPECT_TRUE(fa.match("ccc"));
  fa::Automaton fa2 = fa.createMinimalBrzozowski(fa);
  do_not_have_symbol(fa2, "abc");
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_EQ(fa2.countStates(), ONE*6);
  EXPECT_FALSE(fa.countStates() == fa2.countStates());
  EXPECT_EQ(fa2.countSymbols(), ONE*3);
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_FALSE(fa2.match(""));
  EXPECT_TRUE(fa2.match("ccaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbababababccccccccccccccccabababab"));
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"));
  EXPECT_FALSE(fa2.match("bac"));
  EXPECT_FALSE(fa2.match("cac"));
  EXPECT_FALSE(fa2.match("cab"));
  EXPECT_FALSE(fa2.match("abc"));
  EXPECT_FALSE(fa2.match("ccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("bbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("bbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("bbcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"));
  EXPECT_TRUE(fa2.match("aaa"));
  EXPECT_TRUE(fa2.match("bbb"));
  EXPECT_TRUE(fa2.match("ccc"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}
// ======================================================================================================


// Ça fait tout de même 1000 lignes de tests en ifdef... pour seulement 27 tests
// 27 tests qui prennent 1000 lignes de codes....

// ======================================================================================================
// ======================================================================================================
// -------------------------- TESTS FOR CREATE MINIMAL HOPCROFT -----------------------
// ======================================================================================================
// ======================================================================================================
// IFDEF MY_AUTOMATON
// ======================================================================================================
// ======================================================================================================

#ifdef TEST_HOPCROFT

TEST(createMinimalHopcroft, testFromThe2023CC1) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addState(6));
  EXPECT_TRUE(fa.addState(7));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.setStateFinal(3);
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 5));
  EXPECT_TRUE(fa.addTransition(1, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'b', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 7));
  EXPECT_TRUE(fa.addTransition(3, 'a', 1));
  EXPECT_TRUE(fa.addTransition(4, 'b', 1));
  EXPECT_TRUE(fa.addTransition(4, 'a', 5));
  EXPECT_TRUE(fa.addTransition(5, 'b', 6));
  EXPECT_TRUE(fa.addTransition(5, 'a', 4));
  EXPECT_TRUE(fa.addTransition(6, 'b', 7));
  EXPECT_TRUE(fa.addTransition(6, 'a', 3));
  EXPECT_TRUE(fa.addTransition(7, 'b', 6));
  EXPECT_TRUE(fa.addTransition(7, 'a', 4));
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.match("a"));
  EXPECT_TRUE(fa.match("aababaababaababaababa"));
  EXPECT_TRUE(fa.match("bbaba"));
  EXPECT_TRUE(fa.match("bb"));
  EXPECT_TRUE(fa.match("ba"));
  EXPECT_TRUE(fa.match("bbbbababbba"));
  fa::Automaton fa2 = fa.createMinimalHopcroft(fa);
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa.match("a"));
  EXPECT_TRUE(fa.match("aababaababaababaababa"));
  EXPECT_TRUE(fa.match("bbaba"));
  EXPECT_TRUE(fa.match("bb"));
  EXPECT_TRUE(fa.match("ba"));
  EXPECT_TRUE(fa.match("bbbbababbba"));
  EXPECT_TRUE(fa.countStates() != fa2.countStates());
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalHopcroft, testWithThe2022CC1One) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addState(6));
  fa.setStateInitial(0);
  fa.setStateFinal(2);
  fa.setStateFinal(6);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 4));
  EXPECT_TRUE(fa.addTransition(2, 'b', 6));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(4, 'a', 5));
  EXPECT_TRUE(fa.addTransition(4, 'b', 3));
  EXPECT_TRUE(fa.addTransition(5, 'b', 2));
  EXPECT_TRUE(fa.addTransition(5, 'a', 4));
  EXPECT_TRUE(fa.addTransition(6, 'b', 6));
  EXPECT_TRUE(fa.isValid());
  EXPECT_EQ(fa.countTransitions(), ONE*11);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*7);
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaab"));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_TRUE(fa.match("abb"));
  EXPECT_TRUE(fa.match("abbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("bbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  fa::Automaton fa2 = fa.createMinimalHopcroft(fa);
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaab"));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_TRUE(fa.match("abb"));
  EXPECT_TRUE(fa.match("abbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("bbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa2.countStates() == fa.countStates());
  EXPECT_TRUE(fa2.countSymbols() == fa.countSymbols());
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalHopcroft, testWithOneOfTheTD5Exo16) {
  fa::Automaton fa;
  for (int i = 0 ; i < 6 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(0);
  fa.setStateFinal(5);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 0));
  EXPECT_TRUE(fa.addTransition(2, 'b', 0));
  EXPECT_TRUE(fa.addTransition(1, 'a', 4));
  EXPECT_TRUE(fa.addTransition(1, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(4, 'b', 5));
  EXPECT_TRUE(fa.addTransition(3, 'a', 5));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(5, 'a', 3));
  EXPECT_TRUE(fa.addTransition(5, 'b', 4));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbab"));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_TRUE(fa.match("aaababababababababababb"));
  EXPECT_FALSE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("babababababababababababababababababaa"));
  EXPECT_TRUE(fa.countStates() == 6);
  EXPECT_TRUE(fa.countSymbols() == 2);
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  fa::Automaton fa2 = fa.createMinimalHopcroft(fa);
  EXPECT_TRUE(fa.countStates() == fa2.countStates());
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_TRUE(fa2.isValid());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbab"));
  EXPECT_TRUE(fa2.match("ab"));
  EXPECT_TRUE(fa2.match("aaababababababababababb"));
  EXPECT_FALSE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_FALSE(fa2.match("babababababababababababababababababaa"));
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalHopcroft, testWithOneOfTheTD5Exo17) {
  fa::Automaton fa;
  for (int i = 0 ; i < 6 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(0);
  fa.setStateFinal(5);
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 3));
  EXPECT_TRUE(fa.addTransition(1, 'a', 0));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'b', 5));
  EXPECT_TRUE(fa.addTransition(3, 'a', 1));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 0));
  EXPECT_TRUE(fa.addTransition(4, 'b', 4));
  EXPECT_TRUE(fa.addTransition(5, 'a', 1));
  EXPECT_TRUE(fa.addTransition(5, 'b', 4));
  EXPECT_TRUE(fa.countStates() == 6);
  EXPECT_TRUE(fa.countSymbols() == 2);
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaabb"));
  EXPECT_TRUE(fa.match("baabaabaabaabaabaabaabaabaabababababbb"));
  EXPECT_FALSE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba"));
  EXPECT_FALSE(fa.match("b"));
  EXPECT_TRUE(fa.match("bb"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"));
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  fa::Automaton fa2 = fa.createMinimalHopcroft(fa);
  EXPECT_TRUE(fa.countStates() != fa2.countStates());
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_TRUE(fa2.isValid());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaabb"));
  EXPECT_TRUE(fa2.match("baabaabaabaabaabaabaabaabaabababababbb"));
  EXPECT_FALSE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba"));
  EXPECT_FALSE(fa2.match("b"));
  EXPECT_TRUE(fa2.match("bb"));
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"));
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalHopcroft, testFromTheAutomatonTD5Exo18) {
  fa::Automaton fa;
  for (int i = 0 ; i < 6 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addSymbol('c'));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(0, 'b', 5));
  EXPECT_TRUE(fa.addTransition(0, 'c', 4));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  EXPECT_TRUE(fa.addTransition(2, 'c', 0));
  EXPECT_TRUE(fa.addTransition(3, 'c', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(5, 'b', 3));
  EXPECT_TRUE(fa.countSymbols() == 3);
  EXPECT_TRUE(fa.countStates() == 6);
  do_not_have_symbol(fa, "abc");
  EXPECT_TRUE(fa.match(""));
  EXPECT_TRUE(fa.match("abbb"));
  EXPECT_TRUE(fa.match("abca"));
  EXPECT_TRUE(fa.match("bcbb"));
  EXPECT_TRUE(fa.match("bcca"));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_TRUE(fa.match("bc"));
  EXPECT_TRUE(fa.match("bb"));
  EXPECT_TRUE(fa.match("ca"));
  EXPECT_TRUE(fa.match("abababababababababababab"));
  EXPECT_TRUE(fa.match("bcbcbcbcbcbcbcbcbcbcbcbc"));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("cacacacacacacacacacacaca"));
  EXPECT_TRUE(fa.match("ababababababbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("ccccccccccccccccccccccccccccccc"));
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  fa::Automaton fa2 = fa.createMinimalHopcroft(fa);
  do_not_have_symbol(fa2, "abc");
  EXPECT_TRUE(fa.countStates() != fa2.countStates());
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.match(""));
  EXPECT_TRUE(fa2.match("abbb"));
  EXPECT_TRUE(fa2.match("abca"));
  EXPECT_TRUE(fa2.match("bcbb"));
  EXPECT_TRUE(fa2.match("bcca"));
  EXPECT_TRUE(fa2.match("ab"));
  EXPECT_TRUE(fa2.match("bc"));
  EXPECT_TRUE(fa2.match("bb"));
  EXPECT_TRUE(fa2.match("ca"));
  EXPECT_TRUE(fa2.match("abababababababababababab"));
  EXPECT_TRUE(fa2.match("bcbcbcbcbcbcbcbcbcbcbcbc"));
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("cacacacacacacacacacacaca"));
  EXPECT_TRUE(fa2.match("ababababababbbbbbbbbbbbb"));
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("ccccccccccccccccccccccccccccccc"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalHopcroft, testWithAVerySmallAutomaton) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  fa.setStateFinal(0);
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  EXPECT_TRUE(fa.match(""));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("a"));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_TRUE(fa.countStates() == 1);
  EXPECT_TRUE(fa.countSymbols() == 1);
  do_not_have_symbol(fa, "a");
  fa::Automaton fa2 = fa.createMinimalHopcroft(fa);
  do_not_have_symbol(fa2, "a");
  EXPECT_TRUE(fa.countStates() == fa2.countStates());
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_FALSE(fa2.match("ab"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalHopcroft, testWithTheOneIntoTheCarnetDeTravauxLibresExo31) {
  fa::Automaton fa;
  for (int i = 0 ; i < 5 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 0));
  EXPECT_TRUE(fa.addTransition(2, 'b', 3));
  EXPECT_TRUE(fa.addTransition(3, 'a', 2));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_FALSE(fa.isComplete());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.match("a"));
  EXPECT_TRUE(fa.match("abababababababababababababababa"));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("aab"));
  EXPECT_TRUE(fa.match("aabababababababababababababab"));
  EXPECT_TRUE(fa.match("aabbabababababababababababababa"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.match("bababababababababababababababa"));
  EXPECT_FALSE(fa.match("aababababababababababababababb"));
  do_not_have_symbol(fa, "ab");
  EXPECT_EQ(fa.countStates(), ONE*5);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  fa::Automaton fa2 = fa.createMinimalHopcroft(fa);
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_TRUE(fa2.match("abababababababababababababababa"));
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("aab"));
  EXPECT_TRUE(fa2.match("aabababababababababababababab"));
  EXPECT_TRUE(fa2.match("aabbabababababababababababababa"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_FALSE(fa2.match("bababababababababababababababa"));
  EXPECT_FALSE(fa2.match("aababababababababababababababb"));
  do_not_have_symbol(fa2, "ab");
  EXPECT_FALSE(fa.countStates() == fa2.countStates());
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalHopcroft, testWithTheOneIntoTheCarnetDeTravauxLibresExo32) {
  fa::Automaton fa;
  for (int i = 0 ; i < 7 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(2);
  fa.setStateFinal(0);
  fa.setStateFinal(3);
  fa.setStateFinal(5);
  EXPECT_TRUE(fa.addTransition(0, 'a', 4));
  EXPECT_TRUE(fa.addTransition(0, 'b', 3));
  EXPECT_TRUE(fa.addTransition(1, 'a', 0));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 0));
  EXPECT_TRUE(fa.addTransition(2, 'b', 3));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 1));
  EXPECT_TRUE(fa.addTransition(4, 'b', 6));
  EXPECT_TRUE(fa.addTransition(5, 'a', 4));
  EXPECT_TRUE(fa.addTransition(5, 'b', 3));
  EXPECT_TRUE(fa.addTransition(6, 'a', 5));
  EXPECT_TRUE(fa.addTransition(6, 'b', 3));
  do_not_have_symbol(fa, "ab");
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*7);
  EXPECT_TRUE(fa.match("a"));
  EXPECT_TRUE(fa.match("baaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("abaaaaaaaaaaaaababbaaaaaaaaaaaaaababbaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("bb"));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("aabb"));
  EXPECT_TRUE(fa.match("aabbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_FALSE(fa.match("aaa"));
  fa::Automaton fa2 = fa.createMinimalHopcroft(fa);
  do_not_have_symbol(fa2, "ab");
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  EXPECT_FALSE(fa.countStates() == fa2.countStates());
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_TRUE(fa2.match("baaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("abaaaaaaaaaaaaababbaaaaaaaaaaaaaababbaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("bb"));
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("aabb"));
  EXPECT_TRUE(fa2.match("aabbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_FALSE(fa2.match("aa"));
  EXPECT_FALSE(fa2.match("aaa"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalHopcroft, testWithTheOneIntoThe2023CC1_alias_2InitialsStates) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  fa.setStateInitial(0);
  fa.setStateInitial(5);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 4));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 5));
  EXPECT_TRUE(fa.addTransition(3, 'b', 1));
  EXPECT_TRUE(fa.addTransition(3, 'a', 4));
  EXPECT_TRUE(fa.addTransition(4, 'b', 2));
  EXPECT_TRUE(fa.addTransition(5, 'b', 4));
  EXPECT_TRUE(fa.addTransition(5, 'a', 2));
  EXPECT_TRUE(fa.match("aa"));
  EXPECT_TRUE(fa.match("aab"));
  EXPECT_TRUE(fa.match("aba"));
  EXPECT_TRUE(fa.match("aab"));
  EXPECT_TRUE(fa.match("aababababa"));
  EXPECT_TRUE(fa.match("aabbabababa"));
  EXPECT_TRUE(fa.match("abababababa"));
  EXPECT_TRUE(fa.match("aabbabababa"));
  EXPECT_TRUE(fa.match("aabbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("aabbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("ababbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("aabbbbbbbbbbbbb"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  do_not_have_symbol(fa, "ab");
  EXPECT_EQ(fa.countStates(), ONE*6);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  fa::Automaton fa2 = fa.createMinimalHopcroft(fa);
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_FALSE(fa.countStates() == fa2.countStates());
  EXPECT_TRUE(fa2.match("aa"));
  EXPECT_TRUE(fa2.match("aab"));
  EXPECT_TRUE(fa2.match("aba"));
  EXPECT_TRUE(fa2.match("aab"));
  EXPECT_TRUE(fa2.match("aababababa"));
  EXPECT_TRUE(fa2.match("aabbabababa"));
  EXPECT_TRUE(fa2.match("abababababa"));
  EXPECT_TRUE(fa2.match("aabbabababa"));
  EXPECT_TRUE(fa2.match("aabbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("aabbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("ababbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("aabbbbbbbbbbbbb"));
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isValid());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalHopcroft, testWithAPoorStateLostNoWhere) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateFinal(2);
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_FALSE(fa.match("ba"));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_FALSE(fa.match("aab"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  do_not_have_symbol(fa, "ab");
  EXPECT_EQ(fa.countStates(), ONE*4);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  fa::Automaton fa2 = fa.createMinimalHopcroft(fa);
  EXPECT_TRUE(fa2.match("ab"));
  EXPECT_FALSE(fa2.match("bb"));
  EXPECT_FALSE(fa2.match("ba"));
  EXPECT_FALSE(fa2.match("aa"));
  EXPECT_FALSE(fa2.match("aab"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isValid());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(fa.countStates() == fa2.countStates());
  EXPECT_EQ(fa2.countSymbols(), fa.countSymbols());
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalHopcroft, testWithTwoPoorsStatesLostNoWhere) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.setStateFinal(2);
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_FALSE(fa.match("ba"));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_FALSE(fa.match("aab"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  do_not_have_symbol(fa, "ab");
  EXPECT_EQ(fa.countStates(), ONE*5);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  fa::Automaton fa2 = fa.createMinimalHopcroft(fa);
  EXPECT_TRUE(fa2.match("ab"));
  EXPECT_FALSE(fa2.match("bb"));
  EXPECT_FALSE(fa2.match("ba"));
  EXPECT_FALSE(fa2.match("aa"));
  EXPECT_FALSE(fa2.match("aab"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isValid());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  do_not_have_symbol(fa2, "ab");
  EXPECT_FALSE(fa.countStates() == fa2.countStates());
  EXPECT_EQ(fa2.countSymbols(), fa.countSymbols());
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalHopcroft, testWithThreePoorsStatesLostNoWhere) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  fa.setStateFinal(2);
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_FALSE(fa.match("bb"));
  EXPECT_FALSE(fa.match("ba"));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_FALSE(fa.match("aab"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  do_not_have_symbol(fa, "ab");
  EXPECT_EQ(fa.countStates(), ONE*6);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  fa::Automaton fa2 = fa.createMinimalHopcroft(fa);
  EXPECT_TRUE(fa2.match("ab"));
  EXPECT_FALSE(fa2.match("bb"));
  EXPECT_FALSE(fa2.match("ba"));
  EXPECT_FALSE(fa2.match("aa"));
  EXPECT_FALSE(fa2.match("aab"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isValid());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  do_not_have_symbol(fa2, "ab");
  EXPECT_FALSE(fa.countStates() == fa2.countStates());
  EXPECT_EQ(fa2.countSymbols(), fa.countSymbols());
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalHopcroft, testWithAVerySmallAutomatonButStateIsINT_MAX) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(INT_MAX));
  EXPECT_TRUE(fa.addTransition(INT_MAX, 'a', INT_MAX));
  fa.setStateFinal(INT_MAX);
  fa.setStateInitial(INT_MAX);
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  EXPECT_TRUE(fa.match(""));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("a"));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_TRUE(fa.countStates() == 1);
  EXPECT_TRUE(fa.countSymbols() == 1);
  do_not_have_symbol(fa, "a");
  fa::Automaton fa2 = fa.createMinimalHopcroft(fa);
  do_not_have_symbol(fa2, "a");
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa.countStates() == fa2.countStates());
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_FALSE(fa2.match("ab"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalHopcroft, testWithAVerySmallAutomatonButStateIsINT_MAXandINT_MAX_minus_one) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(INT_MAX));
  EXPECT_TRUE(fa.addState(INT_MAX-1));
  EXPECT_TRUE(fa.addTransition(INT_MAX, 'a', INT_MAX));
  EXPECT_TRUE(fa.addTransition(INT_MAX, 'a', INT_MAX-1));
  EXPECT_TRUE(fa.addTransition(INT_MAX-1, 'a', INT_MAX));
  fa.setStateFinal(INT_MAX);
  fa.setStateInitial(INT_MAX);
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  EXPECT_TRUE(fa.match(""));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("a"));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_TRUE(fa.countStates() == 2);
  EXPECT_TRUE(fa.countSymbols() == 1);
  do_not_have_symbol(fa, "a");
  fa::Automaton fa2 = fa.createMinimalHopcroft(fa);
  do_not_have_symbol(fa2, "a");
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa.countStates() != fa2.countStates());
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_FALSE(fa2.match("ab"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalHopcroft, testWithAVerySmallAutomatonButStateBigValuesAndNotComplete) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(INT_MAX));
  EXPECT_TRUE(fa.addState(INT_MAX-1));
  EXPECT_TRUE(fa.addTransition(INT_MAX, 'a', INT_MAX));
  EXPECT_TRUE(fa.addTransition(INT_MAX, 'a', INT_MAX-1));
  EXPECT_TRUE(fa.addTransition(INT_MAX-1, 'a', INT_MAX));
  fa.setStateFinal(INT_MAX);
  fa.setStateInitial(INT_MAX);
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  EXPECT_TRUE(fa.match(""));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("a"));
  EXPECT_FALSE(fa.match("ab"));
  EXPECT_TRUE(fa.countStates() == 2);
  EXPECT_TRUE(fa.countSymbols() == 2);
  do_not_have_symbol(fa, "ab");
  fa::Automaton fa2 = fa.createMinimalHopcroft(fa);
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa.countStates() == fa2.countStates());
  EXPECT_TRUE(fa.countSymbols() == fa2.countSymbols());
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_FALSE(fa2.match("ab"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalHopcroft, empty) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addSymbol('c'));
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.isLanguageEmpty());
  do_not_have_symbol(fa, "abc");
  EXPECT_EQ(fa.countStates(), ONE);
  EXPECT_EQ(fa.countSymbols(), ONE*3);
  EXPECT_FALSE(fa.isComplete());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isLanguageEmpty());
  fa::Automaton fa2 = fa.createMinimalHopcroft(fa);
  EXPECT_TRUE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.isDeterministic());
  do_not_have_symbol(fa2, "abc");
  EXPECT_EQ(fa2.countStates(), ONE);
  EXPECT_EQ(fa2.countSymbols(), ONE*3);
  EXPECT_TRUE(sameLanguage(fa, fa2));
}

TEST(createMinimalHopcroft, nonDeterministicNonCompleteAndNoFinalState) {
  fa::Automaton toCompare;
  EXPECT_TRUE(toCompare.addState(0));
  EXPECT_TRUE(toCompare.addSymbol('a'));
  toCompare.setStateInitial(0);
  EXPECT_TRUE(toCompare.isLanguageEmpty());
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 0));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("aabbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isIncludedIn(toCompare));
  EXPECT_TRUE(toCompare.isIncludedIn(fa));
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*3);
  do_not_have_symbol(fa, "ab");
  fa::Automaton fa2 = fa.createMinimalHopcroft(fa);
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("aabbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"));
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isIncludedIn(toCompare));
  EXPECT_TRUE(toCompare.isIncludedIn(fa2));
  EXPECT_TRUE(fa.isIncludedIn(fa2));
  EXPECT_TRUE(fa2.isIncludedIn(fa));
  EXPECT_EQ(fa2.countSymbols(), fa.countSymbols());
  EXPECT_EQ(fa2.countStates(), ONE);
  do_not_have_symbol(fa2, "ab");
}

TEST(createMinimalHopcroft, nonDeterministicNonCompleteAndNoInitialState) {
  fa::Automaton toCompare;
  EXPECT_TRUE(toCompare.addState(0));
  EXPECT_TRUE(toCompare.addSymbol('a'));
  toCompare.setStateFinal(0);
  EXPECT_TRUE(toCompare.isLanguageEmpty());
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 0));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("aabbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"));
  EXPECT_FALSE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isIncludedIn(toCompare));
  EXPECT_TRUE(toCompare.isIncludedIn(fa));
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*3);
  do_not_have_symbol(fa, "ab");
  fa::Automaton fa2 = fa.createMinimalHopcroft(fa);
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("aabbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"));
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isIncludedIn(toCompare));
  EXPECT_TRUE(toCompare.isIncludedIn(fa2));
  EXPECT_TRUE(fa.isIncludedIn(fa2));
  EXPECT_TRUE(fa2.isIncludedIn(fa));
  EXPECT_EQ(fa2.countSymbols(), fa.countSymbols());
  EXPECT_EQ(fa2.countStates(), ONE);
  do_not_have_symbol(fa2, "ab");
}

TEST(createMinimalHopcroft, FinalTest_alias_NotMinimal2) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 2));
  EXPECT_TRUE(fa.match("a"));
  EXPECT_TRUE(fa.match("b"));
  EXPECT_TRUE(fa.match("ab"));
  EXPECT_TRUE(fa.match("ba"));
  EXPECT_TRUE(fa.match("ababababababababababababbbbbbbaaaaaaaaaabbbbbbbbbaaaababababababababbb"));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("ababababababababababababababababababababababababababababababababababab"));
  EXPECT_TRUE(fa.match("bababababababababababababababababababababababababababababababababababa"));
  EXPECT_TRUE(fa.match("abababababababababababababababababababababababababababababababababbbbb"));
  EXPECT_TRUE(fa.match("bababababababababababababababababababababababababababababababababaaaaa"));
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match(""));
  EXPECT_EQ(fa.countStates(), ONE*3);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  do_not_have_symbol(fa, "ab");
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  // Automate déterministe, complet, mais pas minimal
  fa::Automaton fa2 = fa.createMinimalHopcroft(fa);
  EXPECT_EQ(fa2.countStates(), ONE*2);
  EXPECT_EQ(fa2.countSymbols(), ONE*2);
  do_not_have_symbol(fa2, "ab");
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_TRUE(fa2.match("ababababababababababababbbbbbbaaaaaaaaaabbbbbbbbbaaaababababababababbb"));
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("ababababababababababababababababababababababababababababababababababab"));
  EXPECT_TRUE(fa2.match("bababababababababababababababababababababababababababababababababababa"));
  EXPECT_TRUE(fa2.match("abababababababababababababababababababababababababababababababababbbbb"));
  EXPECT_TRUE(fa2.match("bababababababababababababababababababababababababababababababababaaaaa"));
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_FALSE(fa2.match(""));
  EXPECT_TRUE(sameLanguage(fa, fa2));  
}

TEST(createMinimalHopcroft, testWithA14StatesAutomatonDeterministicAndCompleteButNotMinimal) {
  fa::Automaton fa;
  for (int i = 0 ; i < 14 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addSymbol('c'));
  fa.setStateInitial(0);
  fa.setStateFinal(13);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(0, 'c', 3));
  EXPECT_TRUE(fa.addTransition(1, 'a', 4));
  EXPECT_TRUE(fa.addTransition(1, 'b', 5));
  EXPECT_TRUE(fa.addTransition(1, 'c', 6));
  EXPECT_TRUE(fa.addTransition(2, 'a', 7));
  EXPECT_TRUE(fa.addTransition(2, 'b', 8));
  EXPECT_TRUE(fa.addTransition(2, 'c', 9));
  EXPECT_TRUE(fa.addTransition(3, 'a', 10));
  EXPECT_TRUE(fa.addTransition(3, 'b', 11));
  EXPECT_TRUE(fa.addTransition(3, 'c', 12));
  for (int i = 4 ; i < 11 ; i+=3) {
    EXPECT_TRUE(fa.addTransition(i, 'a', 13));
    EXPECT_TRUE(fa.addTransition(i, 'b', i));
    EXPECT_TRUE(fa.addTransition(i, 'c', i));
  }
  for (int i = 5 ; i < 12 ; i+=3) {
    EXPECT_TRUE(fa.addTransition(i, 'a', i));
    EXPECT_TRUE(fa.addTransition(i, 'b', 13));
    EXPECT_TRUE(fa.addTransition(i, 'c', i));
  }
  for (int i = 6 ; i < 13 ; i+=3) {
    EXPECT_TRUE(fa.addTransition(i, 'a', i));
    EXPECT_TRUE(fa.addTransition(i, 'b', i));
    EXPECT_TRUE(fa.addTransition(i, 'c', 13));
  }
  EXPECT_TRUE(fa.addTransition(13, 'a', 13));
  EXPECT_TRUE(fa.addTransition(13, 'b', 13));
  EXPECT_TRUE(fa.addTransition(13, 'c', 13));
  do_not_have_symbol(fa, "abc");
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  EXPECT_EQ(fa.countStates(), ONE*14);
  EXPECT_EQ(fa.countSymbols(), ONE*3);
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match(""));
  EXPECT_TRUE(fa.match("ccaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbababababccccccccccccccccabababab"));
  EXPECT_TRUE(fa.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"));
  EXPECT_FALSE(fa.match("bac"));
  EXPECT_FALSE(fa.match("cac"));
  EXPECT_FALSE(fa.match("cab"));
  EXPECT_FALSE(fa.match("abc"));
  EXPECT_FALSE(fa.match("ccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("bbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("bbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match("bbcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"));
  EXPECT_TRUE(fa.match("aaa"));
  EXPECT_TRUE(fa.match("bbb"));
  EXPECT_TRUE(fa.match("ccc"));
  fa::Automaton fa2 = fa.createMinimalHopcroft(fa);
  do_not_have_symbol(fa2, "abc");
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_EQ(fa2.countStates(), ONE*6);
  EXPECT_FALSE(fa.countStates() == fa2.countStates());
  EXPECT_EQ(fa2.countSymbols(), ONE*3);
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_FALSE(fa2.match(""));
  EXPECT_TRUE(fa2.match("ccaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbababababccccccccccccccccabababab"));
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"));
  EXPECT_FALSE(fa2.match("bac"));
  EXPECT_FALSE(fa2.match("cac"));
  EXPECT_FALSE(fa2.match("cab"));
  EXPECT_FALSE(fa2.match("abc"));
  EXPECT_FALSE(fa2.match("ccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("bbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("bbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match("bbcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"));
  EXPECT_TRUE(fa2.match("aaa"));
  EXPECT_TRUE(fa2.match("bbb"));
  EXPECT_TRUE(fa2.match("ccc"));
  EXPECT_TRUE(sameLanguage(fa, fa2));
}
// ======================================================================================================
#endif

#ifdef EPSILON_TEST
// ======================================================================================================
// -------------------------- TESTS FOR CREATE WITHOUT EPSILON -----------------------
// Comme il s'agit d'une méthode bonus, on ne doit pas la tester
// Néanmoins, pour voir si elle fonctionne bien, j'ai créé 8 tests sur celle-ci avant de la remettre
// dans la partie privée de ma classe. Les tests passent tous donc ma méthode ne doit pas être
// trop mauvaise (je l'espère, mais en tout cas elle a été testée comme il le faut)

TEST(createWithoutEpsilon, testWithoutEpsilonTransition) {
  fa::Automaton fa;
  for (int i = 0 ; i < 7 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(2);
  fa.setStateFinal(0);
  fa.setStateFinal(3);
  fa.setStateFinal(5);
  EXPECT_TRUE(fa.addTransition(0, 'a', 4));
  EXPECT_TRUE(fa.addTransition(0, 'b', 3));
  EXPECT_TRUE(fa.addTransition(1, 'a', 0));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 0));
  EXPECT_TRUE(fa.addTransition(2, 'b', 3));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 1));
  EXPECT_TRUE(fa.addTransition(4, 'b', 6));
  EXPECT_TRUE(fa.addTransition(5, 'a', 4));
  EXPECT_TRUE(fa.addTransition(5, 'b', 3));
  EXPECT_TRUE(fa.addTransition(6, 'a', 5));
  EXPECT_TRUE(fa.addTransition(6, 'b', 3));
  do_not_have_symbol(fa, "ab");
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  EXPECT_EQ(fa.countStates(), ONE*7);
  EXPECT_TRUE(fa.match("a"));
  EXPECT_TRUE(fa.match("baaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("abaaaaaaaaaaaaababbaaaaaaaaaaaaaababbaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa.match("bb"));
  EXPECT_TRUE(fa.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa.match("aabb"));
  EXPECT_TRUE(fa.match("aabbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_FALSE(fa.match("aaa"));
  EXPECT_FALSE(fa.hasEpsilonTransition());
  fa::Automaton fa2 = fa.createWithoutEpsilon(fa);
  EXPECT_FALSE(fa2.hasEpsilonTransition());
  do_not_have_symbol(fa2, "ab");
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.isDeterministic());
  EXPECT_TRUE(fa2.isComplete());
  EXPECT_EQ(fa2.countSymbols(), ONE*2);
  EXPECT_EQ(fa2.countStates(), ONE*7);
  EXPECT_TRUE(fa2.match("a"));
  EXPECT_TRUE(fa2.match("baaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("abaaaaaaaaaaaaababbaaaaaaaaaaaaaababbaaaaaaaaaaaaaa"));
  EXPECT_TRUE(fa2.match("bb"));
  EXPECT_TRUE(fa2.match("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));
  EXPECT_TRUE(fa2.match("aabb"));
  EXPECT_TRUE(fa2.match("aabbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.match(""));
  EXPECT_FALSE(fa2.match("aa"));
  EXPECT_FALSE(fa2.match("aaa"));
}

TEST(createWithoutEpsilon, testWithOnlyOneEpsilonTransition) {
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_FALSE(fa.addSymbol(fa::Epsilon));
  EXPECT_TRUE(fa.addTransition(1,fa::Epsilon,2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 1));
  EXPECT_TRUE(fa.hasEpsilonTransition());
  fa::Automaton fa2 = fa.createWithoutEpsilon(fa);
  EXPECT_FALSE(fa2.hasEpsilonTransition());
  EXPECT_TRUE(fa2.isStateFinal(1));
  EXPECT_TRUE(fa2.match("aaaaaaaaaaaaa"));
  EXPECT_FALSE(fa2.isLanguageEmpty());
}

TEST(createWithoutEpsilon, testWithTheOneTD3Exo11) {
  fa::Automaton fa;
  for (int i = 0 ; i < 5 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  fa.setStateInitial(0);
  fa.setStateFinal(3);
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_FALSE(fa.addSymbol(fa::Epsilon));
  EXPECT_TRUE(fa.addTransition(0,'a',1));
  EXPECT_TRUE(fa.addTransition(0,fa::Epsilon,2));
  EXPECT_TRUE(fa.addTransition(1,fa::Epsilon,4));
  EXPECT_TRUE(fa.addTransition(1,'b',2));
  EXPECT_TRUE(fa.addTransition(2,fa::Epsilon,4));
  EXPECT_TRUE(fa.addTransition(2,'b',3));
  EXPECT_TRUE(fa.addTransition(3,'b',0));
  EXPECT_TRUE(fa.addTransition(4,'a',2));
  EXPECT_TRUE(fa.addTransition(4,'b',3));
  EXPECT_TRUE(fa.hasEpsilonTransition());
  fa::Automaton fa2 = fa.createWithoutEpsilon(fa);
  EXPECT_FALSE(fa2.hasEpsilonTransition());
  for (int i = 0 ; i < 5 ; ++i) {
    EXPECT_TRUE(fa2.isStateFinal(i));
  }
  EXPECT_TRUE(fa2.hasTransition(0, 'a', 1));
  EXPECT_TRUE(fa2.hasTransition(0, 'b', 3));
  EXPECT_TRUE(fa2.hasTransition(0, 'a', 2));
  EXPECT_TRUE(fa2.hasTransition(1, 'b', 2));
  EXPECT_TRUE(fa2.hasTransition(1, 'a', 2));
  EXPECT_TRUE(fa2.hasTransition(1, 'b', 3));
  EXPECT_TRUE(fa2.hasTransition(2, 'a', 2));
  EXPECT_TRUE(fa2.hasTransition(2, 'b', 3));
  EXPECT_TRUE(fa2.hasTransition(3, 'b', 0));
  EXPECT_TRUE(fa2.hasTransition(4, 'a', 2));
  EXPECT_TRUE(fa2.hasTransition(4, 'b', 3));
}

TEST(createWithoutEpsilon, testWithAnAutomatonHasOnlyEpsilonTransitions) {
  fa::Automaton fa;
  for (int i = 0 ; i < 5 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  fa.setStateInitial(0);
  fa.setStateFinal(4);
  for (int i = 0 ; i < 4 ; ++i) {
    EXPECT_TRUE(fa.addTransition(i, fa::Epsilon, i+1));
  }
  EXPECT_TRUE(fa.hasEpsilonTransition());
  EXPECT_EQ(fa.countStates(), ONE*5);
  do_not_have_symbol(fa, "a");
  fa::Automaton fa2 = fa.createWithoutEpsilon(fa);
  EXPECT_FALSE(fa2.hasEpsilonTransition());
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.match(""));
  for (int i = 0 ; i < 4 ; ++i) {
    EXPECT_TRUE(fa2.isStateFinal(i));
  }
  EXPECT_EQ(fa.countStates(), fa2.countStates());
  do_not_have_symbol(fa2, "a");
  EXPECT_EQ(fa2.countTransitions(), ZERO);
}

TEST(createWithoutEpsilon, testWithTwoStatesOnItSelfWithOnlyEpsilon) {
  fa::Automaton fa;
  for (int i = 1 ; i < 3 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  fa.setStateInitial(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addTransition(1, fa::Epsilon, 1));
  EXPECT_TRUE(fa.addTransition(1, fa::Epsilon, 2));
  EXPECT_TRUE(fa.addTransition(2, fa::Epsilon, 2));
  EXPECT_EQ(fa.countTransitions(), ONE*3);
  EXPECT_EQ(fa.countSymbols(), ONE);
  EXPECT_EQ(fa.countStates(), ONE*2);
  do_not_have_symbol(fa, "a");
  EXPECT_TRUE(fa.hasEpsilonTransition());
  fa::Automaton fa2 = fa.createWithoutEpsilon(fa);
  do_not_have_symbol(fa2, "a");
  EXPECT_FALSE(fa2.hasEpsilonTransition());
  EXPECT_EQ(fa2.countTransitions(), ZERO);
  EXPECT_EQ(fa2.countSymbols(), fa.countSymbols());
  EXPECT_EQ(fa.countStates(), fa2.countStates());
  EXPECT_FALSE(fa2.isLanguageEmpty());
  EXPECT_TRUE(fa2.match(""));
}

TEST(createWithoutEpsilon, testWithTheOneTD3Exo10) {
  fa::Automaton fa;
  for (int i = 0 ; i < 4 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_FALSE(fa.addSymbol(fa::Epsilon));
  fa.setStateInitial(0);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(0, fa::Epsilon, 3));
  EXPECT_TRUE(fa.addTransition(1, fa::Epsilon, 0));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, fa::Epsilon, 1));
  EXPECT_TRUE(fa.addTransition(3, 'a', 0));
  EXPECT_TRUE(fa.addTransition(3, fa::Epsilon, 2));
  EXPECT_TRUE(fa.hasEpsilonTransition());
  EXPECT_EQ(fa.countStates(), ONE*4);
  EXPECT_EQ(fa.countSymbols(), ONE*2);
  do_not_have_symbol(fa, "ab");
  fa::Automaton fa2 = fa.createWithoutEpsilon(fa);
  do_not_have_symbol(fa2, "ab");
  EXPECT_FALSE(fa2.hasEpsilonTransition());
  EXPECT_EQ(fa.countStates(), fa2.countStates());
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  for (int i = 0 ; i < 4 ; ++i) {
    EXPECT_TRUE(fa2.isStateFinal(i));
  }
}

TEST(createWithoutEpsilon, testWithATwistedCase_v1) {
  fa::Automaton fa;
  for (int i = 0 ; i < 4 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_FALSE(fa.addSymbol(fa::Epsilon));
  fa.setStateInitial(0);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addTransition(1, fa::Epsilon, 0));
  EXPECT_TRUE(fa.addTransition(0, fa::Epsilon, 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.hasEpsilonTransition());
  EXPECT_EQ(fa.countStates(), ONE*4);
  EXPECT_EQ(fa.countSymbols(), ONE);
  do_not_have_symbol(fa, "a");
  fa::Automaton fa2 = fa.createWithoutEpsilon(fa);
  do_not_have_symbol(fa2, "a");
  EXPECT_FALSE(fa2.hasEpsilonTransition());
  EXPECT_EQ(fa.countStates(), fa2.countStates());
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  for (int i = 0 ; i < 3 ; ++i) {
    EXPECT_FALSE(fa2.isStateFinal(i));
  }
  EXPECT_TRUE(fa2.isStateFinal(3));
  EXPECT_TRUE(fa2.match("aa"));
  EXPECT_FALSE(fa2.match("a"));
  EXPECT_FALSE(fa2.match("aaaaaaaaaaaaaaaaaaaaaaaaaa"));
}

TEST(createWithoutEpsilon, testWithATwistedCase_v2) {
  fa::Automaton fa;
  for (int i = 0 ; i < 4 ; ++i) {
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_FALSE(fa.addSymbol(fa::Epsilon));
  fa.setStateInitial(0);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addTransition(0, fa::Epsilon, 3));
  EXPECT_TRUE(fa.addTransition(1, fa::Epsilon, 0));
  EXPECT_TRUE(fa.addTransition(2, fa::Epsilon, 1));
  EXPECT_TRUE(fa.addTransition(3, fa::Epsilon, 2));
  EXPECT_TRUE(fa.hasEpsilonTransition());
  EXPECT_EQ(fa.countStates(), ONE*4);
  EXPECT_EQ(fa.countSymbols(), ONE);
  do_not_have_symbol(fa, "a");
  fa::Automaton fa2 = fa.createWithoutEpsilon(fa);
  do_not_have_symbol(fa2, "a");
  EXPECT_FALSE(fa2.hasEpsilonTransition());
  EXPECT_EQ(fa.countStates(), fa2.countStates());
  EXPECT_EQ(fa.countSymbols(), fa2.countSymbols());
  for (int i = 0 ; i < 3 ; ++i) {
    EXPECT_TRUE(fa2.isStateFinal(i));
  }
  EXPECT_EQ(fa2.countTransitions(), ZERO);
}
// ======================================================================================================
// ENDIF
// ======================================================================================================
#endif
// ======================================================================================================


// ================================== MAIN ==================================
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
// ================================== MAIN ==================================
