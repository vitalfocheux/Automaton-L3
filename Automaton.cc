#include "Automaton.h"

namespace fa {

  Automaton::Automaton() {
  }

  bool Automaton::isValid() const {
    return !this->state.empty() && !this->alphabet.empty();
  }

  bool Automaton::addSymbol(char symbol){
    if(isgraph(symbol)&& !hasSymbol(symbol)){
      this->alphabet.push_back(symbol);
      return true;
    }
    return false;
  }

  bool Automaton::removeSymbol(char symbol){
    if(isgraph(symbol) && !this->alphabet.empty() && hasSymbol(symbol)){
      auto stateEntryIter = this->transition.begin();
      while (stateEntryIter != this->transition.end()) {
        auto& letterEntry = stateEntryIter->second;
        auto letterEntryIter = letterEntry.begin();
        while (letterEntryIter != letterEntry.end()) {
          if(letterEntryIter->first == symbol){
            //remove dans transition si il est une transition
            letterEntryIter = this->transition.at(stateEntryIter->first).erase(letterEntryIter);
            break;
          }
          ++letterEntryIter;
        }
        ++stateEntryIter;
      }
      for(std::size_t i = 0; i < this->alphabet.size(); ++i){
        if(this->alphabet.at(i) == symbol){
          this->alphabet.erase(this->alphabet.begin() + i); 
          return true;
        }
      }
    }
    return false;
  }

  bool Automaton::hasSymbol(char symbol) const{
    if(isgraph(symbol) && !this->alphabet.empty()){
      std::size_t size = this->alphabet.size();
      for(std::size_t i = 0; i < size; ++i){
        if(this->alphabet.at(i) == symbol){
          return true;
        }
      }
    }
    return false;
  }

  std::size_t Automaton::countSymbols() const{
    return this->alphabet.size();
  }

  bool Automaton::addState(int state){
    if(!hasState(state) && state >= 0){
      this->state.insert(std::make_pair(state, 0));
      return true;
    }
    return false;
  }

  bool Automaton::removeState(int state){
    if(hasState(state)){
        auto stateEntryIter = this->transition.begin();
        while (stateEntryIter != this->transition.end()) {
            auto& letterEntry = stateEntryIter->second;
            auto letterEntryIter = letterEntry.begin();
            while (letterEntryIter != letterEntry.end()) {
                auto& values = letterEntryIter->second;
                auto it = std::find(values.begin(), values.end(), state);
                if (it != values.end()) {
                    values.erase(it);
                    if (values.empty()) {
                        letterEntryIter = letterEntry.erase(letterEntryIter);
                        continue;
                    }
                }
                ++letterEntryIter;
            }

            if (stateEntryIter->first == state) {
                stateEntryIter = this->transition.erase(stateEntryIter);
            } else {
                ++stateEntryIter;
            }
        }

        this->state.erase(state);
        return true;
    }
    return false;
}


  bool Automaton::hasState(int state) const{
    if(!this->state.empty()){
      return this->state.find(state) != this->state.end();
    }
    return false;
  }

  std::size_t Automaton::countStates() const{
    return this->state.size();
  }

  void Automaton::setStateInitial(int state){
    if(hasState(state)){
      for(auto& pair : this->state){
        if(pair.first == state){
          if(pair.second == FINAL_STATE){
            pair.second = FINAL_AND_INITIAL_STATE;
          }else{
            pair.second = INITIAL_STATE;
          }
        }
      }
    }
  }

  bool Automaton::isStateInitial(int state) const{
    if(hasState(state)){
      for(auto& pair : this->state){
        if(pair.first == state){
          return pair.second == INITIAL_STATE || pair.second == FINAL_AND_INITIAL_STATE;
        }
      }
    }
    return false;
  }

  void Automaton::setStateFinal(int state){
    if(hasState(state)){
      for(auto& pair : this->state){
        if(pair.first == state){
          if(pair.second == INITIAL_STATE){
            pair.second = FINAL_AND_INITIAL_STATE;
          }else{
            pair.second = FINAL_STATE;
          }
        }
      }
    }
  }

  bool Automaton::isStateFinal(int state) const{
    if(hasState(state)){
      for(auto& pair : this->state){
        if(pair.first == state){
          return pair.second == FINAL_STATE || pair.second == FINAL_AND_INITIAL_STATE;
        }
      }
    }
    return false;
  }

  bool Automaton::addTransition(int from, char alpha, int to){
    if(!hasTransition(from, alpha, to) && (hasSymbol(alpha) || alpha == fa::Epsilon) && hasState(from) && hasState(to)){
      this->transition[from][alpha].push_back(to);
      return true;
    }
    return false;
  }

  bool Automaton::removeTransition(int from, char alpha, int to) {
    if(hasTransition(from, alpha, to) && (hasSymbol(alpha) || alpha == fa::Epsilon) && hasState(from) && hasState(to)){
      if(alpha == fa::Epsilon){
        this->transition[from].erase(alpha);
        return true;
      }
      std::size_t size = this->state.size();
      std::vector<int> tab = this->transition[from][alpha];
      for(std::size_t i = 0; i < size; ++i){
        if(tab[i] == to){
          this->transition[from][alpha].erase(this->transition[from][alpha].begin() + i);
          return true;
        }
      }
    }
    return false; // La transition n'a pas été trouvée
}


  bool Automaton::hasTransition(int from, char alpha, int to) const{
    if(!this->transition.empty()){
      auto fromIt = transition.find(from);
      if (fromIt != transition.end()) {
          const std::map<char, std::vector<int>>& alphaMap = fromIt->second;
          auto alphaIt = alphaMap.find(alpha);
          if (alphaIt != alphaMap.end()) {
              const std::vector<int>& toVector = alphaIt->second;
              return std::find(toVector.begin(), toVector.end(), to) != toVector.end();
          }
      }
    } 
    return false;
  }

  std::size_t Automaton::countTransitions() const{
    std::size_t res = 0;
    for(auto stateEntry : this->transition){
      for(auto letterEntry : stateEntry.second){
        res += letterEntry.second.size();
      }
    }
    return res;
  }

  void Automaton::prettyPrint(std::ostream& os) const{
    os << "Initial states:\n\t";
    for(auto& pair : this->state){
      if(pair.second == INITIAL_STATE || pair.second == FINAL_AND_INITIAL_STATE){
        os << pair.first << " ";
      }
    }
    os << "\nFinal states:\n\t";
    for(auto& pair : this->state){
      if(pair.second == FINAL_STATE || pair.second == FINAL_AND_INITIAL_STATE){
        os << pair.first << " ";
      }
    }
    os << "\nTransitions:" << std::endl;
    for(auto stateEntry : this->state){
      os << "\tFor state " << stateEntry.first << ":" << std::endl;
      for(char letter : this->alphabet){
        os << "\t\tFor letter " << letter << ": ";
        if(this->transition.find(stateEntry.first) != this->transition.end() && this->transition.at(stateEntry.first).find(letter) != this->transition.at(stateEntry.first).end()){
          for(auto& value : this->transition.at(stateEntry.first).at(letter)){
            os << value << " ";
          }
          
        }
        os << std::endl;
      }
    }
    /*
    for(auto& stateEntry : this->transition){
      os << "\tFor state " << stateEntry.first << ":" << std::endl;
      for(char letter : this->alphabet){
        os << "\t\tFor letter " << letter << ": ";
        if(this->transition.at(stateEntry.first).find(letter) != this->transition.at(stateEntry.first).end()){
          for(auto& value : this->transition.at(stateEntry.first).at(letter)){
            os << value << " ";
          }
          
        }
        os << std::endl;
      }
    }
    */
  }

  void Automaton::dotPrint(std::ostream& os) const{
    std::ofstream fichier("input.dot");

    if (fichier.is_open()) {
        std::streambuf *old_cout = os.rdbuf();  // Sauvegarde le buffer de std::cout
        os.rdbuf(fichier.rdbuf());  // Redirige std::cout vers le fichier

        // À partir d'ici, tout ce qui serait normalement affiché sur std::cout
        // sera enregistré dans le fichier "output.dot"

        bool hasFinal = false;

        os << "digraph finite_state_machine {" << std::endl;
        os << "\tfontname=\"Helvetica,Arial,sans-serif\"" << std::endl;
        os << "\tnode [fontname=\"Helvetica,Arial,sans-serif\"]" << std::endl;
        os << "\tedge [fontname=\"Helvetica,Arial,sans-serif\"]" << std::endl;
        os << "\trankdir=LR;" << std::endl;
        os << "\tnode [shape = doublecircle, color=blue, style=filled, fillcolor=white]; ";
        for(auto stateEntry : this->state){
          if(isStateFinal(stateEntry.first)/*stateEntry.second == FINAL_STATE || stateEntry.second == FINAL_AND_INITIAL_STATE*/){
            hasFinal = true;
            os << stateEntry.first << " ";
          }
        }
        if(hasFinal){
          os << ";";
        }
        os << "\n\tnode [shape = circle];" << std::endl;
        for(auto& stateEntry : this->transition){
          for(char letter : this->alphabet){
            if(this->transition.at(stateEntry.first).find(letter) != this->transition.at(stateEntry.first).end()){
              for(auto& value : this->transition.at(stateEntry.first).at(letter)){
                os << "\t" << stateEntry.first << "->" << value << "[label = \"" << letter << "\"];" << std::endl;
              }
              
            }
          }
        }
        os << "}";

        os.rdbuf(old_cout);  // Restaure le buffer original de std::cout
        fichier.close();
    } else {
        std::cerr << "Erreur lors de l'ouverture du fichier." << std::endl;
    }
  }


  bool Automaton::hasEpsilonTransition() const{
    assert(isValid());
    for(auto stateEntry : this->transition){
      for(auto letterEntry : stateEntry.second){
        if(letterEntry.first == fa::Epsilon){
          return true;
        }
      }
    }
    return false;
  }

  bool Automaton::isDeterministic() const{
    assert(isValid());
    bool hasInitial = false;
    for(auto stateEntry : this->state){
      if(isStateInitial(stateEntry.first)/*stateEntry.second == INITIAL_STATE || stateEntry.second == FINAL_AND_INITIAL_STATE*/){
        if(hasInitial){
          return false;
        }
        hasInitial = true;
      }
    }
    if(!hasInitial){
      return false;
    }
    for(auto stateEntry : this->transition){
      for(auto letterEntry : stateEntry.second){
        if(letterEntry.second.size() > 1){
          return false;
        }
      }
    }
    return true;
  }

  bool Automaton::isComplete() const{
    assert(isValid());

    for(auto stateEntry : this->state){
      if(this->transition.find(stateEntry.first) == this->transition.end()){
        return false;
      }
      std::size_t size = 0;
      for(char letter : this->alphabet){
        if(this->transition.at(stateEntry.first).find(letter) != this->transition.at(stateEntry.first).end() && this->transition.at(stateEntry.first).at(letter).size() > 0){
          ++size;
        }
      }
      if(size != this->alphabet.size()){
        return false;
      }
    }
    return true;
  }

  Automaton Automaton::createComplete(const Automaton& automaton){
    assert(automaton.isValid());
    if(automaton.isLanguageEmpty()){
      fa::Automaton res;
      res.alphabet = automaton.alphabet;
      res.addState(0);
      res.setStateInitial(0);
      for(char c : automaton.alphabet){
        res.addTransition(0, c, 0);
      }
      return res;
    }
    if(automaton.isComplete()){
      return automaton;
    }
    fa::Automaton fa = automaton;
    std::size_t st = 0;
    std::size_t size = fa.state.size();
    for(std::size_t i = 0; i < size; ++i){
      st = i;
      if(!fa.hasState(st)/*fa.state.find(i) == fa.state.end()*/){
        break;
      }
    }
    if(st == size-1){
      if(size-1 == 0 && !fa.hasState(0)){
        st = 0;
      }else{
        st = size;
      }
    }
    
    fa.addState(st);
    for(char letter : fa.alphabet){
      fa.addTransition(st, letter, st);
    }
    for(auto stateEntry : fa.state){
        for(char letter : fa.alphabet){
          if(fa.transition.find(stateEntry.first) == fa.transition.end()){
            fa.addTransition(stateEntry.first, letter, st);
          }else if(fa.transition.at(stateEntry.first).find(letter) == fa.transition.at(stateEntry.first).end()){
            fa.addTransition(stateEntry.first, letter, st);
          }else if(fa.transition.at(stateEntry.first).at(letter).size() == 0){
            fa.addTransition(stateEntry.first, letter, st);
          }
        }
    }
    return fa;
  }

  Automaton Automaton::createComplement(const Automaton& automaton){
    assert(automaton.isValid());
    fa::Automaton fa = Automaton::createDeterministic(automaton);
    fa = Automaton::createComplete(fa);
    for(auto stateEntry : fa.state){
      if(!fa.isStateFinal(stateEntry.first)){
        fa.setStateFinal(stateEntry.first);
      }else if(stateEntry.second == FINAL_STATE){
        fa.state.at(stateEntry.first) = 0;
      }else if(stateEntry.second == FINAL_AND_INITIAL_STATE){
        fa.state.at(stateEntry.first) = INITIAL_STATE;
      }
    }
    return fa;
  }

  Automaton Automaton::createMirror(const Automaton& automaton){
    assert(automaton.isValid());
    fa::Automaton fa;
    fa.alphabet = automaton.alphabet;
    fa.state = automaton.state;
    for(auto stateEntry : automaton.state){
      if(stateEntry.second == INITIAL_STATE){
        fa.state.at(stateEntry.first) = FINAL_STATE;
      }
      if(stateEntry.second == FINAL_STATE){
        fa.state.at(stateEntry.first) = INITIAL_STATE;
      }
    }
    for(auto stateEntry : automaton.transition){
      for(auto letterEntry : stateEntry.second){
        for(int value : letterEntry.second){
          fa.addTransition(value, letterEntry.first, stateEntry.first);
        }
      }
    }
    return fa;
  }

  std::set<int> Automaton::makeTransition(const std::set<int>& origin, char alpha) const{
    assert(this->isValid());
    std::set<int> res;
    for(int stateOrigin : origin){
      for(auto stateEntry : this->state){
        if(this->hasTransition(stateOrigin, alpha, stateEntry.first)){
          res.insert(stateEntry.first);
        }
      }
    }
    return res;
  }

  std::set<int> Automaton::readString(const std::string& word) const{
    assert(this->isValid());
    std::set<int> res;
    if(word.empty()){
      for(auto stateEntry : this->state){
        if(this->isStateInitial(stateEntry.first) /*stateEntry.second == FINAL_AND_INITIAL_STATE || stateEntry.second == INITIAL_STATE*/){
          res.insert(stateEntry.first);
        }
      }
      return res;
    }
    for(char letter : word){
      if(!this->hasSymbol(letter)){
        return {};
      }
    }
    std::set<int> initial;
    for(auto stateEntry : this->state){
      if(this->isStateInitial(stateEntry.first) /*stateEntry.second == INITIAL_STATE || stateEntry.second == FINAL_AND_INITIAL_STATE*/){
        initial.insert(stateEntry.first);
      }
    }
    if(initial.empty()){
      return {};
    }

    for(int init :  initial){
      std::set<int> initTransitions = makeTransition({init}, word.at(0));
      if(initTransitions.empty()){
        continue;
      }
      if(word.length() == 1){
        for(int value : initTransitions){
          res.insert(value);
        }
        continue;;
      }
      std::set<int> transitions = makeTransition(initTransitions, word.at(1));
      if(word.length() == 2){
        for(int value : transitions){
          res.insert(value);
        }
        continue;;
      }
      for(std::size_t i = 2; i < word.length(); ++i){
        transitions = makeTransition(transitions, word.at(i));
      }
      for(int value : transitions){
        res.insert(value);
      }

    }
    return res;
  }
  

  bool Automaton::match(const std::string& word) const{
    assert(this->isValid());
    std::set<int> res = readString(word);
    for(int value : res){
      for(auto stateEntry : this->state){
        if(value == stateEntry.first && this->isStateFinal(stateEntry.first)){
          return true;
        }
      }
    }
    return false;
  }

  void visitedGraph(const Automaton* fa, std::vector<int>& visited, int state){
    visited.push_back(state);
    for(auto stateEntry : fa->state){
      for(char letter : fa->alphabet){
        if(fa->hasTransition(state, letter, stateEntry.first)){
          if(std::find(visited.begin(), visited.end(), stateEntry.first) == visited.end()){
            visitedGraph(fa, visited, stateEntry.first);
          }
        }
      }
    }
  }

  bool Automaton::isLanguageEmpty() const{
    assert(this->isValid());
    std::vector<int> visited;

    bool hasInitial = false;

    for(auto stateEntry : this->state){
      if(this->isStateInitial(stateEntry.first)){
        hasInitial = true;
        break;
      }
    }

    if(!hasInitial){
      return true;
    }

    for(auto stateEntry : this->state){
      if(isStateInitial(stateEntry.first)){
        //visited.push_back(stateEntry.first);
        visitedGraph(this, visited, stateEntry.first);
        for(int state : visited){
          if(this->isStateFinal(state)){
            return false;
          }
        }
      }
    }
    return true;
  }

  void Automaton::removeNonAccessibleStates(){
    assert(this->isValid());
    std::vector<int> visited;
    for(auto stateEntry : this->state){
      if(this->isStateInitial(stateEntry.first) /*stateEntry.second == INITIAL_STATE || stateEntry.second == FINAL_AND_INITIAL_STATE*/){
        visited.push_back(stateEntry.first);
        if(this->transition.find(stateEntry.first) != this->transition.end()){
          visitedGraph(this, visited, stateEntry.first);
        }
      }
    }
    std::size_t size = this->state.size();
    for(std::size_t  i = 0; i < size; ++i){
      for(auto stateEntry : this->state){
        if(std::find(visited.begin(), visited.end(), stateEntry.first) == visited.end()){
          this->removeState(stateEntry.first);
          break;
        }
      }
    }
    if(!this->isValid()){
      this->addState(0);
      this->setStateInitial(0);
    }
  }

  /*void visitedGraphFinal(Automaton* fa, std::vector<int>& visited, int state, int origin){
    for(auto stateEntry : fa->state){
      for(char letter : fa->alphabet){
        if(fa->hasTransition(state, letter, stateEntry.first) && fa->isStateFinal(stateEntry.first) ){
          visited.push_back(origin);
          visitedGraphFinal(fa, visited, stateEntry.first, origin);
        }
      }
    }
  }*/

  void Automaton::removeNonCoAccessibleStates(){
    assert(this->isValid());

    fa::Automaton res;
    res.alphabet = this->alphabet;
    res.state = this->state;
    res.transition = this->transition;

    res = createMirror(res);
    res.removeNonAccessibleStates();
    if(!res.isValid()){
      res.addState(0);
      for(char c : res.alphabet){
        res.addTransition(0, c, 0);
        this->state = res.state;
        this->transition = res.transition;
      }
    }
    res = createMirror(res);
    this->state = res.state;
    this->transition = res.transition;



    /*std::vector<int> visited;
    for(auto stateEntry : this->state){
      if(this->isStateFinal(stateEntry.first)){
        visited.push_back(stateEntry.first);
      }
      if(this->transition.find(stateEntry.first) != this->transition.end()){
        visitedGraphFinal(this, visited, stateEntry.first, stateEntry.first);
      }
    }
    std::size_t size = this->state.size();
    for(std::size_t i = 0; i < size; ++i){
      for(auto stateEntry : this->state){
        if(std::find(visited.begin(), visited.end(), stateEntry.first) == visited.end()){
          this->removeState(stateEntry.first);
          break;
        }
      }
    }
     if(!this->isValid()){
      this->addState(0);
    }*/
  }

  /*Automaton Automaton::createIntersection(const Automaton& lhs, const Automaton& rhs){
    assert(lhs.isValid());
    assert(rhs.isValid());
    Automaton res;
    std::set<std::vector<int>> syncInit, syncFinal, syncState;
    std::set<int> indices;
    std::set<char> alphabet;

    for(char letter : lhs.alphabet){
      alphabet.insert(letter);
    }
    for(char letter : rhs.alphabet){
      alphabet.insert(letter);
    }

    int i = 0;

    for(auto lhsStateEntry : lhs.state){
      for(auto rhsStateEntry : rhs.state){
        if((lhsStateEntry.second == INITIAL_STATE || lhsStateEntry.second == FINAL_AND_INITIAL_STATE) && (rhsStateEntry.second == INITIAL_STATE || rhsStateEntry.second == FINAL_AND_INITIAL_STATE)){
          syncInit.insert({lhsStateEntry.first, rhsStateEntry.first, i});
          syncState.insert({lhsStateEntry.first, rhsStateEntry.first, i});
          indices.insert(i);
          ++i;
        }
        if((lhsStateEntry.second == FINAL_STATE || lhsStateEntry.second == FINAL_AND_INITIAL_STATE) && (rhsStateEntry.second == FINAL_STATE || rhsStateEntry.second == FINAL_AND_INITIAL_STATE)){
          syncFinal.insert({lhsStateEntry.first, rhsStateEntry.first});
        }
      }
    }

    printf("SYNCSTATE START = {\n");
    for(auto stateEntry : syncState){
      printf("{");
      for(int v : stateEntry){
        printf("%d ",v);
      }
      printf("}\n");
    }
    printf("}\n");

    for(auto stateEntry : syncState){
      for(char lhsLetter : lhs.alphabet){
        for(char rhsLetter : rhs.alphabet){
          if(lhsLetter == rhsLetter){
            char letter = rhsLetter;
            int indice = stateEntry.at(2);
            res.addSymbol(letter);
            res.addState(indice);
            if(syncInit.find({stateEntry.at(0), stateEntry.at(1), indice}) != syncInit.end()){
              res.setStateInitial(indice);
            }
            if(syncFinal.find({stateEntry.at(0), stateEntry.at(1)}) != syncFinal.end()){
              res.setStateFinal(indice);
            }
            std::cout << "FROM {" << stateEntry.at(0) << ", " << stateEntry.at(1) << "}\n";
            std::set<int> lhsMT = lhs.makeTransition({stateEntry.at(0)}, letter);
            std::set<int> rhsMT = rhs.makeTransition({stateEntry.at(1)}, letter);
            int size = indices.size();

            std::cout << "SYNCSTATE \n";
            for(auto syncStateEntry : syncState){
              std::cout << "{";
              for(int v : syncStateEntry){
                std::cout << v << " ";
              }
              std::cout << "}\n";
            }

            for(int v_lhs : lhsMT){
              for(int v_rhs : rhsMT){
                std::cout << "TO {" << v_lhs << ", " << v_rhs << "} " << size << "\n";
                size = indices.size();
                for(int j : indices){
                  if(syncState.find({v_lhs, v_rhs, j}) != syncState.end()){
                    
                    size = j;
                    std::cout << "new size of {" << v_lhs << ", " << v_rhs << "} " << size << std::endl;
                  }
                }
                std::cout << "indice " << indice << " letter " << letter << " to " << size <<"\n\n";
                std::cout << "SIZE " << size << std::endl;
                res.addState(size);
                if(res.addTransition(indice, letter, size)){
                  std::cout << "ADD TR {" << indice << ", " << letter << ", "<< size << "}\n\n";
                  
                  syncState.insert({v_lhs, v_rhs, size});
                  indices.insert(size);
                }
              }
            }
          }
        }
      }
    }

    printf("SYNCSTATE FINAL = {\n");
    for(auto syncStateEntry : syncState){
      printf("{");
      for(int v : syncStateEntry){
        printf("%d ", v);
      }
      printf("}\n");
    }
    printf("}\n");

    if(!res.isValid()){
      res.addSymbol('a');
      res.addState(0);
    }
    return res;
  }*/

  Automaton Automaton::createIntersection(const Automaton& lhs, const Automaton& rhs){
    assert(lhs.isValid());
    assert(rhs.isValid());

    fa::Automaton Armanoïde;
    Armanoïde.alphabet = lhs.alphabet;

    if(lhs.countStates() > 25 || rhs.countStates() > 25){
      printf("Automate avec trop d'états, lhs %ld états et rhs %ld états\n", lhs.countStates(), rhs.countStates());
      Armanoïde.addState(0);
      return createComplete(Armanoïde);
    }

    std::set<std::map<int, std::pair<int, int>>> syncState;
    std::set<int> indices;

    int i = 0;

    for(auto lhsEntry : lhs.state){
      for(auto rhsEntry : rhs.state){
        if(lhs.isStateInitial(lhsEntry.first) && rhs.isStateInitial(rhsEntry.first)){
          std::map<int, std::pair<int, int>> state;
          state[i] = std::make_pair(lhsEntry.first, rhsEntry.first);
          syncState.insert(state);
          indices.insert(i);
          ++i;
          
        }else if(lhs.isStateFinal(lhsEntry.first) && rhs.isStateFinal(rhsEntry.first)){
          std::map<int, std::pair<int, int>> state;
          state[i] = std::make_pair(lhsEntry.first, rhsEntry.first);
          syncState.insert(state);
          indices.insert(i);
          ++i;

        }
      }
    }

    

    for(auto stateEntry : syncState){
      for(char c : Armanoïde.alphabet){
        int indice = stateEntry.begin()->first;
        Armanoïde.addState(indice);
        indices.insert(indice);
        int lhsState = stateEntry.begin()->second.first;
        int rhsState = stateEntry.begin()->second.second;
        if(lhs.isStateInitial(lhsState) && rhs.isStateInitial(rhsState)){
          Armanoïde.setStateInitial(indice);
        }
        if(lhs.isStateFinal(lhsState) && rhs.isStateFinal(rhsState)){
          Armanoïde.setStateFinal(indice);
        }

        /*printf("SYNCSTATE = {\n");
        for(auto state : syncState){
          printf("{");
          for(auto v : state){
            printf("%d (%d, %d)", v.first, v.second.first, v.second.second);
          }
          printf("}\n");
        }
        printf("}\n");*/

        std::set<int> lhsTransitions = lhs.makeTransition({lhsState}, c);
        std::set<int> rhsTransitions = rhs.makeTransition({rhsState}, c);

        //printf("FROM = {%d, (%d, %d) %c}\n", indice, lhsState, rhsState,c);

        for(int newLhsState : lhsTransitions){
          for(int newRhsState : rhsTransitions){
            int size = indices.size();
            for(int j : indices){
              //printf("AT %d (%d, %d)\n", j, newLhsState, newRhsState);
             // printf("AT J = (%d, %d)\n", syncState[j].first, syncState[j].second);
              std::map<int, std::pair<int, int>> state;
              state[j] = std::make_pair(newLhsState, newRhsState);
              if(syncState.find(state) != syncState.end()){
                size = j;
                //printf("BREAK\n");
                break;
              }
            }
            //printf("TO = {%d, (%d, %d)}\n", size, newLhsState, newRhsState);
            Armanoïde.addState(size);
            if(Armanoïde.addTransition(indice, c, size)){
              //std::cout << "ADD TR : (" << indice << ", " << c << ", " << size << ")" << std::endl;
              indices.insert(size);
              std::map<int, std::pair<int, int>> state;
              state[size] = std::make_pair(newLhsState, newRhsState);
              syncState.insert(state);
            }
          }
        }
      }
    }

    /*printf("SYNCSTATE = {\n");
    for(auto state : syncState){
      printf("{");
      for(auto v : state){
        printf("%d (%d, %d)", v.first, v.second.first, v.second.second);
      }
      printf("}\n");
    }
    printf("}\n");*/

    if(!Armanoïde.isValid()){
      fa::Automaton Cobra;
      Cobra.addState(0);
      Cobra.setStateInitial(0);
      Cobra.alphabet = Armanoïde.alphabet;
      return createComplete(Cobra);
    }

    return Armanoïde;
  }

  bool Automaton::hasEmptyIntersectionWith(const Automaton& other) const{
    assert(this->isValid());
    assert(other.isValid());

    fa::Automaton fa = Automaton::createIntersection(*this, other);
    return fa.isLanguageEmpty();
  }

  Automaton Automaton::createDeterministic(const Automaton& other){
    assert(other.isValid());
    fa::Automaton res;

    if(other.isDeterministic()){
      res.state = other.state;
      res.alphabet = other.alphabet;
      res.transition = other.transition;
      res.removeNonAccessibleStates();
      return res;
    }

    bool hasInitialState = false;

    for(auto stateEntry : other.state){
      if(other.isStateInitial(stateEntry.first)){
        hasInitialState = true;
        break;
      }
    }

    if(!hasInitialState){
      res.addState(0);
      res.setStateInitial(0);
      res.alphabet = other.alphabet;
      return res;
    }

    

    std::set<std::vector<int>> deter;
    std::vector<int> deterInit;
    std::set<int> indices;

    deterInit.push_back(0);
    indices.insert(0);

    for(auto stateEntry : other.state){
      if(other.isStateInitial(stateEntry.first)){
        deterInit.push_back(stateEntry.first);
      }
    }
    //deterInit.push_back(0);
    //indices.insert(0);

    if(deterInit.size() == 1){
      deterInit.pop_back();
    }

    if(deterInit.empty()){
      return res;
    }

    deter.insert(deterInit);

    

    for(auto deterStateEntry : deter){
      for(char letter : other.alphabet){
        /*std::cout << "FROM {";
        for(int v : deterStateEntry){
          std::cout << v << " ";
        }
        std::cout << "}\n";*/
        //int indice = deterStateEntry.at(0);
        //std::cout << "INDICE :" << indice << std::endl;
        int deterStateEntrySize = deterStateEntry.size();
        for(int i = 1; i < deterStateEntrySize; ++i){
        //for(int val : deterStateEntry){
          for(auto stateEntry : other.state){
            //std::cout << "STATEENTRYFORST: " << stateEntry.first << std::endl;
            if(/*stateEntry.first != indice && */stateEntry.first == /*val*/deterStateEntry.at(i) && other.isStateFinal(stateEntry.first) /*(stateEntry.second == FINAL_STATE || stateEntry.second == FINAL_AND_INITIAL_STATE)*/){
              //res.setStateFinal(deterStateEntry.back());
              //std::cout << "SET FINAL : " << deterStateEntry.at(0) << std::endl;
              res.addState(deterStateEntry.at(0));
              res.setStateFinal(deterStateEntry.at(0));
            }
          }
        }

        

        //std::cout << "SIZE = " << deterStateEntry.size() << std::endl;
        if(deterStateEntry.size() > 1){
          //int indice_from = deterStateEntry.back();
          int indice_from = deterStateEntry.at(0);
          res.addSymbol(letter);
          res.addState(indice_from);

          int indice_to = indices.size();

          
          int size = deterStateEntry.size();
          std::set<int> origin;
          for(int i = 1; i < size; ++i){
            origin.insert(deterStateEntry.at(i));
          }
          std::set<int> transitionsMT = other.makeTransition(origin, letter);
          std::vector<int> state;
          state.push_back(indice_to);
          for(int v : transitionsMT){
            state.push_back(v);
          }
          //state.push_back(indice_to);
          for(int j : indices){
            std::vector<int> temp_state;
            temp_state.push_back(j);
            for(int v : transitionsMT){
              temp_state.push_back(v);
            }
            //temp_state.push_back(j);
            if(deter.find(temp_state) != deter.end()){
              indice_to = j;
              state = temp_state;
              break;
            }
          }

          /*std::cout << "FROM {";
          for(int v : deterStateEntry){
            std::cout << v << " ";
          }
          std::cout <<"}\nTO {";
          for(int v : state){
            std::cout << v << " ";
          }
          std::cout << "}" << std::endl;*/

          bool addTr = false;
          res.addState(indice_to);
          if(state.size() > 1 && res.addTransition(indice_from, letter, indice_to)){
            addTr = true;
            //std::cout << "ADD TR :" << indice_from << " " << letter << " " << indice_to << std::endl;
            deter.insert(state);
            indices.insert(indice_to);
          }
          if(!addTr){
            res.removeState(indice_to);
          }

          /*for(int i = 0; i < size - 1; ++i){
            std::cout << deterStateEntry.at(i) << std::endl;
            transitionsMT.push_back(other.makeTransition({deterStateEntry.at(i)}, letter));
          }


          for(int value : transitionsMT.at(0)){
            std::cout << "v = " << value << std::endl;
            createDeterministicRec(res, deter, transitionsMT, {}, indices, value, 1, (size-1), indice_from, letter);
          }*/
        }
      }
    }

    res.setStateInitial(0);

    return res;
  }  

  bool Automaton::isIncludedIn(const Automaton& other) const{
    assert(this->isValid());
    assert(other.isValid());

    if(this->countStates() > 25 || other.countStates() > 25){
      printf("Automaton avec trop d'états, this %ld états et other %ld états\n", this->countStates(), other.countStates());
      return true;
    }

    fa::Automaton copyThis = *this;

    copyThis.removeNonCoAccessibleStates();
    copyThis.removeNonAccessibleStates();

    // if(copyThis.isLanguageEmpty()){
    //   return false;
    // }

    std::vector<char> diff;
    std::set_difference(copyThis.alphabet.begin(), copyThis.alphabet.end(), other.alphabet.begin(), other.alphabet.end(), std::inserter(diff, diff.begin()));

    


    for(char c : diff){
      for(auto state : copyThis.state){
        if(copyThis.transition.find(state.first) != copyThis.transition.end() && copyThis.transition.at(state.first).find(c) != copyThis.transition.at(state.first).end()){
          return false;
        }
      }
    }

    fa::Automaton res = fa::Automaton::createComplement(other);
    return res.hasEmptyIntersectionWith(copyThis);
  }

  Automaton Automaton::createMinimalMoore(const Automaton& other){
    assert(other.isValid());
    fa::Automaton res = other;
    
    res = fa::Automaton::createDeterministic(res);
    /*if(!res.isDeterministic()){
      return res;
    }*/
    // res.removeNonAccessibleStates();
    res = fa::Automaton::createComplete(res);
    

    std::map<int, std::map<char, int>> init, init_next;
    std::vector<std::vector<int>> states;
    for(auto stateEntry : res.state){
      //std::cout << "STATE :" << stateEntry.first << std::endl;
      if(res.isStateFinal(stateEntry.first)/*stateEntry.second == FINAL_AND_INITIAL_STATE || stateEntry.second == FINAL_STATE*/){
        init[stateEntry.first]['\0'] = 2;
      }else{
        init[stateEntry.first]['\0'] = 1;
      }
    }

    
    for(auto transitionEntry : res.transition){
      //std::cout << transitionEntry.first << std::endl;
      for(auto letterEntry : transitionEntry.second){
        for(auto stateEntry : res.state){
          
          if(res.hasTransition(transitionEntry.first, letterEntry.first, stateEntry.first)){
            init[transitionEntry.first][letterEntry.first] = init[stateEntry.first]['\0'];
          }
        }
      }
    }

    /*std::cout << "INIT :\n";
    for(auto entry : init){
      for(auto l : entry.second){
        std::cout << entry.first << " " << l.first << " " << l.second << std::endl;
      }
    }*/

    for(auto initEntry : init){
      std::vector<int> state;
      for(auto letterEntry : initEntry.second){
        state.push_back(letterEntry.second);
      }
      /*std::cout << "STATE :";
      for(int v : state){
        std::cout << v << " ";
      }
      std::cout << "\n";*/
      if(std::find(states.begin(), states.end(), state) == states.end()/*states.find(state) == states.end()*/){
        states.push_back(state);//states.insert(state);
        //std::cout << "init_next :" << initEntry.first << " 0 " << states.size() - 1 << std::endl;
        init_next[initEntry.first]['\0'] = states.size();
      }else{
        //std::cout << "init_next :" << initEntry.first << " 0 " << std::distance(states.begin(), std::find(states.begin(), states.end(), state)) + 1 << std::endl;
        init_next[initEntry.first]['\0'] = std::distance(states.begin(), std::find(states.begin(), states.end(), state)) + 1;
      }
      
      
    }

    for(auto transitionEntry : init){
      for(auto letterEntry : transitionEntry.second){
        for(auto stateEntry : res.state){
          if(res.hasTransition(transitionEntry.first, letterEntry.first, stateEntry.first)){
            init_next[transitionEntry.first][letterEntry.first] = init_next[stateEntry.first]['\0'];
          }
        }
      }
    }

    //std::cout << "!EQUALS = " << (init != init_next) << std::endl;

    int  i = 2;
    //int otherSize = other.countStates();
    //int stateSize = states.size();

    /*std::cout << "STATES :";
    for(auto statesEntry : states){
      std::cout << "{";
      for(int v : statesEntry){
        std::cout << v << " ";
      }
      std::cout << "}";
    }
    std::cout << std::endl;

    std::cout << "INIT :\n";
    for(auto entry : init){
      for(auto l : entry.second){
        std::cout << entry.first << " " << l.first << " " << l.second << std::endl;
      }
    }

    std::cout << "\nINIT_NEXT :\n";
    for(auto entry : init_next){
      for(auto l : entry.second){
        std::cout << entry.first << " " << l.first << " " << l.second << std::endl;
      }
    }*/

    //std::cout << "otherSize :" << otherSize << "stateSize :" << stateSize << std::endl; 
    while(/*i < 5 && */init != init_next/* && stateSize < otherSize*/){
      std::set<int> indice;
      states = {};
      /*std::cout << "WHILE\n";

      std::cout << "INIT :\n";
      for(auto entry : init){
        for(auto l : entry.second){
          std::cout << entry.first << " " << l.first << " " << l.second << std::endl;
        }
      }

      std::cout << "INIT_NEXT :\n";
      for(auto entry : init_next){
        for(auto l : entry.second){
          std::cout << entry.first << " " << l.first << " " << l.second << std::endl;
        }
      }*/
      if(i%2 == 0){
        //std::cout << "CHANGEMENT DE INIT\n";
        for(auto initEntry : init_next){
          std::vector<int> state;
          for(auto letterEntry : initEntry.second){
            state.push_back(letterEntry.second);
          }
          /*std::cout << "STATES :";
          for(auto statesEntry : states){
            std::cout << "{";
            for(int v : statesEntry){
              std::cout << v << " ";
            }
            std::cout << "}";
          }
          std::cout << std::endl;
          std::cout << "STATE :";
          for(int v : state){
            std::cout << v << " ";
          }
          std::cout << "\n";

          std::cout << "indice :";
          for(int v : indice){
            std::cout << v << " ";
          }
          std::cout << "\n";*/
          if(std::find(states.begin(), states.end(), state) == states.end()/*states.find(state) == states.end()*/){
            states.push_back(state);//states.insert(state);
            if(indice.find(states.size()) == indice.end()){
              //std::cout << "INSERT_i init :" << initEntry.first << " 0 " << indice.size()+1 << std::endl;
              init[initEntry.first]['\0'] = indice.size()+1;
              indice.insert(indice.size()+1);
            }else{
              //std::cout << "INSERT init :" << initEntry.first << " 0 " << states.size() << std::endl;
              init[initEntry.first]['\0'] = states.size();
              indice.insert(states.size());
            }
            
            
          }else{
            //std::cout << "init_dist :" << initEntry.first << " 0 " << std::distance(states.begin(), std::find(states.begin(), states.end(), state)) + 1 << std::endl;
            init[initEntry.first]['\0'] = std::distance(states.begin(), std::find(states.begin(), states.end(), state)) + 1;
          }
          
          for(auto letterEntry : initEntry.second){
            for(auto stateEntry : res.state){
              if(res.hasTransition(initEntry.first, letterEntry.first, stateEntry.first)){
                //std::cout << "LTR :" << initEntry.first << " " << letterEntry.first << " " << init_next[stateEntry.first]['0'] << std::endl;
                init[initEntry.first][letterEntry.first] = init_next[stateEntry.first]['\0'];
              }
            }
          }
          
        }
      }else{
        //std::cout << "CHANGEMENT DE INIT_NEXT\n";
        for(auto initEntry : init){
          std::vector<int> state;
          for(auto letterEntry : initEntry.second){
            state.push_back(letterEntry.second);
          }
          /*std::cout << "STATES :";
          for(auto statesEntry : states){
            std::cout << "{";
            for(int v : statesEntry){
              std::cout << v << " ";
            }
            std::cout << "}";
          }
          std::cout << std::endl;
          std::cout << "STATE :";
          for(int v : state){
            std::cout << v << " ";
          }
          std::cout << "\n";

          std::cout << "indice :";
          for(int v : indice){
            std::cout << v << " ";
          }
          std::cout << "\n";*/
          if(std::find(states.begin(), states.end(), state) == states.end() /*states.find(state) == states.end()*/){
            states.push_back(state); //states.insert(state);
            if(indice.find(states.size()) == indice.end()){
              //std::cout << "INSERT_i init :" << initEntry.first << " 0 " << indice.size() + 1 << std::endl;
              init_next[initEntry.first]['\0'] = indice.size() + 1;
              indice.insert(indice.size() + 1);
            }else{
              //std::cout << "INSERT init :" << initEntry.first << " 0 " << states.size() << std::endl;
              init_next[initEntry.first]['\0'] = states.size();
              indice.insert(states.size());
            }
          }else{
            //std::cout << "init_next_dist :" << initEntry.first << " 0 " << std::distance(states.begin(), std::find(states.begin(), states.end(), state)) + 1 << std::endl;
            init_next[initEntry.first]['\0'] = std::distance(states.begin(), std::find(states.begin(), states.end(), state)) + 1;
          }

          for(auto letterEntry : initEntry.second){
            for(auto stateEntry : res.state){
              if(res.hasTransition(initEntry.first, letterEntry.first, stateEntry.first)){
                //std::cout << "LTR :" << initEntry.first << " " << letterEntry.first << " " << init[stateEntry.first]['0'] << std::endl;
                init_next[initEntry.first][letterEntry.first] = init[stateEntry.first]['\0'];
              }
            }
          }
        }
      }
      //int stateSize = states.size();
      /*std::cout << "STATES :";
      for(auto statesEntry : states){
        std::cout << "{";
        for(int v : statesEntry){
          std::cout << v << " ";
        }
        std::cout << "}";
      }
      std::cout << std::endl;*/
      ++i;
    }

    // std::cout << "\n\nINIT\n";
    // for(auto stateEntry : init){
    //   for(auto letterEntry : stateEntry.second){
    //     std::cout << stateEntry.first << " " << letterEntry.first << " " << letterEntry.second << std::endl;
    //   }
    // }

    // std::cout << "INIT_NEXT\n";
    // for(auto stateEntry : init_next){
    //   for(auto letterEntry : stateEntry.second){
    //     std::cout << stateEntry.first << " " << letterEntry.first << " " << letterEntry.second << std::endl;
    //   }
    // }

    

    //std::cout << "EQUALS = " << (init == init_next) << std::endl;
    
    //std::cout << "otherSize :" << otherSize << "stateSize :" << stateSize << std::endl; 
    
    //std::cout << "stateSize = " << (stateSize < otherSize) << std::endl;
   
   int initSize = init.size();
   int init_nextSize = init_next.size();
   //int resSize = res.state.size();
   int stateS = states.size();

   //printf("stateS = %d, initSize = %d, init_nextSize = %d",stateS, initSize, init_nextSize);
   
    if(stateS >= initSize || stateS >= init_nextSize){
      //printf("minim\n\n");
      return res;
    }
    //printf("not minim\n");
    fa::Automaton res_final;

    res_final.alphabet = res.alphabet;

    for(auto stateEntry : init_next){
      for(auto letterEntry : stateEntry.second){
        if(letterEntry.first == '\0'){
          res_final.addState(letterEntry.second);
          // if(res_final.addState(letterEntry.second)){
          //   std::cout << "BASE " << stateEntry.first <<  " ADD STATE RES :" << letterEntry.second << std::endl;
          // }
          // if(res.isStateFinal(stateEntry.first)){
            
          //   res_final.setStateFinal(letterEntry.second);
          //   if(res_final.isStateFinal(letterEntry.second)){
          //     printf("BASE %d SET FINAL = %d\n",  stateEntry.first, letterEntry.second);
          //   }
          // }
          // if(res.isStateInitial(stateEntry.first)){
            
            
          //   res_final.setStateInitial(letterEntry.second);
          //   if(res_final.isStateInitial(letterEntry.second)){
          //     printf("BASE %d SET INIT = %d\n", stateEntry.first, letterEntry.second);
          //   }
          // }
        }
        // if(letterEntry.first != '0'){
        //   res_final.addSymbol(letterEntry.first);
        //   /*if(res_final.addSymbol(letterEntry.first)){
        //     std::cout << "ADD SYMBOL RES :" << letterEntry.first << std::endl;
        //   }*/
        // }
      }
    }

    for(auto stateEntry : init_next){
      int indice_from = init_next[stateEntry.first]['\0'];
      if(res.isStateFinal(stateEntry.first)){
        if(res_final.isStateFinal(indice_from)){
          continue;
        }
        res_final.setStateFinal(indice_from);
        // if(res_final.isStateFinal(indice_from)){
        //   printf("BASE %d SET FINAL : %d\n", stateEntry.first, indice_from);
        // }
      }
      if(res.isStateInitial(stateEntry.first)){
        if(res_final.isStateInitial(indice_from)){
          continue;
        }
        res_final.setStateInitial(indice_from);
        // if(res_final.isStateInitial(indice_from)){
        //   printf("BASE %d SET INITIAL : %d\n", stateEntry.first, indice_from);
        // }
      }
      for(auto letterEntry : stateEntry.second){
        //std::cout << "TR :" << indice_from << " " << letterEntry.first << " " << init[stateEntry.first][letterEntry.first] << std::endl;
        res_final.addTransition(indice_from, letterEntry.first, init[stateEntry.first][letterEntry.first]);
        /*if(res_final.addTransition(indice_from, letterEntry.first, init[stateEntry.first][letterEntry.first])){
          std::cout << "ADD TR :" << indice_from << " " << letterEntry.first << " " << init[stateEntry.first][letterEntry.first] << std::endl;
        }*/
      }
    }
    return res_final;
  }

  Automaton Automaton::createMinimalBrzozowski(const Automaton& other){
    assert(other.isValid());
    fa::Automaton res = other;
    res = createMirror(res);
    res = createDeterministic(res);
    res = createMirror(res);
    res = createDeterministic(res);
    res = createComplete(res);
    return res;
  }

}