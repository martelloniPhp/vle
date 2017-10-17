
#include <vle/utils/State.hpp>

class State;
namespace vle {
namespace utils {

std::vector<State*> State::states;

int State::nb(){ return states.size();}

}}// namespace vle utils
