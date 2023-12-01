
#include "gtest/gtest.h"

#include "Automaton.h"
#include <cstddef>
#include <iosfwd>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <set>
#include <algorithm>
#include <cstdalign>
#include <fstream>
#include <cassert>
#include <climits>
#include <queue>
#include <time.h>

#define N 500
//#define DEVELOPMENT
#define HOPCROFT

std::set<std::string> words;

void enumer_chaines(std::string& chaine, int n) {
  for (char c = 'a'; c <= 'b'; c++) {
    if (n == 1) {
      words.insert(chaine);
    } else {
      chaine.push_back(c);
      enumer_chaines(chaine, n - 1);
      chaine.pop_back();
    }
  }
}

void enumer_chaines_abc(std::string& chaine, int n) {
  for (char c = 'a'; c <= 'c'; c++) {
    if (n == 1) {
      words.insert(chaine);
    } else {
      chaine.push_back(c);
      enumer_chaines(chaine, n - 1);
      chaine.pop_back();
    }
  }
}

bool equivalent(const fa::Automaton a1, const fa::Automaton a2){
  for(std::string word : words){
    if(a1.match(word) != a2.match(word)){
      return false;
    }
  }
  return true;
}

TEST(isValid, noSymbolNoState) {
  fa::Automaton fa;
  EXPECT_FALSE(fa.isValid());
}

TEST(isValid, withSymbolNoState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_FALSE(fa.isValid());
  EXPECT_TRUE(fa.hasSymbol('a'));
}

TEST(isValid, noSymbolWithState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_FALSE(fa.isValid());
  EXPECT_TRUE(fa.hasState(0));
}

TEST(isValid, withSymbolWithState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasSymbol('a'));
}

TEST(AddedSymbol, Epsilon){
  fa::Automaton fa;
  EXPECT_FALSE(fa.addSymbol(fa::Epsilon));
}

TEST(AddedSymbol, Space){
  fa::Automaton fa;
  EXPECT_FALSE(fa.addSymbol(' '));
}

TEST(AddedSymbol, isNotGraph){
  fa::Automaton fa;
  EXPECT_FALSE(fa.addSymbol('\n'));
}

TEST(AddedSymbol, isGraph){
  fa::Automaton fa;
  std::size_t c = 0;
  for(std::size_t i = 0; i < 256; ++i){
    if(isgraph(i)){
      EXPECT_TRUE(fa.addSymbol((char)(i)));
      EXPECT_TRUE(fa.hasSymbol((char)(i)));
      c++;
      EXPECT_EQ(fa.countSymbols(), c);
    }else{
      EXPECT_FALSE(fa.addSymbol((char)(i)));
      EXPECT_FALSE(fa.hasSymbol((char)(i)));
      EXPECT_EQ(fa.countSymbols(), c);
    }
  }
  EXPECT_EQ(fa.countSymbols(), 94u);
}

TEST(AddedSymbol, oneSymbol){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
}

TEST(AddedSymbol, twoSymbol){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_TRUE(fa.hasSymbol('b'));
  EXPECT_EQ(fa.countSymbols(), 2u);
}

TEST(AddedSymbol, twoIdenticalSymbols){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_FALSE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
}

TEST(RemoveSymbol, oneSymbol){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.removeSymbol('a'));
  EXPECT_FALSE(fa.hasSymbol('a'));
}

TEST(RemoveSymbol, Empty){
  fa::Automaton fa;
  EXPECT_FALSE(fa.removeSymbol('a'));
}

TEST(RemoveSymbol, NotInAlphabet){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_FALSE(fa.removeSymbol('b'));
  EXPECT_TRUE(fa.hasSymbol('a'));
}

TEST(RemoveSymbol, allCharacter){
  fa::Automaton fa;
  std::size_t c = 0;
  for(std::size_t i = 0; i < 256; ++i){
    if(isgraph(i)){
      EXPECT_TRUE(fa.addSymbol(i));
      EXPECT_TRUE(fa.hasSymbol(i));
      c++;
      EXPECT_EQ(fa.countSymbols(), c);
    }else{
      EXPECT_FALSE(fa.addSymbol(i));
      EXPECT_FALSE(fa.hasSymbol(i));
      EXPECT_EQ(fa.countSymbols(), c);
    }
  }
  EXPECT_EQ(fa.countSymbols(), 94u);

  for(std::size_t i = 0; i < 256; ++i){
    if(isgraph(i)){
      EXPECT_TRUE(fa.removeSymbol(i));
      EXPECT_FALSE(fa.hasSymbol(i));
      c--;
      EXPECT_EQ(fa.countSymbols(), c);
    }else{
      EXPECT_FALSE(fa.removeSymbol(i));
      EXPECT_FALSE(fa.hasSymbol(i));
      EXPECT_EQ(fa.countSymbols(), c);
    }
  }

  EXPECT_EQ(fa.countSymbols(), 0u);
}

TEST(RemoveSymbol, SymbolInTransition){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.setStateInitial(0);
  fa.setStateInitial(1);
  fa.setStateFinal(1);
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'b', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  EXPECT_TRUE(fa.removeSymbol('a'));
  EXPECT_FALSE(fa.hasSymbol('a'));
  EXPECT_TRUE(fa.hasSymbol('b'));
  for(int i = 0; i < 5; ++i){
    EXPECT_TRUE(fa.hasState(i));
  }
  EXPECT_TRUE(fa.hasTransition(1, 'b', 3));
  EXPECT_TRUE(fa.hasTransition(3, 'b', 4));
  EXPECT_TRUE(fa.hasTransition(2, 'b', 4));
  EXPECT_FALSE(fa.hasTransition(0, 'a', 1));
  EXPECT_FALSE(fa.hasTransition(0, 'a', 2));
  EXPECT_FALSE(fa.hasTransition(0, 'a', 3));
  EXPECT_FALSE(fa.hasTransition(2, 'a', 3));
  EXPECT_FALSE(fa.hasTransition(3, 'a', 3));
  EXPECT_FALSE(fa.hasTransition(4, 'a', 4));
  EXPECT_TRUE(fa.countTransitions() == 3);
  
}

TEST(HasSymbol, Successful){
  fa::Automaton fa;
  for(int i = 0; i < 7; ++i){
    EXPECT_TRUE(fa.addSymbol(i + 'a'));
  }
  EXPECT_TRUE(fa.hasSymbol('f'));
}

TEST(HasSymbol, Empty){
  fa::Automaton fa;
  EXPECT_FALSE(fa.hasSymbol('a'));
  
}

TEST(HasSymbol, NotIsGraph){
  fa::Automaton fa;
  for(int i = 0; i < 7; ++i){
    EXPECT_TRUE(fa.addSymbol(i + 'a'));
  }
  EXPECT_FALSE(fa.hasSymbol('\n'));
}

TEST(CountSymbol, Full){
  fa::Automaton fa;
  for(int i = 0; i < 7; ++i){
    EXPECT_TRUE(fa.addSymbol(i + 'a'));
  }
  EXPECT_EQ(fa.countSymbols(), 7u);
}

TEST(CountSymbol, Empty){
  fa::Automaton fa;
  EXPECT_EQ(fa.countSymbols(), 0u);
}

TEST(AddState, oneState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_EQ(fa.countStates(), 1u);
}

TEST(AddState, twoIdenticalStates){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_FALSE(fa.addState(0));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_EQ(fa.countStates(), 1u);
}

TEST(AddState, twoStates){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 2u);
}

TEST(AddState, Negative){
  fa::Automaton fa;
  EXPECT_FALSE(fa.addState(-1));
  EXPECT_FALSE(fa.hasState(-1));
  EXPECT_EQ(fa.countStates(), 0u);
}

TEST(AddState, MAX){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(INT_MAX));
  EXPECT_TRUE(fa.hasState(INT_MAX));
  EXPECT_EQ(fa.countStates(), 1u);
}

TEST(AddState, MIN){
  fa::Automaton fa;
  EXPECT_FALSE(fa.addState(INT_MIN));
  EXPECT_FALSE(fa.hasState(INT_MIN));
  EXPECT_EQ(fa.countStates(), 0u);
}

TEST(RemoveState, Empty){
  fa::Automaton fa;
  EXPECT_FALSE(fa.removeState(0));

  EXPECT_FALSE(fa.hasState(0));
  EXPECT_EQ(fa.countStates(), 0u);
}

TEST(RemoveState, UnknownState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_FALSE(fa.removeState(1));

  EXPECT_TRUE(fa.hasState(0));
  EXPECT_FALSE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 1u);
}

TEST(RemoveState, OneState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.removeState(0));

  EXPECT_FALSE(fa.hasState(0));
  EXPECT_EQ(fa.countStates(), 0u);
}

TEST(RemoveState, AllStates){
  fa::Automaton fa;
  std::size_t c = 0;
  for(std::size_t i = 0; i < 10; ++i){
    EXPECT_TRUE(fa.addState(i));
    EXPECT_TRUE(fa.hasState(i));
    c++;
    EXPECT_EQ(fa.countStates(), c);
  }

  for(std::size_t i = 0; i < 10; ++i){
    EXPECT_TRUE(fa.removeState(i));
    EXPECT_FALSE(fa.hasState(i));
    c--;
    EXPECT_EQ(fa.countStates(), c);
  }
}

TEST(RemoveState, OriginInTransition){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.removeState(0));

  EXPECT_FALSE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_FALSE(fa.hasTransition(0, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), 0u);
}

TEST(RemoveState, DestinationInTransition){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.removeState(1));

  EXPECT_FALSE(fa.hasState(1));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_EQ(fa.countStates(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_FALSE(fa.hasTransition(0, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), 0u);
}

TEST(RemoveState, OriginAndDestinationInTransitionConstant){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.removeState(0));
  EXPECT_TRUE(fa.removeState(1));

  EXPECT_FALSE(fa.hasState(1));
  EXPECT_FALSE(fa.hasState(0));
  EXPECT_EQ(fa.countStates(), 0u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_FALSE(fa.hasTransition(0, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), 0u);
}

TEST(RemoveState, StateInTransition){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.setStateInitial(0); 
  fa.setStateInitial(1);
  fa.setStateFinal(1); 
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'b', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  EXPECT_TRUE(fa.removeState(3));
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_TRUE(fa.hasSymbol('b'));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_FALSE(fa.hasState(3));
  EXPECT_TRUE(fa.hasState(4));
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
  EXPECT_TRUE(fa.hasTransition(0, 'a', 2));
  EXPECT_FALSE(fa.hasTransition(0, 'a', 3));
  EXPECT_FALSE(fa.hasTransition(1, 'b', 3));
  EXPECT_FALSE(fa.hasTransition(2, 'a', 3));
  EXPECT_TRUE(fa.hasTransition(2, 'b', 4));
  EXPECT_FALSE(fa.hasTransition(3, 'a', 3));
  EXPECT_FALSE(fa.hasTransition(3, 'b', 4));
  EXPECT_TRUE(fa.hasTransition(4, 'a', 4));
  EXPECT_EQ(fa.countTransitions(), 4u);
}

TEST(HasState, Empty){
  fa::Automaton fa;
  EXPECT_FALSE(fa.hasState(0));
}

TEST(HasState, AlreadyIn){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.hasState(0));
}

TEST(HasState, NotIn){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_FALSE(fa.hasState(2));
}

TEST(CountState, Empty){
  fa::Automaton fa;
  EXPECT_EQ(fa.countStates(), 0u);
}

TEST(CountState, NotEmpty){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_EQ(fa.countStates(), 4u);
}

TEST(SetInitialState, oneInitialState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.isStateInitial(0));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_EQ(fa.countStates(), 1u);
}

TEST(SetInitialState, ToFinalAndInitial){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  fa.setStateFinal(0);
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.isStateInitial(0));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_EQ(fa.countStates(), 1u);
}

TEST(SetInitialState, TwoInitialStates){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  fa.setStateInitial(0);
  fa.setStateInitial(1);
  EXPECT_TRUE(fa.isStateInitial(0));
  EXPECT_TRUE(fa.isStateInitial(1));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 2u);
}

TEST(SetInitialState, unknwonState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  fa.setStateInitial(1);
  EXPECT_FALSE(fa.isStateInitial(1));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_EQ(fa.countStates(), 1u);
}

TEST(SetFinalState, oneFinalState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  fa.setStateFinal(0);
  EXPECT_TRUE(fa.isStateFinal(0));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_EQ(fa.countStates(), 1u);
}

TEST(SetFinalState, twoFinalStates){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  fa.setStateFinal(0);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.isStateFinal(0));
  EXPECT_TRUE(fa.isStateFinal(1));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 2u);
}

TEST(SetFinalState, ToFinalAndInitial){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  EXPECT_TRUE(fa.isStateFinal(0));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_EQ(fa.countStates(), 1u);
}

TEST(SetFinalState, unknownState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  fa.setStateFinal(1);
  EXPECT_FALSE(fa.isStateFinal(1));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_EQ(fa.countStates(), 1u);
}

TEST(AddTransition, unknownSymbol){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_FALSE(fa.addTransition(0, 'a', 1));

  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_FALSE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 0u);
  EXPECT_FALSE(fa.hasTransition(0, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), 0u);
}

TEST(AddTransition, unknownOrigin){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_FALSE(fa.addTransition(0, 'a', 1));

  EXPECT_FALSE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_FALSE(fa.hasTransition(0, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), 0u);
}

TEST(AddTransition, unknownTarget){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_FALSE(fa.addTransition(0, 'a', 1));

  EXPECT_TRUE(fa.hasState(0));
  EXPECT_FALSE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_FALSE(fa.hasTransition(0, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), 0u);
}

TEST(AddTransition, oneTransition){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));

  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), 1u);
}

TEST(AddTransition, TwoIdenticalTransitions){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_FALSE(fa.addTransition(0, 'a', 1));

  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), 1u);
}

TEST(AddTransition, SameOriginAndLetter){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));

  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_EQ(fa.countStates(), 3u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
  EXPECT_TRUE(fa.hasTransition(0, 'a', 2));
  EXPECT_EQ(fa.countTransitions(), 2u);
}

TEST(AddTransition, SameOriginAndDestination){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 1));

  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_TRUE(fa.hasSymbol('b'));
  EXPECT_EQ(fa.countSymbols(), 2u);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
  EXPECT_TRUE(fa.hasTransition(0, 'b', 1));
  EXPECT_EQ(fa.countTransitions(), 2u);
}

TEST(AddTransition, SameLetterAndDestination){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));

  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_EQ(fa.countStates(), 3u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 2));
  EXPECT_TRUE(fa.hasTransition(1, 'a', 2));
  EXPECT_EQ(fa.countTransitions(), 2u);
}

TEST(AddTransition, Epsilon){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addTransition(0, fa::Epsilon, 1));

  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_FALSE(fa.hasSymbol(fa::Epsilon));
  EXPECT_EQ(fa.countSymbols(), 0u);
  EXPECT_TRUE(fa.hasTransition(0, fa::Epsilon, 1));
  EXPECT_EQ(fa.countTransitions(), 1u);
}



TEST(RemoveTransition, UnknownSymbol){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_FALSE(fa.removeTransition(0, 'a', 1));

  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_FALSE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 0u);
  EXPECT_FALSE(fa.hasTransition(0, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), 0u);
}

TEST(RemoveTransition, UnknownOrigin){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_FALSE(fa.removeTransition(0, 'a', 1));

  EXPECT_FALSE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_FALSE(fa.hasTransition(0, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), 0u);
}

TEST(RemoveTransition, UnknownTarget){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_FALSE(fa.removeTransition(0, 'a', 1));

  EXPECT_TRUE(fa.hasState(0));
  EXPECT_FALSE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_FALSE(fa.hasTransition(0, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), 0u);
}

TEST(RemoveTransition, Empty){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_FALSE(fa.removeTransition(0, 'a', 1));

  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_FALSE(fa.hasTransition(0, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), 0u);
}

TEST(RemoveTransition, OneTransition){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.removeTransition(0, 'a', 1));

  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_EQ(fa.countStates(), 3u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 2));
  EXPECT_FALSE(fa.hasTransition(0, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), 1u);
}

TEST(RemoveTransition, Success){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.removeTransition(0, 'a', 1));
  EXPECT_FALSE(fa.hasTransition(0, 'a', 1));
}

TEST(HasTransition, Empty){
  fa::Automaton fa;
  EXPECT_FALSE(fa.hasTransition(0, 'a', 1));
}

TEST(HasTransition, DontHaveThisTransition){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_FALSE(fa.hasTransition(1, 'a', 0));
}

TEST(HasTransition, DontHaveSymbol){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_FALSE(fa.hasTransition(1, 'b', 0));
}

TEST(HasTransition, DontHaveStateFrom){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_FALSE(fa.hasTransition(2, 'a', 1));
}

TEST(HasTransition, DontHaveStateTo){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_FALSE(fa.hasTransition(1, 'a', 2));
}

TEST(HasTransition, Success){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
}

TEST(CountTransition, Empty){
  fa::Automaton fa;
  EXPECT_EQ(fa.countTransitions(), 0u);
}

TEST(CountTransition, NotEmpty){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), 1u);
}

TEST(PrettyPrint, test){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.setStateInitial(0); 
  fa.setStateInitial(1);
  fa.setStateFinal(1); 
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'b', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  EXPECT_EQ(fa.countTransitions(), 9u);
  fa.prettyPrint(std::cout);
}

/*TP2*/

TEST(hasEpsilonTransition, WithoutEpsilon){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  fa.setStateInitial(0); 
  fa.setStateInitial(1);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_FALSE(fa.hasEpsilonTransition());

  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_TRUE(fa.isStateInitial(0));
  EXPECT_TRUE(fa.isStateInitial(1));
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_EQ(fa.countTransitions(), 0u);
}

TEST(hasEpsilonTransition, withEpsilon){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(0);
  fa.setStateFinal(1); 
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, fa::Epsilon, 2));
  EXPECT_TRUE(fa.hasEpsilonTransition());

  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_EQ(fa.countStates(), 3u);
  EXPECT_TRUE(fa.isStateInitial(0));
  EXPECT_TRUE(fa.isStateFinal(1));
  EXPECT_TRUE(fa.isStateFinal(2));
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
  EXPECT_TRUE(fa.hasTransition(1, fa::Epsilon, 2));
  EXPECT_EQ(fa.countTransitions(), 2u);
}

TEST(hasEpsilonTransition, Twice){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, fa::Epsilon, 1));
  EXPECT_TRUE(fa.addTransition(0, fa::Epsilon, 2));
  EXPECT_TRUE(fa.hasEpsilonTransition());

  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_EQ(fa.countStates(), 3u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasTransition(0, fa::Epsilon, 1));
  EXPECT_TRUE(fa.hasTransition(0, fa::Epsilon, 2));
  EXPECT_EQ(fa.countTransitions(), 2u);
}

TEST(hasEpsilonTransition, AddAndRemove){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, fa::Epsilon, 1));
  EXPECT_TRUE(fa.hasEpsilonTransition());

  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasTransition(0, fa::Epsilon, 1));
  EXPECT_EQ(fa.countTransitions(), 1u);

  EXPECT_TRUE(fa.removeTransition(0, fa::Epsilon, 1));
  EXPECT_FALSE(fa.hasEpsilonTransition());

  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_FALSE(fa.hasTransition(0, fa::Epsilon, 1));
  EXPECT_EQ(fa.countTransitions(), 0u);
}

TEST(isDeterminist, noTransition){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addSymbol('a'));
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.isDeterministic());

  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_EQ(fa.countStates(), 1u);
  EXPECT_TRUE(fa.isStateInitial(0));
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_EQ(fa.countTransitions(), 0u);
}

TEST(isDeterminist, TwoTransitions){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0,'a', 1));
  EXPECT_TRUE(fa.addTransition(0,'a', 2));
  EXPECT_FALSE(fa.isDeterministic());

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.match("a"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.match("aa"));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_EQ(fa.countStates(), 3u);
  EXPECT_TRUE(fa.isStateInitial(0));
  EXPECT_TRUE(fa.isStateFinal(1));
  EXPECT_TRUE(fa.isStateFinal(2));
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
  EXPECT_TRUE(fa.hasTransition(0, 'a', 2));
  EXPECT_EQ(fa.countTransitions(), 2u);
}

TEST(isDeterminist, ZeroInitialState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0,'a', 1));
  EXPECT_FALSE(fa.isDeterministic());

  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_FALSE(fa.isStateInitial(0));
  EXPECT_TRUE(fa.isStateFinal(1));
  EXPECT_FALSE(fa.isStateInitial(1));
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), 1u);
}

TEST(isDeterminist, NoFinalState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0,'a', 1));
  EXPECT_TRUE(fa.isDeterministic());

  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_TRUE(fa.isStateInitial(0));
  EXPECT_FALSE(fa.isStateFinal(0));
  EXPECT_FALSE(fa.isStateFinal(1));
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), 1u);
}

TEST(isDeterminist, TwoInitialStates){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(0);
  fa.setStateInitial(1);
  fa.setStateFinal(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0,'a', 1));
  EXPECT_TRUE(fa.addTransition(1,'a', 2));
  EXPECT_FALSE(fa.isDeterministic());

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.match(""));
  EXPECT_TRUE(fa.match("a"));
  EXPECT_TRUE(fa.match("aa"));
  EXPECT_FALSE(fa.match("aaa"));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_EQ(fa.countStates(), 3u);
  EXPECT_TRUE(fa.isStateInitial(0));
  EXPECT_TRUE(fa.isStateInitial(1));
  EXPECT_TRUE(fa.isStateFinal(1));
  EXPECT_TRUE(fa.isStateFinal(2));
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
  EXPECT_TRUE(fa.hasTransition(1, 'a', 2));
  EXPECT_EQ(fa.countTransitions(), 2u);
}

TEST(isDeterminist, alreadyDeterminist){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0,'a', 1));
  EXPECT_TRUE(fa.addTransition(0,'b', 2));
  EXPECT_TRUE(fa.isDeterministic());

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.match("a"));
  EXPECT_TRUE(fa.match("b"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_EQ(fa.countStates(), 3u);
  EXPECT_TRUE(fa.isStateInitial(0));
  EXPECT_TRUE(fa.isStateFinal(1));
  EXPECT_TRUE(fa.isStateFinal(2));
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_TRUE(fa.hasSymbol('b'));
  EXPECT_EQ(fa.countSymbols(), 2u);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
  EXPECT_TRUE(fa.hasTransition(0, 'b', 2));
  EXPECT_EQ(fa.countTransitions(), 2u);
}

TEST(isDeterminist, alreadyDeterministAndCreateDeterminist){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0,'a', 1));
  EXPECT_TRUE(fa.addTransition(0,'b', 2));
  EXPECT_TRUE(fa.isDeterministic());

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.match("a"));
  EXPECT_TRUE(fa.match("b"));
  EXPECT_FALSE(fa.match(""));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_EQ(fa.countStates(), 3u);
  EXPECT_TRUE(fa.isStateInitial(0));
  EXPECT_TRUE(fa.isStateFinal(1));
  EXPECT_TRUE(fa.isStateFinal(2));
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_TRUE(fa.hasSymbol('b'));
  EXPECT_EQ(fa.countSymbols(), 2u);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
  EXPECT_TRUE(fa.hasTransition(0, 'b', 2));
  EXPECT_EQ(fa.countTransitions(), 2u);  

  fa::Automaton det = fa::Automaton::createDeterministic(fa);
  EXPECT_TRUE(det.isValid());
  EXPECT_FALSE(det.isLanguageEmpty());
  EXPECT_TRUE(det.isDeterministic());
  EXPECT_TRUE(det.hasSymbol('a'));
  EXPECT_TRUE(det.hasSymbol('b'));
  EXPECT_EQ(det.countSymbols(), 2u);
  EXPECT_TRUE(det.match("a"));
  EXPECT_TRUE(det.match("b"));
  EXPECT_FALSE(det.match(""));


  
}

TEST(isDeterminist, alreadyDeterministAndCreateMinimalMoore){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(0);
  fa.setStateFinal(3);
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'b', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(4, 'b', 5));
  EXPECT_TRUE(fa.addTransition(5, 'a', 5));
  EXPECT_TRUE(fa.addTransition(5, 'b', 5));
  EXPECT_TRUE(fa.isDeterministic());

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_TRUE(fa.hasState(3));
  EXPECT_TRUE(fa.hasState(4));
  EXPECT_TRUE(fa.hasState(5));
  EXPECT_EQ(fa.countStates(), 6u);
  EXPECT_TRUE(fa.isStateInitial(0));
  EXPECT_TRUE(fa.isStateFinal(3));
  EXPECT_TRUE(fa.isStateFinal(4));
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_TRUE(fa.hasSymbol('b'));
  EXPECT_EQ(fa.countSymbols(), 2u);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
  EXPECT_TRUE(fa.hasTransition(0, 'b', 2));
  EXPECT_TRUE(fa.hasTransition(1, 'a', 2));
  EXPECT_TRUE(fa.hasTransition(1, 'b', 3));
  EXPECT_TRUE(fa.hasTransition(2, 'a', 1));
  EXPECT_TRUE(fa.hasTransition(2, 'b', 4));
  EXPECT_TRUE(fa.hasTransition(3, 'a', 4));
  EXPECT_TRUE(fa.hasTransition(3, 'b', 5));
  EXPECT_TRUE(fa.hasTransition(4, 'a', 3));
  EXPECT_TRUE(fa.hasTransition(4, 'b', 5));
  EXPECT_TRUE(fa.hasTransition(5, 'a', 5));
  EXPECT_TRUE(fa.hasTransition(5, 'b', 5));
  EXPECT_EQ(fa.countTransitions(), 12u);

  fa::Automaton moore = fa::Automaton::createMinimalMoore(fa);

  EXPECT_TRUE(equivalent(fa, moore));

  EXPECT_TRUE(moore.isValid());
  EXPECT_FALSE(moore.isLanguageEmpty());
  EXPECT_TRUE(moore.isDeterministic());
  EXPECT_TRUE(moore.isComplete());
  EXPECT_EQ(moore.countStates(), 4u);
  EXPECT_TRUE(moore.hasSymbol('a'));
  EXPECT_TRUE(moore.hasSymbol('b'));
  EXPECT_EQ(moore.countSymbols(), 2u);
}

TEST(isDeterminist, alreadyDeterministAndCreateMinimalBrzozowski){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(0);
  fa.setStateFinal(3);
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'b', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(4, 'b', 5));
  EXPECT_TRUE(fa.addTransition(5, 'a', 5));
  EXPECT_TRUE(fa.addTransition(5, 'b', 5));
  EXPECT_TRUE(fa.isDeterministic());

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_TRUE(fa.hasState(3));
  EXPECT_TRUE(fa.hasState(4));
  EXPECT_TRUE(fa.hasState(5));
  EXPECT_EQ(fa.countStates(), 6u);
  EXPECT_TRUE(fa.isStateInitial(0));
  EXPECT_TRUE(fa.isStateFinal(3));
  EXPECT_TRUE(fa.isStateFinal(4));
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_TRUE(fa.hasSymbol('b'));
  EXPECT_EQ(fa.countSymbols(), 2u);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
  EXPECT_TRUE(fa.hasTransition(0, 'b', 2));
  EXPECT_TRUE(fa.hasTransition(1, 'a', 2));
  EXPECT_TRUE(fa.hasTransition(1, 'b', 3));
  EXPECT_TRUE(fa.hasTransition(2, 'a', 1));
  EXPECT_TRUE(fa.hasTransition(2, 'b', 4));
  EXPECT_TRUE(fa.hasTransition(3, 'a', 4));
  EXPECT_TRUE(fa.hasTransition(3, 'b', 5));
  EXPECT_TRUE(fa.hasTransition(4, 'a', 3));
  EXPECT_TRUE(fa.hasTransition(4, 'b', 5));
  EXPECT_TRUE(fa.hasTransition(5, 'a', 5));
  EXPECT_TRUE(fa.hasTransition(5, 'b', 5));
  EXPECT_EQ(fa.countTransitions(), 12u);

  fa::Automaton brzozowski = fa::Automaton::createMinimalBrzozowski(fa);

  EXPECT_TRUE(equivalent(fa, brzozowski));

  EXPECT_TRUE(brzozowski.isValid());
  EXPECT_FALSE(brzozowski.isLanguageEmpty());
  EXPECT_TRUE(brzozowski.isDeterministic());
  EXPECT_TRUE(brzozowski.isComplete());
  EXPECT_EQ(brzozowski.countStates(), 4u);
  EXPECT_TRUE(brzozowski.hasSymbol('a'));
  EXPECT_TRUE(brzozowski.hasSymbol('b'));
  EXPECT_EQ(brzozowski.countSymbols(), 2u);
}

TEST(isComplete, ZeroTransition){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_FALSE(fa.isComplete());

  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.match("a"));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_EQ(fa.countStates(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_EQ(fa.countTransitions(), 0u);
}

TEST(isComplete, Good){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.isComplete());

  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.match("a"));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_EQ(fa.countStates(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 0));
  EXPECT_EQ(fa.countTransitions(), 1u);
}

TEST(isComplete, twoTransitionWithAlphabetTwoLetter){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 0));
  EXPECT_TRUE(fa.addTransition(1, 'a', 1));
  EXPECT_FALSE(fa.isComplete());

  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.match("a"));
  EXPECT_FALSE(fa.match("b"));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_TRUE(fa.hasSymbol('b'));
  EXPECT_EQ(fa.countSymbols(), 2u);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 0));
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
  EXPECT_TRUE(fa.hasTransition(1, 'a', 0));
  EXPECT_TRUE(fa.hasTransition(1, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), 4u);
}

TEST(isComplete, AddedRemovedTransition){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 0));
  EXPECT_TRUE(fa.isComplete());
  EXPECT_TRUE(fa.removeTransition(1, 'a', 0));
  EXPECT_FALSE(fa.isComplete());

  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.isLanguageEmpty());
  EXPECT_FALSE(fa.match(""));
  EXPECT_FALSE(fa.match("a"));
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), 1u);
}

TEST(createComplete, alreadyComplete){
  fa::Automaton fa, n_fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0,'a',0));
  EXPECT_TRUE(fa.addTransition(0,'b',1));
  EXPECT_TRUE(fa.addTransition(1,'a',1));
  EXPECT_TRUE((fa.addTransition(1,'b',0)));
  fa.setStateInitial(0);
  fa.setStateFinal(1);  
  EXPECT_TRUE(fa.isComplete());

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_TRUE(fa.hasSymbol('b'));
  EXPECT_EQ(fa.countSymbols(), 2u);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 0));
  EXPECT_TRUE(fa.hasTransition(0, 'b', 1));
  EXPECT_TRUE(fa.hasTransition(1, 'b', 0));
  EXPECT_TRUE(fa.hasTransition(1, 'a', 1));
  EXPECT_EQ(fa.countTransitions(), 4u);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");


  n_fa = fa::Automaton::createComplete(fa);
  EXPECT_TRUE(n_fa.isComplete());

  EXPECT_TRUE(n_fa.isValid());
  EXPECT_EQ(n_fa.countSymbols(), 2u);
  EXPECT_TRUE(n_fa.hasSymbol('a'));
  EXPECT_TRUE(n_fa.hasSymbol('b'));
  EXPECT_TRUE(equivalent(fa, n_fa));
}

TEST(createComplete, notComplete){
  fa::Automaton fa, n_fa;
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateInitial(0);
  fa.setStateFinal(3);
  EXPECT_TRUE(fa.addTransition(0, 'b', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 3));
  EXPECT_FALSE(fa.isComplete());

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_TRUE(fa.hasState(3));
  EXPECT_EQ(fa.countStates(), 4u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_TRUE(fa.hasSymbol('b'));
  EXPECT_EQ(fa.countSymbols(), 2u);
  EXPECT_TRUE(fa.hasTransition(0, 'b', 1));
  EXPECT_TRUE(fa.hasTransition(1, 'a', 2));
  EXPECT_TRUE(fa.hasTransition(2, 'a', 2));
  EXPECT_TRUE(fa.hasTransition(2, 'b', 3));
  EXPECT_EQ(fa.countTransitions(), 4u);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  n_fa = fa::Automaton::createComplete(fa);

  EXPECT_TRUE(n_fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_EQ(n_fa.countSymbols(), 2u);
  EXPECT_TRUE(n_fa.hasSymbol('a'));
  EXPECT_TRUE(n_fa.hasSymbol('b'));
  EXPECT_TRUE(n_fa.isComplete());
  EXPECT_TRUE(equivalent(fa, n_fa));
}

TEST(createComplete, noTransition){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addSymbol('a'));
  fa.setStateInitial(0);

  fa::Automaton n_fa = fa::Automaton::createComplete(fa);

  EXPECT_TRUE(n_fa.isValid());
  EXPECT_TRUE(n_fa.isLanguageEmpty());
  EXPECT_EQ(n_fa.countSymbols(), 1u);
  EXPECT_TRUE(n_fa.hasSymbol('a'));
  EXPECT_TRUE(n_fa.isComplete());
}

TEST(createComplete, withMaxState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(INT_MAX));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(0);
  fa.setStateFinal(INT_MAX);
  EXPECT_TRUE(fa.addTransition(0, 'a', INT_MAX));

  fa::Automaton n_fa = fa::Automaton::createComplete(fa);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  EXPECT_TRUE(n_fa.isValid());
  EXPECT_FALSE(n_fa.isLanguageEmpty());
  EXPECT_EQ(n_fa.countSymbols(), 2u);
  EXPECT_TRUE(n_fa.hasSymbol('a'));
  EXPECT_TRUE(n_fa.hasSymbol('b'));
  EXPECT_TRUE(n_fa.isComplete());
  EXPECT_TRUE(equivalent(fa, n_fa));
}

TEST(createComplete, MissingState){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 0));
  fa.setStateInitial(0);
  fa.setStateFinal(1);

  fa::Automaton complete = fa::Automaton::createComplete(fa);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  EXPECT_TRUE(complete.isValid());
  EXPECT_FALSE(complete.isLanguageEmpty());
  EXPECT_TRUE(complete.hasSymbol('a'));
  EXPECT_EQ(complete.countSymbols(), 1u);
  EXPECT_TRUE(complete.isComplete());
  EXPECT_TRUE(equivalent(fa, complete));
}

TEST(createComplete, Flecy){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(0);
  fa.setStateFinal(0);

  fa::Automaton complete = fa::Automaton::createComplete(fa);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  EXPECT_TRUE(complete.isValid());
  EXPECT_FALSE(complete.isLanguageEmpty());
  EXPECT_TRUE(complete.hasSymbol('a'));
  EXPECT_TRUE(complete.hasSymbol('b'));
  EXPECT_EQ(complete.countSymbols(), 2u);
  EXPECT_TRUE(complete.isComplete());
  EXPECT_TRUE(equivalent(fa, complete));
}

TEST(createComplement, notCompleteAndAlreadyDeterminist){
  fa::Automaton fa, n_fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0,'a', 1));
  EXPECT_TRUE(fa.addTransition(0,'b', 2));
  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());

  n_fa = fa::Automaton::createComplement(fa);

  EXPECT_TRUE(n_fa.isValid());
  EXPECT_FALSE(n_fa.isLanguageEmpty());
  EXPECT_EQ(n_fa.countSymbols(), 2u);
  EXPECT_TRUE(n_fa.hasSymbol('a'));
  EXPECT_TRUE(n_fa.hasSymbol('b'));
}

TEST(createComplement, AlreadyCompleteAndAlreadyDeterminist){
  fa::Automaton fa, n_fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0,'a', 1));
  EXPECT_TRUE(fa.addTransition(0,'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 2));

  EXPECT_TRUE(fa.isComplete());
  EXPECT_TRUE(fa.isDeterministic());

  n_fa = fa::Automaton::createComplement(fa);

  EXPECT_TRUE(n_fa.isValid());
  EXPECT_FALSE(n_fa.isLanguageEmpty());
  EXPECT_EQ(n_fa.countSymbols(), 2u);
  EXPECT_TRUE(n_fa.hasSymbol('a'));
  EXPECT_TRUE(n_fa.hasSymbol('b'));
}

TEST(createComplement, NotCompleteAndNotDeterminist){
  fa::Automaton fa, n_fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0,'a', 1));
  EXPECT_TRUE(fa.addTransition(0,'a', 2));
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());

  n_fa = fa::Automaton::createComplement(fa);

  EXPECT_TRUE(n_fa.isValid());
  EXPECT_FALSE(n_fa.isLanguageEmpty());
  EXPECT_EQ(n_fa.countSymbols(), 2u);
  EXPECT_TRUE(n_fa.hasSymbol('a'));
  EXPECT_TRUE(n_fa.hasSymbol('b'));
}

TEST(createComplement, noInitialState){
  fa::Automaton fa, n_fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateFinal(1);
  fa.setStateFinal(2);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0,'a', 1));
  EXPECT_TRUE(fa.addTransition(0,'a', 2));
  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());

  n_fa = fa::Automaton::createComplement(fa);

  EXPECT_TRUE(n_fa.isValid());
  EXPECT_FALSE(n_fa.isLanguageEmpty());
  EXPECT_EQ(n_fa.countSymbols(), 2u);
  EXPECT_TRUE(n_fa.hasSymbol('a'));
  EXPECT_TRUE(n_fa.hasSymbol('b'));
}

TEST(createComplement, multipleInitialState){
  fa::Automaton fa, n_fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.setStateInitial(0);
  fa.setStateInitial(1);
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'b', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b' , 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));

  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());

  n_fa = fa::Automaton::createComplement(fa);

  EXPECT_TRUE(n_fa.isValid());
  EXPECT_FALSE(n_fa.isLanguageEmpty());
  EXPECT_EQ(n_fa.countSymbols(), 2u);
  EXPECT_TRUE(n_fa.hasSymbol('a'));
  EXPECT_TRUE(n_fa.hasSymbol('b'));
}

TEST(createMirror, Empty){
  fa::Automaton fa, n_fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addSymbol('a'));
  fa.setStateInitial(0);

  n_fa = fa::Automaton::createMirror(fa);

  EXPECT_TRUE(n_fa.isValid());
  EXPECT_TRUE(n_fa.isLanguageEmpty());
  EXPECT_EQ(n_fa.countStates(), 1u);
  EXPECT_EQ(n_fa.countSymbols(), 1u);
  EXPECT_TRUE(n_fa.hasSymbol('a'));
  EXPECT_EQ(n_fa.countTransitions(), 0u);
}

TEST(createMirror, mirror){
  fa::Automaton fa, n_fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  fa.setStateInitial(0);
  fa.setStateFinal(1);

  n_fa = fa::Automaton::createMirror(fa);

  EXPECT_TRUE(n_fa.isValid());
  EXPECT_FALSE(n_fa.isLanguageEmpty());
  EXPECT_EQ(n_fa.countStates(), 2u);
  EXPECT_EQ(n_fa.countSymbols(), 1u);
  EXPECT_TRUE(n_fa.hasSymbol('a'));
  EXPECT_EQ(n_fa.countTransitions(), 1u);
}

TEST(createMirror, deterministicAndNotCompleteWithMultipleFinalState){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.setStateFinal(2);

  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());

  fa::Automaton n_fa = fa::Automaton::createMirror(fa);

  EXPECT_TRUE(n_fa.isValid());
  EXPECT_FALSE(n_fa.isLanguageEmpty());
  EXPECT_EQ(n_fa.countStates(), 3u);
  EXPECT_EQ(n_fa.countSymbols(), 2u);
  EXPECT_TRUE(n_fa.hasSymbol('a'));
  EXPECT_TRUE(n_fa.hasSymbol('b'));
  EXPECT_EQ(n_fa.countTransitions(), 2u);
}

TEST(createMirror, deterministicAndNotCompleteWithOneFinalState){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  fa.setStateInitial(0);
  fa.setStateFinal(1);

  EXPECT_TRUE(fa.isDeterministic());
  EXPECT_FALSE(fa.isComplete());

  fa::Automaton n_fa = fa::Automaton::createMirror(fa);

  EXPECT_TRUE(n_fa.isValid());
  EXPECT_FALSE(n_fa.isLanguageEmpty());
  EXPECT_EQ(n_fa.countStates(), 3u);
  EXPECT_EQ(n_fa.countSymbols(), 2u);
  EXPECT_TRUE(n_fa.hasSymbol('a'));
  EXPECT_TRUE(n_fa.hasSymbol('b'));
  EXPECT_EQ(n_fa.countTransitions(), 2u);
}

TEST(createMirror, notDeterministicAndComplete){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 2));
  fa.setStateInitial(0);
  fa.setStateInitial(1);
  fa.setStateFinal(2);

  EXPECT_FALSE(fa.isDeterministic());
  EXPECT_TRUE(fa.isComplete());

  fa::Automaton n_fa = fa::Automaton::createMirror(fa);

  EXPECT_TRUE(n_fa.isValid());
  EXPECT_FALSE(n_fa.isLanguageEmpty());
  EXPECT_EQ(n_fa.countStates(), 3u);
  EXPECT_EQ(n_fa.countSymbols(), 2u);
  EXPECT_TRUE(n_fa.hasSymbol('a'));
  EXPECT_TRUE(n_fa.hasSymbol('b'));
  EXPECT_EQ(n_fa.countTransitions(), 7u);
}

TEST(makeTransition, noTransition){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addSymbol('a'));
  fa.setStateInitial(0);
  std::set<int> res, attend = {};
  res = fa.makeTransition({0}, 'a');
  EXPECT_EQ(res, attend);
}

TEST(makeTransition, originNull){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  std::set<int> res, attend = {};
  res = fa.makeTransition({}, 'a');
  EXPECT_EQ(res, attend);
}

TEST(makeTransition, noState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  std::set<int> res, attend = {};
  res = fa.makeTransition({2}, 'a');
  EXPECT_EQ(res, attend);
}

TEST(makeTransition, noSymbol){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  std::set<int> res, attend = {};
  res = fa.makeTransition({0}, 'b');
  EXPECT_EQ(res, attend);
}

TEST(makeTransition, originNullAndNoSymbol){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  std::set<int> res, attend = {};
  res = fa.makeTransition({}, 'b');
  EXPECT_EQ(res, attend);
}

TEST(makeTransition, noStateAndNoState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  std::set<int> res, attend = {};
  res = fa.makeTransition({2}, 'b');
  EXPECT_EQ(res, attend);
}

TEST(makeTransition, noTransitionAndNoSymbol){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  std::set<int> res, attend = {};
  res = fa.makeTransition({1}, 'b');
  EXPECT_EQ(res, attend);
}

TEST(makeTransition, succes){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.setStateInitial(0); 
  fa.setStateInitial(1);
  fa.setStateFinal(1); 
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));

  std::set<int> o = {0,1};
  std::set<int> r = fa.makeTransition(o, 'a');
  o = {1, 2, 3};
  EXPECT_FALSE(r.empty());
  EXPECT_EQ(r, o);
  
}

TEST(readString, notInAlphabetWithFinalAndInitialState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.setStateInitial(0); 
  fa.setStateInitial(1);
  fa.setStateFinal(1); 
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  std::set<int> res = fa.readString("abc");
  std::set<int> attend = {};
  EXPECT_EQ(res, attend);
}

TEST(readString, notInAlphabetWithNoFinalAndInitialState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.setStateInitial(0); 
  fa.setStateInitial(1);
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  std::set<int> res = fa.readString("abc");
  std::set<int> attend = {};
  EXPECT_EQ(res, attend);
}

TEST(readString, noStateInitial){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.isStateFinal(1);
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  std::set<int> res = fa.readString("a");
  std::set<int> attend = {};
  EXPECT_EQ(res, attend);
}

TEST(readString, noStateFinal){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  std::set<int> res = fa.readString("a");
  std::set<int> attend = {1,2,3};
  EXPECT_EQ(res, attend);
}

TEST(readString, noStateFinalAndNoStateInitial){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  std::set<int> res = fa.readString("a");
  std::set<int> attend = {};
  EXPECT_EQ(res, attend);
}

TEST(readString, toShortWord){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.setStateInitial(0); 
  fa.setStateInitial(1);
  fa.setStateFinal(1); 
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  std::set<int> res = fa.readString("ab");
  std::set<int> attend = {3,4};
  EXPECT_EQ(res, attend);
}

TEST(readString, toLongWord){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.setStateInitial(0); 
  fa.setStateInitial(1);
  fa.setStateFinal(1); 
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  std::set<int> res = fa.readString("abbaa");
  std::set<int> attend = {};
  EXPECT_EQ(res, attend);
}

TEST(readString, stringNullAccept){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 0));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  fa.setStateFinal(1);
  std::set<int> res = fa.readString("");
  std::set<int> attend = {0};
  EXPECT_EQ(res, attend);
}

TEST(readString, stringNullNotAccept){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 0));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  std::set<int> res = fa.readString("");
  std::set<int> attend = {0};
  EXPECT_EQ(res, attend);
}

TEST(readString, succes){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.setStateInitial(0); 
  fa.setStateInitial(1);
  fa.setStateFinal(1); 
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  std::set<int> res = fa.readString("abbaaaa");
  std::set<int> attend = {4};
  EXPECT_EQ(res, attend);
}

TEST(match, notInAlphabetWithFinalAndInitialState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.setStateInitial(0); 
  fa.setStateInitial(1);
  fa.setStateFinal(1); 
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  EXPECT_FALSE(fa.match("abc"));
}

TEST(match, notInAlphabetWithNoFinalAndInitialState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.setStateInitial(0); 
  fa.setStateInitial(1);
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  EXPECT_FALSE(fa.match("abc"));
}

TEST(match, noStateInitial){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.isStateFinal(1);
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  EXPECT_FALSE(fa.match("a"));
}

TEST(match, noStateFinal){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  EXPECT_FALSE(fa.match("a"));
}

TEST(match, noStateFinalAndNoStateInitial){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  EXPECT_FALSE(fa.match("a"));
}

TEST(match, toShortWord){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.setStateInitial(0); 
  fa.setStateInitial(1);
  fa.setStateFinal(1); 
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  EXPECT_TRUE(fa.match("ab"));
}

TEST(match, toLongWord){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.setStateInitial(0); 
  fa.setStateInitial(1);
  fa.setStateFinal(1); 
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_FALSE(fa.match("abbaa"));
}

TEST(match, stringNullAccept){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 0));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.match(""));
}

TEST(match, stringNullNotAccept){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 0));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  EXPECT_FALSE(fa.match(""));
}

TEST(match, succes){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  fa.setStateInitial(0); 
  fa.setStateInitial(1);
  fa.setStateFinal(1); 
  fa.setStateFinal(4);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  EXPECT_TRUE(fa.match("abbaaaa"));
}



/*TP3*/

TEST(isLanguageEmpty, notEmpty){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addSymbol('a'));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_FALSE(fa.isLanguageEmpty());
}

TEST(isLanguageEmpty, noFinalState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.isLanguageEmpty());
}

TEST(isLanguageEmpty, empty){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addSymbol('a'));
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.isLanguageEmpty());
}

TEST(isLanguageEmpty, multipleInitialStateWithAcceptWord){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(1, 'a', 1));
  fa.setStateInitial(0);
  fa.setStateInitial(1);
  fa.setStateFinal(1);
  EXPECT_FALSE(fa.isLanguageEmpty());
}

TEST(isLanguageEmpty, noInitial){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.isLanguageEmpty());
}

TEST(isLanguageEmpty, notLinked){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(1, 'a', 0));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  fa.setStateInitial(0);
  fa.setStateFinal(2);

  EXPECT_TRUE(fa.isLanguageEmpty());
}

TEST(removeNonAccessibleStates, noTransition){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0,'a',1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_EQ(fa.countStates(), 3u);
  EXPECT_EQ(fa.countTransitions(), 2u);
  fa.removeNonAccessibleStates();

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_FALSE(fa.hasState(2));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countTransitions(), 1u);
  EXPECT_FALSE(fa.hasTransition(2, 'a', 2));
}

TEST(removeNonAccessibleStates, multipleNoTransition){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0,'a',1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_EQ(fa.countStates(), 4u);
  EXPECT_EQ(fa.countTransitions(), 3u);
  fa.removeNonAccessibleStates();
  
  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  
  EXPECT_FALSE(fa.hasState(2));
  EXPECT_FALSE(fa.hasTransition(2,'a', 2));
  EXPECT_FALSE(fa.hasState(3));
  EXPECT_FALSE(fa.hasTransition(3, 'a', 3));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_EQ(fa.countTransitions(), 1u);
}

TEST(removeNonAccessibleStates, noTransitionWithInitialState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.hasTransition(2, 'a', 1));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_EQ(fa.countStates(), 3u);
  EXPECT_EQ(fa.countTransitions(), 2u);
  fa.removeNonAccessibleStates();

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));


  EXPECT_FALSE(fa.hasTransition(2, 'a', 1));
  EXPECT_FALSE(fa.hasState(2));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_EQ(fa.countTransitions(), 1u);
}

TEST(removeNonAccessibleStates, multipleNoTransitionWithInitialState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  EXPECT_TRUE(fa.addTransition(3, 'a', 1));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.hasTransition(2, 'a', 1));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_TRUE(fa.hasTransition(3, 'a', 1));
  EXPECT_TRUE(fa.hasState(3));
  EXPECT_EQ(fa.countStates(), 4u);
  EXPECT_EQ(fa.countTransitions(), 3u);
  fa.removeNonAccessibleStates();

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));


  EXPECT_FALSE(fa.hasTransition(2, 'a', 1));
  EXPECT_FALSE(fa.hasState(2));
  EXPECT_FALSE(fa.hasTransition(3, 'a', 1));
  EXPECT_FALSE(fa.hasState(3));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_EQ(fa.countTransitions(), 1u);
}

TEST(removeNonAccessibleStates, AllNoTransition){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.hasTransition(2, 'a', 1));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_TRUE(fa.hasTransition(3, 'a', 3));
  EXPECT_TRUE(fa.hasState(3));
  EXPECT_EQ(fa.countStates(), 4u);
  EXPECT_EQ(fa.countTransitions(), 3u);
  fa.removeNonAccessibleStates();

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));

  EXPECT_FALSE(fa.hasTransition(2, 'a', 1));
  EXPECT_FALSE(fa.hasState(2));
  EXPECT_FALSE(fa.hasTransition(3, 'a', 3));
  EXPECT_FALSE(fa.hasState(3));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_EQ(fa.countTransitions(), 1u);
}

TEST(removeNonAccessibleStates, noInitialState){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.hasTransition(2, 'a', 1));
  EXPECT_TRUE(fa.hasState(2));
  fa.removeNonAccessibleStates();

  EXPECT_TRUE(fa.isValid());
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));

  EXPECT_EQ(fa.countStates(), 1u);
  EXPECT_EQ(fa.countTransitions(), 0u);
}

TEST(removeNonAccessibleStates, AllAccessibleState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.hasTransition(2, 'a', 1));
  EXPECT_TRUE(fa.hasState(2));
  fa.removeNonAccessibleStates();

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_EQ(fa.countStates(), 3u);
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countTransitions(), 3u);
}

TEST(removeNonAccessibleStates, InitialStateWithNoTransition){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  fa.setStateInitial(0);
  fa.setStateInitial(1);
  fa.setStateFinal(2);
  fa.removeNonAccessibleStates();

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_EQ(fa.countStates(), 3u);
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countTransitions(), 1u);
  EXPECT_TRUE(fa.hasTransition(1, 'a', 2));
}

TEST(removeNonCoAccessibleStates, noTransition){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0,'a',1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_TRUE(fa.hasTransition(2,'a', 2));
  EXPECT_EQ(fa.countStates(), 3u);
  EXPECT_EQ(fa.countTransitions(), 2u);
  fa.removeNonCoAccessibleStates();

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));

  EXPECT_FALSE(fa.hasState(2));
  EXPECT_FALSE(fa.hasTransition(2, 'a', 2));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_EQ(fa.countTransitions(), 1u);
}

TEST(removeNonCoAccessibleStates, MultipleNoTransition){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0,'a',1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_TRUE(fa.hasState(3));
  EXPECT_TRUE(fa.hasTransition(2,'a', 2));
  EXPECT_TRUE(fa.hasTransition(3, 'a', 3));
  EXPECT_EQ(fa.countStates(), 4u);
  EXPECT_EQ(fa.countTransitions(), 3u);
  fa.removeNonCoAccessibleStates();

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));

  EXPECT_FALSE(fa.hasState(2));
  EXPECT_FALSE(fa.hasTransition(2, 'a', 2));
  EXPECT_FALSE(fa.hasState(3));
  EXPECT_FALSE(fa.hasTransition(3, 'a', 3));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_EQ(fa.countTransitions(), 1u);
}

TEST(removeNonCoAccessibleStates, noTransitionWithFinalState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 2));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_EQ(fa.countStates(), 3u);
  EXPECT_EQ(fa.countTransitions(), 2u);
  fa.removeNonCoAccessibleStates();

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));

  EXPECT_FALSE(fa.hasTransition(0, 'a', 2));
  EXPECT_FALSE(fa.hasState(2));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_EQ(fa.countTransitions(), 1u);
}

TEST(removeNonCoAccessibleStates, multipleNoTransitionWithFinalState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 2));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_TRUE(fa.hasTransition(0, 'a', 3));
  EXPECT_TRUE(fa.hasState(3));
  EXPECT_EQ(fa.countStates(), 4u);
  EXPECT_EQ(fa.countTransitions(), 3u);
  fa.removeNonCoAccessibleStates();

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));

  EXPECT_FALSE(fa.hasTransition(0, 'a', 2));
  EXPECT_FALSE(fa.hasState(2));
  EXPECT_FALSE(fa.hasTransition(0, 'a', 3));
  EXPECT_FALSE(fa.hasState(3));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_EQ(fa.countTransitions(), 1u);
}

TEST(removeNonCoAccessibleStates, allNoTransition){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 2));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_TRUE(fa.hasTransition(3, 'a', 3));
  EXPECT_TRUE(fa.hasState(3));
  EXPECT_EQ(fa.countStates(), 4u);
  EXPECT_EQ(fa.countTransitions(), 3u);
  fa.removeNonCoAccessibleStates();

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));

  EXPECT_FALSE(fa.hasTransition(0, 'a', 2));
  EXPECT_FALSE(fa.hasState(2));
  EXPECT_FALSE(fa.hasTransition(3, 'a', 3));
  EXPECT_FALSE(fa.hasState(3));
  EXPECT_EQ(fa.countStates(), 2u);
  EXPECT_EQ(fa.countTransitions(), 1u);
}

TEST(removeNonCoAccessibleStates, noFinalState){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  fa.setStateInitial(0);
  EXPECT_TRUE(fa.hasTransition(2, 'a', 1));
  EXPECT_TRUE(fa.hasState(2));
  fa.removeNonCoAccessibleStates();

  EXPECT_TRUE(fa.isValid());
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));

  EXPECT_EQ(fa.countTransitions(), 0u);
  EXPECT_EQ(fa.countStates(), 1u);
}

TEST(removeNonCoAccessibleStates, allCoAccessible){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  EXPECT_TRUE(fa.hasTransition(2, 'a', 1));
  EXPECT_TRUE(fa.hasState(2));
  fa.removeNonCoAccessibleStates();

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_EQ(fa.countStates(), 3u);
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countTransitions(), 2u);

  EXPECT_TRUE(fa.hasTransition(2, 'a', 1));
  EXPECT_TRUE(fa.hasState(2));
}

TEST(removeNonCoAccessibleStates, FinalStateWithNoTransition){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  fa.setStateInitial(0);
  fa.setStateFinal(1);
  fa.setStateFinal(2);
  fa.removeNonCoAccessibleStates();

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_EQ(fa.countStates(), 3u);
  EXPECT_TRUE(fa.hasState(0));
  EXPECT_TRUE(fa.hasState(1));
  EXPECT_TRUE(fa.hasState(2));
  EXPECT_EQ(fa.countSymbols(), 1u);
  EXPECT_TRUE(fa.hasSymbol('a'));
  EXPECT_EQ(fa.countTransitions(), 1u);
  EXPECT_TRUE(fa.hasTransition(0, 'a', 1));
}

TEST(createIntersection, lhsDeterministicAndRhsDeterministic){
  fa::Automaton lhs, rhs, fa;
  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 0));
  lhs.setStateInitial(0);
  lhs.setStateFinal(1);
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);
  EXPECT_TRUE(lhs.isDeterministic());
  EXPECT_TRUE(rhs.isDeterministic());
  EXPECT_TRUE(lhs.match("ababababababaabababababa"));
  EXPECT_TRUE(rhs.match("babababababbaabbabbb"));

  fa = fa::Automaton::createIntersection(lhs, rhs);
  
  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
  EXPECT_TRUE(fa.match("abababababaababbbb"));
}

TEST(createIntersection, lhsNotDeterministicWithMultipleInitStateAndRhsDeterministic){
  fa::Automaton lhs, rhs, fa;
  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(lhs.addState(2));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 2));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 2));
  EXPECT_TRUE(lhs.addTransition(2, 'a', 2));
  EXPECT_TRUE(lhs.addTransition(2, 'b', 2));
  lhs.setStateInitial(0);
  lhs.setStateInitial(1);
  lhs.setStateFinal(2);

  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 0));
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);

  fa = fa::Automaton::createIntersection(lhs, rhs);

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
}

TEST(createIntersection, lhsNotDeterministicWithMultipleInitStateAndRhsNotDeterministicWithMultipleInitState){
  fa::Automaton lhs, rhs;

  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(lhs.addState(2));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 2));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 2));
  EXPECT_TRUE(lhs.addTransition(2, 'a', 2));
  EXPECT_TRUE(lhs.addTransition(2, 'b', 2));
  lhs.setStateInitial(0);
  lhs.setStateInitial(1);
  lhs.setStateFinal(2);
  EXPECT_FALSE(lhs.isDeterministic());

  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(rhs.addState(2));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 2));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 2));
  EXPECT_TRUE(rhs.addTransition(2, 'b', 2));
  EXPECT_TRUE(rhs.addTransition(2, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(2, 'b', 1));
  rhs.setStateInitial(0);
  rhs.setStateInitial(1);
  rhs.setStateFinal(2);
  EXPECT_FALSE(rhs.isDeterministic());

  fa::Automaton fa = fa::Automaton::createIntersection(lhs, rhs);
  
  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
}

TEST(createIntersection, lhsNotDeterministicWithMultipleInitFinalStateAndRhsDeterministic){
  fa::Automaton lhs, rhs;

  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(lhs.addState(2));
  EXPECT_TRUE(lhs.addState(3));
  EXPECT_TRUE(lhs.addState(4));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 2));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 2));
  EXPECT_TRUE(lhs.addTransition(2, 'a', 2));
  EXPECT_TRUE(lhs.addTransition(2, 'b', 2));
  EXPECT_TRUE(lhs.addTransition(2, 'a', 3));
  EXPECT_TRUE(lhs.addTransition(2, 'b', 4));
  lhs.setStateInitial(0);
  lhs.setStateInitial(1);
  lhs.setStateFinal(3);
  lhs.setStateFinal(4);
  EXPECT_FALSE(lhs.isDeterministic());

  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 0));
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);
  EXPECT_TRUE(rhs.isDeterministic());

  fa::Automaton fa = fa::Automaton::createIntersection(lhs, rhs);

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());

}

TEST(createIntersection, lhsNotDeterministicWithMultipleInitFinalStateAndRhsNotDeterministicWithMultipleInitState){
  fa::Automaton lhs, rhs;

  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(lhs.addState(2));
  EXPECT_TRUE(lhs.addState(3));
  EXPECT_TRUE(lhs.addState(4));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 2));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 2));
  EXPECT_TRUE(lhs.addTransition(2, 'a', 2));
  EXPECT_TRUE(lhs.addTransition(2, 'b', 2));
  EXPECT_TRUE(lhs.addTransition(2, 'a', 3));
  EXPECT_TRUE(lhs.addTransition(2, 'b', 4));
  lhs.setStateInitial(0);
  lhs.setStateInitial(1);
  lhs.setStateFinal(3);
  lhs.setStateFinal(4);
  EXPECT_FALSE(lhs.isDeterministic());

  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(rhs.addState(2));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 2));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 2));
  EXPECT_TRUE(rhs.addTransition(2, 'b', 2));
  EXPECT_TRUE(rhs.addTransition(2, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(2, 'b', 1));
  rhs.setStateInitial(0);
  rhs.setStateInitial(1);
  rhs.setStateFinal(2);
  EXPECT_FALSE(rhs.isDeterministic());

  fa::Automaton fa = fa::Automaton::createIntersection(lhs, rhs);

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
}

TEST(createIntersection, lhsNotDeterministicWithMultipleInitFinalStateAndRhsNotDeterministicWithMultipleInitFinalState){
  fa::Automaton lhs, rhs;

  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(lhs.addState(2));
  EXPECT_TRUE(lhs.addState(3));
  EXPECT_TRUE(lhs.addState(4));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 2));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 2));
  EXPECT_TRUE(lhs.addTransition(2, 'a', 2));
  EXPECT_TRUE(lhs.addTransition(2, 'b', 2));
  EXPECT_TRUE(lhs.addTransition(2, 'a', 3));
  EXPECT_TRUE(lhs.addTransition(2, 'b', 4));
  lhs.setStateInitial(0);
  lhs.setStateInitial(1);
  lhs.setStateFinal(3);
  lhs.setStateFinal(4);
  EXPECT_FALSE(lhs.isDeterministic());

  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(rhs.addState(2));
  EXPECT_TRUE(rhs.addState(3));
  EXPECT_TRUE(rhs.addState(4));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 2));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 2));
  EXPECT_TRUE(rhs.addTransition(2, 'b', 2));
  EXPECT_TRUE(rhs.addTransition(2, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(2, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(2, 'a', 3));
  EXPECT_TRUE(rhs.addTransition(2, 'b', 4));
  EXPECT_TRUE(rhs.addTransition(3, 'a', 3));
  EXPECT_TRUE(rhs.addTransition(3, 'b', 3));
  EXPECT_TRUE(rhs.addTransition(4, 'a', 4));
  EXPECT_TRUE(rhs.addTransition(4, 'b', 4));
  rhs.setStateInitial(0);
  rhs.setStateInitial(1);
  rhs.setStateFinal(3);
  rhs.setStateFinal(4);
  EXPECT_FALSE(rhs.isDeterministic());

  fa::Automaton fa = fa::Automaton::createIntersection(lhs, rhs);

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
}

TEST(createIntersection, lhsNotDeterministicRhsDeterministic){
  fa::Automaton lhs, rhs;

  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 0));
  lhs.setStateInitial(0);
  lhs.setStateFinal(1);
  EXPECT_FALSE(lhs.isDeterministic());

  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 0));
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);
  EXPECT_TRUE(rhs.isDeterministic());

  fa::Automaton fa = fa::Automaton::createIntersection(lhs, rhs);
  
  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
}

TEST(createIntersection, lhsNotDeterministicRhsNotDeterministic){
  fa::Automaton lhs, rhs;

  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 0));
  lhs.setStateInitial(0);
  lhs.setStateFinal(1);
  EXPECT_FALSE(lhs.isDeterministic());

  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 0));
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);
  EXPECT_FALSE(rhs.isDeterministic());

  fa::Automaton fa = fa::Automaton::createIntersection(lhs, rhs);
  
  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
}

TEST(createIntersection, lhsHaveMoreLetterThanRhs){
  fa::Automaton lhs, rhs;

  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(lhs.addState(2));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(lhs.addSymbol('c'));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'c', 2));
  lhs.setStateInitial(0);
  lhs.setStateFinal(1);
  lhs.setStateFinal(2);

  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 0));
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);

  fa::Automaton fa = fa::Automaton::createIntersection(lhs, rhs);

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
}

TEST(createIntersection, lhsAlphabetDifferentRhsAlphabetWithSameLetter){
fa::Automaton lhs, rhs;

  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(lhs.addState(2));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(lhs.addSymbol('c'));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'c', 2));
  lhs.setStateInitial(0);
  lhs.setStateFinal(1);
  lhs.setStateFinal(2);

  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(rhs.addState(2));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addSymbol('d'));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(1, 'd', 2));
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);
  rhs.setStateFinal(2);

  fa::Automaton fa = fa::Automaton::createIntersection(lhs, rhs);

  EXPECT_TRUE(fa.isValid());
  EXPECT_FALSE(fa.isLanguageEmpty());
}

TEST(createIntersection, notSameLetter){
  fa::Automaton lhs, rhs;

  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 1));
  lhs.setStateInitial(0);
  lhs.setStateFinal(1);

  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(rhs.addSymbol('c'));
  EXPECT_TRUE(rhs.addSymbol('d'));
  EXPECT_TRUE(rhs.addTransition(0, 'c', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'd', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'd', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'c', 0));
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);

  fa::Automaton fa = fa::Automaton::createIntersection(lhs, rhs);

  EXPECT_TRUE(fa.isValid());
  EXPECT_TRUE(fa.isLanguageEmpty()); 
}

TEST(createIntersection, lhsNoTransition){
  fa::Automaton lhs, rhs;

  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  lhs.setStateInitial(0);
  lhs.setStateFinal(0);

  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 0));
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);

  fa::Automaton inter = fa::Automaton::createIntersection(lhs, rhs);

  EXPECT_TRUE(inter.isLanguageEmpty());
  EXPECT_TRUE(inter.isValid());
  EXPECT_FALSE(inter.match(""));
}

TEST(createIntersection, lhsNoTransitionAndRhsNoTransition){
  fa::Automaton lhs, rhs;

  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  lhs.setStateInitial(0);
  lhs.setStateFinal(0);

  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);

  fa::Automaton inter = fa::Automaton::createIntersection(lhs, rhs);

  EXPECT_TRUE(inter.isLanguageEmpty());
  EXPECT_TRUE(inter.isValid());
  EXPECT_FALSE(inter.match(""));
}

TEST(hasEmptyIntersectionWith, noEmpty){
  fa::Automaton lhs, rhs;

  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 1));
  lhs.setStateInitial(0);
  lhs.setStateFinal(1);

  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'a', 0));
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);

  EXPECT_FALSE(lhs.hasEmptyIntersectionWith(rhs));
}

TEST(hasEmptyIntersectionWith, notSameLetter){
  fa::Automaton lhs, rhs;

  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addState(1));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  EXPECT_TRUE(lhs.addTransition(0, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'a', 1));
  EXPECT_TRUE(lhs.addTransition(1, 'b', 1));
  lhs.setStateInitial(0);
  lhs.setStateFinal(1);

  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(rhs.addSymbol('c'));
  EXPECT_TRUE(rhs.addSymbol('d'));
  EXPECT_TRUE(rhs.addTransition(0, 'c', 0));
  EXPECT_TRUE(rhs.addTransition(0, 'd', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'd', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'c', 0));
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);

  EXPECT_TRUE(lhs.hasEmptyIntersectionWith(rhs));
}

TEST(hasEmptyIntersectionWith, lhsNoTransitionAndRhsNoTransition){
  fa::Automaton lhs, rhs;

  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  lhs.setStateInitial(0);
  lhs.setStateFinal(0);

  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);

  EXPECT_TRUE(lhs.hasEmptyIntersectionWith(rhs));
}

TEST(hasEmptyIntersectionWith, lhsNoTransition){
  fa::Automaton lhs, rhs;

  EXPECT_TRUE(lhs.addState(0));
  EXPECT_TRUE(lhs.addSymbol('a'));
  EXPECT_TRUE(lhs.addSymbol('b'));
  lhs.setStateInitial(0);
  lhs.setStateFinal(0);

  EXPECT_TRUE(rhs.addState(0));
  EXPECT_TRUE(rhs.addState(1));
  EXPECT_TRUE(rhs.addSymbol('a'));
  EXPECT_TRUE(rhs.addSymbol('b'));
  EXPECT_TRUE(rhs.addTransition(0, 'a', 1));
  EXPECT_TRUE(rhs.addTransition(1, 'b', 0));
  rhs.setStateInitial(0);
  rhs.setStateFinal(1);

  EXPECT_TRUE(lhs.hasEmptyIntersectionWith(rhs));
}

TEST(createDeterministic, DS2022){
  fa::Automaton fa;

  for(int i = 1; i < 11; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  for(int i = 0; i < 6; ++i){
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 5));
  EXPECT_TRUE(fa.addTransition(0, 'b', 4));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 0));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  EXPECT_TRUE(fa.addTransition(5, 'a', 5));
  EXPECT_TRUE(fa.addTransition(5, 'b', 4));
  fa.setStateInitial(0);
  fa.setStateFinal(3);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton deter = fa::Automaton::createDeterministic(fa);

  EXPECT_TRUE(deter.isValid());
  EXPECT_FALSE(deter.isLanguageEmpty());
  EXPECT_TRUE(deter.isDeterministic());
  EXPECT_EQ(deter.countSymbols(), 2u);
  EXPECT_TRUE(deter.hasSymbol('a'));
  EXPECT_TRUE(deter.hasSymbol('b'));
  EXPECT_TRUE(equivalent(fa, deter));
  deter.dotPrint(std::cout);
}

TEST(createDeterministic, exemplePDF){
  fa::Automaton fa;
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'b', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 4));
  EXPECT_TRUE(fa.addTransition(2, 'b', 1));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'a', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  EXPECT_TRUE(fa.addTransition(4, 'b', 4));
  fa.setStateInitial(0);
  fa.setStateFinal(4);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton n_fa = fa::Automaton::createDeterministic(fa);

  EXPECT_TRUE(n_fa.isValid());
  EXPECT_FALSE(n_fa.isLanguageEmpty());
  EXPECT_TRUE(n_fa.isDeterministic());
  EXPECT_EQ(n_fa.countSymbols(), 2u);
  EXPECT_TRUE(n_fa.hasSymbol('a'));
  EXPECT_TRUE(n_fa.hasSymbol('b'));
  EXPECT_TRUE(equivalent(fa, n_fa));
}

TEST(createDeterministic, alreadyDeterministic){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 1));
  fa.setStateInitial(0);
  fa.setStateFinal(1);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton n_fa = fa::Automaton::createDeterministic(fa);

  EXPECT_TRUE(n_fa.isValid());
  EXPECT_FALSE(n_fa.isLanguageEmpty());
  EXPECT_TRUE(n_fa.isDeterministic());
  EXPECT_EQ(n_fa.countSymbols(), 2u);
  EXPECT_TRUE(n_fa.hasSymbol('a'));
  EXPECT_TRUE(n_fa.hasSymbol('b'));
  EXPECT_TRUE(equivalent(fa, n_fa));
}

TEST(createDeterministic, noInitialState){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 0));
  fa.setStateFinal(1);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton n_fa = fa::Automaton::createDeterministic(fa);

  EXPECT_TRUE(n_fa.isValid());
  EXPECT_TRUE(n_fa.isLanguageEmpty());
  EXPECT_TRUE(n_fa.isDeterministic());
  EXPECT_EQ(n_fa.countSymbols(), 2u);
  EXPECT_TRUE(n_fa.hasSymbol('a'));
  EXPECT_TRUE(n_fa.hasSymbol('b'));
  EXPECT_TRUE(equivalent(fa, n_fa));
}

TEST(createDeterministic, noFinalState){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 0));
  fa.setStateInitial(0);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton n_fa = fa::Automaton::createDeterministic(fa);

  EXPECT_TRUE(n_fa.isValid());
  EXPECT_TRUE(n_fa.isLanguageEmpty());
  EXPECT_TRUE(n_fa.isDeterministic());
  EXPECT_EQ(n_fa.countSymbols(), 2u);
  EXPECT_TRUE(n_fa.hasSymbol('a'));
  EXPECT_TRUE(n_fa.hasSymbol('b'));
  EXPECT_TRUE(equivalent(fa, n_fa));
}

TEST(createDeterministic, multipleInitialState){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 2));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 3));
  EXPECT_TRUE(fa.addTransition(2, 'b', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  fa.setStateInitial(0);
  fa.setStateInitial(1);
  fa.setStateFinal(4);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton n_fa = fa::Automaton::createDeterministic(fa);

  EXPECT_TRUE(n_fa.isValid());
  EXPECT_FALSE(n_fa.isLanguageEmpty());
  EXPECT_TRUE(n_fa.isDeterministic());
  EXPECT_EQ(n_fa.countSymbols(), 2u);
  EXPECT_TRUE(n_fa.hasSymbol('a'));
  EXPECT_TRUE(n_fa.hasSymbol('b'));
  EXPECT_TRUE(equivalent(fa, n_fa));
}

TEST(createDeterministic, DS){
  fa::Automaton fa;

  for(int i = 0; i <6; ++i){
    EXPECT_TRUE(fa.addState(i));
  }

  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));

  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 3));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 4));
  EXPECT_TRUE(fa.addTransition(2, 'b', 5));
  EXPECT_TRUE(fa.addTransition(3, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 1));
  EXPECT_TRUE(fa.addTransition(4, 'b', 2));
  EXPECT_TRUE(fa.addTransition(5, 'a', 2));
  EXPECT_TRUE(fa.addTransition(5, 'b', 4));
  fa.setStateInitial(0);
  fa.setStateInitial(5);
  fa.setStateFinal(2);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton n_fa = fa::Automaton::createDeterministic(fa);
  
  EXPECT_TRUE(n_fa.isValid());
  EXPECT_FALSE(n_fa.isLanguageEmpty());
  EXPECT_TRUE(n_fa.isDeterministic());
  EXPECT_EQ(n_fa.countSymbols(), 2u);
  EXPECT_TRUE(n_fa.hasSymbol('a'));
  EXPECT_TRUE(n_fa.hasSymbol('b'));
  EXPECT_FALSE(n_fa.match("b"));
  EXPECT_TRUE(equivalent(fa, n_fa));
}

TEST(createDeterministic, noTransition){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  fa.setStateInitial(0);
  fa.setStateFinal(0);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton deter = fa::Automaton::createDeterministic(fa);

  EXPECT_TRUE(deter.isValid());
  EXPECT_FALSE(deter.isLanguageEmpty());
  EXPECT_TRUE(deter.isDeterministic());
  EXPECT_EQ(deter.countSymbols(), 2u);
  EXPECT_TRUE(deter.hasSymbol('a'));
  EXPECT_TRUE(deter.hasSymbol('b'));
  EXPECT_TRUE(equivalent(fa, deter));
}

TEST(createDeterministic, alreadyMinimal){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addState(6));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addSymbol('c'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(0, 'c', 3));
  EXPECT_TRUE(fa.addTransition(1, 'a', 6));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  EXPECT_TRUE(fa.addTransition(1, 'c', 6));
  EXPECT_TRUE(fa.addTransition(2, 'a', 6));
  EXPECT_TRUE(fa.addTransition(2, 'b', 4));
  EXPECT_TRUE(fa.addTransition(2, 'c', 0));
  EXPECT_TRUE(fa.addTransition(3, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 6));
  EXPECT_TRUE(fa.addTransition(3, 'c', 6));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  EXPECT_TRUE(fa.addTransition(4, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'c', 3));
  EXPECT_TRUE(fa.addTransition(5, 'a', 6));
  EXPECT_TRUE(fa.addTransition(5, 'b', 4));
  EXPECT_TRUE(fa.addTransition(5, 'c', 6));
  EXPECT_TRUE(fa.addTransition(6, 'a', 6));
  EXPECT_TRUE(fa.addTransition(6, 'b', 6));
  EXPECT_TRUE(fa.addTransition(6, 'c', 6));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  fa.setStateFinal(4);

  EXPECT_TRUE(fa.isDeterministic());

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton deter = fa::Automaton::createDeterministic(fa);

  EXPECT_TRUE(deter.isValid());
  EXPECT_FALSE(deter.isLanguageEmpty());
  EXPECT_TRUE(deter.isDeterministic());
  EXPECT_EQ(deter.countSymbols(), 3u);
  EXPECT_TRUE(deter.hasSymbol('a'));
  EXPECT_TRUE(deter.hasSymbol('b'));
  EXPECT_TRUE(deter.hasSymbol('c'));
  EXPECT_TRUE(equivalent(fa, deter));
}

TEST(isIncludedIn, Same){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  fa.setStateInitial(0);
  fa.setStateFinal(2);

  EXPECT_TRUE(fa.isIncludedIn(fa));
}

TEST(isIncludedIn, DifferentSymbols){
  fa::Automaton fa, fa2;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  fa.setStateInitial(0);
  fa.setStateFinal(2);

  EXPECT_TRUE(fa2.addState(0));
  EXPECT_TRUE(fa2.addState(1));
  EXPECT_TRUE(fa2.addState(2));
  EXPECT_TRUE(fa2.addSymbol('b'));
  EXPECT_TRUE(fa2.addTransition(0, 'b', 1));
  EXPECT_TRUE(fa2.addTransition(1, 'b', 2));
  fa2.setStateInitial(0);
  fa2.setStateFinal(2);

  EXPECT_TRUE(fa.isIncludedIn(fa2));
  EXPECT_TRUE(fa2.isIncludedIn(fa));
}

TEST(isIncludedIn, EmptyInEmpty){
  fa::Automaton fa1, fa2;

  EXPECT_TRUE(fa1.addState(0));
  EXPECT_TRUE(fa1.addSymbol('a'));

  EXPECT_TRUE(fa2.addState(0));
  EXPECT_TRUE(fa2.addSymbol('a'));

  EXPECT_TRUE(fa1.isIncludedIn(fa2));
  EXPECT_TRUE(fa2.isIncludedIn(fa1));
}

TEST(isIncludedIn, Royer1){
  fa::Automaton fa, fa2;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addSymbol('c'));
  EXPECT_TRUE(fa.addSymbol('d'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  fa.setStateInitial(0);
  fa.setStateFinal(1);

  EXPECT_TRUE(fa2.addState(0));
  EXPECT_TRUE(fa2.addState(1));
  EXPECT_TRUE(fa2.addSymbol('a'));
  EXPECT_TRUE(fa2.addTransition(0, 'a', 1));
  fa2.setStateInitial(0);
  fa2.setStateFinal(1);

  EXPECT_TRUE(fa.isIncludedIn(fa2));
}

TEST(isIncludedIn, Royer2){
  fa::Automaton fa, fa2;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addSymbol('c'));
  EXPECT_TRUE(fa.addSymbol('d'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'a', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 2));
  EXPECT_TRUE(fa.addTransition(2, 'c', 3));
  EXPECT_TRUE(fa.addTransition(2, 'd', 3));
  fa.setStateInitial(0);
  fa.setStateFinal(3);

  EXPECT_TRUE(fa2.addState(0));
  EXPECT_TRUE(fa2.addState(1));
  EXPECT_TRUE(fa2.addSymbol('a'));
  EXPECT_TRUE(fa2.addTransition(0, 'a', 1));
  fa2.setStateInitial(0);
  fa2.setStateFinal(1);

  EXPECT_TRUE(fa.isIncludedIn(fa2));
}

TEST(createMinimalMoore, alreadyCompleteAndAlreadyDeterministic){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'b', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(4, 'b', 5));
  EXPECT_TRUE(fa.addTransition(5, 'a', 5));
  EXPECT_TRUE(fa.addTransition(5, 'b', 5));
  fa.setStateInitial(0);
  fa.setStateFinal(3);
  fa.setStateFinal(4);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton n = fa::Automaton::createMinimalMoore(fa);

  EXPECT_TRUE(n.isValid());
  EXPECT_FALSE(n.isLanguageEmpty());
  EXPECT_EQ(n.countStates(), 4u);
  EXPECT_EQ(n.countSymbols(), 2u);
  EXPECT_TRUE(n.hasSymbol('a'));
  EXPECT_TRUE(n.hasSymbol('b'));
  EXPECT_TRUE(n.isDeterministic());
  EXPECT_TRUE(n.isComplete());
  EXPECT_TRUE(equivalent(fa, n));
}

TEST(createMinimalMoore, alreadyMinimal_ex18_1TD){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addSymbol('c'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(0, 'b', 4));
  EXPECT_TRUE(fa.addTransition(0, 'c', 5));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  EXPECT_TRUE(fa.addTransition(2, 'c', 0));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(3, 'c', 5));
  EXPECT_TRUE(fa.addTransition(4, 'b', 3));
  EXPECT_TRUE(fa.addTransition(5, 'a', 3));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  fa.setStateFinal(3);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton minim = fa::Automaton::createMinimalMoore(fa);

  EXPECT_TRUE(minim.isValid());
  EXPECT_FALSE(minim.isLanguageEmpty());
  EXPECT_EQ(minim.countStates(), 7u);
  EXPECT_EQ(minim.countSymbols(), 3u);
  EXPECT_TRUE(minim.hasSymbol('a'));
  EXPECT_TRUE(minim.hasSymbol('b'));
  EXPECT_TRUE(minim.hasSymbol('c'));
  EXPECT_TRUE(minim.isDeterministic());
  EXPECT_TRUE(minim.isComplete());
  EXPECT_TRUE(equivalent(fa, minim));
}

TEST(createMinimalMoore, noInitialState){
  fa::Automaton fa;
  
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'b', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(4, 'b', 5));
  EXPECT_TRUE(fa.addTransition(5, 'a', 5));
  EXPECT_TRUE(fa.addTransition(5, 'b', 5));
  fa.setStateFinal(3);
  fa.setStateFinal(4);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton n = fa::Automaton::createMinimalMoore(fa);

  EXPECT_TRUE(n.isValid());
  EXPECT_TRUE(n.isLanguageEmpty());
  EXPECT_EQ(n.countStates(), 1u);
  EXPECT_EQ(n.countSymbols(), 2u);
  EXPECT_TRUE(n.hasSymbol('a'));
  EXPECT_TRUE(n.hasSymbol('b'));
  EXPECT_TRUE(n.isDeterministic());
  EXPECT_TRUE(n.isComplete());
  EXPECT_TRUE(equivalent(fa, n));
}

TEST(createMinimalMoore, NonAccessibleState){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  fa.setStateInitial(0);
  fa.setStateFinal(0);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton n = fa::Automaton::createMinimalMoore(fa);
  n.prettyPrint(std::cout);
  EXPECT_TRUE(n.isValid());
  EXPECT_FALSE(n.isLanguageEmpty());
  EXPECT_EQ(n.countStates(), 1u);
  EXPECT_EQ(n.countSymbols(), 1u);
  EXPECT_TRUE(n.hasSymbol('a'));
  EXPECT_TRUE(n.isDeterministic());
  EXPECT_TRUE(n.isComplete());
  EXPECT_TRUE(equivalent(fa, n));
}
/*
TEST(createMinimalMoore, aléatoire) {
  // Un automate avec un grand nombre d'états et de symboles accepte un langage complexe.
  fa::Automaton automaton;
  srand(time(NULL));
  for (int i = 0; i < 10; i++) {
    automaton.addState(i);
    for (char c = 'a'; c <= 'c'; c++) {
      automaton.addSymbol(c);
    }
  }

  // On génère les transitions de l'automate de manière aléatoire.
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < (int)'d'; j++) {
      int next_state = rand() % 10;
      automaton.addTransition(i, 'a' + j, next_state);
    }
  }

  // On choisit un état initial aléatoire.
  int initial_state = rand() % 10;
  automaton.setStateInitial(initial_state);

  // On choisit un état final aléatoire.
  int final_state = rand() % 10;
  automaton.setStateFinal(final_state);

  fa::Automaton minimal = fa::Automaton::createMinimalMoore(automaton);

  EXPECT_TRUE(minimal.isValid());
  EXPECT_EQ(minimal.countSymbols(), 3u);
  EXPECT_TRUE(minimal.hasSymbol('a'));
  EXPECT_TRUE(minimal.hasSymbol('b'));
  EXPECT_TRUE(minimal.hasSymbol('c'));
  EXPECT_TRUE(minimal.isDeterministic());
  EXPECT_TRUE(minimal.isComplete());

  int k = 0;
  for(std::string word : words){
    if(automaton.match(word)){
      ++k;
    }
  } 

  int j = 0;
  for(std::string word : words){
    if(minimal.match(word)){
      ++j;
    }
  }

  EXPECT_EQ(k, j);
}

TEST(createMinimalMoore, MultipleAléatoire) {
  for(int i = 0; i < N; ++i){
    // Un automate avec un grand nombre d'états et de symboles accepte un langage complexe.
    fa::Automaton automaton;
    srand(time(NULL));
    for (int i = 0; i < 10; i++) {
      automaton.addState(i);
      
    }

    for (char c = 'a'; c <= 'c'; c++) {
      automaton.addSymbol(c);
    }

    // On génère les transitions de l'automate de manière aléatoire.
    for (int i = 0; i < 10; i++) {
      for (int j = 0; j < (int)'d'; j++) {
        int next_state = rand() % 10;
        automaton.addTransition(i, 'a' + j, next_state);
      }
    }

    // On choisit un état initial aléatoire.
    int initial_state = rand() % 10;
    automaton.setStateInitial(initial_state);

    // On choisit un état final aléatoire.
    int final_state = rand() % 10;
    automaton.setStateFinal(final_state);

    //automaton.prettyPrint(std::cout);

    fa::Automaton minimal = fa::Automaton::createMinimalMoore(automaton);

    EXPECT_TRUE(minimal.isValid());
    EXPECT_EQ(minimal.countSymbols(), 3u);
    EXPECT_TRUE(minimal.hasSymbol('a'));
    EXPECT_TRUE(minimal.hasSymbol('b'));
    EXPECT_TRUE(minimal.hasSymbol('c'));
    if(minimal.countStates() == 1u){
      if(initial_state == final_state){
        EXPECT_TRUE(minimal.isDeterministic());
      }else{
        EXPECT_FALSE(minimal.isDeterministic());
      }
      
      EXPECT_FALSE(minimal.isComplete());
    }else{
      EXPECT_TRUE(minimal.isDeterministic());
      EXPECT_TRUE(minimal.isComplete());
    }      

    int k = 0;
    for(std::string word : words){
      if(automaton.match(word)){
        ++k;
      }
    } 

    int j = 0;
    for(std::string word : words){
      if(minimal.match(word)){
        ++j;
      }
    }

    EXPECT_EQ(k, j);
  }
}*/

TEST(createMinimalBrzozowski, exempleCours){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'b', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(4, 'b', 5));
  EXPECT_TRUE(fa.addTransition(5, 'a', 5));
  EXPECT_TRUE(fa.addTransition(5, 'b', 5));
  fa.setStateInitial(0);
  fa.setStateFinal(3);
  fa.setStateFinal(4);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton n = fa::Automaton::createMinimalBrzozowski(fa);

  EXPECT_TRUE(n.isValid());
  EXPECT_FALSE(n.isLanguageEmpty());
  EXPECT_EQ(n.countStates(), 4u);
  EXPECT_EQ(n.countSymbols(), 2u);
  EXPECT_TRUE(n.hasSymbol('a'));
  EXPECT_TRUE(n.hasSymbol('b'));
  EXPECT_TRUE(n.isDeterministic());
  EXPECT_TRUE(n.isComplete());
  EXPECT_TRUE(equivalent(fa, n));
}

TEST(createMinimalBrzozowski, noInitialState){
  fa::Automaton fa;
  
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'b', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(4, 'b', 5));
  EXPECT_TRUE(fa.addTransition(5, 'a', 5));
  EXPECT_TRUE(fa.addTransition(5, 'b', 5));
  fa.setStateFinal(3);
  fa.setStateFinal(4);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton n = fa::Automaton::createMinimalBrzozowski(fa);
  EXPECT_TRUE(n.isValid());
  EXPECT_TRUE(n.isLanguageEmpty());
  EXPECT_EQ(n.countStates(), 1u);
  EXPECT_EQ(n.countSymbols(), 2u);
  EXPECT_TRUE(n.hasSymbol('a'));
  EXPECT_TRUE(n.hasSymbol('b'));
  EXPECT_TRUE(n.isDeterministic());
  EXPECT_TRUE(n.isComplete());
  EXPECT_TRUE(equivalent(fa, n));
}

TEST(createMinimalBrzozowski, noFinalState){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'b', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(4, 'b', 5));
  EXPECT_TRUE(fa.addTransition(5, 'a', 5));
  EXPECT_TRUE(fa.addTransition(5, 'b', 5));
  fa.setStateInitial(0);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton n = fa::Automaton::createMinimalBrzozowski(fa);

  n.prettyPrint(std::cout);
  EXPECT_TRUE(n.isValid());
  EXPECT_TRUE(n.isLanguageEmpty());
  EXPECT_EQ(n.countStates(), 1u);
  EXPECT_EQ(n.countSymbols(), 2u);
  EXPECT_TRUE(n.hasSymbol('a'));
  EXPECT_TRUE(n.hasSymbol('b'));
  EXPECT_TRUE(n.isDeterministic());
  EXPECT_TRUE(n.isComplete());
  EXPECT_TRUE(equivalent(fa, n));
}

TEST(createMinimalBrzozowski, DS2022_Exo_Deter){
  fa::Automaton fa;
  
  for(int i = 0; i < 6; ++i){
    EXPECT_TRUE(fa.addState(i));
  }
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'a', 5));
  EXPECT_TRUE(fa.addTransition(0, 'b', 4));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 1));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'b', 3));
  EXPECT_TRUE(fa.addTransition(3, 'b', 0));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(4, 'a', 4));
  EXPECT_TRUE(fa.addTransition(5, 'a', 5));
  EXPECT_TRUE(fa.addTransition(5, 'b', 4));
  fa.setStateInitial(0);
  fa.setStateFinal(3);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton n = fa::Automaton::createMinimalBrzozowski(fa);
  n.prettyPrint(std::cout);
  EXPECT_TRUE(n.isValid());
  EXPECT_FALSE(n.isLanguageEmpty());
  EXPECT_EQ(n.countStates(), 15u);
  EXPECT_EQ(n.countSymbols(), 2u);
  EXPECT_TRUE(n.hasSymbol('a'));
  EXPECT_TRUE(n.hasSymbol('b'));
  EXPECT_TRUE(n.isDeterministic());
  EXPECT_TRUE(n.isComplete());
  EXPECT_TRUE(equivalent(fa, n));
}

TEST(createMinimalBrzozowski, NonAccessibleState){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  fa.setStateInitial(0);
  fa.setStateFinal(0);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton n = fa::Automaton::createMinimalBrzozowski(fa);
  n.prettyPrint(std::cout);
  EXPECT_TRUE(n.isValid());
  EXPECT_FALSE(n.isLanguageEmpty());
  EXPECT_EQ(n.countStates(), 1u);
  EXPECT_EQ(n.countSymbols(), 1u);
  EXPECT_TRUE(n.hasSymbol('a'));
  EXPECT_TRUE(n.isDeterministic());
  EXPECT_TRUE(n.isComplete());
  EXPECT_TRUE(equivalent(fa, n));
}

TEST(createMinimalBrzozowski, NonAccessibleStatesWithTransition){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  fa.setStateInitial(0);
  fa.setStateFinal(0);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton n = fa::Automaton::createMinimalBrzozowski(fa);
  n.prettyPrint(std::cout);
  EXPECT_TRUE(n.isValid());
  EXPECT_FALSE(n.isLanguageEmpty());
  EXPECT_EQ(n.countStates(), 1u);
  EXPECT_EQ(n.countSymbols(), 1u);
  EXPECT_TRUE(n.hasSymbol('a'));
  EXPECT_TRUE(n.isDeterministic());
  EXPECT_TRUE(n.isComplete());
  EXPECT_TRUE(equivalent(fa, n));
}

TEST(createMinimalBrzozowski, NonAccessibleStatesWithTransitionNonDeterminist){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  EXPECT_TRUE(fa.addTransition(0, 'b', 0));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  fa.setStateInitial(0);
  fa.setStateFinal(0);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton n = fa::Automaton::createMinimalBrzozowski(fa);
  n.prettyPrint(std::cout);
  EXPECT_TRUE(n.isValid());
  EXPECT_FALSE(n.isLanguageEmpty());
  EXPECT_EQ(n.countStates(), 1u);
  EXPECT_EQ(n.countSymbols(), 2u);
  EXPECT_TRUE(n.hasSymbol('a'));
  EXPECT_TRUE(n.hasSymbol('b'));
  EXPECT_TRUE(n.isDeterministic());
  EXPECT_TRUE(n.isComplete());
  EXPECT_TRUE(equivalent(fa, n));
}

#ifdef DEVELOPMENT

/*TEST(createMinimalBrzozowski, aléatoire) {
  // Un automate avec un grand nombre d'états et de symboles accepte un langage complexe.
  fa::Automaton automaton;
  srand(time(NULL));
  for (int i = 0; i < 10; i++) {
    automaton.addState(i);
    for (char c = 'a'; c <= 'c'; c++) {
      automaton.addSymbol(c);
    }
  }

  // On génère les transitions de l'automate de manière aléatoire.
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < (int)'d'; j++) {
      int next_state = rand() % 10;
      automaton.addTransition(i, 'a' + j, next_state);
    }
  }

  // On choisit un état initial aléatoire.
  int initial_state = rand() % 10;
  automaton.setStateInitial(initial_state);

  // On choisit un état final aléatoire.
  int final_state = rand() % 10;
  automaton.setStateFinal(final_state);

  //automaton.prettyPrint(std::cout);

  fa::Automaton minimal = fa::Automaton::createMinimalBrzozowski(automaton);

  EXPECT_TRUE(minimal.isValid());
  EXPECT_EQ(minimal.countSymbols(), 3u);
  EXPECT_TRUE(minimal.hasSymbol('a'));
  EXPECT_TRUE(minimal.hasSymbol('b'));
  EXPECT_TRUE(minimal.hasSymbol('c'));
  if(minimal.countStates() == 1u){
      if(initial_state == final_state){
        EXPECT_TRUE(minimal.isDeterministic());
      }else{
        EXPECT_FALSE(minimal.isDeterministic());
      }
      
      EXPECT_FALSE(minimal.isComplete());
    }else{
      EXPECT_TRUE(minimal.isDeterministic());
      EXPECT_TRUE(minimal.isComplete());
    }   
  

  int k = 0;
  for(std::string word : words){
    if(automaton.match(word)){
      ++k;
    }
  } 

  int j = 0;
  for(std::string word : words){
    if(minimal.match(word)){
      ++j;
    }
  }

  EXPECT_EQ(k, j);
}
*/
TEST(createMinimalBrzozowski, MultipleAléatoire) {
  for(int i = 0; i < N; ++i){
    // Un automate avec un grand nombre d'états et de symboles accepte un langage complexe.
    fa::Automaton automaton;
    srand(time(NULL));
    for (int i = 0; i < 10; i++) {
      automaton.addState(i);
      
    }

    for (char c = 'a'; c <= 'c'; c++) {
      automaton.addSymbol(c);
    }

    // On génère les transitions de l'automate de manière aléatoire.
    for (int i = 0; i < 10; i++) {
      for (int j = 0; j < (int)'d'; j++) {
        int next_state = rand() % 10;
        automaton.addTransition(i, 'a' + j, next_state);
      }
    }

    // On choisit un état initial aléatoire.
    int initial_state = rand() % 10;
    automaton.setStateInitial(initial_state);

    // On choisit un état final aléatoire.
    int final_state = rand() % 10;
    automaton.setStateFinal(final_state);

    //automaton.prettyPrint(std::cout);

    
    

    fa::Automaton minimal = fa::Automaton::createMinimalBrzozowski(automaton);

    

    words = {};
    std::size_t size = automaton.countStates();
    for(std::size_t i = 1; i < size + 1; ++i){
      std::string chaine;
      enumer_chaines_abc(chaine, i);
    }
    words.insert("");

    EXPECT_TRUE(minimal.isValid());
    EXPECT_EQ(minimal.countSymbols(), 3u);
    EXPECT_TRUE(minimal.hasSymbol('a'));
    EXPECT_TRUE(minimal.hasSymbol('b'));
    EXPECT_TRUE(minimal.hasSymbol('c'));  

    EXPECT_TRUE(minimal.isComplete());
    EXPECT_TRUE(minimal.isDeterministic()); 
    EXPECT_TRUE(equivalent(automaton, minimal));
    
  }
}

/*TEST(Aléatoire, CompareMooreBrzozowski) {
  for(int i = 0; i < N; ++i){
    // Un automate avec un grand nombre d'états et de symboles accepte un langage complexe.
    fa::Automaton automaton;
    srand(time(NULL));
    for (int i = 0; i < 10; i++) {
      automaton.addState(i);
      
    }

    for (char c = 'a'; c <= 'c'; c++) {
      automaton.addSymbol(c);
    }

    // On génère les transitions de l'automate de manière aléatoire.
    for (int i = 0; i < 10; i++) {
      for (int j = 0; j < (int)'d'; j++) {
        int next_state = rand() % 10;
        automaton.addTransition(i, 'a' + j, next_state);
      }
    }

    // On choisit un état initial aléatoire.
    int initial_state = rand() % 10;
    automaton.setStateInitial(initial_state);

    // On choisit un état final aléatoire.
    int final_state = rand() % 10;
    automaton.setStateFinal(final_state);

    //automaton.prettyPrint(std::cout);

    
    

    fa::Automaton minimalB = fa::Automaton::createMinimalBrzozowski(automaton);
    fa::Automaton minimalM = fa::Automaton::createMinimalMoore(automaton);

    EXPECT_TRUE(minimalB.isValid());
    EXPECT_EQ(minimalB.countSymbols(), 3u);
    EXPECT_TRUE(minimalB.hasSymbol('a'));
    EXPECT_TRUE(minimalB.hasSymbol('b'));
    EXPECT_TRUE(minimalB.hasSymbol('c'));
    if(minimalB.countStates() == 1u){
      if(initial_state == final_state){
        EXPECT_TRUE(minimalB.isDeterministic());
      }else{
        EXPECT_FALSE(minimalB.isDeterministic());
      }
      
      EXPECT_FALSE(minimalB.isComplete());
    }else{
      EXPECT_TRUE(minimalB.isDeterministic());
      EXPECT_TRUE(minimalB.isComplete());
    }     

    EXPECT_TRUE(minimalM.isValid());
    EXPECT_EQ(minimalM.countSymbols(), 3u);
    EXPECT_TRUE(minimalM.hasSymbol('a'));
    EXPECT_TRUE(minimalM.hasSymbol('b'));
    EXPECT_TRUE(minimalM.hasSymbol('c'));
    if(minimalM.countStates() == 1u){
      if(initial_state == final_state){
        EXPECT_TRUE(minimalM.isDeterministic());
      }else{
        EXPECT_FALSE(minimalM.isDeterministic());
      }
      
      EXPECT_FALSE(minimalM.isComplete());
    }else{
      EXPECT_TRUE(minimalM.isDeterministic());
      EXPECT_TRUE(minimalM.isComplete());
    }     

    int k = 0;
    for(std::string word : words){
      if(automaton.match(word)){
        ++k;
      }
    } 

    int j = 0;
    for(std::string word : words){
      if(minimalB.match(word)){
        ++j;
      }
    }

    int l = 0;
    for(std::string word : words){
      if(minimalM.match(word)){
        ++l;
      }
    }


    EXPECT_EQ(k, j);
    EXPECT_EQ(k, l);
    
  }
}
*/
#endif // DEVELOPMENT

#ifdef HOPCROFT

TEST(createMinimalHopcroft, exempleCours){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'b', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(4, 'b', 5));
  EXPECT_TRUE(fa.addTransition(5, 'a', 5));
  EXPECT_TRUE(fa.addTransition(5, 'b', 5));
  fa.setStateInitial(0);
  fa.setStateFinal(3);
  fa.setStateFinal(4);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton n = fa::Automaton::createMinimalHopcroft(fa);

  EXPECT_TRUE(n.isValid());
  EXPECT_FALSE(n.isLanguageEmpty());
  EXPECT_EQ(n.countStates(), 4u);
  EXPECT_EQ(n.countSymbols(), 2u);
  EXPECT_TRUE(n.hasSymbol('a'));
  EXPECT_TRUE(n.hasSymbol('b'));
  EXPECT_TRUE(n.isDeterministic());
  EXPECT_TRUE(n.isComplete());
  EXPECT_TRUE(equivalent(fa, n));
}

TEST(createMinimalHopcroft, alreadyCompleteAndAlreadyDeterministic){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'b', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(4, 'b', 5));
  EXPECT_TRUE(fa.addTransition(5, 'a', 5));
  EXPECT_TRUE(fa.addTransition(5, 'b', 5));
  fa.setStateInitial(0);
  fa.setStateFinal(3);
  fa.setStateFinal(4);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton n = fa::Automaton::createMinimalHopcroft(fa);

  EXPECT_TRUE(n.isValid());
  EXPECT_FALSE(n.isLanguageEmpty());
  EXPECT_EQ(n.countStates(), 4u);
  EXPECT_EQ(n.countSymbols(), 2u);
  EXPECT_TRUE(n.hasSymbol('a'));
  EXPECT_TRUE(n.hasSymbol('b'));
  EXPECT_TRUE(n.isDeterministic());
  EXPECT_TRUE(n.isComplete());
  EXPECT_TRUE(equivalent(fa, n));
}

TEST(createMinimalHopcroft, alreadyMinimal_ex18_1TD){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addSymbol('c'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(0, 'b', 4));
  EXPECT_TRUE(fa.addTransition(0, 'c', 5));
  EXPECT_TRUE(fa.addTransition(1, 'b', 0));
  EXPECT_TRUE(fa.addTransition(2, 'c', 0));
  EXPECT_TRUE(fa.addTransition(3, 'b', 4));
  EXPECT_TRUE(fa.addTransition(3, 'c', 5));
  EXPECT_TRUE(fa.addTransition(4, 'b', 3));
  EXPECT_TRUE(fa.addTransition(5, 'a', 3));
  fa.setStateInitial(0);
  fa.setStateFinal(0);
  fa.setStateFinal(3);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton minim = fa::Automaton::createMinimalHopcroft(fa);

  EXPECT_TRUE(minim.isValid());
  EXPECT_FALSE(minim.isLanguageEmpty());
  EXPECT_EQ(minim.countStates(), 7u);
  EXPECT_EQ(minim.countSymbols(), 3u);
  EXPECT_TRUE(minim.hasSymbol('a'));
  EXPECT_TRUE(minim.hasSymbol('b'));
  EXPECT_TRUE(minim.hasSymbol('c'));
  EXPECT_TRUE(minim.isDeterministic());
  EXPECT_TRUE(minim.isComplete());
  EXPECT_TRUE(equivalent(fa, minim));
}

TEST(createMinimalHopcroft, noInitialState){
  fa::Automaton fa;
  
  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addState(2));
  EXPECT_TRUE(fa.addState(3));
  EXPECT_TRUE(fa.addState(4));
  EXPECT_TRUE(fa.addState(5));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addSymbol('b'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 1));
  EXPECT_TRUE(fa.addTransition(0, 'b', 2));
  EXPECT_TRUE(fa.addTransition(1, 'a', 2));
  EXPECT_TRUE(fa.addTransition(1, 'b', 3));
  EXPECT_TRUE(fa.addTransition(2, 'a', 1));
  EXPECT_TRUE(fa.addTransition(2, 'b', 4));
  EXPECT_TRUE(fa.addTransition(3, 'a', 4));
  EXPECT_TRUE(fa.addTransition(3, 'b', 5));
  EXPECT_TRUE(fa.addTransition(4, 'a', 3));
  EXPECT_TRUE(fa.addTransition(4, 'b', 5));
  EXPECT_TRUE(fa.addTransition(5, 'a', 5));
  EXPECT_TRUE(fa.addTransition(5, 'b', 5));
  fa.setStateFinal(3);
  fa.setStateFinal(4);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton n = fa::Automaton::createMinimalHopcroft(fa);

  EXPECT_TRUE(n.isValid());
  EXPECT_TRUE(n.isLanguageEmpty());
  EXPECT_EQ(n.countStates(), 1u);
  EXPECT_EQ(n.countSymbols(), 2u);
  EXPECT_TRUE(n.hasSymbol('a'));
  EXPECT_TRUE(n.hasSymbol('b'));
  EXPECT_TRUE(n.isDeterministic());
  EXPECT_TRUE(n.isComplete());
  EXPECT_TRUE(equivalent(fa, n));
}

TEST(createMinimalHopcroft, NonAccessibleState){
  fa::Automaton fa;

  EXPECT_TRUE(fa.addState(0));
  EXPECT_TRUE(fa.addState(1));
  EXPECT_TRUE(fa.addSymbol('a'));
  EXPECT_TRUE(fa.addTransition(0, 'a', 0));
  fa.setStateInitial(0);
  fa.setStateFinal(0);

  std::size_t size = fa.countStates();
  words = {};
  for(std::size_t i = 1; i < size + 1; ++i){
    std::string chaine;
    enumer_chaines(chaine, i);
  }
  words.insert("");

  fa::Automaton n = fa::Automaton::createMinimalHopcroft(fa);
  n.prettyPrint(std::cout);
  EXPECT_TRUE(n.isValid());
  EXPECT_FALSE(n.isLanguageEmpty());
  EXPECT_EQ(n.countStates(), 1u);
  EXPECT_EQ(n.countSymbols(), 1u);
  EXPECT_TRUE(n.hasSymbol('a'));
  EXPECT_TRUE(n.isDeterministic());
  EXPECT_TRUE(n.isComplete());
  EXPECT_TRUE(equivalent(fa, n));
}

#endif // HOPCROFT

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
