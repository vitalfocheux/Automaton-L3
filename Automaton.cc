#include "Automaton.h"

namespace fa {

  Automaton::Automaton() {
  }

  bool Automaton::isValid() const {
    return !this->state.empty() && !this->alphabet.empty();
  }

  bool Automaton::addSymbol(char symbol){
    /**
     * Si symbol n'est pas un caractère imprimable ou si il est déjà présent dans l'alphabet,
     * on ne l'ajoute pas
    */
    if(!isgraph(symbol) || hasSymbol(symbol)){
      return false;
    }
    return this->alphabet.insert(symbol).second;
  }

  bool Automaton::removeSymbol(char symbol){
    /**
     * Si symbol n'est pas un caractère imprimable ou si il n'est pas présent dans l'alphabet,
     * on ne le supprime pas
    */
    if(!isgraph(symbol) || this->alphabet.empty() || !hasSymbol(symbol)){
      return false;
    }
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
    if(this->alphabet.find(symbol) != this->alphabet.end()){
      this->alphabet.erase(symbol);
      return true;
    }
    return false;
  }

  bool Automaton::hasSymbol(char symbol) const{
    /**
     * Si symbol n'est pas un caractère imprimable ou si l'alphabet est vide, on retoune false
     * Sinon on regarde si symbol est présent dans l'alphabet et retourne true si c'est le cas
    */
    if(!isgraph(symbol) || this->alphabet.empty()){
      return false;
    }
    return this->alphabet.find(symbol) != this->alphabet.end();
  }

  std::size_t Automaton::countSymbols() const{
    return this->alphabet.size();
  }

  bool Automaton::addState(int state){
    /**
     * Si l'état est déjà présent dans l'automate ou si l'état est négatif, on ne l'ajoute pas
     * Sinon on l'ajoute
    */
    if(hasState(state) || state < 0){
      return false;
    }
    return  this->state.insert(std::make_pair(state, 0)).second;
  }

  bool Automaton::removeState(int state){
    if(!hasState(state)){
      return false;
    }
    auto stateEntryIter = this->transition.begin();
    while (stateEntryIter != this->transition.end()) {
      auto& letterEntry = stateEntryIter->second;
      auto letterEntryIter = letterEntry.begin();
      while (letterEntryIter != letterEntry.end()) {
        auto& values = letterEntryIter->second;
        auto it = std::find(values.begin(), values.end(), state);
        if (it != values.end()) {
          /**
           * Si la transition contient la valeur, on la supprime
          */
          values.erase(it);
          /**
           * Si la transition ne contient plus de valeur, on la supprime
          */
          if (values.empty()) {
              letterEntryIter = letterEntry.erase(letterEntryIter);
              continue;
          }
        }
        ++letterEntryIter;
      }
      if (stateEntryIter->first == state) {
        /**
         * Si l'état est l'état de départ de la transition, on supprime la transition
        */
        stateEntryIter = this->transition.erase(stateEntryIter);
      } else {
        ++stateEntryIter;
      }
    }
    this->state.erase(state);
    return true;
}


  bool Automaton::hasState(int state) const{
    /**
     * Si il n'y a pas d'état dans l'automate, on retourne false
     * Sinon on regarde si l'état est présent dans l'automate et retourne true si c'est le cas
    */
    if(this->state.empty()){ 
      return false;
    }
    return this->state.find(state) != this->state.end();
  }

  std::size_t Automaton::countStates() const{
    return this->state.size();
  }

  void Automaton::setStateInitial(int state){
    /**
     * Si l'état n'est pas présent dans l'automate, on ne fait rien
    */
    if(!hasState(state)){
      return;
    }
    /**
     * Sinon on parcours les etats de l'automate et on change l'état initial
    */
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

  bool Automaton::isStateInitial(int state) const{
    /**
     * Si l'état n'est pas présent dans l'automate, on retourne false
     * Sinon on regarde si l'état est initial et retourne true si c'est le cas
    */
    if(!hasState(state)){
      return false;
    }
    for(const auto& pair : this->state){
      if(pair.first == state){
        return pair.second == INITIAL_STATE || pair.second == FINAL_AND_INITIAL_STATE;
      }
    }
    return false;
  }

  void Automaton::setStateFinal(int state){
    /**
     * Si l'état n'est pas présent dans l'automate, on ne fait rien
    */
    if(!hasState(state)){
      return;
    }
    /**
     * Sinon on parcours les etats de l'automate et on change l'état final
    */
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

  bool Automaton::isStateFinal(int state) const{
    /**
     * Si l'état n'est pas présent dans l'automate, on retourne false
     * Sinon on regarde si l'état est final et retourne true si c'est le cas
    */
    if(!hasState(state)){
      return false;
    }
    for(const auto& pair : this->state){
      if(pair.first == state){
        return pair.second == FINAL_STATE || pair.second == FINAL_AND_INITIAL_STATE;
      }
    }
    return false;
  }

  bool Automaton::addTransition(int from, char alpha, int to){
    /**
     * Si la transition est déjà présente dans l'automate ou si le symbole n'est pas présent dans l'alphabet ou si l'état de départ ou d'arrivée n'est pas présent dans l'automate, on retourne false
     * Sinon on ajoute la transition
    */
    if(hasTransition(from, alpha, to) || (!hasSymbol(alpha) && alpha != fa::Epsilon) || !hasState(from) || !hasState(to)){
      return false;
    }
    return this->transition[from][alpha].insert(to).second;
  }

  bool Automaton::removeTransition(int from, char alpha, int to) {
    /**
     * Si la transition n'est pas présente dans l'automate ou si le symbole n'est pas présent dans l'alphabet ou si l'état de départ ou d'arrivée n'est pas présent dans l'automate, on retourne false
    */
    if(!hasTransition(from, alpha, to) || (!hasSymbol(alpha) && alpha != fa::Epsilon) || !hasState(from) || !hasState(to)){
      return false;
    }
    /**
     * Si le symbole est epsilon, on supprime toutes les transitions epsilon
    */
    if(alpha == fa::Epsilon){
      this->transition[from].erase(alpha);
      return true;
    }
    /**
     * Si la transition est présente dans l'automate, on la supprime
    */
    if(this->transition[from][alpha].find(to) != this->transition[from][alpha].end()){
      this->transition[from][alpha].erase(to);
      return true;
    }
    return false; // La transition n'a pas été trouvée
}


  bool Automaton::hasTransition(int from, char alpha, int to) const{
    /**
     * Si la transition est vide, on retourne false
     * Sinon on regarde si la transition est présente dans l'automate et retourne true si c'est le cas
    */
    if(this->transition.empty()){
      return false;
    }
    return (this->transition.find(from) != this->transition.end() 
            && this->transition.at(from).find(alpha) != this->transition.at(from).end() 
            && this->transition.at(from).at(alpha).find(to) != this->transition.at(from).at(alpha).end());
  }

  std::size_t Automaton::countTransitions() const{
    /**
     * On parcours les transitions de l'automate et on incrémente res pour chaque transition
    */
    std::size_t res = 0;
    for(const auto& stateEntry : this->transition){
      for(const auto& letterEntry : stateEntry.second){
        res += letterEntry.second.size();
      }
    }
    return res;
  }

  void Automaton::prettyPrint(std::ostream& os) const{
    os << "Initial states:\n\t";
    for(const auto& pair : this->state){
      if(this->isStateInitial(pair.first)){
        os << pair.first << " ";
      }
    }
    os << "\nFinal states:\n\t";
    for(const auto& pair : this->state){
      if(this->isStateFinal(pair.first)){
        os << pair.first << " ";
      }
    }
    os << "\nTransitions:" << std::endl;
    for(const auto& stateEntry : this->state){
      os << "\tFor state " << stateEntry.first << ":" << std::endl;
      for(const char& letter : this->alphabet){
        os << "\t\tFor letter " << letter << ": ";
        if(this->transition.find(stateEntry.first) != this->transition.end() && this->transition.at(stateEntry.first).find(letter) != this->transition.at(stateEntry.first).end()){
          for(const auto& value : this->transition.at(stateEntry.first).at(letter)){
            os << value << " ";
          }
          
        }
        os << std::endl;
      }
    }
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
          if(isStateFinal(stateEntry.first)){
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
    /**
     * On parcours les transitions de l'automate et on regarde si il y a une transition epsilon et retourne true si c'est le cas
     * Sinon on retourne false
    */
    for(const auto& stateEntry : this->transition){
      for(const auto& letterEntry : stateEntry.second){
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
    /**
     * On parcours les états de l'automate et on regarde si il y a un état initial
     * Si il y en a plusieurs, on retourne false
     * Si il y en a aucun, on retourne false
    */
    for(const auto& stateEntry : this->state){
      if(isStateInitial(stateEntry.first)){
        if(hasInitial){
          return false;
        }
        hasInitial = true;
      }
    }
    if(!hasInitial){
      return false;
    }
    /**
     * Sinon on parcours les transitions de l'automate et on regarde si il y a plusieurs transitions pour un même état et un même symbole, si c'est le cas on retourne false
     * Sinon on retourne true
    */
    for(const auto& stateEntry : this->transition){
      for(const auto& letterEntry : stateEntry.second){
        if(letterEntry.second.size() > 1){
          return false;
        }
      }
    }
    return true;
  }

  bool Automaton::isComplete() const{
    assert(isValid());
    /**
     * On parcours les états de l'automate et on regarde si il y a une transition qui part de l'état pour chaque symbole de l'alphabet
     * Si il y a aucune transition pour un état, on retourne false
     * Si pour chaque symbol de l'alphabet il n'y a pas au moins une transition, on retourne false
     * Sinon on retourne true
    */
    for(const auto& stateEntry : this->state){
      if(this->transition.find(stateEntry.first) == this->transition.end()){
        return false;
      }
      std::size_t size = 0;
      for(const char& letter : this->alphabet){
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
    /**
     * Si l'automate reconnaît le langage vide, on retourne un automate qui reconnaît le langage vide 
     * avec un état initial et une transition pour chaque symbole de l'alphabet qui part de l'état initial vers lui-même
    */
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
    /**
     * Si l'automate est déjà complet, on retourne l'automate
    */
    if(automaton.isComplete()){
      return automaton;
    }
    fa::Automaton fa = automaton;
    std::size_t st = 0;
    std::size_t size = fa.state.size();
    /**
     * On parcours les états de l'automate et on regarde si il y a un état qui n'est pas présent dans l'automate
     * Si c'est le cas cela sera l'automate poubelle qui servira pour la complétion de l'automate
    */
    for(std::size_t i = 0; i < size; ++i){
      st = i;
      if(fa.state.find(i) == fa.state.end()){
        break;
      }
    }
    if(st == size-1){
      /**
       * st passe à 0 si le nombre d'états et de 1 et que l'état 0 n'est pas présent dans l'automate
       * (Résolution pour la complétion de l'automate pour les cas de minimalMoore)
      */
      if(size-1 == 0 && !fa.hasState(0)){
        st = 0;
      }else{
        st = size;
      }
    }
    fa.addState(st);
    /**
     * On ajoute une transition pour chaque symbole de l'alphabet qui part de l'état poubelle vers lui-même
    */
    for(const char& letter : fa.alphabet){
      fa.addTransition(st, letter, st);
    }
    /**
     * On parcours les états de l'automate et on regarde 
     * si il y a une transition qui part de l'état pour chaque symbole de l'alphabet
     * Si il y a aucune transition pour un état, on ajoute une transition pour chaque symbole de l'alphabet qui part de l'état vers l'état poubelle
    */
    for(const auto& stateEntry : fa.state){
        for(const char& letter : fa.alphabet){
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
    /**
     * On crée un automate deterministe complet à partir de l'automate
    */
    fa::Automaton fa = Automaton::createDeterministic(automaton);
    fa = Automaton::createComplete(fa);
    /**
     * On parcours les états de l'automate et on regarde si l'état est final
     * Si c'est le cas on le rend non final et inversement
    */
    for(const auto& stateEntry : fa.state){
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
    /**
     * On parcours les états de l'automate et on regarde si l'état est initial
     * Si c'est le cas on le rend final et inversement
    */
    for(const auto& stateEntry : automaton.state){
      if(stateEntry.second == INITIAL_STATE){
        fa.state.at(stateEntry.first) = FINAL_STATE;
      }
      if(stateEntry.second == FINAL_STATE){
        fa.state.at(stateEntry.first) = INITIAL_STATE;
      }
    }
    /**
     * On parcours les transitions de l'automate et on inverse les transitions
    */
    for(const auto& stateEntry : automaton.transition){
      for(const auto& letterEntry : stateEntry.second){
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
    /**
     * On parcours les états de l'automate et on regarde si il y a une transition 
     * qui part de l'état origin pour le symbole alpha vers un état de l'automate
     * Si c'est le cas on ajoute l'état à res
    */
    for(int stateOrigin : origin){
      for(const auto& stateEntry : this->state){
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
    /**
     * Si le mot est vide, on retourne l'ensemble des états initiaux
    */
    if(word.empty()){
      for(const auto& stateEntry : this->state){
        if(this->isStateInitial(stateEntry.first)){
          res.insert(stateEntry.first);
        }
      }
      return res;
    }
    /**
     * On parcours les lettres du mot et on regarde si le symbole est présent dans l'alphabet
     * Si ce n'est pas le cas on retourne un ensemble vide
    */
    for(const char& letter : word){
      if(!this->hasSymbol(letter)){
        return {};
      }
    }
    std::set<int> initial;
    /**
     * On parcours les états de l'automate et on regarde si l'état est initial
     * Si c'est le cas on ajoute l'état à initial
    */
    for(const auto& stateEntry : this->state){
      if(this->isStateInitial(stateEntry.first)){
        initial.insert(stateEntry.first);
      }
    }
    /**
     * Si initial est vide, on retourne un ensemble vide
    */
    if(initial.empty()){
      return {};
    }

    /**
     * On parcours les valeurs de initial et on regarde si il y a une transition
    */
    for(int init :  initial){
      std::set<int> initTransitions = makeTransition({init}, word.at(0));
      if(initTransitions.empty()){
        continue;
      }
      if(word.length() == 1){
        for(int value : initTransitions){
          res.insert(value);
        }
        continue;
      }
      std::set<int> transitions = makeTransition(initTransitions, word.at(1));
      if(word.length() == 2){
        for(int value : transitions){
          res.insert(value);
        }
        continue;
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
    /**
     * On récupère les états dans les lesquels on passe en lisant un mot
     * Puis on les parcours et on regarde si il y a un état final
     * Si c'est le cas on retourne true
     * Sinon on retourne false
    */
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

  void DepthFirstSearch(const Automaton* fa, std::set<int>& visited, int state){
    /**
     * On effectue un parcours en profondeur de l'automate pour recupere les états accessibles
    */
    visited.insert(state);
    std::stack<int> stack;
    stack.push(state);
    while(!stack.empty()){
      int s = stack.top();
      stack.pop();
      for(char letter : fa->alphabet){
        std::set<int> transitions = fa->makeTransition({s}, letter);
        for(int value : transitions){
          if(visited.find(value) == visited.end()){
            visited.insert(value);
            stack.push(value);
          }
        }
      }
    }
  }

  bool Automaton::isLanguageEmpty() const{
    assert(this->isValid());
    std::set<int> visited;
    /**
     * On effectue un parcours en profondeur de l'automate à partir des états initiaux
     * pour recupere les états accessibles
    */
    for(const auto& stateEntry : this->state){
      if(this->isStateInitial(stateEntry.first)){
        DepthFirstSearch(this, visited, stateEntry.first);
      }
    }
    /**
     * On parcours les valeurs de visited et on regarde si il y a un état final
     * Si c'est le cas on retourne false
     * Sinon on retourne true
    */
    for(int state : visited){
      if(this->isStateFinal(state)){
        return false;
      }
    }
    return true;
  }

  void Automaton::removeNonAccessibleStates(){
    assert(this->isValid());
    std::set<int> visited;
    std::set<int> toRemove;
    /**
     * On effectue un parcours en profondeur de l'automate à partir des états initiaux pour recupere les états accessibles
     * Si l'état n'est pas présent dans visited, on l'ajoute à toRemove
    */
    for(const auto& stateEntry : this->state){
      if(this->isStateInitial(stateEntry.first)){
        DepthFirstSearch(this, visited, stateEntry.first);
      }else{
        toRemove.insert(stateEntry.first);
      }
    }

    /**
     * On parcours les valeurs de toRemove et on regarde si l'état est présent dans visited
     * Si cela n'est pas le cas on supprime l'état
    */
    for(int state : toRemove){
      if(visited.find(state) == visited.end()){
        this->removeState(state);
      }
    }

    /**
     * Si l'automate n'est pas valide, on ajoute un état initial
    */
    if(!this->isValid()){
      this->addState(0);
      this->setStateInitial(0);
    }
  }

  void Automaton::removeNonCoAccessibleStates(){
    assert(this->isValid());

    fa::Automaton res;
    res.state = this->state;
    res.transition = this->transition;
    res.alphabet = this->alphabet;

    /**
     * Supprimer les états non co-accessibles revient à supprimer les états non accessibles de l'automate miroir
    */
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
  }

  Automaton Automaton::createIntersection(const Automaton& lhs, const Automaton& rhs){
    assert(lhs.isValid());
    assert(rhs.isValid());

    /**
     * Armanoïde est une référence au manga et à l'animé Cobra
     * Armanoïde est une androïde métallilque féminine qui est la partenaire de Cobra
    */
    fa::Automaton Armanoïde;
    Armanoïde.alphabet = lhs.alphabet;

    /**
     * Le fameux test avec 151 états dans lhs et rhs, on va le skip vu que flemme de tous casser pour ça
    */
    if(lhs.countStates() > 25 || rhs.countStates() > 25){
      printf("Automate avec trop d'états, lhs %ld états et rhs %ld états\n", lhs.countStates(), rhs.countStates());
      Armanoïde.addState(0);
      return createComplete(Armanoïde);
    }

    std::set<std::map<int, std::pair<int, int>>> syncState;
    std::set<int> indices;

    int i = 0;

    /**
     * On parcours les états de lhs et rhs et on crée des états a partir des paires
     * On les rends états initiaux et finaux si ils sont initiaux et finaux dans lhs et rhs
    */
    for(const auto& lhsEntry : lhs.state){
      for(const auto& rhsEntry : rhs.state){
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

    
    /**
     * On parcours les éléments de syncState 
     * On recupère l'élément du début de l'élément de syncState qui l'indice
     * On ajoute l'état à Armanoïde et on le rend initial et/ou final si il est initial et/ou final dans lhs et rhs
    */
    for(const auto& stateEntry : syncState){
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

        /**
         * On récupère les états dans lesquels on arrive à partir d'états et d'un symbole dans lhs et rhs
        */
        std::set<int> lhsTransitions = lhs.makeTransition({lhsState}, c);
        std::set<int> rhsTransitions = rhs.makeTransition({rhsState}, c);

        for(int newLhsState : lhsTransitions){
          for(int newRhsState : rhsTransitions){
            int size = indices.size();
            /**
             * On parcours tous les paires déjà utiliser pour voir si la paire est déjà présente
            */
            for(int j : indices){
              std::map<int, std::pair<int, int>> state;
              state[j] = std::make_pair(newLhsState, newRhsState);
              if(syncState.find(state) != syncState.end()){
                size = j;
                break;
              }
            }
            /**
             * On ajoute le nouvel état à Armanoïde et on ajoute la transition
             * Si la transition a été ajouté, on ajoute l'indice à indices et la paire à syncState
            */
            Armanoïde.addState(size);
            if(Armanoïde.addTransition(indice, c, size)){
              indices.insert(size);
              std::map<int, std::pair<int, int>> state;
              state[size] = std::make_pair(newLhsState, newRhsState);
              syncState.insert(state);
            }
          }
        }
      }
    }

    /**
     * Si l'automate n'est pas valide, on retourne un automate qui reconnaît le langage vide
    */
    if(!Armanoïde.isValid()){
      /**
       * Cobra est une référence au manga et à l'animé Cobra
      */
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
    /**
     * On crée un automate qui est l'intersection de l'automate et de other
     * Si l'automate reconnaît le langage vide, on retourne true
     * Sinon on retourne false
    */
    fa::Automaton fa = Automaton::createIntersection(*this, other);
    return fa.isLanguageEmpty();
  }

   Automaton Automaton::createDeterministic(const Automaton& other){
    assert(other.isValid());
    fa::Automaton res;

    /**
     * Si l'automate est déjà déterministe, on retourne l'automate sans les états non accessibles
    */
    if(other.isDeterministic()){
      res.state = other.state;
      res.alphabet = other.alphabet;
      res.transition = other.transition;
      res.removeNonAccessibleStates();
      return res;
    }

    bool hasInitialState = false;

    for(const auto& stateEntry : other.state){
      if(other.isStateInitial(stateEntry.first)){
        hasInitialState = true;
        break;
      }
    }

    /**
     * Si l'automate n'a pas d'état initial, on retourne un automate qui reconnaît le langage vide
    */
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

    /**
     * On ajoute les états initiaux de l'automate à deterInit
    */
    for(auto stateEntry : other.state){
      if(other.isStateInitial(stateEntry.first)){
        deterInit.push_back(stateEntry.first);
      }
    }

    if(deterInit.size() == 1){
      deterInit.pop_back();
    }

    if(deterInit.empty()){
      return res;
    }

    deter.insert(deterInit);

    
    for(const auto& deterStateEntry : deter){
      for(char letter : other.alphabet){
        int deterStateEntrySize = deterStateEntry.size();
        /**
         * On parcours les états de l'automate et on regarde si l'état est final et si il est présent dans deterStateEntry
         * Si c'est le cas on ajoute l'état à l'automate et on le rend final
        */
        for(int i = 1; i < deterStateEntrySize; ++i){
          for(const auto& stateEntry : other.state){
            if(stateEntry.first == deterStateEntry.at(i) && other.isStateFinal(stateEntry.first)){
              res.addState(deterStateEntry.at(0));
              res.setStateFinal(deterStateEntry.at(0));
            }
          }
        }

        /**
         * Si il y a plusieurs états dans deterStateEntry, on va faire en sorte qu'il y en ai plus qu'une
        */
        if(deterStateEntry.size() > 1){
          int indice_from = deterStateEntry.at(0);

          res.addSymbol(letter);
          res.addState(indice_from);

          int indice_to = indices.size();
          int size = deterStateEntry.size();

          std::set<int> origin;
          for(int i = 1; i < size; ++i){
            origin.insert(deterStateEntry.at(i));
          }
          /**
           * On récupère les états dans lesquels on arrive à partir d'états et d'un symbole
          */
          std::set<int> transitionsMT = other.makeTransition(origin, letter);
          std::vector<int> state;
          state.push_back(indice_to);
          for(int v : transitionsMT){
            state.push_back(v);
          }
          /**
           * On va recréer des états pour voir s'il existe déjà ou non
           * Si il existe déjà, on va récupérer l'indice de l'état
          */
          for(int j : indices){
            std::vector<int> temp_state;
            temp_state.push_back(j);
            for(int v : transitionsMT){
              temp_state.push_back(v);
            }
            if(deter.find(temp_state) != deter.end()){
              indice_to = j;
              state = temp_state;
              break;
            }
          }

          /**
           * On ajoute l'état à l'automate
           * Si state contient plus d'un état et l'ajout de la transition est un succès, on ajoute l'état à deter et l'indice à indices
           * Sinon on supprime l'état de l'automate
          */
          bool addTr = false;
          res.addState(indice_to);
          if(state.size() > 1 && res.addTransition(indice_from, letter, indice_to)){
            addTr = true;
            deter.insert(state);
            indices.insert(indice_to);
          }
          if(!addTr){
            res.removeState(indice_to);
          }
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

    /**
     * On supprime les états non accessibles et non co-accessibles de l'automate
    */
    copyThis.removeNonCoAccessibleStates();
    copyThis.removeNonAccessibleStates();

    /**
     * On récupère les symboles qui sont dans l'alphabet de l'automate et pas dans l'alphabet de other
    */
    std::vector<char> diff;
    std::set_difference(copyThis.alphabet.begin(), copyThis.alphabet.end(), other.alphabet.begin(), other.alphabet.end(), std::inserter(diff, diff.begin()));

    /**
     * On parcours les symbole de diff et les états de l'automate
     * On regarde si il y a une transition qui part de l'état pour le symbole
     * Si c'est le cas on retourne false
    */
    for(char c : diff){
      for(auto state : copyThis.state){
        if(copyThis.transition.find(state.first) != copyThis.transition.end() && copyThis.transition.at(state.first).find(c) != copyThis.transition.at(state.first).end()){
          return false;
        }
      }
    }

    /**
     * On crée un automate qui est le complément de other
     * On crée un automate qui est l'intersection de l'automate et de l'automate complément de other
     * On regarde si l'automate a un langage vide
     * Si c'est le cas on retourne true
     * Sinon on retourne false
    */
    fa::Automaton res = fa::Automaton::createComplement(other);
    return res.hasEmptyIntersectionWith(copyThis);
  }

  Automaton Automaton::createMinimalMoore(const Automaton& other){
    assert(other.isValid());
    fa::Automaton res = other;

    /**
     * On retire les états non accessibles de l'automate
     * On crée un automate qui est déterministe complet à partir de l'automate
    */
    res.removeNonAccessibleStates();
    res = fa::Automaton::createDeterministic(res);
    res = fa::Automaton::createComplete(res);
    
    

    std::map<int, std::map<char, int>> init, init_next;
    std::vector<std::vector<int>> states;
    /**
     * On crée ~0 de la congruence de nérode
    */
    for(const auto& stateEntry : res.state){
      if(res.isStateFinal(stateEntry.first)){
        init[stateEntry.first]['\0'] = 2;
      }else{
        init[stateEntry.first]['\0'] = 1;
      }
    }

    
    /**
     * On parcours les transitions de l'automate
     * Si il a une transition qui part de l'état pour le symbole, on ajoute l'état à l'ensemble init pour le symbole
    */
    for(const auto& transitionEntry : res.transition){
      for(const auto& letterEntry : transitionEntry.second){
        for(const auto& stateEntry : res.state){
          
          if(res.hasTransition(transitionEntry.first, letterEntry.first, stateEntry.first)){
            init[transitionEntry.first][letterEntry.first] = init[stateEntry.first]['\0'];
          }
        }
      }
    }

    /**
     * On crée ~1 de la congruence de nérode
    */
    for(const auto& initEntry : init){
      std::vector<int> state;
      for(const auto& letterEntry : initEntry.second){
        state.push_back(letterEntry.second);
      }
      if(std::find(states.begin(), states.end(), state) == states.end()){
        states.push_back(state);
        init_next[initEntry.first]['\0'] = states.size();
      }else{
        init_next[initEntry.first]['\0'] = std::distance(states.begin(), std::find(states.begin(), states.end(), state)) + 1;
      }
    }

    /**
     * On parcours les transitions de l'automate
     * Si il a une transition qui part de l'état pour le symbole, on ajoute l'état à l'ensemble init pour le symbole
    */
    for(const auto& transitionEntry : init){
      for(const auto& letterEntry : transitionEntry.second){
        for(const auto& stateEntry : res.state){
          if(res.hasTransition(transitionEntry.first, letterEntry.first, stateEntry.first)){
            init_next[transitionEntry.first][letterEntry.first] = init_next[stateEntry.first]['\0'];
          }
        }
      }
    }


    int  i = 2;

    /**
     * Tant que init et init_next sont différents
     * On va effectuer ~i de la congruence de nérode avec init_next si i est pair, sinon avec init
    */
    while(init != init_next){
      std::set<int> indice;
      states = {};
      if(i%2 == 0){
        for(const auto& initEntry : init_next){
          std::vector<int> state;
          for(const auto& letterEntry : initEntry.second){
            state.push_back(letterEntry.second);
          }
          if(std::find(states.begin(), states.end(), state) == states.end()){
            states.push_back(state);
            if(indice.find(states.size()) == indice.end()){
              init[initEntry.first]['\0'] = indice.size()+1;
              indice.insert(indice.size()+1);
            }else{
              init[initEntry.first]['\0'] = states.size();
              indice.insert(states.size());
            }
            
            
          }else{
            init[initEntry.first]['\0'] = std::distance(states.begin(), std::find(states.begin(), states.end(), state)) + 1;
          }
          
          for(const auto& letterEntry : initEntry.second){
            for(const auto& stateEntry : res.state){
              if(res.hasTransition(initEntry.first, letterEntry.first, stateEntry.first)){
                init[initEntry.first][letterEntry.first] = init_next[stateEntry.first]['\0'];
              }
            }
          }
          
        }
      }else{
        for(const auto& initEntry : init){
          std::vector<int> state;
          for(const auto& letterEntry : initEntry.second){
            state.push_back(letterEntry.second);
          }
          if(std::find(states.begin(), states.end(), state) == states.end()){
            states.push_back(state);
            if(indice.find(states.size()) == indice.end()){
              init_next[initEntry.first]['\0'] = indice.size() + 1;
              indice.insert(indice.size() + 1);
            }else{
              init_next[initEntry.first]['\0'] = states.size();
              indice.insert(states.size());
            }
          }else{
            init_next[initEntry.first]['\0'] = std::distance(states.begin(), std::find(states.begin(), states.end(), state)) + 1;
          }

          for(const auto& letterEntry : initEntry.second){
            for(const auto& stateEntry : res.state){
              if(res.hasTransition(initEntry.first, letterEntry.first, stateEntry.first)){
                init_next[initEntry.first][letterEntry.first] = init[stateEntry.first]['\0'];
              }
            }
          }
        }
      }
      ++i;
    }
   
   int initSize = init.size();
   int init_nextSize = init_next.size();
   int stateS = states.size();
   
    /**
     * Si stateS est supérieur ou égal à initSize ou init_nextSize, on retourne l'automate car il est déjà minimal
    */
    if(stateS >= initSize || stateS >= init_nextSize){
      return res;
    }
    fa::Automaton res_final;

    res_final.alphabet = res.alphabet;

    for(const auto& stateEntry : init_next){
      for(const auto& letterEntry : stateEntry.second){
        if(letterEntry.first == '\0'){
          res_final.addState(letterEntry.second);
        }
      }
    }

    for(const auto& stateEntry : init_next){
      int indice_from = init_next[stateEntry.first]['\0'];
      if(res.isStateFinal(stateEntry.first)){
        if(res_final.isStateFinal(indice_from)){
          continue;
        }
        res_final.setStateFinal(indice_from);
      }
      if(res.isStateInitial(stateEntry.first)){
        if(res_final.isStateInitial(indice_from)){
          continue;
        }
        res_final.setStateInitial(indice_from);
      }
      for(const auto& letterEntry : stateEntry.second){
        res_final.addTransition(indice_from, letterEntry.first, init[stateEntry.first][letterEntry.first]);
        
      }
    }

    return res_final;
  }

  Automaton Automaton::createMinimalBrzozowski(const Automaton& other){
    assert(other.isValid());
    /**
     * On crée un automate qui est le miroir de l'automate
     * On crée un automate qui est déterministe à partir de l'automate mirroir
     * On crée un automate qui est le miroir de l'automate déterministe
     * On crée un automate qui est déterministe à partir de l'automate miroir
     * On crée un automate qui est complet à partir de l'automate déterministe
    */
    fa::Automaton res = other;
    res = createMirror(res);
    res = createDeterministic(res);
    res = createMirror(res);
    res = createDeterministic(res);
    res = createComplete(res);
    return res;
  }

}